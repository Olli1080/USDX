#include "UMusic.h"

namespace UMusic
{

    TAudioFormatInfo::TAudioFormatInfo(uint8_t Channels, double SampleRate, TAudioSampleFormat Format)
    {
        inherited Create();
        fChannels = Channels;
        fSampleRate = SampleRate;
        fFormat = Format;
        UpdateFrameSize();
    }

    void TAudioFormatInfo::SetChannels(uint8_t Channels)
    {
        fChannels = Channels;
        UpdateFrameSize();
    }

    void TAudioFormatInfo::SetFormat(TAudioSampleFormat Format)
    {
        fFormat = Format;
        UpdateFrameSize();
    }

    double TAudioFormatInfo::GetBytesPerSec() const
    {
        return FrameSize() * SampleRate();
    }

    int TAudioFormatInfo::GetSampleSize() const
    {
        return AudioSampleSize[static_cast<size_t>(fFormat)];
    }

    void TAudioFormatInfo::UpdateFrameSize()
    {
        fFrameSize = GetSampleSize() * fChannels;
    }

    TAudioFormatInfo TAudioFormatInfo::Copy()
    {
        return TAudioFormatInfo(Channels(), SampleRate(), Format());
    }

    double TAudioFormatInfo::GetRatio(TAudioFormatInfo TargetInfo)
    {
        return (TargetInfo.FrameSize() / FrameSize()) *
            (TargetInfo.SampleRate() / SampleRate());
    }


    TInterfaceList MediaManager()
    {
        if (!assigned(MediaInterfaceList))
            MediaInterfaceList = TInterfaceList.Create();
        return MediaInterfaceList;
    }

    IVideoPlayback VideoPlayback()
    {
        return DefaultVideoPlayback;
    }

    IVideoPlayback Visualization()
    {
        return DefaultVisualization;
    }

    IAudioPlayback AudioPlayback()
    {
        return DefaultAudioPlayback;
    }

    IAudioInput AudioInput()
    {
        return DefaultAudioInput;
    }

    TInterfaceList AudioDecoders()
    {
        return AudioDecoderList;
    }

    void FilterInterfaceList(const TGUID IID, TInterfaceList InList, TInterfaceList OutList)
        /*var
        i: int;
        obj: IInterface;*/
    {
        if (!assigned(OutList))
            return;

        OutList.Clear;
        for (auto& il : InList)
        {
            if (assigned(il))
            {
                // add object to list if it implements the interface searched for
                if (il.QueryInterface(IID, obj) == 0)
                    OutList.Add(obj);
            }
        }
    }

    void InitializeSound()
        /*var
        i: int;
        InterfaceList: TInterfaceList;
        CurrentAudioDecoder: IAudioDecoder;
        CurrentAudioPlayback: IAudioPlayback;
        CurrentAudioInput: IAudioInput;*/
    {
        // create a temporary list for interface enumeration
        TInterfaceList InterfaceList = TInterfaceList.Create();

        // initialize all audio-decoders first
        FilterInterfaceList(IAudioDecoder, MediaManager, InterfaceList);
        for (auto& il : InterfaceList)
        {
            CurrentAudioDecoder = il as IAudioDecoder;
            if (not CurrentAudioDecoder.InitializeDecoder()) then
            {
                ULog::Log.LogError("Initialize failed, Removing - " + CurrentAudioDecoder.GetName);
              MediaManager.Remove(CurrentAudioDecoder);
            }
        }

        // create and setup decoder-list (see AudioDecoders())
        AudioDecoderList = TInterfaceList.Create;
        FilterInterfaceList(IAudioDecoder, MediaManager, AudioDecoders);

        // find and initialize playback interface
        DefaultAudioPlayback = nil;
        FilterInterfaceList(IAudioPlayback, MediaManager, InterfaceList);
        for i : = 0 to InterfaceList.Count - 1 do
        {
            CurrentAudioPlayback = InterfaceList[i] as IAudioPlayback;
            if (CurrentAudioPlayback.InitializePlayback()) then
            {
              DefaultAudioPlayback = CurrentAudioPlayback;
              break;
            }
            Log.LogError("Initialize failed, Removing - " + CurrentAudioPlayback.GetName);
            MediaManager.Remove(CurrentAudioPlayback);
        }

        // find and initialize input interface
        DefaultAudioInput = nil;
        FilterInterfaceList(IAudioInput, MediaManager, InterfaceList);
        for i : = 0 to InterfaceList.Count - 1 do
        {
            CurrentAudioInput = InterfaceList[i] as IAudioInput;
            if (CurrentAudioInput.InitializeRecord()) then
            {
              DefaultAudioInput = CurrentAudioInput;
              break;
            }
            Log.LogError("Initialize failed, Removing - " + CurrentAudioInput.GetName);
            MediaManager.Remove(CurrentAudioInput);
        }

        InterfaceList.Free;

        // Update input-device list with registered devices
        AudioInputProcessor.UpdateInputDeviceConfig();

        // Load in-game sounds
        SoundLib = TSoundLibrary::Create;
    }

