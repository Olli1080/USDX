#pragma once
/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/UMusic.pas $
 * $Id: UMusic.pas 3103 2014-11-22 23:21:19Z k-m_schindler $
 */

#include "../switches.h"

#include <array>
#include <vector>
#include <memory>
#include <cstdint>
#include <functional>
#include <filesystem>
#include <iostream>
#include <optional>

#include <boost/signals2.hpp>

#include "ULog.h"
#include "UTime.h"
#include "UBeatTimer.h"

namespace UMusic
{
    typedef std::unique_ptr<void, std::function<void(void*)>> EngineDataPtr;

    enum TNoteType
    {
        ntFreestyle, ntNormal, ntGolden, ntRap, ntRapGolden, Size
    };

    struct TPos // Tracks[track].Lines[line].Notes[note]
    {
        size_t track;
        size_t line;
        size_t note;
    };

    /**
       * acoStretch: Stretch to screen width and height
       *   - ignores aspect
       *   + no borders
       *   + no image data loss
       * acoCrop: Stretch to screen width or height, crop the other dimension
       *   + keeps aspect
       *   + no borders
       *   - frame borders are cropped (image data loss)
       * acoLetterBox: Stretch to screen width, add bars at or crop top and bottom
       *   + keeps aspect
       *   - borders at top and bottom
       *   o top/bottom is cropped if width < height (unusual)
       */

    enum TAspectCorrection
    {
        acoStretch, acoCrop, acoLetterBox
    };

    struct TRectCoords
    {
        double Left, Right;
        double Upper, Lower;
    };

    /*
    uses
      SysUtils,
      Classes,
      UTime,
      UBeatTimer,
      UPath,
      UWebcam;
    */


    // ScoreFactor defines how a notehit of a specified notetype is
    // measured in comparison to the other types
    // 0 means this notetype is not rated at all
    // 2 means a hit of this notetype will be rated w/ twice as much
    // points as a hit of a notetype w/ ScoreFactor 1
    const std::array<int, TNoteType::Size> ScoreFactor = { 0, 1, 2, 1, 2 };

    /**
     * TLineFragment represents a fragment of a lyrics line.
     * This is a text-fragment (e.g. a syllable) assigned to a note pitch,
     * represented by a bar in the sing-screen.
     */

    struct TLineFragment
    {
        int Color;
        int StartBeat;    // beat the fragment starts at
        int Duration;    // duration in beats
        int Tone;    // full range tone
        std::string Text; // text assigned to this fragment (a syllable, word, etc.)
        TNoteType NoteType;  // note-type: golden-note/freestyle etc.

        bool IsMedley; // just for editor
        bool IsStartPreview; // just for editor

    private:
        [[nodiscard]] int GetEnd() const;

    public:
        [[nodiscard]] int EndBeat() const { return GetEnd(); }
    };

    typedef std::shared_ptr<TLineFragment> PLineFragment;

    /**
    * TLine represents one lyrics line and consists of multiple
    * notes.
    */
    struct TLine
    {
        int StartBeat; // the start beat of this line (!= start beat of the first note of this line)
        std::string Lyric;
        //LyricWidth: double;    // @deprecated: width of the line in pixels.
                             // Do not use this as the width is not correct.
                             // Use TLyricsEngine.GetUpperLine().Width instead.
        int EndBeat;
        int BaseNote;
        int HighNote; // index of last note in line (= High(Note)?)
        int ScoreValue; // value of all notes in the line
        bool LastLine;
        std::vector<TLineFragment> Notes;

    private:

        

    public:

        [[nodiscard]] std::optional<int> GetLength() const;
        // Returns whether the line has a valid length. }
        /*[[nodiscard]] bool HasLength() const;
        // Returns whether the line has a valid length and passes length. }
        [[nodiscard]] bool HasLength(int& Len) const;
        // Returns whether the line has a valid length and passes length.Output converted to Double }
        [[nodiscard]] bool HasLength(double& Len) const;

        [[nodiscard]] int Length_() const { return GetLength(); }*/
    };

    typedef std::shared_ptr<TLine> PLine;

