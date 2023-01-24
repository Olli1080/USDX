#include "URecord.h"

#include <bitset>
#include <boost/algorithm/string/trim.hpp>
/*
	uses
	  ULog,
	  UNote;
	  */

namespace URecord
{
    // Global }

    TAudioInputProcessor AudioInputProcessor()
    {
        if singleton_AudioInputProcessor = nil then
            singleton_AudioInputProcessor = TAudioInputProcessor::create();

        result = singleton_AudioInputProcessor;
    }

    // TAudioInputDevice }

    /*destructor TAudioInputDevice::Destroy;
    {
        Stop();
Source = nil;
CaptureChannel = nil;
    FreeAndNil(AudioFormat);
    inherited Destroy;
    }*/

    void TAudioInputDevice::LinkCaptureBuffer(int ChannelIndex, TCaptureBuffer::SPtr Sound)
        /*var
            OldSound : TCaptureBuffer;*/
    {
        // check bounds
        if (ChannelIndex < 0 || ChannelIndex >= CaptureChannel.size())
            return;

        // reset previously assigned (old) capture-buffer
        if (auto OldSound = CaptureChannel[ChannelIndex]; OldSound)
            OldSound->StopCapture();

        // set audio-format of new capture-buffer

        if (Sound)
            Sound->StartCapture(AudioFormat);

        // replace old with new buffer (Note: Sound might be nil)
        CaptureChannel[ChannelIndex] = Sound;
    }

    // TSound
    TCaptureBuffer::TCaptureBuffer()
    {
        /*inherited;
        LogBuffer = TMemoryStream.Create;
        fAnalysisBufferLock = SDL_CreateMutex();
        AnalysisBufferSize = Length(AnalysisBuffer);*/
    }

    TCaptureBuffer::~TCaptureBuffer()
    {
        FreeAndNil(LogBuffer);
        FreeAndNil(fVoiceStream);
        FreeAndNil(fAudioFormat);
        fAnalysisBufferLock = nil;
    }

    void TCaptureBuffer::Clear()
    {
        if (LogBuffer)
            LogBuffer.Clear();
        std::unique_lock lock(fAnalysisBufferLock);
        AnalysisBuffer = { 0 };
    }

    void TCaptureBuffer::ProcessNewBuffer(std::vector<int16_t>& Buffer)
        /*var
            BufferOffset : int;
    SampleCount:  int;
    i:            int;*/
    {
        // apply software boost
        BoostBuffer(Buffer);

        // voice passthrough (send data to playback-device)
        if (fVoiceStream)
            fVoiceStream->WriteData(Buffer);

        // we assume that samples are in S16Int format
        // TODO: support float too
        if (fAudioFormat.Format() != UMusic::TAudioSampleFormat::asfS16)
            return;

        // process BufferArray
        size_t BufferOffset = 0;
        size_t SampleCount = Buffer.size();
        // check if we have more new samples than we can store
        if (SampleCount >= MaxBufferSize)
        {
            // discard the oldest of the new samples
            AnalysisBuffer = { Buffer.end() - MaxBufferSize, Buffer.end() };
        }
        else
        {
            std::unique_lock lock(fAnalysisBufferLock);

            // move old samples to the beginning of the array (if necessary)
            const int toRemove = SampleCount - (MaxBufferSize - AnalysisBuffer.size());
            if (toRemove > 0)
				AnalysisBuffer.erase(AnalysisBuffer.begin(), AnalysisBuffer.begin() + toRemove);

            // copy new samples to analysis buffer
            AnalysisBuffer.insert_range(AnalysisBuffer.end(), Buffer);
        }

        // save capture-data to BufferLong if enabled
        if (UIni::Ini.SavePlayback == 1)
        {
            // this is just for debugging (approx 15MB per player for a 3min song!!!)
            // For an in-game replay-mode we need to compress data so we do not
            // waste that much memory. Maybe ogg-vorbis with voice-preset in fast-mode?
            // Or we could use a faster but not that efficient lossless compression.
            LogBuffer.Write(Buffer);
        }
    }