    void InitializeVideo()
        /*var
        i: int;
        InterfaceList: TInterfaceList;
        VideoInterface: IVideoPlayback;
        VisualInterface: IVideoVisualization;*/
    {
        InterfaceList = TInterfaceList.Create;

        // initialize and set video-playback singleton
        DefaultVideoPlayback = nil;
        FilterInterfaceList(IVideoPlayback, MediaManager, InterfaceList);
        for i : = 0 to InterfaceList.Count - 1 do
        {
            VideoInterface = InterfaceList[i] as IVideoPlayback;
            if (VideoInterface.Init()) then
            {
              DefaultVideoPlayback = VideoInterface;
              break;
            }
            Log.LogError("Initialize failed, Removing - " + VideoInterface.GetName);
            MediaManager.Remove(VideoInterface);
        }

        // initialize and set visualization singleton
        DefaultVisualization = nil;
        FilterInterfaceList(IVideoVisualization, MediaManager, InterfaceList);
        for i : = 0 to InterfaceList.Count - 1 do
        {
            VisualInterface = InterfaceList[i] as IVideoVisualization;
            if (VisualInterface.Init()) then
            {
              DefaultVisualization = VisualInterface;
              break;
            }
            Log.LogError("Initialize failed, Removing - " + VisualInterface.GetName);
            MediaManager.Remove(VisualInterface);
        }

        InterfaceList.Free;

        // now that we have all interfaces, we can dump them
        // TODO: move this to another place
        if FindCmdLineSwitch(cMediaInterfaces) then
        {
            DumpMediaInterfaces();
            halt;
        }
    }

    void UnloadMediaModules()
        /*var
        i: int;
        InterfaceList: TInterfaceList;*/
    {
        FreeAndNil(AudioDecoderList);
        DefaultAudioPlayback = nil;
        DefaultAudioInput = nil;
        DefaultVideoPlayback = nil;
        DefaultVisualization = nil;

        // create temporary interface list
        InterfaceList = TInterfaceList.Create();

        // finalize audio playback interfaces (should be done before the decoders)
        FilterInterfaceList(IAudioPlayback, MediaManager, InterfaceList);
        for i : = 0 to InterfaceList.Count - 1 do
            (InterfaceList[i] as IAudioPlayback).FinalizePlayback();

        // finalize audio input interfaces
        FilterInterfaceList(IAudioInput, MediaManager, InterfaceList);
        for i : = 0 to InterfaceList.Count - 1 do
            (InterfaceList[i] as IAudioInput).FinalizeRecord();

        // finalize audio decoder interfaces
        FilterInterfaceList(IAudioDecoder, MediaManager, InterfaceList);
        for i : = 0 to InterfaceList.Count - 1 do
            (InterfaceList[i] as IAudioDecoder).FinalizeDecoder();

        // finalize video interfaces
        FilterInterfaceList(IVideoPlayback, MediaManager, InterfaceList);
        for i : = 0 to InterfaceList.Count - 1 do
            (InterfaceList[i] as IVideoPlayback).Finalize();

        // finalize audio decoder interfaces
        FilterInterfaceList(IVideoVisualization, MediaManager, InterfaceList);
        for i : = 0 to InterfaceList.Count - 1 do
            (InterfaceList[i] as IVideoVisualization).Finalize();

        InterfaceList.Free;

        // finally free interfaces (by removing all references to them)
        FreeAndNil(MediaInterfaceList);
    }