    /**
    * TLines stores sets of lyric lines and information on them.
    * Normally just one set is defined but in duet mode it might for example
    * contain two sets.
    */
    struct TLines
    {
        int CurrentLine;  // for drawing of current line
        //int High;  // = High(Line)!
        int Number;
        //int Resolution;
        //int NotesGAP;
        int Rel = 0;
        int ScoreValue;
        std::vector<TLine> Lines;
    };
    constexpr int FFTSize = 512; // size of FFT data (output: FFTSize/2 values)

    typedef std::array<float, FFTSize / 2> TFFTData;
    typedef std::array<int16_t, 2> TPCMStereoSample;
    typedef std::shared_ptr<TPCMStereoSample> PPCMStereoSample;
    typedef std::array<TPCMStereoSample, 512> TPCMData;

    enum class TStreamStatus { ssStopped, ssPlaying, ssPaused, Size };
    const std::array<std::string, static_cast<size_t>(TStreamStatus::Size)> StreamStatusStr = { "Stopped", "Playing", "Paused" };

    enum class TAudioSampleFormat
    {
        asfU8, asfS8,         // unsigned/signed  8 bits
        asfU16LSB, asfS16LSB, // unsigned/signed 16 bits (endianness: LSB)
        asfU16MSB, asfS16MSB, // unsigned/signed 16 bits (endianness: MSB)
        asfU16, asfS16,       // unsigned/signed 16 bits (endianness: System)
        asfS32,               // signed 32 bits (endianness: System)
        asfFloat,             // float
        asfDouble,             // double
        Size
    };


    // Size of one sample (one channel only) in bytes
    const std::array<int, static_cast<size_t>(TAudioSampleFormat::Size)> AudioSampleSize =
    {
        1, 1,     // asfU8, asfS8
        2, 2,     // asfU16LSB, asfS16LSB
        2, 2,     // asfU16MSB, asfS16MSB
        2, 2,     // asfU16,    asfS16
        4,        // asfS32
        4,        // asfFloat
        8         // asfDouble
    };

    const int CHANNELMAP_LEFT = 0b01;
    const int CHANNELMAP_RIGHT = 0b10;
    const int CHANNELMAP_FRONT = CHANNELMAP_LEFT | CHANNELMAP_RIGHT;


    class TAudioFormatInfo
    {
    private:
        double fSampleRate;
        uint8_t fChannels;
        TAudioSampleFormat fFormat;
        int fFrameSize;

        void SetChannels(uint8_t Channels);
        void SetFormat(TAudioSampleFormat Format);
        void UpdateFrameSize();
        [[nodiscard]] double GetBytesPerSec() const;
        [[nodiscard]] int GetSampleSize() const;

    public:

        TAudioFormatInfo(uint8_t Channels, double SampleRate, TAudioSampleFormat Format);
        TAudioFormatInfo Copy(); //TODO::copy constructor

        /**
            *Returns the inverse ratio of the size of data in this format to its
            * size in a given target format.
            * Example: SrcSize * SrcInfo.GetRatio(TgtInfo) = TgtSize
            */
        double GetRatio(TAudioFormatInfo TargetInfo);

        [[nodiscard]] double SampleRate() const { return fSampleRate; }
        void SampleRate(double rate) { fSampleRate = rate; }

        [[nodiscard]] uint8_t Channels() const { return fChannels; }
        void Channels(uint8_t channel) { SetChannels(channel); }

        [[nodiscard]] TAudioSampleFormat Format() const { return fFormat; }
        void Format(TAudioSampleFormat format) { SetFormat(format); }

        [[nodiscard]] int FrameSize() const { return fFrameSize; }
        [[nodiscard]] int SampleSize() const { return GetSampleSize(); }
        [[nodiscard]] double BytesPerSec() const { return GetBytesPerSec(); }
    };

    class TSoundEffect
    {
    public:
        EngineDataPtr EngineData; // can be used for engine-specific data
        virtual void Callback(const std::vector<uint8_t>& Buffer) = 0;

        typedef std::shared_ptr<TSoundEffect> SPtr;
    };

    class TVoiceRemoval : public TSoundEffect
    {
    public:
        void Callback(const std::vector<uint8_t>& Buffer) override;
    };

    class TSoundFX
    {
    public:

        EngineDataPtr EngineData; // can be used for engine-specific data
        virtual void Init() = 0;
        virtual void Removed() = 0;