    void TCaptureBuffer::AnalyzeBuffer()
    {
        ToneValid = false;
        ToneAbs = -1;
        Tone = -1;

        std::unique_lock lock(fAnalysisBufferLock);

        // find maximum volume of first 1024 samples
        double MaxVolume = 0;
        for (size_t SampleIndex = 0; SampleIndex < 1024; ++SampleIndex)
        {
            double Volume = std::abs<double>(AnalysisBuffer[SampleIndex]) / static_cast<double>(-std::numeric_limits<int16_t>::min());
            MaxVolume = std::max(Volume, MaxVolume);
        }
        const double Threshold = UIni::IThresholdVals[UIni::Ini.ThresholdIndex];

        // check if signal has an acceptable volume (ignore background-noise)
        if (MaxVolume >= Threshold)
        {
            // analyse the current voice pitch
            //ToneValid := AnalyzePitch(PDA_AMDF);
            //Write("AMDF: ",ToneAbs:2," (f: ",Frequencies[ToneAbs]:7:2," Hz)");
            //Write(Tone:2);
            ToneValid = AnalyzePitch(TPDAType::PDA_CAMDF);
            //Writeln("   /   CAMDF: ",ToneAbs:2," (f: ",Frequencies[ToneAbs]:7:2," Hz)");
            //Writeln(" ",Tone:2);
        }
    }

    void TCaptureBuffer::SetDelays()
    {
        const auto& sampleRate = fAudioFormat.SampleRate();
        if (sampleRate == prevSampleRate)
            return;

        prevSampleRate = sampleRate;
        for (size_t ToneIndex = 0; ToneIndex < NumHalftones; ++ToneIndex)
            Delays[ToneIndex] = std::round<int>(sampleRate / Frequencies[ToneIndex]);
    }

    bool TCaptureBuffer::AnalyzePitch(TPDAType PDA)
    {
        // prepare to analyze
        SetDelays();

        // analyze halftones
        // Note: at the lowest tone (~65 Hz) and a buffer-size of 4096
        // at 44.1 (or 48 kHz) only 6 (or 5) samples are compared, this might be
        // too few samples -> use a bigger buffer-size
        TCorrelationArray Correlation;
        switch (PDA)
        {
        case TPDAType::PDA_AMDF:
        {
            Correlation = AverageMagnitudeDifference();
            break;
        }
        case TPDAType::PDA_CAMDF:
        {
            Correlation = CircularAverageMagnitudeDifference();
            break;
        }
        }
        ToneAbs = std::ranges::min_element(Correlation) - Correlation.begin();
        Tone = ToneAbs % 12;
        return true;
    }

    // Average Magnitude Difference Function (AMDF) is defined as
    //   D(\tau)=\frac{1}{N-\tau-1}\sum_{n=0}^{N-\tau-1}|x(n) - x(n+\tau)|
    // where \tau = Delay, n = SampleIndex, N = AnalysisBufferSize, x = AnalysisBuffer
    // See: Equation (1) in http://www.utdallas.edu/~hxb076000/citing_papers/Muhammad%20Extended%20Average%20Magnitude%20Difference.pdf
    TCorrelationArray TCaptureBuffer::AverageMagnitudeDifference()
    {
        TCorrelationArray Correlation = { 0 };
        // accumulate the magnitude differences for samples in AnalysisBuffer
        for (size_t ToneIndex = 0; ToneIndex < NumHalftones; ++ToneIndex)
        {
            Correlation[ToneIndex] = 0;
            for (size_t SampleIndex = 0; SampleIndex < AnalysisBuffer.size() - Delays[ToneIndex]; ++SampleIndex)
                Correlation[ToneIndex] += std::abs(AnalysisBuffer[SampleIndex] - AnalysisBuffer[SampleIndex + Delays[ToneIndex]]);
            Correlation[ToneIndex] /= (AnalysisBuffer.size() - Delays[ToneIndex]);
        }
        // return average magnitude difference
        return Correlation;
    }


