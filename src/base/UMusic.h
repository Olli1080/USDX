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

namespace UMusic
{

    enum TNoteType
    {
        ntFreestyle, ntNormal, ntGolden, ntRap, ntRapGolden, Size
    };

    struct TPos // Tracks[track].Lines[line].Notes[note]
    {
        int track;
        int line;
        int note;
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
        int GetEnd() const;

    public:
        int EndBeat() const { return GetEnd(); }
    };

    typedef std::shared_ptr<TLineFragment> PLineFragment;

    /**
    * TLine represents one lyrics line and consists of multiple
    * notes.
    */
    struct TLine
    {
        int StartBeat; // the start beat of this line (<> start beat of the first note of this line)
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
        int GetLength() const;

    public:
        // Returns whether the line has a valid length. }
        bool HasLength() const;
        // Returns whether the line has a valid lengthand passes length. }
        bool HasLength(int& Len);
        // Returns whether the line has a valid lengthand passes length.Output converted to Double }
        bool HasLength(double& Len);

        int Length_() const { return GetLength(); }
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
        int High;  // = High(Line)!
        int Number;
        int Resolution;
        int NotesGAP;
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
        double GetBytesPerSec() const;
        int GetSampleSize() const;
    public:
        TAudioFormatInfo(uint8_t Channels, double SampleRate, TAudioSampleFormat Format);
        TAudioFormatInfo Copy(); //TODO::copy constructor

        /**
            *Returns the inverse ratio of the size of data in this format to its
            * size in a given target format.
            * Example: SrcSize * SrcInfo.GetRatio(TgtInfo) = TgtSize
            */
        double GetRatio(TAudioFormatInfo TargetInfo);

        double SampleRate() const { return fSampleRate; }
        void SampleRate(double rate) { fSampleRate = rate; }

        uint8_t Channels() const { return fChannels; }
        void Channels(uint8_t channel) { SetChannels(channel); }

        TAudioSampleFormat Format() const { return fFormat; }
        void Format(TAudioSampleFormat format) { SetFormat(format); }

        int FrameSize() const { return fFrameSize; }
        int SampleSize() const { return GetSampleSize(); }
        double BytesPerSec() const { return GetBytesPerSec(); }
    };

    class TSoundEffect
    {
    public:
        EngineData : Pointer; // can be used for engine-specific data
        void Callback(Buffer: PByteArray; BufSize: int); virtual; abstract;
    };

    class TVoiceRemoval : public TSoundEffect
    {
        ;
    public:
        void Callback(Buffer : PByteArray; BufSize: int); override;
    };

    class TSoundFX
    {
    public:
        EngineData : Pointer; // can be used for engine-specific data
        void Init(); virtual; abstract;
        void Removed(); virtual; abstract;

        class function CanEnable : bool; virtual; abstract;

        function GetType : DWORD; virtual; abstract;
        function GetPriority : LongInt; virtual; abstract;
        function GetName : string; virtual; abstract;
    };
	class TReplayGain : public TSoundFX
    {};

type
  TAudioProcessingStream = class;
  TOnCloseHandler = void(Stream: TAudioProcessingStream);

  class TAudioProcessingStream
  {
  protected:
      OnCloseHandlers : array of TOnCloseHandler;

      function GetLength() : double;           virtual; abstract;
      function GetPosition() : double;         virtual; abstract;
      void SetPosition(Time: double);    virtual; abstract;
      function GetLoop() : bool;          virtual; abstract;
      void SetLoop(Enabled: bool);  virtual; abstract;

      void PerformOnClose();
  public:
      function GetAudioFormatInfo() : TAudioFormatInfo; virtual; abstract;
      void Close(); virtual; abstract;

      /**
       * Adds a new OnClose action handler.
       * The handlers are performed in the order they were added.
       * If not stated explicitely, member-variables might have been invalidated
       * already. So do not use any member (variable/method/...) if you are not
       * sure it is valid.
       */
      void AddOnCloseHandler(Handler: TOnCloseHandler);

      property Length: double read GetLength;
      property Position: double read GetPosition write SetPosition;
      property Loop: bool read GetLoop write SetLoop;
  };

  TAudioSourceStream = class(TAudioProcessingStream)
    protected
      function IsEOF(): bool;            virtual; abstract;
      function IsError(): bool;          virtual; abstract;
    public
      function ReadData(Buffer: PByteArray; BufferSize: int): int; virtual; abstract;

      property EOF: bool read IsEOF;
      property Error: bool read IsError;
  end;

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

  TAudioPlaybackStream = class(TAudioProcessingStream)
    protected
      AvgSyncDiff: double;  //** average difference between stream and sync clock
      SyncSource: TSyncSource;
      SourceStream: TAudioSourceStream;