        virtual bool CanEnable() = 0;

        virtual uint32_t GetType() = 0;
        virtual int32_t GetPriority() = 0;
        virtual std::string GetName() = 0;

        typedef std::shared_ptr<TSoundFX> SPtr;
    };
    class TReplayGain : public TSoundFX
    {};

    class TAudioProcessingStream
    {
    public:

        //typedef std::function<void(TAudioProcessingStream Stream)> TOnCloseHandler;
    protected:

        //std::vector<TOnCloseHandler> OnCloseHandlers;

        [[nodiscard]] virtual std::optional<double> GetLength() const = 0;
        [[nodiscard]] virtual std::optional<double> GetPosition() const = 0;
        virtual void SetPosition(double Time) = 0;
        [[nodiscard]] virtual bool GetLoop() const = 0;
        virtual void SetLoop(bool Enabled) = 0;

    public: 

        boost::signals2::signal<void(TAudioProcessingStream&)> close;

        virtual TAudioFormatInfo GetAudioFormatInfo() = 0;
        virtual void Close() = 0;

        /**
         * Adds a new OnClose action handler.
         * The handlers are performed in the order they were added.
         * If not stated explicitely, member-variables might have been invalidated
         * already. So do not use any member (variable/method/...) if you are not
         * sure it is valid.
         */
        //void AddOnCloseHandler(TOnCloseHandler Handler);

        [[nodiscard]] std::optional<double> Length() const { return GetLength(); }

        void Position(double pos) { SetPosition(pos); }
        [[nodiscard]] std::optional<double> Position() const { return GetPosition(); }

        void Loop(bool l) { SetLoop(l); }
        [[nodiscard]] bool Loop() const { return GetLoop(); }
    };

    class TAudioSourceStream : public TAudioProcessingStream
    {
    protected:
        virtual bool IsEOF() = 0;
        virtual bool IsError() = 0;
    public:

        virtual ~TAudioSourceStream() = default;
        virtual int ReadData(const std::vector<uint8_t>& Buffer, int BufferSize) = 0;

        typedef std::shared_ptr<TAudioSourceStream> SPtr;

        //property EOF: bool read IsEOF;
        //property Error: bool read IsError;
    };

    /*
    * State-Chart for playback-stream state transitions
    * []: Transition, (): State
    *
    *               /---[Play/FadeIn]--->-\  /-------[Pause]----->-\
    * -[Create]->(Stop)                  (Play)                 (Pause)
    *              \\-<-[Stop/EOFError]-/  \-<---[Play/FadeIn]--//
    *               \-<------------[Stop/EOFError]--------------/
    *
    * *: if not looped, otherwise stream is repeated
    * Note: SetPosition() does not change the state.
    */

    class TAudioPlaybackStream : public TAudioProcessingStream
    {
    protected:

        double AvgSyncDiff;  //** average difference between stream and sync clock
        UTime::TSyncSource::SPtr SyncSource;
        TAudioSourceStream::SPtr SourceStream;

        virtual double GetLatency() = 0;
        virtual TStreamStatus GetStatus() = 0;
        virtual float GetVolume() = 0;
        virtual void SetVolume(float Volume) = 0;
        int Synchronize(int BufferSize, TAudioFormatInfo FormatInfo);
        void FillBufferWithFrame(std::vector<uint8_t>& Buffer, const std::vector<uint8_t>& Frame);

    public:
        /**
           * Opens a SourceStream for playback.
           * Note that the caller (not the TAudioPlaybackStream) is responsible to
           * free the SourceStream after the Playback-Stream is closed.
           * You may use an OnClose-handler to achieve this. GetSourceStream()
           * guarantees to deliver this method's SourceStream parameter to
           * the OnClose-handler. Freeing SourceStream at OnClose is allowed.
           */
        virtual bool Open(TAudioSourceStream::SPtr SourceStream) = 0;

        virtual void Play() = 0;
        virtual void Pause() = 0;
        virtual void Stop() = 0;
        virtual void FadeIn(double Time, float TargetVolume) = 0;
        virtual void Fade(double Time, float TargetVolume) = 0;