    // Circular Average Magnitude Difference Function (CAMDF) is defined as
    //   D_C(\tau)=\sum_{n=0}^{N-1}|x(mod(n+\tau, N)) - x(n)|
    // where \tau = Delay, n = SampleIndex, N = AnalysisBufferSize, x = AnalysisBuffer
    // See: Equation (4) in http://www.utdallas.edu/~hxb076000/citing_papers/Muhammad%20Extended%20Average%20Magnitude%20Difference.pdf
    TCorrelationArray TCaptureBuffer::CircularAverageMagnitudeDifference()
    {
        TCorrelationArray Correlation = { 0 };
        // accumulate the magnitude differences for samples in AnalysisBuffer
        for (size_t ToneIndex = 0; ToneIndex < NumHalftones; ++ToneIndex)
        {
            Correlation[ToneIndex] = 0;
            for (size_t SampleIndex = 0; SampleIndex < AnalysisBuffer.size(); ++SampleIndex)
                // Suggestion for calculation efficiency improvement from deuteragenie:
                // Replacing "i mod buffersize" by "i & (buffersize-1)" when i is positive and buffersize is a power of two should speed the modulo computation by 5x-10x
                //Correlation[ToneIndex] += Abs(AnalysisBuffer[(SampleIndex+Delays[ToneIndex]) mod AnalysisBufferSize] - AnalysisBuffer[SampleIndex]);
                Correlation[ToneIndex] += std::abs(AnalysisBuffer[(SampleIndex + Delays[ToneIndex]) & (AnalysisBuffer.size() - 1)] - AnalysisBuffer[SampleIndex]);
            Correlation[ToneIndex] /= AnalysisBuffer.size();
        }
        // return circular average magnitude difference
        return Correlation;
    }

    TFrequencyArray TCaptureBuffer::generateFrequencies()
    {
        TFrequencyArray frequencies;
        for (size_t ToneIndex = 0; ToneIndex < NumHalftones; ++ToneIndex)
        {
            // Freq(ToneIndex) = 440 Hz * 2^((ToneIndex-33)/12) --> Freq(ToneIndex=0) = 65.4064 Hz = C2
            frequencies[ToneIndex] = BaseToneFreq * std::pow(2, static_cast<float>(ToneIndex - 33) / 12);
        }
        return frequencies;
    }

    float TCaptureBuffer::MaxSampleVolume()
    {
        std::unique_lock lock(fAnalysisBufferLock);
        uint16_t lMaxVol = 0;
        for (const auto& sample : AnalysisBuffer)
        {
            lMaxVol = std::max<uint16_t>(lMaxVol, std::abs(sample));
        }
        return static_cast<float>(lMaxVol) / -std::numeric_limits<uint16_t>::min();
    }

    const auto ToneStrings = std::to_array({
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        });

    std::string TCaptureBuffer::GetToneString()
    {
        if (ToneValid)
            return ToneStrings[Tone] + std::to_string(ToneAbs / 12 + 2);
        return "-";
    }