    void FinalizeMedia()
    {
        // stop, close and free sounds
        SoundLib.Free;

        // release webcam
        Webcam.Free;

        // stop and close music stream
        if (AudioPlayback != nil)
            AudioPlayback.Close;

        // stop any active captures
        if (AudioInput != nil)
            AudioInput.CaptureStop;

        UnloadMediaModules();
    }

    void DumpMediaInterfaces()
    {
        std::cout << "" << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "  In-use Media Interfaces                                     " << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Registered Audio Playback Interface : " + AudioPlayback.GetName << std::endl;
        std::cout << "Registered Audio Input    Interface : " + AudioInput.GetName << std::endl;
        std::cout << "Registered Video Playback Interface : " + VideoPlayback.GetName << std::endl;
        std::cout << "Registered Visualization  Interface : " + Visualization.GetName << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "" << std::endl;
    }


    //{ TSoundLibrary }

    TSoundLibrary::TSoundLibrary()
    {
        inherited;
        LoadSounds();
    }

    TSoundLibrary::~TSoundLibrary()
    {
        UnloadSounds();
        inherited;
    }

    void TSoundLibrary::LoadSounds()
    {
        UnloadSounds();

        Start = AudioPlayback.OpenSound(SoundPath.Append("Common start.mp3"));
        Back = AudioPlayback.OpenSound(SoundPath.Append("Common back.mp3"));
        Swoosh = AudioPlayback.OpenSound(SoundPath.Append("menu swoosh.mp3"));
        Change = AudioPlayback.OpenSound(SoundPath.Append("select music change music 50.mp3"));
        Option = AudioPlayback.OpenSound(SoundPath.Append("option change col.mp3"));
        Click = AudioPlayback.OpenSound(SoundPath.Append("rimshot022b.mp3"));
        Applause = AudioPlayback.OpenSound(SoundPath.Append("Applause.mp3"));

        BGMusic = AudioPlayback.OpenSound(SoundPath.Append("background track.mp3"));

        if (BGMusic != nil)
            BGMusic.Loop = True;
    }

    void TSoundLibrary::UnloadSounds()
    {
        FreeAndNil(Start);
        FreeAndNil(Back);
        FreeAndNil(Swoosh);
        FreeAndNil(Change);
        FreeAndNil(Option);
        FreeAndNil(Click);
        FreeAndNil(Applause);
        FreeAndNil(BGMusic);
    }

    /* TODO
    function TSoundLibrary::GetSound(ID: int): TAudioPlaybackStream;
    {
    if ((ID >= 0) and (ID < Length(Sounds))) then
    Result = Sounds[ID]
    else
    Result = nil;
    }
    */

    void TSoundLibrary::StartBgMusic()
    {
        if (TBackgroundMusicOption(Ini.BackgroundMusicOption) == bmoOn) &&
            (Soundlib.BGMusic != nil) and not (Soundlib.BGMusic.Status == ssPlaying)
        {
            AudioPlayback.PlaySound(Soundlib.BGMusic);
        }
    }

    void TSoundLibrary::PauseBgMusic()
    {
        if (Soundlib.BGMusic != nil)
        {
            Soundlib.BGMusic.Pause;
        }
    }