        virtual void GetFFTData(TFFTData& data) = 0;
        virtual uint32_t GetPCMData(TPCMData& data) = 0;

        virtual void AddSoundEffect(TSoundEffect::SPtr Effect) = 0;
        virtual void RemoveSoundEffect(TSoundEffect::SPtr Effect) = 0;

        virtual void AddSoundFX(TSoundFX::SPtr FX) = 0;
        virtual void RemoveSoundFX(TSoundFX::SPtr FX) = 0;

        void SetSyncSource(UTime::TSyncSource::SPtr syncSource);
        TAudioSourceStream::SPtr GetSourceStream() const;

        //property Status: TStreamStatus read GetStatus;
        //property Volume: single read GetVolume write SetVolume;
        typedef std::shared_ptr<TAudioPlaybackStream> SPtr;
    };

    class TAudioDecodeStream : public TAudioSourceStream
    {};

    class TAudioVoiceStream : public TAudioSourceStream
    {
    protected:

        TAudioFormatInfo FormatInfo;
        int ChannelMap;

    public:

        ~TAudioVoiceStream() override;

        virtual bool Open(int ChannelMap, TAudioFormatInfo FormatInfo);
        void Close() override;

        virtual void WriteData(const std::vector<uint8_t>& Buffer) = 0;
        TAudioFormatInfo GetAudioFormatInfo() override;

        std::optional<double> GetLength() const override;
        std::optional<double> GetPosition() const override;
        void SetPosition(double Time) override;
        bool GetLoop() const override;
        void SetLoop(bool Enabled) override;
    };

    // soundcard output-devices information
    class TAudioOutputDevice
    {
    public:
        std::string Name; // soundcard name
    };

    typedef std::vector<TAudioOutputDevice> TAudioOutputDeviceList;

    struct IGenericPlayback
    {
        virtual std::string GetName() = 0;
    };

    struct IVideo
    {
        virtual void Play() = 0;
        virtual void Pause() = 0;
        virtual void Stop() = 0;

        virtual void SetLoop(bool Enable) = 0;
        virtual bool GetLoop() = 0;

        virtual void SetPosition(double Time) = 0;
        virtual double GetPosition() = 0;

        virtual void SetScreen(int Screen) = 0;
        virtual int GetScreen() = 0;

        virtual void SetScreenPosition(double X, double Y, double Z = 0.0) = 0;
        virtual void GetScreenPosition(double& X, double& Y, double& Z) = 0;

        virtual void SetWidth(double Width) = 0;
        virtual double GetWidth() = 0;

        virtual void SetHeight(double Height) = 0;
        virtual double GetHeight() = 0;

        /**
         * Sub-image of the video frame to draw.
         * This can be used for zooming or similar purposes.
         */
        virtual void SetFrameRange(TRectCoords Range) = 0;
        virtual TRectCoords GetFrameRange() = 0;

        virtual double GetFrameAspect() = 0;

        virtual void SetAspectCorrection(TAspectCorrection AspectCorrection) = 0;
        virtual TAspectCorrection GetAspectCorrection() = 0;


        virtual void SetAlpha(double Alpha) = 0;
        virtual double GetAlpha() = 0;

        virtual void SetReflectionSpacing(double Spacing) = 0;
        virtual double GetReflectionSpacing() = 0;

        virtual void GetFrame(double Time) = 0;
        virtual void Draw() = 0;
        virtual void DrawReflection() = 0;

        /*
        property Screen: int read GetScreen;
        property Width: double read GetWidth write SetWidth;
        property Height: double read GetHeight write SetHeight;
        property Alpha: double read GetAlpha write SetAlpha;
        property ReflectionSpacing: double read GetReflectionSpacing write SetReflectionSpacing;
        property FrameAspect: double read GetFrameAspect;
        property AspectCorrection: TAspectCorrection read GetAspectCorrection write SetAspectCorrection;
        property Loop: bool read GetLoop write SetLoop;
        property Position: double read GetPosition write SetPosition;
        */
    };

    struct IVideoPlayback : IGenericPlayback
    {
        virtual bool Init() = 0;
        virtual bool Finalize() = 0;

        virtual IVideo Open(const std::filesystem::path FileName) = 0;

        typedef std::shared_ptr<IVideoPlayback> SPtr;
    };