    void TCaptureBuffer::BoostBuffer(std::vector<int16_t>& Buffer)
        /*var
            i : int;
    Value:        longint;
    SampleCount:  int;
    SampleBuffer: PSmallIntArray; // buffer handled as array of samples
    Boost:        byte;*/
    {
        uint8_t Boost;
        // TODO: set boost per device
        switch (UIni::Ini.MicBoost)
        {
        case 0:
            Boost = 1;
            break;
        case 1:
            Boost = 2;
            break;
        case 2:
            Boost = 4;
            break;
        case 3:
            Boost = 8;
            break;
        default:
            Boost = 1;
        }

        //TODO:: at the moment we will boost SInt16 data only
        if (fAudioFormat.Format() != UMusic::TAudioSampleFormat::asfS16)
            return;

        // interpret buffer as buffer of bytes

        //SampleBuffer = PSmallIntArray(Buffer);
        //SampleCount = Size div AudioFormat.FrameSize;

        // boost buffer
        for (auto& element : Buffer)
            element = std::clamp<int16_t>(element * Boost,
                std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
    }

    void TCaptureBuffer::StartCapture(UMusic::TAudioFormatInfo Format)
    {
        // free old audio-format info
        //FreeAndNil(fAudioFormat);

        // copy the new input-device audio-format ...
        fAudioFormat = Format.Copy();
        // and adjust it because capture buffers are always mono
        fAudioFormat.Channels(1);

        if (UIni::Ini.VoicePassthrough == 1)
        {
            // TODO: map odd players to the left and even players to the right speaker
            fVoiceStream = UMusic::AudioPlayback->CreateVoiceStream(UMusic::CHANNELMAP_FRONT, fAudioFormat);
        }
    }

    void TCaptureBuffer::StopCapture()
    {
        fVoiceStream.reset();
        //FreeAndNil(fVoiceStream);
    }

    // TAudioInputProcessor
    TAudioInputProcessor::TAudioInputProcessor()
        /*var
            i : int;*/
    {
        //inherited;

        Sound.resize(UIni::IMaxPlayerCount);
        for (auto& s : Sound)
            s = TCaptureBuffer();
    }
    /*
    destructor TAudioInputProcessor::Destroy;
    var
        i : int;
    {
        for i  = 0 to High(Sound) do
            Sound[i].Free;
        SetLength(Sound, 0);
        inherited;
    }*/

    // updates InputDeviceConfig with current input-device information
    // See: TIni.LoadInputDeviceCfg()
    void TAudioInputProcessor::UpdateInputDeviceConfig()
        /*var
            deviceIndex : int;
    newDevice:      bool;
    deviceIniIndex: int;
    deviceCfg:      PInputDeviceConfig;
    device:         TAudioInputDevice;
    channelCount:   int;
    channelIndex:   int;
    i:              int;*/
    {
        // Input devices - append detected soundcards
        for (size_t deviceIndex = 0; deviceIndex < DeviceList.size(); ++deviceIndex)
        {
            bool newDevice = true;
            //Search for Card in List
            for (size_t deviceIniIndex = 0; deviceIniIndex < UIni::Ini.InputDeviceConfig.size(); ++deviceIniIndex)
            {
                auto& deviceCfg = UIni::Ini.InputDeviceConfig[deviceIniIndex];
                auto& device = DeviceList[deviceIndex];

                if (deviceCfg.Name != boost::algorithm::trim_copy(device.Name))
                    continue;
                
				newDevice = false;

				// store highest channel index as an offset for the new channels
				size_t channelIndex = deviceCfg.ChannelToPlayerMap.size() - 1;
				// add missing channels or remove non-existing ones
				deviceCfg.ChannelToPlayerMap.resize(device.AudioFormat.Channels());
				// assign added channels to no player
				for (size_t i = channelIndex + 1; i < deviceCfg.ChannelToPlayerMap.size(); ++i)
				{
					deviceCfg.ChannelToPlayerMap[i] = UIni::CHANNEL_OFF;
				}
				// associate ini-index with device
				device.CfgIndex = deviceIniIndex;
				break;
            }

            //If not in List -> Add
            if (!newDevice)
                continue;

            // resize list
            //UIni::Ini.InputDeviceConfig.resize() Length(Ini.InputDeviceConfig) + 1);
            UIni::Ini.InputDeviceConfig.emplace_back();
            auto& deviceCfg = UIni::Ini.InputDeviceConfig.back();
			auto& device = DeviceList[deviceIndex];

			// associate ini-index with device
			device.CfgIndex = UIni::Ini.InputDeviceConfig.size() - 1;

			deviceCfg.Name = boost::algorithm::trim_copy(device.Name);
			deviceCfg.Input = 0;
			deviceCfg.Latency = UIni::LATENCY_AUTODETECT;

			int channelCount = device.AudioFormat.Channels();
			deviceCfg.ChannelToPlayerMap.resize(channelCount);

            for (size_t channelIndex = 0; channelIndex < channelCount; ++channelIndex)
            {
                // Do not set any default on first start of USDX.
                // Otherwise most probably the wrong device (internal sound card)
                // will be selected.
                // It is better to force the user to configure the mics himself.
                deviceCfg.ChannelToPlayerMap[channelIndex] = UIni::CHANNEL_OFF;
            }
        }
    }

    int TAudioInputProcessor::ValidateSettings()
    {
        // mark all players as unassigned
        std::bitset<UIni::IMaxPlayerCount> PlayerMap = { 0 };

        // iterate over all active devices
        for (const auto& InputDevice : DeviceList)
        {
            const auto& InputDeviceCfg = UIni::Ini.InputDeviceConfig[InputDevice.CfgIndex];
            // iterate over all channels of the current devices
            for (const auto& PlayerID : InputDeviceCfg.ChannelToPlayerMap)
            {
                // get player that was mapped to the current device channel
                if (PlayerID == UIni::CHANNEL_OFF)
                    continue;

                // check if player is already assigned to another device/channel
                if (PlayerMap.test(PlayerID - 1))
                    return PlayerID;

                // mark player as assigned to a device
                PlayerMap.set(PlayerID - 1);
            }
        }
        return 0;
    }

int TAudioInputProcessor::CheckPlayersConfig(uint32_t PlayerCount, TBooleanDynArray& PlayerState)
{
    // set all entries to "not configured"
    PlayerState = { PlayerCount, false };

    // check each used device
    for (const auto& Device : AudioInputProcessor->DeviceList)
    {
        if (!Device)
            continue;
        const auto& DeviceCfg = UIni::Ini.InputDeviceConfig[Device.CfgIndex];

        // check if device is used
        for (const auto& Player : DeviceCfg.ChannelToPlayerMap)
        {
            size_t PlayerIndex = Player - 1;
            if (PlayerIndex >= 0 && PlayerIndex < PlayerCount)
                PlayerState[PlayerIndex] = true;
        }
    }
    for (size_t I = 0; I < PlayerState.size(); ++I)
    {
        if (!PlayerState[I])
			return I + 1;
    }
    return 0;
}

int TAudioInputProcessor::CheckPlayersConfig(uint32_t PlayerCount)
{
    TBooleanDynArray dummy;
    return CheckPlayersConfig(PlayerCount, dummy);
}

/*
    *Handles captured microphone input data.
    * Params:
	* Buffer - buffer of signed 16bit interleaved stereo PCM - samples.
    * Interleaved means that a right - channel sample follows a left -
    * channel sample and vice versa(0:left[0], 1 : right[0], 2 : left[1], ...).
    * Length - number of bytes in Buffer
    * Input - Soundcard - Input used for capture
    */
void TAudioInputProcessor::HandleMicrophoneData(std::vector<int16_t>& Buffer, const TAudioInputDevice& InputDevice)
/*var
MultiChannelBuffer : PByteArray;  // buffer handled as array of bytes (offset relative to channel)
SingleChannelBuffer:     PByteArray;  // temporary buffer for new samples per channel
SingleChannelBufferSize: int;
ChannelIndex:            int;
CaptureChannel:          TCaptureBuffer;
AudioFormat:             TAudioFormatInfo;
SampleSize:              int;
SamplesPerChannel:       int;
i:                       int;*/
{
    const auto& AudioFormat = InputDevice.AudioFormat;
    int SampleSize = UMusic::AudioSampleSize[static_cast<size_t>(AudioFormat.Format())];
    SamplesPerChannel = Buffer.size() / AudioFormat.FrameSize(); //TODO:: not the right way

    SingleChannelBufferSize = SamplesPerChannel * SampleSize;
    GetMem(SingleChannelBuffer, SingleChannelBufferSize);

    // process channels
    for ChannelIndex = 0 to High(InputDevice.CaptureChannel) do
    {
        CaptureChannel = InputDevice.CaptureChannel[ChannelIndex];
        // check if a capture buffer was assigned, otherwise there is nothing to do
        if (CaptureChannel <> nil) then
        {
            // set offset according to channel index
            MultiChannelBuffer = @Buffer[ChannelIndex * SampleSize];
        // separate channel-data from interleaved multi-channel (e.g. stereo) data
        for i = 0 to SamplesPerChannel - 1 do
            {
            Move(MultiChannelBuffer[i * AudioFormat.FrameSize],
                SingleChannelBuffer[i * SampleSize],
                SampleSize);
        }
        CaptureChannel.ProcessNewBuffer(SingleChannelBuffer, SingleChannelBufferSize);
        }
    }

    FreeMem(SingleChannelBuffer);
}

// TAudioInputBase
bool TAudioInputBase::FinalizeRecord()
/*var
i : int;*/
{
    for (i = 0 to High(AudioInputProcessor.DeviceList))
        AudioInputProcessor.DeviceList[i].Free();
    AudioInputProcessor.DeviceList = nil;
    Result = true;
}

/*
    *Start capturing on all used input - device.
    */
void TAudioInputBase::CaptureStart()
/*var
S : int;
DeviceIndex:  int;
ChannelIndex: int;
Device:       TAudioInputDevice;
DeviceCfg:    PInputDeviceConfig;
DeviceUsed:   bool;
Player:       int;*/
{
    if (Started)
        CaptureStop();

    // reset buffers
    for (auto& processor : AudioInputProcessor->Sound)
        processor.Clear;

    // start capturing on each used device
    for (const auto& Device : AudioInputProcessor.DeviceList)
    {
        if (!Device)
            continue;

        const auto& DeviceCfg = UIni::Ini.InputDeviceConfig[Device.CfgIndex];

        bool DeviceUsed = false;

        // check if device is used
        for (size_t ChannelIndex = 0; ChannelIndex < DeviceCfg.ChannelToPlayerMap.size(); ++ChannelIndex)
        {
            int Player = DeviceCfg.ChannelToPlayerMap[ChannelIndex] - 1;
            if (Player < 0 || Player >= PlayersPlay)
            {
                Device.LinkCaptureBuffer(ChannelIndex, nullptr);
            }
            else
            {
                Device.LinkCaptureBuffer(ChannelIndex, AudioInputProcessor.Sound[Player]);
                DeviceUsed = true;
            }
        }

        // start device if used
        if (DeviceUsed)
        {
            //Log.BenchmarkStart(2);
            Device.Start();
			//Log.BenchmarkEnd(2);
			//Log.LogBenchmark("Device.Start", 2) ;
        }
    }
    Started = true;
}

/*
* Stop input - capturing on all soundcards.
*/
void TAudioInputBase::CaptureStop()
/*var
DeviceIndex : int;
ChannelIndex: int;
Device:       TAudioInputDevice;
DeviceCfg:    PInputDeviceConfig;*/
{
    for (const auto& Device : AudioInputProcessor.DeviceList)
    {
        if (!Device)
            continue;

        Device.Stop();

        // disconnect capture buffers
        const auto& DeviceCfg = UIni::Ini.InputDeviceConfig[Device.CfgIndex];
        for (const auto& ChannelIndex : DeviceCfg.ChannelToPlayerMap)
            Device.LinkCaptureBuffer(ChannelIndex, nullptr);
    }
    Started = false;
}

std::string TAudioInputBase::UnifyDeviceName(const std::string name, int deviceIndex)
{
    int count = 1; // count of devices with this name
    auto result = name;

    //TODO:: audioinputprocessor
    auto IsDuplicate = [&deviceIndex](const std::string name)
    {
        // search devices with same description
        for (size_t i = 0; i < deviceIndex; ++i)
        {
            if (!AudioInputProcessor->DeviceList[i])
                continue;

            if (AudioInputProcessor.DeviceList[i].Name == name)
                return true;
        }
        return false;
    };

    // if there is another device with the same ID, search for an available name
    while (IsDuplicate(result))
    {
        ++count;
        // set description
        result = name + " (" + std::to_string(count) + ")";
    }
}
}