    //{ TVoiceRemoval }

    void TVoiceRemoval::Callback(PByteArray Buffer, int BufSize)
        /*var
        FrameIndex, FrameSize: int;
        Value: int;
        Sample: PPCMStereoSample;*/
    {
        FrameSize = 2 * sizeof(int16_t);
        for FrameIndex : = 0 to(BufSize div FrameSize) - 1 do
        {
            Sample = PPCMStereoSample(Buffer);
            // channel difference
            Value = Sample[0] - Sample[1];
            // clip
            if (Value > High(int16_t)) then
                Value = High(int16_t)
            else if (Value < Low(int16_t)) then
                Value = Low(int16_t);
            // assign result
            Sample[0] : = Value;
            Sample[1] : = Value;
            // increase to next frame
            Inc(PByte(Buffer), FrameSize);
        }
    }

    //{ TAudioConverter }

    bool TAudioConverter::Init(TAudioFormatInfo SrcFormatInfo, TAudioFormatInfo DstFormatInfo)
    {
        fSrcFormatInfo = SrcFormatInfo.Copy();
        fDstFormatInfo = DstFormatInfo.Copy();
        return true;
    }

    TAudioConverter::~TAudioConverter()
    {
        FreeAndNil(fSrcFormatInfo);
        FreeAndNil(fDstFormatInfo);
    }


    //{ TAudioProcessingStream }

    void TAudioProcessingStream::AddOnCloseHandler(TOnCloseHandler Handler)
    {
        if (@Handler != nil)
        {
            SetLength(OnCloseHandlers, System.Length(OnCloseHandlers) + 1);
            OnCloseHandlers[High(OnCloseHandlers)] : = @Handler;
        }
    }

    void TAudioProcessingStream::PerformOnClose()
        //var i: int;
    {
        for (size_t i = 0; i < OnCloseHandlers.size(); ++i)
        {
            OnCloseHandlers[i](Self);
        }
    }


    //{ TAudioPlaybackStream }

    TAudioSourceStream TAudioPlaybackStream::GetSourceStream()
    {
        return SourceStream;
    }

    void TAudioPlaybackStream::SetSyncSource(TSyncSource SyncSource)
    {
        Self.SyncSource = SyncSource;
        AvgSyncDiff = -1;
    }

    //{.$DEFINE LOG_SYNC}