      function GetLatency(): double; virtual; abstract;
      function GetStatus(): TStreamStatus;  virtual; abstract;
      function GetVolume(): single;         virtual; abstract;
      void SetVolume(Volume: single);  virtual; abstract;
      function Synchronize(BufferSize: int; FormatInfo: TAudioFormatInfo): int;
      void FillBufferWithFrame(Buffer: PByteArray; BufferSize: int; Frame: PByteArray; FrameSize: int);
    public
      (**
       * Opens a SourceStream for playback.
       * Note that the caller (not the TAudioPlaybackStream) is responsible to
       * free the SourceStream after the Playback-Stream is closed.
       * You may use an OnClose-handler to achieve this. GetSourceStream()
       * guarantees to deliver this method's SourceStream parameter to
       * the OnClose-handler. Freeing SourceStream at OnClose is allowed.
       *)
      function Open(SourceStream: TAudioSourceStream): bool; virtual; abstract;

      void Play();                     virtual; abstract;
      void Pause();                    virtual; abstract;
      void Stop();                     virtual; abstract;
      void FadeIn(Time: double; TargetVolume: single);  virtual; abstract;
      void Fade(Time: double; TargetVolume: single);  virtual; abstract;

      void GetFFTData(var data: TFFTData);          virtual; abstract;
      function GetPCMData(var data: TPCMData): uint32_t; virtual; abstract;

      void AddSoundEffect(Effect: TSoundEffect);    virtual; abstract;
      void RemoveSoundEffect(Effect: TSoundEffect); virtual; abstract;

      void AddSoundFX(FX: TSoundFX);    virtual; abstract;
      void RemoveSoundFX(FX: TSoundFX); virtual; abstract;

      void SetSyncSource(SyncSource: TSyncSource);
      function GetSourceStream(): TAudioSourceStream;

      property Status: TStreamStatus read GetStatus;
      property Volume: single read GetVolume write SetVolume;
  end;

  TAudioDecodeStream = class(TAudioSourceStream)
  end;

  TAudioVoiceStream = class(TAudioSourceStream)
    protected
      FormatInfo: TAudioFormatInfo;
      ChannelMap: int;
    public
      destructor Destroy; override;

      function Open(ChannelMap: int; FormatInfo: TAudioFormatInfo): bool; virtual;
      void Close(); override;

      void WriteData(Buffer: PByteArray; BufferSize: int); virtual; abstract;
      function GetAudioFormatInfo(): TAudioFormatInfo; override;

      function GetLength(): double;           override;
      function GetPosition(): double;         override;
      void SetPosition(Time: double);    override;
      function GetLoop(): bool;          override;
      void SetLoop(Enabled: bool);  override;
  end;

type
  // soundcard output-devices information
  TAudioOutputDevice = class
    public
      Name: UTF8String; // soundcard name
  end;
  TAudioOutputDeviceList = array of TAudioOutputDevice;

type
  IGenericPlayback = Interface
  ['{63A5EBC3-3F4D-4F23-8DFB-B5165FCE33DD}']
      function GetName: String;
  end;

  IVideo = interface
  ['{58DFC674-9168-41EA-B59D-A61307242B80}']
      void Play;
      void Pause;
      void Stop;

      void SetLoop(Enable: bool);
      function GetLoop(): bool;

      void SetPosition(Time: double);
      function GetPosition: double;

      void SetScreen(Screen: int);
      function GetScreen(): int;

      void SetScreenPosition(X, Y: double; Z: double = 0.0);
      void GetScreenPosition(var X, Y, Z: double);

      void  SetWidth(Width: double);
       function GetWidth(): double;

      void  SetHeight(Height: double);
       function GetHeight(): double;

      /**
       * Sub-image of the video frame to draw.
       * This can be used for zooming or similar purposes.
       */
      void SetFrameRange(Range: TRectCoords);
      function GetFrameRange(): TRectCoords;

      function GetFrameAspect(): double;

      void SetAspectCorrection(AspectCorrection: TAspectCorrection);
      function GetAspectCorrection(): TAspectCorrection;


      void SetAlpha(Alpha: double);
      function GetAlpha(): double;

      void SetReflectionSpacing(Spacing: double);
      function GetReflectionSpacing(): double;

      void GetFrame(Time: Extended);
      void Draw();
      void DrawReflection();


      property Screen: int read GetScreen;
      property Width: double read GetWidth write SetWidth;
      property Height: double read GetHeight write SetHeight;
      property Alpha: double read GetAlpha write SetAlpha;
      property ReflectionSpacing: double read GetReflectionSpacing write SetReflectionSpacing;
      property FrameAspect: double read GetFrameAspect;
      property AspectCorrection: TAspectCorrection read GetAspectCorrection write SetAspectCorrection;
      property Loop: bool read GetLoop write SetLoop;
      property Position: double read GetPosition write SetPosition;
  end;

  IVideoPlayback = Interface( IGenericPlayback )
  ['{3574C40C-28AE-4201-B3D1-3D1F0759B131}']
      function Init(): bool;
      function Finalize: bool;

      function Open(const FileName : IPath): IVideo;
  end;

  IVideoVisualization = Interface( IVideoPlayback )
  ['{5AC17D60-B34D-478D-B632-EB00D4078017}']
  end;

