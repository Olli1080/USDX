#include "URecord.h"

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
        SDL_UnlockMutex(fAnalysisBufferLock);
        SDL_DestroyMutex(fAnalysisBufferLock);
        fAnalysisBufferLock = nil;
        inherited;
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
        if (SampleCount > AnalysisBuffer.size())
        {
            // discard the oldest of the new samples
            AnalysisBuffer = { Buffer.end() - AnalysisBuffer.size(), Buffer.end() };
        }
        else
        {
            std::unique_lock lock(fAnalysisBufferLock);

            // move old samples to the beginning of the array (if necessary)
            AnalysisBuffer.erase(AnalysisBuffer.begin(), AnalysisBuffer.begin() + SampleCount);

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
        float MaxVolume = 0;
        for (size_t SampleIndex = 0; SampleIndex < 1024; ++SampleIndex)
        {
            float Volume = std::abs<float>(AnalysisBuffer[SampleIndex]) / static_cast<float>(-std::numeric_limits<int16_t>::min());
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

    void TCaptureBuffer::SetFrequenciesAndDelays()
    {
        for (size_t ToneIndex = 0; ToneIndex < NumHalftones; ++ToneIndex)
        {
            // Freq(ToneIndex) = 440 Hz * 2^((ToneIndex-33)/12) --> Freq(ToneIndex=0) = 65.4064 Hz = C2
            Frequencies[ToneIndex] = BaseToneFreq * std::pow(2, (ToneIndex - 33) / 12);
            Delays[ToneIndex] = std::round(fAudioFormat.SampleRate() / Frequencies[ToneIndex]);
        }
    }

    bool TCaptureBuffer::AnalyzePitch(TPDAType PDA)
    {
        // prepare to analyze
        SetFrequenciesAndDelays();

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
        /*var
            ToneIndex : int;
    Correlation: TCorrelationArray;
    SampleIndex: int; // index of sample to analyze*/
    {
        // accumulate the magnitude differences for samples in AnalysisBuffer
        for (size_t ToneIndex = 0; ToneIndex < NumHalftones; ++ToneIndex)
        {
            Correlation[ToneIndex] = 0;
            for (const auto& sample : AnalysisBuffer)
                for SampleIndex = 0 to(AnalysisBufferSize - Delays[ToneIndex] - 1) do
                {
                    Correlation[ToneIndex] = Correlation[ToneIndex] + std::abs(AnalysisBuffer[SampleIndex] - AnalysisBuffer[SampleIndex + Delays[ToneIndex]]);
                }
            Correlation[ToneIndex] = Correlation[ToneIndex] / (AnalysisBufferSize - Delays[ToneIndex] - 1);
        }
        // return average magnitude difference
        Result = Correlation;
    }


    // Circular Average Magnitude Difference Function (CAMDF) is defined as
    //   D_C(\tau)=\sum_{n=0}^{N-1}|x(mod(n+\tau, N)) - x(n)|
    // where \tau = Delay, n = SampleIndex, N = AnalysisBufferSize, x = AnalysisBuffer
    // See: Equation (4) in http://www.utdallas.edu/~hxb076000/citing_papers/Muhammad%20Extended%20Average%20Magnitude%20Difference.pdf
    TCorrelationArray TCaptureBuffer::CircularAverageMagnitudeDifference()
        /*var
            ToneIndex : int;
    Correlation: TCorrelationArray;
    SampleIndex: int; // index of sample to analyze*/
    {
        // accumulate the magnitude differences for samples in AnalysisBuffer
        for (ToneIndex = 0 to NumHalftones - 1)
        {
            Correlation[ToneIndex] = 0;
            for (SampleIndex = 0 to(AnalysisBufferSize - 1))
            {
                // Suggestion for calculation efficiency improvement from deuteragenie:
                // Replacing "i mod buffersize" by "i & (buffersize-1)" when i is positive and buffersize is a power of two should speed the modulo computation by 5x-10x
                //Correlation[ToneIndex] += Abs(AnalysisBuffer[(SampleIndex+Delays[ToneIndex]) mod AnalysisBufferSize] - AnalysisBuffer[SampleIndex]);
                Correlation[ToneIndex] = Correlation[ToneIndex] + Abs(AnalysisBuffer[(SampleIndex + Delays[ToneIndex]) and (AnalysisBufferSize - 1)] - AnalysisBuffer[SampleIndex]);
            }
            Correlation[ToneIndex] = Correlation[ToneIndex] / AnalysisBufferSize;
        }

        // return circular average magnitude difference
        Result = Correlation;
    }

    float TCaptureBuffer::MaxSampleVolume()
        /*var
            lSampleIndex : int;
    lMaxVol:      longint;*/
    {
        std::unique_lock lock(fAnalysisBufferLock);
        lMaxVol = 0;
        for (lSampleIndex = 0 to High(AnalysisBuffer))
        {
            if (Abs(AnalysisBuffer[lSampleIndex]) > lMaxVol)
                lMaxVol = Abs(AnalysisBuffer[lSampleIndex]);
        }
        result = lMaxVol / -Low(Smallint);
    }

    const auto ToneStrings = std::to_array({
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        });

    std::string TCaptureBuffer::GetToneString()
    {
        if (ToneValid)
            return ToneStrings[Tone] + std::to_string(ToneAbs / 12 + 2);
        else
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
        fAudioFormat.Channels = 1;

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
        SetLength(Sound, UIni::IMaxPlayerCount);
        for i = 0 to High(Sound) do
            Sound[i] = TCaptureBuffer::Create;
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
        for deviceIndex = 0 to High(DeviceList) do
        {
            newDevice = true;
            //Search for Card in List
            for deviceIniIndex = 0 to High(Ini.InputDeviceConfig) do
            {
                deviceCfg = @Ini.InputDeviceConfig[deviceIniIndex];
                device = DeviceList[deviceIndex];

                if (deviceCfg.Name = Trim(device.Name)) then
                {
                newDevice = false;

                // store highest channel index as an offset for the new channels
            channelIndex = High(deviceCfg.ChannelToPlayerMap);
            // add missing channels or remove non-existing ones
            SetLength(deviceCfg.ChannelToPlayerMap, device.AudioFormat.Channels);
            // assign added channels to no player
            for i = channelIndex + 1 to High(deviceCfg.ChannelToPlayerMap) do
                {
                deviceCfg.ChannelToPlayerMap[i] = CHANNEL_OFF;
            }

            // associate ini-index with device
            device.CfgIndex = deviceIniIndex;
            break;
                }
            }

            //If not in List -> Add
            if newDevice then
            {
                // resize list
                SetLength(Ini.InputDeviceConfig, Length(Ini.InputDeviceConfig) + 1);
        deviceCfg = @Ini.InputDeviceConfig[High(Ini.InputDeviceConfig)];
        device = DeviceList[deviceIndex];

        // associate ini-index with device
        device.CfgIndex = High(Ini.InputDeviceConfig);

        deviceCfg.Name = Trim(device.Name);
        deviceCfg.Input = 0;
        deviceCfg.Latency = LATENCY_AUTODETECT;

    channelCount = device.AudioFormat.Channels;
        SetLength(deviceCfg.ChannelToPlayerMap, channelCount);

        for channelIndex = 0 to channelCount - 1 do
            {
            // Do not set any default on first start of USDX.
            // Otherwise most probably the wrong device (internal sound card)
            // will be selected.
            // It is better to force the user to configure the mics himself.
            deviceCfg.ChannelToPlayerMap[channelIndex] = CHANNEL_OFF;
        }
            }
        }
    }

    function TAudioInputProcessor::ValidateSettings: int;
    var
        I, J: int;
PlayerID: int;
PlayerMap: array[0 ..UIni.IMaxPlayerCount - 1] of bool;
InputDevice: TAudioInputDevice;
InputDeviceCfg: PInputDeviceConfig;
{
    // mark all players as unassigned
    for I = 0 to High(PlayerMap) do
        PlayerMap[I] = false;

    // iterate over all active devices
    for I = 0 to High(DeviceList) do
    {
        InputDevice = DeviceList[I];
        InputDeviceCfg = @Ini.InputDeviceConfig[InputDevice.CfgIndex];
        // iterate over all channels of the current devices
        for J = 0 to High(InputDeviceCfg.ChannelToPlayerMap) do
        {
            // get player that was mapped to the current device channel
            PlayerID = InputDeviceCfg.ChannelToPlayerMap[J];
            if (PlayerID <> CHANNEL_OFF) then
            {
                // check if player is already assigned to another device/channel
                if (PlayerMap[PlayerID - 1]) then
                    {
                    Result = PlayerID;
            Exit;
            }

                    // mark player as assigned to a device
                    PlayerMap[PlayerID - 1] = true;
            }
        }
    }
    Result = 0;
}

function TAudioInputProcessor::CheckPlayersConfig(PlayerCount: cardinal;
var PlayerState : TBooleanDynArray) : int;
var
DeviceIndex : int;
ChannelIndex: int;
Device:       TAudioInputDevice;
DeviceCfg:    PInputDeviceConfig;
PlayerIndex:  int;
I: int;
{
    SetLength(PlayerState, PlayerCount);
    // set all entries to "not configured"
    for I = 0 to High(PlayerState) do
    {
        PlayerState[I] = false;
    }

    // check each used device
    for DeviceIndex = 0 to High(AudioInputProcessor.DeviceList) do
    {
        Device = AudioInputProcessor.DeviceList[DeviceIndex];
        if not assigned(Device) then
            continue;
        DeviceCfg = @Ini.InputDeviceConfig[Device.CfgIndex];

        // check if device is used
        for ChannelIndex = 0 to High(DeviceCfg.ChannelToPlayerMap) do
        {
            PlayerIndex = DeviceCfg.ChannelToPlayerMap[ChannelIndex] - 1;
            if (PlayerIndex >= 0) and (PlayerIndex < PlayerCount) then
                PlayerState[PlayerIndex] = true;
        }
    }

    Result = 0;
    for I = 0 to High(PlayerState) do
    {
        if (PlayerState[I] = false) then
        {
        Result = I + 1;
Break;
        }
    }
}

int TAudioInputProcessor::CheckPlayersConfig(uint32_t PlayerCount)
/*var
PlayerState : TBooleanDynArray;*/
{
    return CheckPlayersConfig(PlayerCount, PlayerState);
}

/*
    *Handles captured microphone input data.
    * Params:
*Buffer - buffer of signed 16bit interleaved stereo PCM - samples.
    * Interleaved means that a right - channel sample follows a left -
    *channel sample and vice versa(0:left[0], 1 : right[0], 2 : left[1], ...).
    * Length - number of bytes in Buffer
    * Input - Soundcard - Input used for capture
    */
void TAudioInputProcessor::HandleMicrophoneData(std::vector<int16_t>& Buffer, TAudioInputDevice::SPtr InputDevice)
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
    AudioFormat = InputDevice.AudioFormat;
    SampleSize = UMusic::AudioSampleSize[AudioFormat.Format];
    SamplesPerChannel = Size div AudioFormat.FrameSize;

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
    for (S = 0 to High(AudioInputProcessor.Sound))
        AudioInputProcessor.Sound[S].Clear;

    // start capturing on each used device
    for (DeviceIndex = 0 to High(AudioInputProcessor.DeviceList))
    {
        Device = AudioInputProcessor.DeviceList[DeviceIndex];
        if not assigned(Device) then
            continue;
        DeviceCfg = @Ini.InputDeviceConfig[Device.CfgIndex];

        DeviceUsed = false;

        // check if device is used
        for (ChannelIndex = 0 to High(DeviceCfg.ChannelToPlayerMap))
        {
            Player = DeviceCfg.ChannelToPlayerMap[ChannelIndex] - 1;
            if (Player < 0) or (Player >= PlayersPlay) then
            {
                Device.LinkCaptureBuffer(ChannelIndex, nil);
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
    *Stop input - capturing on all soundcards.
    */
void TAudioInputBase::CaptureStop()
/*var
DeviceIndex : int;
ChannelIndex: int;
Device:       TAudioInputDevice;
DeviceCfg:    PInputDeviceConfig;*/
{
    for (DeviceIndex = 0 to High(AudioInputProcessor.DeviceList))
    {
        auto Device = AudioInputProcessor.DeviceList[DeviceIndex];
        if (!Device)
            continue;

        Device.Stop();

        // disconnect capture buffers
        DeviceCfg = @Ini.InputDeviceConfig[Device.CfgIndex];
        for (ChannelIndex = 0 to High(DeviceCfg.ChannelToPlayerMap))
            Device.LinkCaptureBuffer(ChannelIndex, nil);
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