    /*
    * Results an adjusted size of the input buffer size to keep the stream in sync
    * with the SyncSource. If no SyncSource was assigned to this stream, the
    * input buffer size will be returned, so this method will have no effect.
    *
    * These are the possible cases:
    *   - Result > BufferSize: stream is behind the sync-source (stream is too slow),
    *                          (Result-BufferSize) bytes of the buffer must be skipped.
    *   - Result = BufferSize: stream is in sync,
    *                          there is nothing to do.
    *   - Result < BufferSize: stream is ahead of the sync-source (stream is too fast),
    *                          (BufferSize-Result) bytes of the buffer must be padded.
    */
    int TAudioPlaybackStream::Synchronize(int BufferSize, TAudioFormatInfo FormatInfo)
        /*var
        TimeDiff: double;
        FrameDiff: double;
        FrameSkip: int;
        ReqFrames: int;
        MasterClock: double;
        CurPosition: double;*/
    {
        constexpr double AVG_HISTORY_FACTOR = 0.7;
        constexpr double SYNC_REPOS_THRESHOLD = 5.000;
        constexpr double SYNC_SOFT_THRESHOLD = 0.010;


        int Result = BufferSize;

        if (!assigned(SyncSource))
            return Result;

        if (BufferSize <= 0)
            return Result;

        auto CurPosition = Position();
        auto MasterClock = SyncSource.GetClock();

        // difference between sync-source and stream position
        // (negative if the music-stream's position is ahead of the master clock)
        auto TimeDiff = MasterClock - CurPosition;

        // calculate average time difference (some sort of weighted mean).
        // The bigger AVG_HISTORY_FACTOR is, the smoother is the average diff.
        // This means that older diffs are weighted more with a higher history factor
        // than with a lower. Do not use a too low history factor. FFmpeg produces
        // very instable timestamps (pts) for ogg due to some bugs. They may differ
        // +-50ms from the double stream position. Without filtering those glitches we
        // would synch without any need, resulting in ugly plopping sounds.
        if (AvgSyncDiff == -1)
            AvgSyncDiff = TimeDiff;
        else
            AvgSyncDiff = TimeDiff * (1 - AVG_HISTORY_FACTOR) +
            AvgSyncDiff * AVG_HISTORY_FACTOR;

#ifdef LOG_SYNC
        //Log.LogError(Format("c:%.3f | p:%.3f | d:%.3f | a:%.3f",
        //    [MasterClock, CurPosition, TimeDiff, AvgSyncDiff]), "Synch");
#endif

  // check if we are out of sync
        if (std::abs(AvgSyncDiff) >= SYNC_REPOS_THRESHOLD)
        {
#ifdef LOG_SYNC
            Log.LogError(Format("ReposSynch: %.3f > %.3f",
                [Abs(AvgSyncDiff), SYNC_REPOS_THRESHOLD]), "Synch");
#endif

            // diff far is too large -> reposition stream
            // (resulting position might still be out of sync)
            SetPosition(CurPosition + AvgSyncDiff);

            // reset sync info
            AvgSyncDiff = -1;
        }
        else if (std::abs(AvgSyncDiff) >= SYNC_SOFT_THRESHOLD)
        {
#ifdef LOG_SYNC
            Log.LogError(Format("SoftSynch: %.3f > %.3f",
                [Abs(AvgSyncDiff), SYNC_SOFT_THRESHOLD]), "Synch");
#endif

            // hard sync: directly jump to the current position
            const int FrameSkip = std::round(AvgSyncDiff * FormatInfo.SampleRate());
            Result = BufferSize + FrameSkip * FormatInfo.FrameSize();
            if (Result < 0)
                Result = 0;

            // reset sync info
            AvgSyncDiff = -1;
        }
        return Result;
    }

    /*
    *Fills a buffer with copies of the given Frame or with 0 if Frame is nil.
    */
    void TAudioPlaybackStream::FillBufferWithFrame(PByteArray Buffer, int BufferSize, PByteArray Frame, int FrameSize)
        /*var
        i : int;
        FrameCopyCount: int;*/
    {
        // the buffer must at least contain place for one copy of the frame.
        if (Buffer == nil || BufferSize <= 0 || BufferSize < FrameSize)
            return;

        // no valid frame -> fill with 0
        if ((Frame = nil) or (FrameSize <= 0))
        {
            FillChar(Buffer[0], BufferSize, 0);
            return;
        }

        // number of frames to copy
        FrameCopyCount = BufferSize div FrameSize;
        // insert as many copies of frame into the buffer as possible
        for (i = 0 to FrameCopyCount - 1)
            Move(Frame[0], Buffer[i * FrameSize], FrameSize);
    }

    //{ TAudioVoiceStream }

    bool TAudioVoiceStream::Open(int ChannelMap, TAudioFormatInfo FormatInfo)
    {
        this->ChannelMap = ChannelMap;
        this->FormatInfo = FormatInfo.Copy();
        // a voice stream is always mono, reassure the the format is correct
        this->FormatInfo.Channels = 1;
        return true;
    }

    TAudioVoiceStream::~TAudioVoiceStream()
    {
        Close();
        inherited;
    }

    void TAudioVoiceStream::Close();
    {
        PerformOnClose();
        FreeAndNil(FormatInfo);
    }

    TAudioFormatInfo TAudioVoiceStream::GetAudioFormatInfo()
    {
        return FormatInfo;
    }