  IAudioPlayback = Interface( IGenericPlayback )
  ['{E4AE0B40-3C21-4DC5-847C-20A87E0DFB96}']
      function InitializePlayback: bool;
      function FinalizePlayback: bool;

      function GetOutputDeviceList(): TAudioOutputDeviceList;

      void SetAppVolume(Volume: single);
      void SetVolume(Volume: single);
      void SetLoop(Enabled: bool);

      void FadeIn(Time: double; TargetVolume: single);
      void Fade(Time: double; TargetVolume: single);
      void SetSyncSource(SyncSource: TSyncSource);

      void Rewind;
      function  Finished: bool;
      function  Length: double;

      function Open(const Filename: IPath): bool; // true if succeed
      void Close;

      void Play;
      void Pause;
      void Stop;

      void SetPosition(Time: double);
      function GetPosition: double;

      property Position: double read GetPosition write SetPosition;

      // Sounds
      // TODO:
      // add a TMediaDummyPlaybackStream implementation that will
      // be used by the TSoundLib whenever OpenSound() fails, so checking for
      // nil-pointers is not neccessary anymore.
      // PlaySound/StopSound will be removed then, OpenSound will be renamed to
      // CreateSound.
      function OpenSound(const Filename: IPath): TAudioPlaybackStream;
      function OpenSoundBuffer(Buffer: TStream; Format: TAudioFormatInfo): TAudioPlaybackStream;
      void PlaySound(Stream: TAudioPlaybackStream);
      void StopSound(Stream: TAudioPlaybackStream);

      // Equalizer
      void GetFFTData(var Data: TFFTData);

      // Interface for Visualizer
      function GetPCMData(var Data: TPCMData): uint32_t;

      function CreateVoiceStream(ChannelMap: int; FormatInfo: TAudioFormatInfo): TAudioVoiceStream;
  end;

  IGenericDecoder = Interface
  ['{557B0E9A-604D-47E4-B826-13769F3E10B7}']
      function GetName(): string;
      function InitializeDecoder(): bool;
      function FinalizeDecoder(): bool;
      //function IsSupported(const Filename: string): bool;
  end;

  (*
  IVideoDecoder = Interface( IGenericDecoder )
  ['{2F184B2B-FE69-44D5-9031-0A2462391DCA}']
       function Open(const Filename: IPath): TVideoDecodeStream;

       void SetPosition(Time: double);
       function GetPosition:  double;

       void UpdateTexture(Texture: glUint);

       property Loop: bool read GetLoop write SetLoop;
       property Position: double read GetPosition write SetPosition;
  end;
  *)

  IAudioDecoder = Interface( IGenericDecoder )
  ['{AB47B1B6-2AA9-4410-BF8C-EC79561B5478}']
      function Open(const Filename: IPath): TAudioDecodeStream;
  end;

  IAudioInput = Interface
  ['{A5C8DA92-2A0C-4AB2-849B-2F7448C6003A}']
      function GetName: String;
      function InitializeRecord: bool;
      function FinalizeRecord(): bool;

      void CaptureStart;
      void CaptureStop;
  end;

type
  TAudioConverter = class
    protected
      fSrcFormatInfo: TAudioFormatInfo;
      fDstFormatInfo: TAudioFormatInfo;
    public
      function Init(SrcFormatInfo: TAudioFormatInfo; DstFormatInfo: TAudioFormatInfo): bool; virtual;
      destructor Destroy(); override;

      (**
       * Converts the InputBuffer and stores the result in OutputBuffer.
       * If the result is not -1, InputSize will be set to the actual number of
       * input-buffer bytes used.
       * Returns the number of bytes written to the output-buffer or -1 if an error occured.
       *)
      function Convert(InputBuffer: PByteArray; OutputBuffer: PByteArray; var InputSize: int): int; virtual; abstract;

      (**
       * Destination/Source size ratio
       *)
      function GetRatio(): double; virtual; abstract;