    struct IVideoVisualization : IVideoPlayback
    {};

    struct IAudioPlayback : IGenericPlayback
    {
        virtual bool InitializePlayback() = 0;
        virtual bool FinalizePlayback() = 0;

        virtual TAudioOutputDeviceList GetOutputDeviceList() = 0;

        virtual void SetAppVolume(float Volume) = 0;
        virtual void SetVolume(float Volume) = 0;
        virtual void SetLoop(bool Enabled) = 0;

        virtual void FadeIn(double Time, float TargetVolume) = 0;
        virtual void Fade(double Time, float TargetVolume) = 0;
        virtual void SetSyncSource(UTime::TSyncSource::SPtr SyncSource) = 0;

        virtual void Rewind() = 0;
        virtual bool Finished() = 0;
        virtual double Length() = 0;

        virtual bool Open(const std::filesystem::path Filename) = 0; // true if succeed
        virtual void Close() = 0;

        virtual void Play() = 0;
        virtual void Pause() = 0;
        virtual void Stop() = 0;

        virtual void SetPosition(double Time) = 0;
        virtual double GetPosition() = 0;
        /*
        property Position: double read GetPosition write SetPosition;
        */
        // Sounds
        // TODO:
        // add a TMediaDummyPlaybackStream implementation that will
        // be used by the TSoundLib whenever OpenSound() fails, so checking for
        // nil-pointers is not neccessary anymore.
        // PlaySound/StopSound will be removed then, OpenSound will be renamed to
        // CreateSound.
        virtual TAudioPlaybackStream OpenSound(const std::filesystem::path Filename) = 0;
        virtual TAudioPlaybackStream OpenSoundBuffer(TStream Buffer, TAudioFormatInfo Format) = 0;
        virtual void PlaySound(const TAudioPlaybackStream& Stream) = 0;
        virtual void StopSound(const TAudioPlaybackStream& Stream) = 0;

        // Equalizer
        virtual void GetFFTData(TFFTData& Data) = 0;

        // Interface for Visualizer
        virtual uint32_t GetPCMData(TPCMData& Data) = 0;

        virtual TAudioVoiceStream CreateVoiceStream(int ChannelMap, TAudioFormatInfo FormatInfo) = 0;

        typedef std::shared_ptr<IAudioPlayback> SPtr;
    };

    struct IGenericDecoder
    {
        virtual std::string GetName() = 0;
        virtual bool InitializeDecoder() = 0;
        virtual bool FinalizeDecoder() = 0;
        //function IsSupported(const Filename: std::string): bool;
    };

    /*
    IVideoDecoder = Interface( IGenericDecoder )
    ["{2F184B2B-FE69-44D5-9031-0A2462391DCA}"]
       function Open(const Filename: IPath): TVideoDecodeStream;

       void SetPosition(Time: double);
       double GetPosition();

       void UpdateTexture(Texture: glUint);

       property Loop: bool read GetLoop write SetLoop;
       property Position: double read GetPosition write SetPosition;
    }
    */

    struct IAudioDecoder : IGenericDecoder
    {
        virtual TAudioDecodeStream Open(const std::filesystem::path& Filename) = 0;

        typedef std::shared_ptr<IAudioDecoder> SPtr;
    };

    struct IAudioInput
    {
        virtual std::string GetName() = 0;
        virtual bool InitializeRecord() = 0;
        virtual bool FinalizeRecord() = 0;

        virtual void CaptureStart() = 0;
        virtual void CaptureStop() = 0;

        typedef std::shared_ptr<IAudioInput> SPtr;
    };

    class TAudioConverter
    {
    protected:
        TAudioFormatInfo fSrcFormatInfo;
        TAudioFormatInfo fDstFormatInfo;
    public:
        virtual bool Init(TAudioFormatInfo SrcFormatInfo, TAudioFormatInfo DstFormatInfo);
        ~TAudioConverter();

        /**
         * Converts the InputBuffer and stores the result in OutputBuffer.
         * If the result is not -1, InputSize will be set to the actual number of
         * input-buffer bytes used.
         * Returns the number of bytes written to the output-buffer or -1 if an error occured.
         */
        virtual int Convert(const std::vector<uint8_t>& InputBuffer, std::vector<uint8_t>& OutputBuffer, int& InputSize) = 0;