    double TAudioVoiceStream::GetLength()
    {
        return  -1;
    }

    double TAudioVoiceStream::GetPosition()
    {
        return -1;
    }

    void TAudioVoiceStream::SetPosition(double Time)
    {
    }

    bool TAudioVoiceStream::GetLoop()
    {
        return false;
    }

    void TAudioVoiceStream::SetLoop(bool Enabled)
    {
    }

    //{ TLineFragment }

    int TLineFragment::GetEnd() const
    {
        return StartBeat + Duration;
    }

    //{ TLine }

    bool TLine::HasLength()
        //var tempi : int;
    {
        //TODO:: dunno why?!?
        return HasLength(tempi);
    }

    bool TLine::HasLength(int& Len)
    {
        if (Length(Notes) >= 0)
        {
            Len = EndBeat - Notes[0].StartBeat;
            return true;
        }
        return false;
    }

    bool TLine::HasLength(double& Len)
        //var tempi : int;
    {
        Result = HasLength(tempi);
        Len = tempi;
    }

    bool TLine::HasLength(double& Len)
        //var tempi : int;
    {
        Result = HasLength(tempi);
        Len = tempi;
    }

    int TLine::GetLength() const
    {
        return (Length(Notes) < 0) ? 0 : EndBeat - Notes[0].StartBeat;
    }

    TPos FindNote(int beat)
        /*var
          LineIndex : int;
        NoteIndex: int;
        found:     bool;
        min:       int;
        diff:      int;*/
    {
        bool found = false;
        for (size_t LineIndex = 0; LineIndex < Tracks[0].Lines.size(); ++LineIndex)
        {
            const auto& line = Tracks[0].Lines[LineIndex];
            for (size_t NoteIndex = 0; NoteIndex < line.Notes.size(); ++NoteIndex)
            {
                const auto& note = line.Notes[NoteIndex];
                if (beat >= note.StartBeat &&
                    beat <= note.StartBeat + note.Duration)
                {
                    return
                    {
                        0,
                        LineIndex,
                        NoteIndex
                    };
                }
            }
        }

        if (CurrentSong.isDuet && PlayersPlay != 1)
        {
            for LineIndex : = 0 to High(Tracks[1].Lines) do
            {
                for NoteIndex : = 0 to High(Tracks[1].Lines[LineIndex].Notes) do
                {
                    if (beat >= Tracks[1].Lines[LineIndex].Notes[NoteIndex].StartBeat) and
                        (beat <= Tracks[1].Lines[LineIndex].Notes[NoteIndex].StartBeat + Tracks[1].Lines[LineIndex].Notes[NoteIndex].Duration) then
                    {
                        Result.track = 1;
                        Result.line = LineIndex;
                        Result.note = NoteIndex;
                        found = true;
                        break;
                    }
                }
            }
        }

        if (found) //found exactly
            return;

        min = high(int);
        //second try (approximating)
        for LineIndex : = 0 to High(Tracks[0].Lines) do
        {
            for NoteIndex : = 0 to High(Tracks[0].Lines[LineIndex].Notes) do
            {
                diff = abs(Tracks[0].Lines[LineIndex].Notes[NoteIndex].StartBeat - beat);
                if (diff < min)
                {
                    Result.track = 0;
                    Result.line = LineIndex;
                    Result.note = NoteIndex;
                    min = diff;
                }
            }
        }

        if (CurrentSong.isDuet && PlayersPlay != 1)
        {
            for LineIndex : = 0 to High(Tracks[1].Lines) do
            {
                for NoteIndex : = 0 to High(Tracks[1].Lines[LineIndex].Notes) do
                {
                    diff = abs(Tracks[1].Lines[LineIndex].Notes[NoteIndex].StartBeat - beat);
                    if diff < min then
                    {
                      Result.track = 1;
                      Result.line = LineIndex;
                      Result.note = NoteIndex;
                      min = diff;
                    }
                }
            }
        }
    }
}