      function GetOutputBufferSize(InputSize: int): int; virtual; abstract;
      property SrcFormatInfo: TAudioFormatInfo read fSrcFormatInfo;
      property DstFormatInfo: TAudioFormatInfo read fDstFormatInfo;
  end;

(* TODO
const
  SOUNDID_START    = 0;
  SOUNDID_BACK     = 1;
  SOUNDID_SWOOSH   = 2;
  SOUNDID_CHANGE   = 3;
  SOUNDID_OPTION   = 4;
  SOUNDID_CLICK    = 5;
  LAST_SOUNDID = SOUNDID_CLICK;

  BaseSoundFilenames: array[0..LAST_SOUNDID] of IPath = (
    '%SOUNDPATH%/Common start.mp3',                 // Start
    '%SOUNDPATH%/Common back.mp3',                  // Back
    '%SOUNDPATH%/menu swoosh.mp3',                  // Swoosh
    '%SOUNDPATH%/select music change music 50.mp3', // Change
    '%SOUNDPATH%/option change col.mp3',            // Option
    '%SOUNDPATH%/rimshot022b.mp3'                   // Click
    {
    '%SOUNDPATH%/bassdrumhard076b.mp3',             // Drum (unused)
    '%SOUNDPATH%/hihatclosed068b.mp3',              // Hihat (unused)
    '%SOUNDPATH%/claps050b.mp3',                    // Clap (unused)
    '%SOUNDPATH%/Shuffle.mp3'                       // Shuffle (unused)
    }
  );
*)

type
  TSoundLibrary = class
    private
      // TODO
      //Sounds: array of TAudioPlaybackStream;
    public
      // TODO: move sounds to the private section
      // and provide IDs instead.
      Start:   TAudioPlaybackStream;
      Back:    TAudioPlaybackStream;
      Swoosh:  TAudioPlaybackStream;
      Change:  TAudioPlaybackStream;
      Option:  TAudioPlaybackStream;
      Click:   TAudioPlaybackStream;
      Applause:TAudioPlaybackStream;
      BGMusic: TAudioPlaybackStream;

      constructor Create();
      destructor Destroy(); override;

      void LoadSounds();
      void UnloadSounds();

      void StartBgMusic();
      void PauseBgMusic();
      // TODO
      //function AddSound(Filename: IPath): int;
      //void RemoveSound(ID: int);
      //function GetSound(ID: int): TAudioPlaybackStream;
      //property Sound[ID: int]: TAudioPlaybackStream read GetSound; default;
  end;

var
  // TODO: JB --- THESE SHOULD NOT BE GLOBAL
  Tracks: array of TLines;
  LyricsState: TLyricsState;
  SoundLib: TSoundLibrary;


void InitializeSound;
void InitializeVideo;
void FinalizeMedia;

function  Visualization(): IVideoPlayback;
function  VideoPlayback(): IVideoPlayback;
function  AudioPlayback(): IAudioPlayback;
function  AudioInput(): IAudioInput;
function  AudioDecoders(): TInterfaceList;

function  MediaManager: TInterfaceList;

void DumpMediaInterfaces();

function FindNote(beat: int): TPos;

implementation

uses
  math,
  UIni,
  UNote,
  UCommandLine,
  URecord,
  ULog,
  UPathUtils;

var
  DefaultVideoPlayback : IVideoPlayback;
  DefaultVisualization : IVideoPlayback;
  DefaultAudioPlayback : IAudioPlayback;
  DefaultAudioInput    : IAudioInput;
  AudioDecoderList     : TInterfaceList;
  MediaInterfaceList   : TInterfaceList;


constructor TAudioFormatInfo.Create(Channels: byte; SampleRate: double; Format: TAudioSampleFormat);
begin
  inherited Create();
  fChannels := Channels;
  fSampleRate := SampleRate;
  fFormat := Format;
  UpdateFrameSize();
end;

void TAudioFormatInfo.SetChannels(Channels: byte);
begin
  fChannels := Channels;
  UpdateFrameSize();
end;

void TAudioFormatInfo.SetFormat(Format: TAudioSampleFormat);
begin
  fFormat := Format;
  UpdateFrameSize();
end;

function TAudioFormatInfo.GetBytesPerSec(): double;
begin
  Result := FrameSize * SampleRate;
end;

function TAudioFormatInfo.GetSampleSize(): int;
begin
  Result := AudioSampleSize[fFormat];
end;

void TAudioFormatInfo.UpdateFrameSize();
begin
  fFrameSize := AudioSampleSize[fFormat] * fChannels;
end;

function TAudioFormatInfo.Copy(): TAudioFormatInfo;
begin
  Result := TAudioFormatInfo.Create(Self.Channels, Self.SampleRate, Self.Format);
end;

function TAudioFormatInfo.GetRatio(TargetInfo: TAudioFormatInfo): double;
begin
  Result := (TargetInfo.FrameSize / Self.FrameSize) *
            (TargetInfo.SampleRate / Self.SampleRate)
end;


function MediaManager: TInterfaceList;
begin
  if (not assigned(MediaInterfaceList)) then
    MediaInterfaceList := TInterfaceList.Create();
  Result := MediaInterfaceList;
end;

function  VideoPlayback(): IVideoPlayback;
begin
  Result := DefaultVideoPlayback;
end;

function  Visualization(): IVideoPlayback;
begin
  Result := DefaultVisualization;
end;

function AudioPlayback(): IAudioPlayback;
begin
  Result := DefaultAudioPlayback;
end;

function AudioInput(): IAudioInput;
begin
  Result := DefaultAudioInput;
end;

function AudioDecoders(): TInterfaceList;
begin
  Result := AudioDecoderList;
end;

void FilterInterfaceList(const IID: TGUID; InList, OutList: TInterfaceList);
var
  i: int;
  obj: IInterface;
begin
  if (not assigned(OutList)) then
    Exit;