        /**
         * Destination/Source size ratio
         */
        virtual double GetRatio() = 0;

        virtual int GetOutputBufferSize(int InputSize) = 0;
        TAudioFormatInfo SrcFormatInfo() const { return fSrcFormatInfo; };
        TAudioFormatInfo DstFormatInfo() const { return fDstFormatInfo; };
    };

    /* TODO
    const
    SOUNDID_START    = 0;
    SOUNDID_BACK     = 1;
    SOUNDID_SWOOSH   = 2;
    SOUNDID_CHANGE   = 3;
    SOUNDID_OPTION   = 4;
    SOUNDID_CLICK    = 5;
    LAST_SOUNDID = SOUNDID_CLICK;

    BaseSoundFilenames: array[0..LAST_SOUNDID] of IPath = (
    "%SOUNDPATH%/Common start.mp3",                 // Start
    "%SOUNDPATH%/Common back.mp3",                  // Back
    "%SOUNDPATH%/menu swoosh.mp3",                  // Swoosh
    "%SOUNDPATH%/select music change music 50.mp3", // Change
    "%SOUNDPATH%/option change col.mp3",            // Option
    "%SOUNDPATH%/rimshot022b.mp3"                   // Click
    {
    "%SOUNDPATH%/bassdrumhard076b.mp3",             // Drum (unused)
    "%SOUNDPATH%/hihatclosed068b.mp3",              // Hihat (unused)
    "%SOUNDPATH%/claps050b.mp3",                    // Clap (unused)
    "%SOUNDPATH%/Shuffle.mp3"                       // Shuffle (unused)
    }
    );
    */

    class TSoundLibrary
    {
    private:
        // TODO
        //std::vector<TAudioPlaybackStream> Sounds;
    public:
        // TODO: move sounds to the private section
        // and provide IDs instead.
        TAudioPlaybackStream::SPtr Start;
        TAudioPlaybackStream::SPtr Back;
        TAudioPlaybackStream::SPtr Swoosh;
        TAudioPlaybackStream::SPtr Change;
        TAudioPlaybackStream::SPtr Option;
        TAudioPlaybackStream::SPtr Click;
        TAudioPlaybackStream::SPtr Applause;
        TAudioPlaybackStream::SPtr BGMusic;

        TSoundLibrary();
        ~TSoundLibrary();

        void LoadSounds();
        void UnloadSounds();

        void StartBgMusic();
        void PauseBgMusic();
        // TODO
        //function AddSound(Filename: IPath): int;
        //void RemoveSound(ID: int);
        //function GetSound(ID: int): TAudioPlaybackStream;
        //property Sound[ID: int]: TAudioPlaybackStream read GetSound; default;
    };

    // TODO: JB --- THESE SHOULD NOT BE GLOBAL
    typedef std::vector<TLines> TrackVec;
	TrackVec Tracks;
    TLyricsState<std::chrono::system_clock> LyricsState;
    TSoundLibrary SoundLib;


    void InitializeSound();
    void InitializeVideo();
    void FinalizeMedia();

    IVideoPlayback::SPtr Visualization();
    IVideoPlayback::SPtr VideoPlayback();
    IAudioPlayback::SPtr AudioPlayback();
    IAudioInput::SPtr AudioInput();
    std::list<IAudioDecoder::SPtr> AudioDecoders();

    //TInterfaceList MediaManager();

    void DumpMediaInterfaces();

    TPos FindNote(int beat);
    /*
    implementation

    uses
    math,
    UIni,
    UNote,
    UCommandLine,
    URecord,
    ULog,
    UPathUtils;
    */

    IVideoPlayback::SPtr DefaultVideoPlayback;
    IVideoPlayback::SPtr DefaultVisualization;
    IAudioPlayback::SPtr DefaultAudioPlayback;
    IAudioInput::SPtr DefaultAudioInput;
    std::list<IAudioDecoder::SPtr> AudioDecoderList;
    std::list<IAudioPlayback::SPtr> AudioPlaybackList;
    std::list<IAudioInput::SPtr> AudioInputList;
    //TInterfaceList MediaInterfaceList;
}