  OutList.Clear;
  for i := 0 to InList.Count-1 do
  begin
    if assigned(InList[i]) then
    begin
      // add object to list if it implements the interface searched for
      if (InList[i].QueryInterface(IID, obj) = 0) then
        OutList.Add(obj);
    end;
  end;
end;

void InitializeSound;
var
  i: int;
  InterfaceList: TInterfaceList;
  CurrentAudioDecoder: IAudioDecoder;
  CurrentAudioPlayback: IAudioPlayback;
  CurrentAudioInput: IAudioInput;
begin
  // create a temporary list for interface enumeration
  InterfaceList := TInterfaceList.Create();

  // initialize all audio-decoders first
  FilterInterfaceList(IAudioDecoder, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
  begin
    CurrentAudioDecoder := InterfaceList[i] as IAudioDecoder;
    if (not CurrentAudioDecoder.InitializeDecoder()) then
    begin
      Log.LogError('Initialize failed, Removing - '+ CurrentAudioDecoder.GetName);
      MediaManager.Remove(CurrentAudioDecoder);
    end;
  end;

  // create and setup decoder-list (see AudioDecoders())
  AudioDecoderList := TInterfaceList.Create;
  FilterInterfaceList(IAudioDecoder, MediaManager, AudioDecoders);

  // find and initialize playback interface
  DefaultAudioPlayback := nil;
  FilterInterfaceList(IAudioPlayback, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
  begin
    CurrentAudioPlayback := InterfaceList[i] as IAudioPlayback;
    if (CurrentAudioPlayback.InitializePlayback()) then
    begin
      DefaultAudioPlayback := CurrentAudioPlayback;
      break;
    end;
    Log.LogError('Initialize failed, Removing - '+ CurrentAudioPlayback.GetName);
    MediaManager.Remove(CurrentAudioPlayback);
  end;

  // find and initialize input interface
  DefaultAudioInput := nil;
  FilterInterfaceList(IAudioInput, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
  begin
    CurrentAudioInput := InterfaceList[i] as IAudioInput;
    if (CurrentAudioInput.InitializeRecord()) then
    begin
      DefaultAudioInput := CurrentAudioInput;
      break;
    end;
    Log.LogError('Initialize failed, Removing - '+ CurrentAudioInput.GetName);
    MediaManager.Remove(CurrentAudioInput);
  end;

  InterfaceList.Free;

  // Update input-device list with registered devices
  AudioInputProcessor.UpdateInputDeviceConfig();

  // Load in-game sounds
  SoundLib := TSoundLibrary.Create;
end;

void InitializeVideo();
var
  i: int;
  InterfaceList: TInterfaceList;
  VideoInterface: IVideoPlayback;
  VisualInterface: IVideoVisualization;
begin
  InterfaceList := TInterfaceList.Create;

  // initialize and set video-playback singleton
  DefaultVideoPlayback := nil;
  FilterInterfaceList(IVideoPlayback, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
  begin
    VideoInterface := InterfaceList[i] as IVideoPlayback;
    if (VideoInterface.Init()) then
    begin
      DefaultVideoPlayback := VideoInterface;
      break;
    end;
    Log.LogError('Initialize failed, Removing - '+ VideoInterface.GetName);
    MediaManager.Remove(VideoInterface);
  end;

  // initialize and set visualization singleton
  DefaultVisualization := nil;
  FilterInterfaceList(IVideoVisualization, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
  begin
    VisualInterface := InterfaceList[i] as IVideoVisualization;
    if (VisualInterface.Init()) then
    begin
      DefaultVisualization := VisualInterface;
      break;
    end;
    Log.LogError('Initialize failed, Removing - '+ VisualInterface.GetName);
    MediaManager.Remove(VisualInterface);
  end;

  InterfaceList.Free;

  // now that we have all interfaces, we can dump them
  // TODO: move this to another place
  if FindCmdLineSwitch(cMediaInterfaces) then
  begin
    DumpMediaInterfaces();
    halt;
  end;
end;

void UnloadMediaModules;
var
  i: int;
  InterfaceList: TInterfaceList;
begin
  FreeAndNil(AudioDecoderList);
  DefaultAudioPlayback := nil;
  DefaultAudioInput := nil;
  DefaultVideoPlayback := nil;
  DefaultVisualization := nil;

  // create temporary interface list
  InterfaceList := TInterfaceList.Create();

  // finalize audio playback interfaces (should be done before the decoders)
  FilterInterfaceList(IAudioPlayback, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
    (InterfaceList[i] as IAudioPlayback).FinalizePlayback();

  // finalize audio input interfaces
  FilterInterfaceList(IAudioInput, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
    (InterfaceList[i] as IAudioInput).FinalizeRecord();

  // finalize audio decoder interfaces
  FilterInterfaceList(IAudioDecoder, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
    (InterfaceList[i] as IAudioDecoder).FinalizeDecoder();

  // finalize video interfaces
  FilterInterfaceList(IVideoPlayback, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
    (InterfaceList[i] as IVideoPlayback).Finalize();

  // finalize audio decoder interfaces
  FilterInterfaceList(IVideoVisualization, MediaManager, InterfaceList);
  for i := 0 to InterfaceList.Count-1 do
    (InterfaceList[i] as IVideoVisualization).Finalize();

  InterfaceList.Free;

  // finally free interfaces (by removing all references to them)
  FreeAndNil(MediaInterfaceList);
end;

void FinalizeMedia;
begin
  // stop, close and free sounds
  SoundLib.Free;

  // release webcam
  Webcam.Free;

  // stop and close music stream
  if (AudioPlayback <> nil) then
    AudioPlayback.Close;

  // stop any active captures
  if (AudioInput <> nil) then
    AudioInput.CaptureStop;

  UnloadMediaModules();
end;

void DumpMediaInterfaces();
begin
  writeln( '' );
  writeln( '--------------------------------------------------------------' );
  writeln( '  In-use Media Interfaces                                     ' );
  writeln( '--------------------------------------------------------------' );
  writeln( 'Registered Audio Playback Interface : ' + AudioPlayback.GetName );
  writeln( 'Registered Audio Input    Interface : ' + AudioInput.GetName    );
  writeln( 'Registered Video Playback Interface : ' + VideoPlayback.GetName );
  writeln( 'Registered Visualization  Interface : ' + Visualization.GetName );
  writeln( '--------------------------------------------------------------' );
  writeln( '' );
end;


{ TSoundLibrary }

constructor TSoundLibrary.Create();
begin
  inherited;
  LoadSounds();
end;

destructor TSoundLibrary.Destroy();
begin
  UnloadSounds();
  inherited;
end;

void TSoundLibrary.LoadSounds();
begin
  UnloadSounds();

  Start   := AudioPlayback.OpenSound(SoundPath.Append('Common start.mp3'));
  Back    := AudioPlayback.OpenSound(SoundPath.Append('Common back.mp3'));
  Swoosh  := AudioPlayback.OpenSound(SoundPath.Append('menu swoosh.mp3'));
  Change  := AudioPlayback.OpenSound(SoundPath.Append('select music change music 50.mp3'));
  Option  := AudioPlayback.OpenSound(SoundPath.Append('option change col.mp3'));
  Click   := AudioPlayback.OpenSound(SoundPath.Append('rimshot022b.mp3'));
  Applause:= AudioPlayback.OpenSound(SoundPath.Append('Applause.mp3'));

  BGMusic := AudioPlayback.OpenSound(SoundPath.Append('background track.mp3'));

  if (BGMusic <> nil) then
    BGMusic.Loop := True;
end;

void TSoundLibrary.UnloadSounds();
begin
  FreeAndNil(Start);
  FreeAndNil(Back);
  FreeAndNil(Swoosh);
  FreeAndNil(Change);
  FreeAndNil(Option);
  FreeAndNil(Click);
  FreeAndNil(Applause);
  FreeAndNil(BGMusic);
end;

(* TODO
function TSoundLibrary.GetSound(ID: int): TAudioPlaybackStream;
begin
  if ((ID >= 0) and (ID < Length(Sounds))) then
    Result := Sounds[ID]
  else
    Result := nil;
end;
*)

void TSoundLibrary.StartBgMusic();
begin
  if (TBackgroundMusicOption(Ini.BackgroundMusicOption) = bmoOn) and
    (Soundlib.BGMusic <> nil) and not (Soundlib.BGMusic.Status = ssPlaying) then
  begin
    AudioPlayback.PlaySound(Soundlib.BGMusic);
  end;
end;

void TSoundLibrary.PauseBgMusic();
begin
  If (Soundlib.BGMusic <> nil) then
  begin
    Soundlib.BGMusic.Pause;
  end;
end;

{ TVoiceRemoval }

void TVoiceRemoval.Callback(Buffer: PByteArray; BufSize: int);
var
  FrameIndex, FrameSize: int;
  Value: int;
  Sample: PPCMStereoSample;
begin
  FrameSize := 2 * SizeOf(int16_t);
  for FrameIndex := 0 to (BufSize div FrameSize)-1 do
  begin
    Sample := PPCMStereoSample(Buffer);
    // channel difference
    Value := Sample[0] - Sample[1];
    // clip
    if (Value > High(int16_t)) then
      Value := High(int16_t)
    else if (Value < Low(int16_t)) then
      Value := Low(int16_t);
    // assign result
    Sample[0] := Value;
    Sample[1] := Value;
    // increase to next frame
    Inc(PByte(Buffer), FrameSize);
  end;
end;

{ TAudioConverter }

function TAudioConverter.Init(SrcFormatInfo: TAudioFormatInfo; DstFormatInfo: TAudioFormatInfo): bool;
begin
  fSrcFormatInfo := SrcFormatInfo.Copy();
  fDstFormatInfo := DstFormatInfo.Copy();
  Result := true;
end;

destructor TAudioConverter.Destroy();
begin
  FreeAndNil(fSrcFormatInfo);
  FreeAndNil(fDstFormatInfo);
end;


{ TAudioProcessingStream }

void TAudioProcessingStream.AddOnCloseHandler(Handler: TOnCloseHandler);
begin
  if (@Handler <> nil) then
  begin
    SetLength(OnCloseHandlers, System.Length(OnCloseHandlers)+1);
    OnCloseHandlers[High(OnCloseHandlers)] := @Handler;
  end;
end;

void TAudioProcessingStream.PerformOnClose();
var i: int;
begin
  for i := 0 to High(OnCloseHandlers) do
  begin
    OnCloseHandlers[i](Self);
  end;
end;


{ TAudioPlaybackStream }

function TAudioPlaybackStream.GetSourceStream(): TAudioSourceStream;
begin
  Result := SourceStream;
end;

void TAudioPlaybackStream.SetSyncSource(SyncSource: TSyncSource);
begin
  Self.SyncSource := SyncSource;
  AvgSyncDiff := -1;
end;

{.$DEFINE LOG_SYNC}

(*
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
 *)
function TAudioPlaybackStream.Synchronize(BufferSize: int; FormatInfo: TAudioFormatInfo): int;
var
  TimeDiff: double;
  FrameDiff: double;
  FrameSkip: int;
  ReqFrames: int;
  MasterClock: double;
  CurPosition: double;
const
  AVG_HISTORY_FACTOR = 0.7;
  SYNC_REPOS_THRESHOLD = 5.000;
  SYNC_SOFT_THRESHOLD  = 0.010;
begin
  Result := BufferSize;

  if (not assigned(SyncSource)) then
    Exit;

  if (BufferSize <= 0) then
    Exit;

  CurPosition := Position;
  MasterClock := SyncSource.GetClock();

  // difference between sync-source and stream position
  // (negative if the music-stream's position is ahead of the master clock)
  TimeDiff := MasterClock - CurPosition;

  // calculate average time difference (some sort of weighted mean).
  // The bigger AVG_HISTORY_FACTOR is, the smoother is the average diff.
  // This means that older diffs are weighted more with a higher history factor
  // than with a lower. Do not use a too low history factor. FFmpeg produces
  // very instable timestamps (pts) for ogg due to some bugs. They may differ
  // +-50ms from the double stream position. Without filtering those glitches we
  // would synch without any need, resulting in ugly plopping sounds.
  if (AvgSyncDiff = -1) then
    AvgSyncDiff := TimeDiff
  else
    AvgSyncDiff := TimeDiff * (1-AVG_HISTORY_FACTOR) +
                   AvgSyncDiff * AVG_HISTORY_FACTOR;

  {$IFDEF LOG_SYNC}
  //Log.LogError(Format('c:%.3f | p:%.3f | d:%.3f | a:%.3f',
  //    [MasterClock, CurPosition, TimeDiff, AvgSyncDiff]), 'Synch');
  {$ENDIF}

  // check if we are out of sync
  if (Abs(AvgSyncDiff) >= SYNC_REPOS_THRESHOLD) then
  begin
    {$IFDEF LOG_SYNC}
    Log.LogError(Format('ReposSynch: %.3f > %.3f',
        [Abs(AvgSyncDiff), SYNC_REPOS_THRESHOLD]), 'Synch');
    {$ENDIF}

    // diff far is too large -> reposition stream
    // (resulting position might still be out of sync)
    SetPosition(CurPosition + AvgSyncDiff);

    // reset sync info
    AvgSyncDiff := -1;
  end
  else if (Abs(AvgSyncDiff) >= SYNC_SOFT_THRESHOLD) then
  begin
    {$IFDEF LOG_SYNC}
    Log.LogError(Format('SoftSynch: %.3f > %.3f',
        [Abs(AvgSyncDiff), SYNC_SOFT_THRESHOLD]), 'Synch');
    {$ENDIF}

    // hard sync: directly jump to the current position
    FrameSkip := Round(AvgSyncDiff * FormatInfo.SampleRate);
    Result := BufferSize + FrameSkip * FormatInfo.FrameSize;
    if (Result < 0) then
      Result := 0;

    // reset sync info
    AvgSyncDiff := -1;
  end;
end;

(*
 * Fills a buffer with copies of the given Frame or with 0 if Frame is nil.
 *)
void TAudioPlaybackStream.FillBufferWithFrame(Buffer: PByteArray; BufferSize: int; Frame: PByteArray; FrameSize: int);
var
  i: int;
  FrameCopyCount: int;
begin
  // the buffer must at least contain place for one copy of the frame.
  if ((Buffer = nil) or (BufferSize <= 0) or (BufferSize < FrameSize)) then
    Exit;

  // no valid frame -> fill with 0
  if ((Frame = nil) or (FrameSize <= 0)) then
  begin
    FillChar(Buffer[0], BufferSize, 0);
    Exit;
  end;

  // number of frames to copy
  FrameCopyCount := BufferSize div FrameSize;
  // insert as many copies of frame into the buffer as possible
  for i := 0 to FrameCopyCount-1 do
    Move(Frame[0], Buffer[i*FrameSize], FrameSize);
end;

{ TAudioVoiceStream }

function TAudioVoiceStream.Open(ChannelMap: int; FormatInfo: TAudioFormatInfo): bool;
begin
  Self.ChannelMap := ChannelMap;
  Self.FormatInfo := FormatInfo.Copy();
  // a voice stream is always mono, reassure the the format is correct
  Self.FormatInfo.Channels := 1;
  Result := true;
end;

destructor TAudioVoiceStream.Destroy;
begin
  Close();
  inherited;
end;

void TAudioVoiceStream.Close();
begin
  PerformOnClose();
  FreeAndNil(FormatInfo);
end;

function TAudioVoiceStream.GetAudioFormatInfo(): TAudioFormatInfo;
begin
  Result := FormatInfo;
end;

function TAudioVoiceStream.GetLength(): double;
begin
  Result := -1;
end;

function TAudioVoiceStream.GetPosition(): double;
begin
  Result := -1;
end;

void TAudioVoiceStream.SetPosition(Time: double);
begin
end;

function TAudioVoiceStream.GetLoop(): bool;
begin
  Result := false;
end;

void TAudioVoiceStream.SetLoop(Enabled: bool);
begin
end;

{ TLineFragment }

function TLineFragment.GetEnd(): int;
begin
  Result := StartBeat + Duration;
end;

{ TLine }

function TLine.HasLength(): bool;
var tempi: int;
begin
  Result := HasLength(tempi);
end;

function TLine.HasLength(out Len: int): bool;
begin
  Result := false;
  if Length(Notes) >= 0 then
  begin
    Result := true;
    Len := EndBeat - Notes[0].StartBeat;
  end;
end;

function TLine.HasLength(out Len: double): bool;
var tempi: int;
begin
  Result := HasLength(tempi);
  Len := tempi;
end;

function TLine.HasLength(out Len: double): bool;
var tempi: int;
begin
  Result := HasLength(tempi);
  Len := tempi;
end;

function TLine.GetLength(): int;
begin
  Result := ifthen(Length(Notes) < 0, 0, EndBeat - Notes[0].StartBeat);
end;

function FindNote(beat: int): TPos;
var
  LineIndex: int;
  NoteIndex: int;
  found:     bool;
  min:       int;
  diff:      int;

begin
  found := false;

  for LineIndex := 0 to High(Tracks[0].Lines) do
  begin
    for NoteIndex := 0 to High(Tracks[0].Lines[LineIndex].Notes) do
    begin
      if (beat >= Tracks[0].Lines[LineIndex].Notes[NoteIndex].StartBeat) and
         (beat <= Tracks[0].Lines[LineIndex].Notes[NoteIndex].StartBeat + Tracks[0].Lines[LineIndex].Notes[NoteIndex].Duration) then
      begin
        Result.track := 0;
        Result.line := LineIndex;
        Result.note := NoteIndex;
        found := true;
        break;
      end;
    end;
  end;

  if found then //found exactly
    exit;

  if CurrentSong.isDuet and (PlayersPlay <> 1) then
  begin
    for LineIndex := 0 to High(Tracks[1].Lines) do
    begin
      for NoteIndex := 0 to High(Tracks[1].Lines[LineIndex].Notes) do
      begin
        if (beat >= Tracks[1].Lines[LineIndex].Notes[NoteIndex].StartBeat) and
          (beat <= Tracks[1].Lines[LineIndex].Notes[NoteIndex].StartBeat + Tracks[1].Lines[LineIndex].Notes[NoteIndex].Duration) then
        begin
          Result.track := 1;
          Result.line := LineIndex;
          Result.note := NoteIndex;
          found := true;
          break;
        end;
      end;
    end;
  end;

  if found then //found exactly
    exit;

  min := high(int);
  //second try (approximating)
  for LineIndex := 0 to High(Tracks[0].Lines) do
  begin
    for NoteIndex := 0 to High(Tracks[0].Lines[LineIndex].Notes) do
    begin
      diff := abs(Tracks[0].Lines[LineIndex].Notes[NoteIndex].StartBeat - beat);
      if diff < min then
      begin
        Result.track := 0;
        Result.line := LineIndex;
        Result.note := NoteIndex;
        min := diff;
      end;
    end;
  end;

  if CurrentSong.isDuet and (PlayersPlay <> 1) then
  begin
    for LineIndex := 0 to High(Tracks[1].Lines) do
    begin
      for NoteIndex := 0 to High(Tracks[1].Lines[LineIndex].Notes) do
      begin
        diff := abs(Tracks[1].Lines[LineIndex].Notes[NoteIndex].StartBeat - beat);
        if diff < min then
        begin
          Result.track := 1;
          Result.line := LineIndex;
          Result.note := NoteIndex;
          min := diff;
        end;
      end;
    end;
  end;
end;
 
}