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
 */

#include "../../switches.h"

#include <string>
#include <array>

#include "../../base/UTime.h"

namespace UScreenSingController
{
/*uses
  UAvatars,
  UCommon,
  UFiles,
  UGraphicClasses,
  UHookableEvent,
  UIni,
  ULyrics,
  UMenu,
  UMusic,
  UPath,
  UScreenSingView,
  USingScores,
  USkins,
  USongs,
  UTexture,
  UThemes,
  UTime,
  dglOpenGL,
  sdl2,
  SysUtils,
  TextGL;*/

class TLyricsSyncSource : public UTime::TSyncSource
{
    double GetClock() override;
};

class TMusicSyncSource : public UTime::TSyncSource
{
    double GetClock() override;
};

enum TTimebarMode
{
    tbmCurrent,   // current song position
    tbmRemaining, // remaining time
    tbmTotal      // total time
};

class TScreenSingController : public TMenu
    {
private:

    TPos StartNote, EndNote;

    void LoadNextSong();

    void SongError();
public:
    THookableEvent eSongLoaded; //< event is called after lyrics of a song are loaded on OnShow
    bool Paused; //pause Mod
    std::array<int, 2> NumEmptySentences;
    // views
    bool fShowVisualization;
	bool fShowBackground;

    IVideo fCurrentVideo;
	IVideo fVideoClip;
    TLyricsSyncSource fLyricsSync;
    TMusicSyncSource fMusicSync;
    TTimebarMode fTimebarMode;

    bool PlayMidi;

	bool removeVoice;
	bool fShowWebcam;

    int Act_Level;
    std::string Act_MD5Song;

    double MedleyStart, MedleyEnd;

    TLyricEngine Lyrics;
	  TLyricEngine LyricsDuetP1;
	  TLyricEngine LyricsDuetP2;

    // score manager:
    TSingScores Scores;

    //the song was sung to the end
    bool SungToEnd;

    //use pause
    bool SungPaused;

    // some settings to be set by plugins
    struct
    {
        bool Finish; //< if true, screen will finish on next draw

        bool OscilloscopeVisible; //< shows or hides oscilloscope
        bool LyricsVisible; //< shows or hides lyrics
        int NotesVisible; //< if bit[playernum] is set the notes for the specified player are visible. By default all players notes are visible
        bool ScoresVisible; //< shows or hides scores
        bool AvatarsVisible; //< shows or hides avatars
        bool TimeBarVisible; //< shows or hides timebar
        bool InputVisible; //< shows or hides input (sung notes)

        int PlayerEnabled; //< defines whether a player can score atm

        bool SoundEnabled; //< mute or unmute sound
    } Settings;

    // MIDI
    int ChannelOff;

    bool MidiFadeIn;
	bool MidiFadeOut;
    uint32_t FadeTime;

    uint32_t InfoMessageBG;
    uint32_t InfoMessageText;

    uint32_t MessageTime;
    uint32_t MessageTimeFade;

    bool TextMedleyFadeOut;
    uint32_t TextMedleyFadeTime;

    // names
    std::array<std::string, IMaxPlayerCount> PlayerNames//: array[1..IMaxPlayerCount] of UTF8String;
    PlayerDuetNames:array [1..IMaxPlayerCount] of UTF8String;

    Tex_Background: TTexture;
    FadeOut: bool;

    void ClearSettings;
    void ApplySettings; //< applies changes of settings record
    void EndSong;

    constructor Create; override;
    void OnShow; override;
    void OnShowFinish; override;
    void OnHide; override;
    function Draw: bool; override;

    function ParseInput(PressedKey: uint32_t; CharCode: UCS4Char;
      PressedDown: bool): bool; override;

    function FinishedMusic: bool;

    void AutoSendScore;
    void AutoSaveScore;

    void Finish; virtual;
    void Pause; // toggle pause
    void UpdateMedleyStats(medley_end: bool);
    void OnSentenceEnd(Track: int; SentenceIndex: uint32_t);     // for linebonus + singbar
    void OnSentenceChange(Track: int; SentenceIndex: uint32_t);  // for golden notes
  };

var screenSingViewRef: TScreenSingView;
    TotalTime:              double;

const std::string ID = "ID_022";   //for help system

implementation

uses
  UDatabase,
  UDisplay,
  UDLLManager,
  UDraw,
  UGraphic,
  UHelp,
  ULanguage,
  ULog,
  UNote,
  URecord,
  UParty,
  UPathUtils,
  USong,
  UUnicodeUtils,
  UWebcam,
  UWebSDK,
  Classes,
  Math;

const
  MAX_MESSAGE = 3;

// method for input parsing. if false is returned, getnextwindow
// should be checked to know the next window to load;

function TScreenSingController.ParseInput(PressedKey: uint32_t; CharCode: UCS4Char;
  PressedDown: bool): bool;
var
  SDL_ModState: word;
  i1:           int;
  Color:        TRGB;
begin
  Result := true;
  if (PressedDown) then
  begin // key down

    SDL_ModState := SDL_GetModState and (KMOD_LSHIFT + KMOD_RSHIFT
    + KMOD_LCTRL + KMOD_RCTRL + KMOD_LALT  + KMOD_RALT);

    // check normal keys
    case UCS4UpperCase(CharCode) of
      Ord('Q'):
      begin
        // when not ask before exit then finish now
        if (Ini.AskbeforeDel <> 1) then
          Finish
        // else just pause and let the popup make the work
        else if not Paused then
          Pause;

        Result := false;
        Exit;
      }

      //Restart and pause song
      Ord('R'):
      begin
        if ScreenSong.Mode = smMedley then Exit;
        for i1 := 0 to High(Player) do
        with Player[i1] do
        begin
          Score          := 0;
          ScoreLine      := 0;
          ScoreGolden    := 0;

          ScoreInt       := 0;
          ScoreLineInt   := 0;
          ScoreGoldenInt := 0;
          ScoreTotalInt  := 0;

          ScoreLast      := 0;

          LastSentencePerfect := false;
        }
        AudioPlayback.SetPosition(CurrentSong.Start);
        if (Assigned(fCurrentVideo)) then
           fCurrentVideo.Position := CurrentSong.VideoGAP + CurrentSong.Start;// + (CurrentSong.gap / 1000.0 - 5.0);
        Scores.KillAllPopUps;
        // setup score manager
        Scores.ClearPlayers; // clear old player values
        Color.R := 0;
        Color.G := 0;
        Color.B := 0;

        // add new players
        for i1 := 0 to PlayersPlay - 1 do
        begin
          Scores.AddPlayer(Tex_ScoreBG[i1], Color);
        }
        LyricsState.SetCurrentTime(CurrentSong.Start);
        Scores.Init;
        Exit;
      }

      // show visualization
      Ord('V'):
      begin
        if fShowWebcam then
        begin
          Webcam.Release;
          fShowWebCam:=false;
        }
        if ((fShowBackground = true) and (Ini.VideoEnabled = 1) and CurrentSong.Video.IsSet())
                             or (fShowVisualization and not CurrentSong.Background.IsSet()) then //switch to video
        begin
          Log.LogStatus('decided to switch to video', 'UScreenSing.ParseInput');
          fShowBackground := false;
          fCurrentVideo := nil;
          fShowVisualization := false;
          fCurrentVideo := fVideoClip;
          if (Assigned(fCurrentVideo)) then
               fCurrentVideo.Position := CurrentSong.VideoGAP + CurrentSong.Start + AudioPlayback.Position;
          Log.LogStatus('finished switching to video', 'UScreenSing.ParseInput');
        }
        else
        begin
          if fShowVisualization and CurrentSong.Background.IsSet() then
          begin //switch to Background only
            Log.LogStatus('decided to switch to background', 'UScreenSing.ParseInput');
            fShowBackground := true;
            fCurrentVideo := nil;
            fShowVisualization := false;
            Log.LogStatus('finished switching to background', 'UScreenSing.ParseInput');
          }
          else
          begin //Video is currently visible, change to visualization
            Log.LogStatus('decided to switch to visualization', 'UScreenSing.ParseInput');
            fShowVisualization := true;
            fCurrentVideo := Visualization.Open(PATH_NONE);
            fCurrentVideo.play;
            Log.LogStatus('finished switching to visualization', 'UScreenSing.ParseInput');
          }
        }
        Exit;
      }

      // show Webcam
      Ord('W'):
      begin
        if (fShowWebCam = false) then
        begin
          fCurrentVideo := nil;
          fShowVisualization := false;
          fShowBackground := false;
          Webcam.Restart;
          if (Webcam.Capture = nil) then
          begin
            fShowWebCam := false;
            fShowBackground := true;
            ScreenPopupError.ShowPopup(Language.Translate('SING_OPTIONS_WEBCAM_NO_WEBCAM'))
          }
          else
            fShowWebCam := true;
        //  ChangeEffectLastTick := SDL_GetTicks;
        //  SelectsS[WebcamParamsSlide].Visible := true;
        //  LastTickFrame := SDL_GetTicks;
        }
        else
        begin
          Webcam.Release;
          fShowWebCam:=false;
        }

        Exit;
      }

      // pause
      Ord('P'):
      begin
        Pause;
        Exit;
      }

      // toggle time display: running time/remaining time/total time / SHIFT: show/hide time bar
      Ord('T'):
      begin
        if (SDL_ModState = KMOD_LSHIFT) then
        begin
          if (ScreenSong.Mode <> smNormal) and (ScreenSong.Mode <> smMedley) then
            Exit;

          ScreenSing.Settings.TimeBarVisible := not ScreenSing.Settings.TimeBarVisible;
          Exit;
        }
        else
        begin
          if (fTimebarMode = High(TTimebarMode)) then
            fTimebarMode := Low(TTimebarMode)
          else
            Inc(fTimebarMode);

          Ini.SingTimebarMode := Ord(fTimebarMode);
          Ini.SaveSingTimebarMode;
          Exit;
        }
      }

      // skip intro / SHIFT: show/hide score display
      Ord('S'):
      begin
        if (SDL_ModState = KMOD_LSHIFT) then
        begin
          if (ScreenSong.Mode <> smNormal) and (ScreenSong.Mode <> smMedley) then
            Exit;

          ScreenSing.Settings.ScoresVisible := not ScreenSing.Settings.ScoresVisible;
          Exit;
        }
        else
        begin
          if (AudioPlayback.Position < CurrentSong.gap / 1000 - 6) then
          begin
            AudioPlayback.SetPosition(CurrentSong.gap / 1000.0 - 5.0);
              if (Assigned(fCurrentVideo)) then
                 fCurrentVideo.Position := CurrentSong.VideoGAP + CurrentSong.Start + (CurrentSong.gap / 1000.0 - 5.0);
          }
          Exit;
        }
      }

      // SHIFT: show/hide oscilloscope
      Ord('O'):
      begin
        if (SDL_ModState = KMOD_LSHIFT) then
        begin
          if (ScreenSong.Mode <> smNormal) and (ScreenSong.Mode <> smMedley) then
            Exit;

          ScreenSing.Settings.OscilloscopeVisible := not ScreenSing.Settings.OscilloscopeVisible;
          Exit;
        }
      }

      // SHIFT: show/hide notes
      Ord('N'):
      begin
        if (SDL_ModState = KMOD_LSHIFT) then
        begin
          if (ScreenSong.Mode <> smNormal) and (ScreenSong.Mode <> smMedley) then
            Exit;

          ScreenSing.Settings.NotesVisible := ifthen(ScreenSing.Settings.NotesVisible = 0, High(Integer), 0);
          Exit;
        }
      }

      // SHIFT: show/hide notes
      Ord('L'):
      begin
        if (SDL_ModState = KMOD_LSHIFT) then
        begin
          if (ScreenSong.Mode <> smNormal) and (ScreenSong.Mode <> smMedley) then
            Exit;

          ScreenSing.Settings.LyricsVisible := not ScreenSing.Settings.LyricsVisible;
          Exit;
        }
      }

      // SHIFT: show/hide avatars and player names
      Ord('A'):
      begin
        if (SDL_ModState = KMOD_LSHIFT) then
        begin
          if (ScreenSong.Mode <> smNormal) and (ScreenSong.Mode <> smMedley) then
            Exit;

          ScreenSing.Settings.AvatarsVisible := not ScreenSing.Settings.AvatarsVisible;
          Exit;
        }
      }

      // SHIFT: show/hide microphone input/sung notes
      Ord('I'):
      begin
        if (SDL_ModState = KMOD_LSHIFT) then
        begin
          if (ScreenSong.Mode <> smNormal) and (ScreenSong.Mode <> smMedley) then
            Exit;

          ScreenSing.Settings.InputVisible := not ScreenSing.Settings.InputVisible;
          Exit;
        }
      }

      // SHIFT: show/hide (toggle) all display elements
      Ord('H'):
      begin
        if (SDL_ModState = KMOD_LSHIFT) then
        begin
          if (ScreenSong.Mode <> smNormal) and (ScreenSong.Mode <> smMedley) then
            Exit;

          ScreenSing.Settings.NotesVisible        := ifthen(ScreenSing.Settings.NotesVisible = 0, High(Integer), 0);
          // synchronize show/hide of all other elements with NotesVisible
          ScreenSing.Settings.LyricsVisible       := (ScreenSing.Settings.NotesVisible <> 0);
          ScreenSing.Settings.ScoresVisible       := (ScreenSing.Settings.NotesVisible <> 0);
          ScreenSing.Settings.AvatarsVisible      := (ScreenSing.Settings.NotesVisible <> 0);
          ScreenSing.Settings.TimeBarVisible      := (ScreenSing.Settings.NotesVisible <> 0);
          ScreenSing.Settings.InputVisible        := (ScreenSing.Settings.NotesVisible <> 0);
          ScreenSing.Settings.OscilloscopeVisible := (ScreenSing.Settings.NotesVisible <> 0);
          Exit;
        }
      }
    }

    // check special keys
    case PressedKey of
      SDLK_ESCAPE,
      SDLK_BACKSPACE:
      begin
        // record sound hack:
        //Sound[0].BufferLong
        if ScreenSong.Mode = smMedley then
          PlaylistMedley.NumMedleySongs := PlaylistMedley.CurrentMedleySong;

        Finish;
        FadeOut := true;
        AudioPlayback.PlaySound(SoundLib.Back);
      }

      SDLK_SPACE:
      begin
        Pause;
      }

      SDLK_RIGHT:
      begin
        if (SDL_ModState = KMOD_LCTRL) then // seek 5 seconds forward
        AudioPlayback.SetPosition(AudioPlayback.Position + 5.0);
        if (Assigned(fCurrentVideo)) then
          fCurrentVideo.Position := AudioPlayback.Position + 5.0;
      }

      SDLK_LEFT:
      begin
        if (SDL_ModState = KMOD_LCTRL) then // seek 5 seconds backward and reset scores to avoid cheating
	begin
	if (AudioPlayback.Position < 20.0) then
	  exit;
        for i1 := 0 to High(Player) do
        with Player[i1] do
        begin
          Score          := 0;
          ScoreLine      := 0;
          ScoreGolden    := 0;

          ScoreInt       := 0;
          ScoreLineInt   := 0;
          ScoreGoldenInt := 0;
          ScoreTotalInt  := 0;

          ScoreLast      := 0;

          LastSentencePerfect := false;
        }
        Scores.KillAllPopUps;
        // setup score manager
        Scores.ClearPlayers; // clear old player values
        // add new players
        for i1 := 0 to PlayersPlay - 1 do
        begin
          Scores.AddPlayer(Tex_ScoreBG[i1], Color);
        }
	Scores.Init;

        AudioPlayback.SetPosition(AudioPlayback.Position - 5.0);
	LyricsState.SetCurrentTime(AudioPlayback.Position - 5.0);
	Lyrics.Clear(CurrentSong.BPM[0].BPM, CurrentSong.Resolution);
	LyricsState.UpdateBeats();
        if (Assigned(fCurrentVideo)) then
          fCurrentVideo.Position := AudioPlayback.Position - 5.0;
        }
      }

      SDLK_TAB:
      begin
        if (SDL_ModState = KMOD_LCTRL) then // change visualization preset
        begin
          if fShowVisualization then
            fCurrentVideo.Position := now; // move to a random position

          if (fShowWebcam) then
          begin
            if (Ini.WebCamEffect < 10) then
              Ini.WebCamEffect := Ini.WebCamEffect + 1
            else
              Ini.WebCamEffect := 0;
          }
        }
        else // show help popup
        begin
          if not paused then
            Pause;
          ScreenPopupHelp.ShowPopup();
        }
      }
    }
  }
}

void TScreenSingController.Pause;
begin
  if (not Paused) then  // enable pause
  begin
    // pause time
    Paused := true;

    LyricsState.Pause();

    // pause music
      AudioPlayback.Pause;

    // pause video
    if (fCurrentVideo <> nil) then
      fCurrentVideo.Pause;

  }
  else              // disable pause
  begin
    LyricsState.Start();

    // play music
      AudioPlayback.Play;

    // video
    if (fCurrentVideo <> nil) then
      fCurrentVideo.Pause;

    Paused := false;
  }
}

constructor TScreenSingController.Create;
var
  Col: array [1..6] of TRGB;
  I: int;
  Color: uint32_t;
begin
  inherited Create;
  ScreenSing := self;
  screenSingViewRef := TScreenSingView.Create();

  ClearSettings;
}

void TScreenSingController.OnShow;
var
  BadPlayer: int;
  Col, ColP1, ColP2: TRGB;
  I: int;
begin
  inherited;

  Log.LogStatus('Begin', 'OnShow');

  if not Help.SetHelpID(ID) then
    Log.LogWarn('No Entry for Help-ID ' + ID, 'ScreenSingController');

  FadeOut := false;

  screenSingViewRef.CloseMessage();

  //the song was sung to the }
  SungToEnd := false;
  SungPaused := false;

  ClearSettings;
  Party.CallBeforeSing;

  // prepare players
  SetLength(Player, PlayersPlay);

  //Reset Player Medley stats
  if (ScreenSong.Mode = smMedley) then
  begin
    PlaylistMedley.CurrentMedleySong:=1;

    PlaylistMedley.NumPlayer := PlayersPlay;
    SetLength(PlaylistMedley.Stats, 0);
  }

  fTimebarMode := TTimebarMode(Ini.SingTimebarMode);

  if (ScreenSong.Mode = smMedley) then
    CatSongs.Selected := PlaylistMedley.Song[PlaylistMedley.CurrentMedleySong-1];

  CurrentSong := CatSongs.Song[CatSongs.Selected];

  {for I := 0 to High(screenSingViewRef.StaticDuet) do
    Statics[screenSingViewRef.StaticDuet[I]].Visible := CurrentSong.isDuet and (PlayersPlay > 1);}
  Statics[screenSingViewRef.StaticLyricsBarDuet].Visible := CurrentSong.isDuet and (PlayersPlay > 1);

  Statics[screenSingViewRef.SongNameStatic].Visible := false;
  Text[screenSingViewRef.SongNameText].Visible := false;

  BadPlayer := AudioInputProcessor.CheckPlayersConfig(PlayersPlay);
  if (BadPlayer <> 0) then
  begin
    ScreenPopupError.ShowPopup(
        Format(Language.Translate('ERROR_PLAYER_NO_DEVICE_ASSIGNMENT'),
        [BadPlayer]));
  }

  if (CurrentSong.isDuet) then
  begin
    if (PlayersPlay = 4) then
    begin
      screenSingViewRef.ColPlayer[0] := GetPlayerColor(Ini.PlayerColor[0]);
      screenSingViewRef.ColPlayer[1] := GetPlayerColor(Ini.PlayerColor[1]);
      screenSingViewRef.ColPlayer[2] := GetPlayerColor(Ini.PlayerColor[2]);
      screenSingViewRef.ColPlayer[3] := GetPlayerColor(Ini.PlayerColor[3]);
    }
  }

  // set custom options
  if (CurrentSong.isDuet) and (PlayersPlay <> 1) then
  begin
    ColP1 := GetPlayerColor(Ini.PlayerColor[0]);
    ColP2 := GetPlayerColor(Ini.PlayerColor[1]);

    LyricsDuetP1.FontFamily := Ini.LyricsFont;
    LyricsDuetP2.FontFamily := Ini.LyricsFont;

    LyricsDuetP1.FontStyle := Ini.LyricsStyle;
    LyricsDuetP2.FontStyle := Ini.LyricsStyle;

    // set custom options
    LyricsDuetP1.LineColor_act.R := ColP1.R;
    LyricsDuetP1.LineColor_act.G := ColP1.G;
    LyricsDuetP1.LineColor_act.B := ColP1.B;
    LyricsDuetP1.LineColor_act.A := 1;

    LyricsDuetP2.LineColor_act.R := ColP2.R;
    LyricsDuetP2.LineColor_act.G := ColP2.G;
    LyricsDuetP2.LineColor_act.B := ColP2.B;
    LyricsDuetP2.LineColor_act.A := 1;

    if (Ini.JukeboxActualLineColor = High(UIni.IActualLineColor)) then
      Col := GetJukeboxLyricOtherColor(1)
    else
      Col := GetLyricGrayColor(Ini.JukeboxActualLineColor);
    LyricsDuetP1.LineColor_en.R := Col.R;
    LyricsDuetP1.LineColor_en.G := Col.G;
    LyricsDuetP1.LineColor_en.B := Col.B;
    LyricsDuetP1.LineColor_en.A := 1;

    LyricsDuetP2.LineColor_en.R := Col.R;
    LyricsDuetP2.LineColor_en.G := Col.G;
    LyricsDuetP2.LineColor_en.B := Col.B;
    LyricsDuetP2.LineColor_en.A := 1;

    if (Ini.JukeboxNextLineColor = High(UIni.INextLineColor)) then
      Col := GetJukeboxLyricOtherColor(2)
    else
      Col := GetLyricGrayColor(Ini.JukeboxNextLineColor);
    LyricsDuetP1.LineColor_dis.R := Col.R;
    LyricsDuetP1.LineColor_dis.G := Col.G;
    LyricsDuetP1.LineColor_dis.B := Col.B;
    LyricsDuetP1.LineColor_dis.A := 1;

    LyricsDuetP2.LineColor_dis.R := Col.R;
    LyricsDuetP2.LineColor_dis.G := Col.G;
    LyricsDuetP2.LineColor_dis.B := Col.B;
    LyricsDuetP2.LineColor_dis.A := 1;

  }
  else
  begin

    Lyrics.FontFamily := Ini.LyricsFont;
    Lyrics.FontStyle := Ini.LyricsStyle;

    if (Ini.JukeboxSingLineColor = High(UIni.ISingLineColor)) then
      Col := GetJukeboxLyricOtherColor(0)
    else
      Col := GetLyricColor(Ini.JukeboxSingLineColor);
    Lyrics.LineColor_act.R := Col.R;
    Lyrics.LineColor_act.G := Col.G;
    Lyrics.LineColor_act.B := Col.B;
    Lyrics.LineColor_act.A := 1;

    if (Ini.JukeboxActualLineColor = High(UIni.IActualLineColor)) then
      Col := GetJukeboxLyricOtherColor(1)
    else
      Col := GetLyricGrayColor(Ini.JukeboxActualLineColor);
    Lyrics.LineColor_en.R := Col.R;
    Lyrics.LineColor_en.G := Col.G;
    Lyrics.LineColor_en.B := Col.B;
    Lyrics.LineColor_en.A := 1;

    if (Ini.JukeboxNextLineColor = High(UIni.INextLineColor)) then
      Col := GetJukeboxLyricOtherColor(2)
    else
      Col := GetLyricGrayColor(Ini.JukeboxNextLineColor);
    Lyrics.LineColor_dis.R := Col.R;
    Lyrics.LineColor_dis.G := Col.G;
    Lyrics.LineColor_dis.B := Col.B;
    Lyrics.LineColor_dis.A := 1;
  }

  // deactivate pause
  Paused := false;

  LoadNextSong();

  Log.LogStatus('End', 'OnShow');
}

void TScreenSingController.onShowFinish;
var
  I, PlayerIndex: int;
begin
  // hide cursor on singscreen show
  Display.SetCursor;

  // clear the scores of all players
  for PlayerIndex := 0 to High(Player) do
    with Player[PlayerIndex] do
    begin
      Score          := 0;
      ScoreLine      := 0;
      ScoreGolden    := 0;

      ScoreInt       := 0;
      ScoreLineInt   := 0;
      ScoreGoldenInt := 0;
      ScoreTotalInt  := 0;

      ScoreLast      := 0;

      LastSentencePerfect := false;
    }

  // prepare music
  // Important: AudioPlayback must not be initialized in onShow() as TScreenSong
  // uses stops AudioPlayback in onHide() which interferes with TScreenSings onShow.
  PlayMidi := false;
  MidiFadeIn := false;

  AudioPlayback.Open(CurrentSong.Path.Append(CurrentSong.Mp3));
  if ScreenSong.Mode = smMedley then
    AudioPlayback.SetVolume(0.1)
  else
    AudioPlayback.SetVolume(1.0);
  //AudioPlayback.Position := CurrentSong.Start;
  AudioPlayback.Position := LyricsState.GetCurrentTime();


  // set time
  if (CurrentSong.Finish > 0) then
    LyricsState.TotalTime := CurrentSong.Finish / 1000
  else
  {
    LyricsState.TotalTime := AudioPlayback.Length;
  }

  LyricsState.UpdateBeats();

  // synchronize music
  if Ini.SyncTo = Ord(stLyrics) then
    AudioPlayback.SetSyncSource(fLyricsSync)
  else
    AudioPlayback.SetSyncSource(nil);

  // synchronize lyrics (do not set this before AudioPlayback is initialized)
  if Ini.SyncTo = Ord(stMusic) then
    LyricsState.SetSyncSource(fMusicSync)
  else
    LyricsState.SetSyncSource(nil);

  // start lyrics
  LyricsState.Start(true);

  // start music
  if ScreenSong.Mode = smMedley then
    AudioPlayback.FadeIn(CurrentSong.Medley.FadeIn_time, 1.0)
  else
    AudioPlayback.Play();

  // Send Score
  Act_MD5Song := CurrentSong.MD5;
  Act_Level := Ini.PlayerLevel[0];

  // start timer
  CountSkipTimeSet;

}

void TScreenSingController.SongError();
var
  I, len:  int;

begin
  if ScreenSong.Mode <> smMedley then
  begin
    // error loading song -> go back to previous screen and show some error message
    Display.AbortScreenChange;

    // select new song in party mode
    if ScreenSong.Mode = smPartyClassic then
      ScreenSong.SelectRandomSong();

    if (Length(CurrentSong.LastError) > 0) then
      ScreenPopupError.ShowPopup(Format(Language.Translate(CurrentSong.LastError), [CurrentSong.ErrorLineNo]))
    else
      ScreenPopupError.ShowPopup(Language.Translate('ERROR_CORRUPT_SONG'));
    // FIXME: do we need this?
    CurrentSong.Path := CatSongs.Song[CatSongs.Selected].Path;
    Exit;
  }
  else
  begin
    if (PlaylistMedley.CurrentMedleySong<PlaylistMedley.NumMedleySongs) then
    begin
      //Error Loading Song in Medley Mode -> skip actual Medley Song an go on if possible
      len := Length(PlaylistMedley.Song);
      for I := PlaylistMedley.CurrentMedleySong-1 to len - 1 do
        PlaylistMedley.Song[I] := PlaylistMedley.Song[I+1];

      SetLength(PlaylistMedley.Song, Len-1);
      Dec(PlaylistMedley.NumMedleySongs);
      LoadNextSong;
      Exit;
    }
    else
    begin
      if (PlaylistMedley.NumMedleySongs=1) then
      begin
        //Error Loading Song in Medley Mode -> Go back to Song Screen and Show some Error Message
        Display.AbortScreenChange;

        // select new song in party mode
        if ScreenSong.Mode = smPartyClassic then
          ScreenSong.SelectRandomSong();

        if (Length(CurrentSong.LastError) > 0) then
          ScreenPopupError.ShowPopup(Format(Language.Translate(CurrentSong.LastError), [CurrentSong.ErrorLineNo]))
        else
          ScreenPopupError.ShowPopup(Language.Translate('ERROR_CORRUPT_SONG'));

        // FIXME: do we need this?
        CurrentSong.Path := CatSongs.Song[CatSongs.Selected].Path;
        Exit;
      }
      else
      begin
        //Error Loading Song in Medley Mode -> Finish actual round
        len := Length(PlaylistMedley.Song);
        SetLength(PlaylistMedley.Song, len-1);
        Dec(PlaylistMedley.NumMedleySongs);
        Finish;
        Exit;
      }
    }
  }
}

void TScreenSingController.LoadNextSong();
var
  Color:      TRGB;
  Index:      int;
  VideoFile:  IPath;
  BgFile:     IPath;
  success:    bool;

begin
  // background texture (garbage disposal)
  if (Tex_Background.TexNum > 0) then
  begin
    glDeleteTextures(1, PGLuint(@Tex_Background.TexNum));
    Tex_Background.TexNum := 0;
  }

  // reset video playback engine
  fCurrentVideo := nil;

  // setup score manager
  Scores.ClearPlayers; // clear old player values
  Color.R := 0;
  Color.G := 0;
  Color.B := 0;

  // add new players
  for Index := 0 to PlayersPlay - 1 do
  begin
    Scores.AddPlayer(Tex_ScoreBG[Index], Color);
  }

  Scores.Init; // get positions for players

  // FIXME: sets path and filename to ''
  ResetSingTemp;

  PlaylistMedley.ApplausePlayed := false;

  if ScreenSong.Mode = smMedley then
  begin
    if length(PlaylistMedley.Song) >= PlaylistMedley.CurrentMedleySong then
    begin
      CatSongs.Selected := PlaylistMedley.Song[PlaylistMedley.CurrentMedleySong-1];
      //Music.Open(CatSongs.Song[CatSongs.Selected].Path + CatSongs.Song[CatSongs.Selected].Mp3);
    }
    else
    begin
      SongError;
      Exit;
    }
  }

  CurrentSong := CatSongs.Song[CatSongs.Selected];
  success := false;
  // FIXME: bad style, put the try-except into loadsong() and not here
  try
    // check if file is xml
    if CurrentSong.FileName.GetExtension.ToUTF8 = '.xml' then
      success := CurrentSong.AnalyseXML and CurrentSong.LoadXMLSong()
    else
      success := CurrentSong.Analyse(false, ScreenSong.DuetChange); // and CurrentSong.LoadSong();
  except
    on E: EInOutError do Log.LogWarn(E.Message, 'TScreenSing.LoadNextSong');
  }

  if (not success) then
  begin
    SongError();
    Exit;
  }

  // Set up Medley timings
  if ScreenSong.Mode = smMedley then
  begin
    CurrentSong.SetMedleyMode();

    if (PlaylistMedley.NumMedleySongs > 1) then
      Text[screenSingViewRef.SongNameText].Text := IntToStr(PlaylistMedley.CurrentMedleySong) +
        '/' + IntToStr(PlaylistMedley.NumMedleySongs) + ': ' +
        CurrentSong.Artist + ' - ' + CurrentSong.Title
    else
      Text[screenSingViewRef.SongNameText].Text := CurrentSong.Artist + ' - ' + CurrentSong.Title;

    //medley start and } timestamps
    StartNote := FindNote(CurrentSong.Medley.StartBeat - round(CurrentSong.BPM[0].BPM*CurrentSong.Medley.FadeIn_time/60));
    MedleyStart := GetTimeFromBeat(Tracks[0].Lines[StartNote.line].Notes[0].StartBeat);

    //check Medley-Start
    if (MedleyStart+CurrentSong.Medley.FadeIn_time*0.5>GetTimeFromBeat(CurrentSong.Medley.StartBeat)) then
      MedleyStart := GetTimeFromBeat(CurrentSong.Medley.StartBeat) - CurrentSong.Medley.FadeIn_time;
    if MedleyStart<0 then
      MedleyStart := 0;

    MedleyEnd := GetTimeFromBeat(CurrentSong.Medley.EndBeat) + CurrentSong.Medley.FadeOut_time;
  }

  {*
   * == Background ==
   * We have five types of backgrounds:
   *   + Blank        : Nothing has been set, this is our fallback
   *   + Picture      : Picture has been set, and exists - otherwise we fallback
   *   + Video        : Video has been set, and exists - otherwise we fallback
   *   + Visualization: + Off                 : No visualization
   *                    + WhenNoVideo         : Overwrites blank and picture
   *                    + WhenNoVideoAndImage : Overwrites blank
   *                    + On                  : Overwrites blank, picture and video
   *}

  {*
   * set background to: video
   * Note: ffmpeg / this is also used for many background formats"
   *}
  fShowVisualization := false;
  VideoFile := CurrentSong.Path.Append(CurrentSong.Video);
  if (Ini.VideoEnabled = 1) and CurrentSong.Video.IsSet() and VideoFile.IsFile then
  begin
    fVideoClip := VideoPlayback.Open(VideoFile);
    fCurrentVideo := fVideoClip;
    if (fVideoClip <> nil) then
    begin
      fShowVisualization := false;
      if ScreenSong.Mode = smMedley then
        fCurrentVideo.Position := CurrentSong.VideoGAP + MedleyStart
      else
        fCurrentVideo.Position := CurrentSong.VideoGAP + CurrentSong.Start;
      fCurrentVideo.Play;
    }
  }

  {*
   * set background to: picture
   *}
  if (CurrentSong.Background.IsSet) then
  begin
    BgFile := CurrentSong.Path.Append(CurrentSong.Background);
    try
      Tex_Background := Texture.LoadTexture(BgFile);
    except
      Log.LogError('Background could not be loaded: ' + BgFile.ToNative);
      Tex_Background.TexNum := 0;
    }
  }
  else
  begin
    Tex_Background.TexNum := 0;
  }

  {*
   * set background to: visualization (Overwrites all)
   *}
  if (TVisualizerOption(Ini.VisualizerOption) in [voOn]) then
  begin
    fShowVisualization := true;
    fCurrentVideo := Visualization.Open(PATH_NONE);
    if (fCurrentVideo <> nil) then
      fCurrentVideo.Play;
  }

  {*
   * set background to: visualization (if video and image is not set)
   *}
  else if (TVisualizerOption(Ini.VisualizerOption) in [voWhenNoVideoAndImage]) and
     (not CurrentSong.Background.IsSet) and (fVideoClip = nil) then
  begin
    fShowVisualization := true;
    fCurrentVideo := Visualization.Open(PATH_NONE);
    if fCurrentVideo <> nil then
      fCurrentVideo.Play;
  }

  {*
   * set background to: visualization (Videos are still shown)
   *}
  else if ((TVisualizerOption(Ini.VisualizerOption) in [voWhenNoVideo]) and
     (fVideoClip = nil)) then
  begin
    fShowVisualization := true;
    fCurrentVideo := Visualization.Open(PATH_NONE);
    if fCurrentVideo <> nil then
      fCurrentVideo.Play;
  }

  // prepare lyrics timer
  LyricsState.Reset();

  if ScreenSong.Mode = smMedley then
  begin
    LyricsState.SetCurrentTime(MedleyStart);
    LyricsState.StartTime := CurrentSong.Gap;
    LyricsState.TotalTime := MedleyEnd;
  }
  else
  begin
    LyricsState.SetCurrentTime(CurrentSong.Start);
    LyricsState.StartTime := CurrentSong.Gap;
    if CurrentSong.Finish > 0 then
      LyricsState.TotalTime := CurrentSong.Finish / 1000
    else
      LyricsState.TotalTime := AudioPlayback.Length;
  }

  LyricsState.UpdateBeats();

  // prepare and start voice-capture
  AudioInput.CaptureStart;

  // main text
  Lyrics.Clear(CurrentSong.BPM[0].BPM, CurrentSong.Resolution);
  LyricsDuetP1.Clear(CurrentSong.BPM[0].BPM, CurrentSong.Resolution);
  LyricsDuetP2.Clear(CurrentSong.BPM[0].BPM, CurrentSong.Resolution);

  if (CurrentSong.isDuet) and (PlayersPlay <> 1) then
  begin
    // initialize lyrics by filling its queue
    while (not LyricsDuetP1.IsQueueFull) and
          (LyricsDuetP1.LineCounter <= High(Tracks[0].Lines)) do
    begin
      LyricsDuetP1.AddLine(@Tracks[0].Lines[LyricsDuetP1.LineCounter]);
    }

    // initialize lyrics by filling its queue
    while (not LyricsDuetP2.IsQueueFull) and
          (LyricsDuetP2.LineCounter <= High(Tracks[1].Lines)) do
    begin
      LyricsDuetP2.AddLine(@Tracks[1].Lines[LyricsDuetP2.LineCounter]);
    }
  }
  else
  begin
    // initialize lyrics by filling its queue
    while (not Lyrics.IsQueueFull) and
          (Lyrics.LineCounter <= High(Tracks[0].Lines)) do
    begin
      Lyrics.AddLine(@Tracks[0].Lines[Lyrics.LineCounter]);
    }
  }

  // kill all stars not killed yet (goldenstarstwinkle mod)
  GoldenRec.SentenceChange(0);
  if (CurrentSong.isDuet) and (PlayersPlay <> 1) then
    GoldenRec.SentenceChange(1);

  // set position of line bonus - line bonus }
  // set number of empty sentences for line bonus
  NumEmptySentences[0] := 0;
  NumEmptySentences[1] := 0;

  if (CurrentSong.isDuet) and (PlayersPlay <> 1) then
  begin
    for Index := Low(Tracks[1].Lines) to High(Tracks[1].Lines) do
    if Tracks[1].Lines[Index].ScoreValue = 0 then
      Inc(NumEmptySentences[1]);

    for Index := Low(Tracks[0].Lines) to High(Tracks[0].Lines) do
    if Tracks[0].Lines[Index].ScoreValue = 0 then
      Inc(NumEmptySentences[0]);
  }
  else
  begin
    for Index := Low(Tracks[0].Lines) to High(Tracks[0].Lines) do
      if Tracks[0].Lines[Index].ScoreValue = 0 then
        Inc(NumEmptySentences[0]);
  }

  eSongLoaded.CallHookChain(False);

  if (ScreenSong.Mode = smMedley) and (PlaylistMedley.CurrentMedleySong>1) then
    onShowFinish;
}

void TScreenSingController.ClearSettings;
begin
  Settings.Finish := False;
  Settings.OscilloscopeVisible := bool(Ini.Oscilloscope);
  Settings.LyricsVisible := True;
  Settings.NotesVisible := high(Integer);
  Settings.ScoresVisible := True;
  Settings.AvatarsVisible := True;
  Settings.TimeBarVisible := True;
  Settings.InputVisible := True;
  Settings.PlayerEnabled := high(Integer);
  Settings.SoundEnabled := True;
}

{ applies changes of settings record }
void TScreenSingController.ApplySettings;
begin
  //
}

void TScreenSingController.EndSong;
begin
  Settings.Finish := True;
}

void TScreenSingController.OnHide;
begin
  // background texture
  if Tex_Background.TexNum > 0 then
  begin
    glDeleteTextures(1, PGLuint(@Tex_Background.TexNum));
    Tex_Background.TexNum := 0;
  }
  if fShowWebcam then
        begin
          Webcam.Release;
          fShowWebCam:=false;
        }
  Background.OnFinish;
  Display.SetCursor;
}

function TScreenSingController.Draw: bool;
var
  V1:     bool;
  V1TwoP: bool;   // position of score box in two player mode
  V1ThreeP: bool; // position of score box in three player mode
  V2R:    bool;
  V2M:    bool;
  V3R:    bool;
  VDuet1ThreeP: bool;
  VDuet2M:    bool;
  VDuet3R:    bool;
  V1FourP: bool;
  V2FourP: bool;
  V3FourP: bool;
  V4FourP: bool;
  V1SixP: bool;
  V2SixP: bool;
  V3SixP: bool;
  V4SixP: bool;
  V5SixP: bool;
  V6SixP: bool;
  V1DuetFourP: bool;
  V2DuetFourP: bool;
  V3DuetFourP: bool;
  V4DuetFourP: bool;
  V1DuetSixP: bool;
  V2DuetSixP: bool;
  V3DuetSixP: bool;
  V4DuetSixP: bool;
  V5DuetSixP: bool;
  V6DuetSixP: bool;
begin
  V1     := false;
  V1TwoP := false;
  V1ThreeP := false;
  V2R    := false;
  V2M    := false;
  V3R    := false;

  VDuet1ThreeP := false;
  VDuet2M := false;
  VDuet3R := false;

  V1FourP := false;
  V2FourP := false;
  V3FourP := false;
  V4FourP := false;

  V1SixP := false;
  V2SixP := false;
  V3SixP := false;
  V4SixP := false;
  V5SixP := false;
  V6SixP := false;

  V1DuetFourP := false;
  V2DuetFourP := false;
  V3DuetFourP := false;
  V4DuetFourP := false;

  V1DuetSixP := false;
  V2DuetSixP := false;
  V3DuetSixP := false;
  V4DuetSixP := false;
  V5DuetSixP := false;
  V6DuetSixP := false;

  case PlayersPlay of
    1:
    begin
      V1     := true;
    }
    2:
    begin
      V1TwoP := true;
      V2R    := true;
    }
    3:
    begin
      if (CurrentSong.isDuet) then
      begin
        VDuet1ThreeP := true;
        VDuet2M := true;
        VDuet3R := true;
      }
      else
      begin
        V1ThreeP := true;
        V2M    := true;
        V3R    := true;
      }
    }
    4:
    begin // double screen
      if (Ini.Screens = 1) then
      begin
        V1TwoP := true;
        V2R    := true;
      }
      else
      begin
        if (CurrentSong.isDuet) then
        begin
          V1DuetFourP := true;
          V2DuetFourP := true;
          V3DuetFourP := true;
          V4DuetFourP := true;
        }
        else
        begin
          V1FourP := true;
          V2FourP := true;
          V3FourP := true;
          V4FourP := true;
        }
      }
    }
    6:
    begin // double screen
      if (Ini.Screens = 1) then
      begin
        if (CurrentSong.isDuet) then
        begin
          VDuet1ThreeP := true;
          VDuet2M := true;
          VDuet3R := true;
        }
        else
        begin
          V1ThreeP := true;
          V2M    := true;
          V3R    := true;
        }
      }
      else
      begin
       if (CurrentSong.isDuet) then
        begin
          V1DuetSixP := true;
          V2DuetSixP := true;
          V3DuetSixP := true;
          V4DuetSixP := true;
          V5DuetSixP := true;
          V6DuetSixP := true;
        }
        else
        begin
          V1SixP := true;
          V2SixP := true;
          V3SixP := true;
          V4SixP := true;
          V5SixP := true;
          V6SixP := true;
        }
      }
    }
  }

  Text[screenSingViewRef.TextP1].Visible           := V1 and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP1TwoP].Visible       := V1TwoP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP2R].Visible          := V2R and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP1ThreeP].Visible     := V1ThreeP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP2M].Visible          := V2M and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP3R].Visible          := V3R and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextDuetP1ThreeP].Visible := VDuet1ThreeP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextDuetP2M].Visible      := VDuet2M and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextDuetP3R].Visible      := VDuet3R and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP1FourP].Visible      := V1FourP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP2FourP].Visible      := V2FourP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP3FourP].Visible      := V3FourP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP4FourP].Visible      := V4FourP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP1SixP].Visible       := V1SixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP2SixP].Visible       := V2SixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP3SixP].Visible       := V3SixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP4SixP].Visible       := V4SixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP5SixP].Visible       := V5SixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP6SixP].Visible       := V6SixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP1DuetFourP].Visible  := V1DuetFourP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP2DuetFourP].Visible  := V2DuetFourP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP3DuetFourP].Visible  := V3DuetFourP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP4DuetFourP].Visible  := V4DuetFourP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP1DuetSixP].Visible   := V1DuetSixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP2DuetSixP].Visible   := V2DuetSixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP3DuetSixP].Visible   := V3DuetSixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP4DuetSixP].Visible   := V4DuetSixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP5DuetSixP].Visible   := V5DuetSixP and ScreenSing.Settings.AvatarsVisible;
  Text[screenSingViewRef.TextP6DuetSixP].Visible   := V6DuetSixP and ScreenSing.Settings.AvatarsVisible;

  Result := screenSingViewRef.Draw();
}

function TScreenSingController.FinishedMusic: bool;
begin
  Result := AudioPlayback.Finished;
}

void TScreenSingController.Finish;
var
  I, J:     int;
  len, num: int;

begin
  AudioInput.CaptureStop;
  AudioPlayback.Stop;
  AudioPlayback.SetSyncSource(nil);

  if (ScreenSong.Mode = smNormal) and (SungPaused = false) and (SungToEnd) and (Length(DllMan.Websites) > 0) then
  begin
    AutoSendScore;
    AutoSaveScore;
  }

  LyricsState.Stop();
  LyricsState.SetSyncSource(nil);

  // close video files
  fVideoClip := nil;
  fCurrentVideo := nil;

  // kill all stars and effects
  GoldenRec.KillAll;

  if (Ini.SavePlayback = 1) then
  begin
    Log.BenchmarkStart(0);
    Log.LogVoice(0);
    if (PlayersPlay > 1) then
      Log.LogVoice(1);
    if (PlayersPlay > 2) then
      Log.LogVoice(2);
    Log.BenchmarkEnd(0);
    Log.LogBenchmark('Creating files', 0);
  }

  SetFontItalic(false);

  if ScreenSong.Mode = smMedley then
  begin
    if not FadeOut then
    begin
      for I := 0 to PlayersPlay - 1 do
        PlaylistMedley.Stats[Length(PlaylistMedley.Stats) - 1].Player[I] := Player[I];

      Inc(PlaylistMedley.CurrentMedleySong);
      if PlaylistMedley.CurrentMedleySong <= PlaylistMedley.NumMedleySongs then
      begin
        LoadNextSong;
      }
      else
      begin
        //build sums
        len := Length(PlaylistMedley.Stats);
        num := PlaylistMedley.NumPlayer;

        SetLength(PlaylistMedley.Stats, len + 1);
        SetLength(PlaylistMedley.Stats[len].Player, num);

        for J := 0 to len - 1 do
        begin
          for I := 0 to num - 1 do
          begin
            PlaylistMedley.Stats[len].Player[I].Score :=
              PlaylistMedley.Stats[len].Player[I].Score +
              PlaylistMedley.Stats[J].Player[I].Score;

            PlaylistMedley.Stats[len].Player[I].ScoreLine :=
              PlaylistMedley.Stats[len].Player[I].ScoreLine +
              PlaylistMedley.Stats[J].Player[I].ScoreLine;

            PlaylistMedley.Stats[len].Player[I].ScoreGolden :=
              PlaylistMedley.Stats[len].Player[I].ScoreGolden +
              PlaylistMedley.Stats[J].Player[I].ScoreGolden;

            PlaylistMedley.Stats[len].Player[I].ScoreInt :=
              PlaylistMedley.Stats[len].Player[I].ScoreInt +
              PlaylistMedley.Stats[J].Player[I].ScoreInt;

            PlaylistMedley.Stats[len].Player[I].ScoreLineInt :=
              PlaylistMedley.Stats[len].Player[I].ScoreLineInt +
              PlaylistMedley.Stats[J].Player[I].ScoreLineInt;

            PlaylistMedley.Stats[len].Player[I].ScoreGoldenInt :=
              PlaylistMedley.Stats[len].Player[I].ScoreGoldenInt +
              PlaylistMedley.Stats[J].Player[I].ScoreGoldenInt;

            PlaylistMedley.Stats[len].Player[I].ScoreTotalInt :=
              PlaylistMedley.Stats[len].Player[I].ScoreTotalInt +
              PlaylistMedley.Stats[J].Player[I].ScoreTotalInt;
          } //of for I
        } //of for J

        //build mean on sum
        for I := 0 to num - 1 do
        begin
          PlaylistMedley.Stats[len].Player[I].Score := round(
            PlaylistMedley.Stats[len].Player[I].Score / len);

          PlaylistMedley.Stats[len].Player[I].ScoreLine := round(
            PlaylistMedley.Stats[len].Player[I].ScoreLine / len);

          PlaylistMedley.Stats[len].Player[I].ScoreGolden := round(
            PlaylistMedley.Stats[len].Player[I].ScoreGolden / len);

          PlaylistMedley.Stats[len].Player[I].ScoreInt := round(
            PlaylistMedley.Stats[len].Player[I].ScoreInt / len);

          PlaylistMedley.Stats[len].Player[I].ScoreLineInt := round(
            PlaylistMedley.Stats[len].Player[I].ScoreLineInt / len);

          PlaylistMedley.Stats[len].Player[I].ScoreGoldenInt := round(
            PlaylistMedley.Stats[len].Player[I].ScoreGoldenInt / len);

          PlaylistMedley.Stats[len].Player[I].ScoreTotalInt := round(
            PlaylistMedley.Stats[len].Player[I].ScoreTotalInt / len);
        }

        Party.CallAfterSing;
        FadeOut:=true;
      }
    }
  }
  else
  begin
    SetLength(PlaylistMedley.Stats, 1);
    SetLength(PlaylistMedley.Stats[0].Player, PlayersPlay);
    for I := 0 to PlayersPlay - 1 do
      PlaylistMedley.Stats[0].Player[I] := Player[I];

    PlaylistMedley.Stats[0].SongArtist := CurrentSong.Artist;
    PlaylistMedley.Stats[0].SongTitle := CurrentSong.Title;

    if not FadeOut then
      Party.CallAfterSing;

    FadeOut := true;
  }

}

void TScreenSingController.OnSentenceEnd(Track: int; SentenceIndex: uint32_t); //ToDo: split and redo
var
  PlayerIndex: byte;
  CurrentPlayer: PPLayer;
  CurrentScore: double;
  Line:      PLine;
  LinePerfection: double;  // perfection of singing performance on the current line
  Rating:    int;
  LineScore: double;
  LineBonus: double;
  MaxSongScore: int; // max. points for the song (without line bonus)
  MaxLineScore: double;    // max. points for the current line
  Index: int;
const
  // TODO: move this to a better place
  MAX_LINE_RATING = 8;        // max. rating for singing performance
begin
  Line := @Tracks[Track].Lines[SentenceIndex];

  // check for empty sentence
  if Line.ScoreValue <= 0 then
    Exit;

  // set max song score
  if Ini.LineBonus = 0 then
    MaxSongScore := MAX_SONG_SCORE
  else
    MaxSongScore := MAX_SONG_SCORE - MAX_SONG_LINE_BONUS;

  // Note: ScoreValue is the sum of all note values of the song
  MaxLineScore := MaxSongScore * (Line.ScoreValue / Tracks[Track].ScoreValue);

  for PlayerIndex := 0 to High(Player) do
  begin
    //PlayerIndex := Index;

    if (not CurrentSong.isDuet) or (PlayerIndex mod 2 = Track) or (PlayersPlay = 1)then
    begin
      CurrentPlayer := @Player[PlayerIndex];
      CurrentScore  := CurrentPlayer.Score + CurrentPlayer.ScoreGolden;

      // line bonus

      // points for this line
      LineScore := CurrentScore - CurrentPlayer.ScoreLast;

      // check for lines with low points
      if MaxLineScore <= 2 then
        LinePerfection := 1
      else
        // determine LinePerfection
        // Note: the "+2" extra points are a little bonus so the player does not
        // have to be that perfect to reach the bonus steps.
        LinePerfection := LineScore / (MaxLineScore - 2);

      // clamp LinePerfection to range [0..1]
      if LinePerfection < 0 then
        LinePerfection := 0
      else if LinePerfection > 1 then
        LinePerfection := 1;

      // add line-bonus if enabled
      if Ini.LineBonus > 0 then
      begin
        // line-bonus points (same for each line, no matter how long the line is)
        LineBonus := MAX_SONG_LINE_BONUS / (Length(Tracks[Track].Lines) -
          NumEmptySentences[Track]);
        // apply line-bonus
        CurrentPlayer.ScoreLine :=
          CurrentPlayer.ScoreLine + LineBonus * LinePerfection;
        CurrentPlayer.ScoreLineInt := Floor(CurrentPlayer.ScoreLine / 10) * 10;
        // update total score
        CurrentPlayer.ScoreTotalInt :=
          CurrentPlayer.ScoreInt +
          CurrentPlayer.ScoreGoldenInt
          + CurrentPlayer.ScoreLineInt;

        // spawn rating pop-up
        Rating := Round(LinePerfection * MAX_LINE_RATING);
        Scores.SpawnPopUp(PlayerIndex, Rating, CurrentPlayer.ScoreTotalInt);
      }
      else
        Scores.RaiseScore(PlayerIndex, CurrentPlayer.ScoreTotalInt);

      // PerfectLineTwinkle (effect), part 1
      if Ini.EffectSing = 1 then
        CurrentPlayer.LastSentencePerfect := (LinePerfection >= 1);

      // refresh last score
      CurrentPlayer.ScoreLast := CurrentScore;
    }
  }

  // PerfectLineTwinkle (effect), part 2
  if Ini.EffectSing = 1 then
  begin
    GoldenRec.SpawnPerfectLineTwinkle;

    for PlayerIndex := 0 to High(Player) do
    begin
      CurrentPlayer := @Player[PlayerIndex];
      CurrentPlayer.LastSentencePerfect := false;
    }
  }

}

 // Called on sentence change
 // SentenceIndex: index of the new active sentence
void TScreenSingController.OnSentenceChange(Track: int; SentenceIndex: uint32_t);  //ToDo: split and redo
var
  tmp_Lyric: TLyricEngine;
begin
  // goldenstarstwinkle
  GoldenRec.SentenceChange(Track);

  if (CurrentSong.isDuet) and (PlayersPlay <> 1) then
  begin
    if (Track = 1) then
      tmp_Lyric := LyricsDuetP2
    else
      tmp_Lyric := LyricsDuetP1;
  }
  else
    tmp_Lyric := Lyrics;

  // fill lyrics queue and set upper line to the current sentence
  while (tmp_Lyric.GetUpperLineIndex() < SentenceIndex) or
    (not tmp_Lyric.IsQueueFull) do
  begin
    // add the next line to the queue or a dummy if no more lines are available
    if (tmp_Lyric.LineCounter <= High(Tracks[Track].Lines)) then
    begin
      tmp_Lyric.AddLine(@Tracks[Track].Lines[tmp_Lyric.LineCounter]);
    }
    else
      tmp_Lyric.AddLine(nil);
  }

}

function TLyricsSyncSource.GetClock(): double;
begin
  Result := LyricsState.GetCurrentTime();
}

function TMusicSyncSource.GetClock(): double;
begin
  Result := AudioPlayback.Position;
}

void TScreenSingController.UpdateMedleyStats(medley_end: bool);   //TODO: view or controller? unsure
var
  len, num, I : int;

begin
  len := Length(PlaylistMedley.Stats);
  num := PlaylistMedley.NumPlayer;

  if (PlaylistMedley.CurrentMedleySong > len) and
    (PlaylistMedley.CurrentMedleySong <= PlaylistMedley.NumMedleySongs) then
  begin
    inc(len);
    SetLength(PlaylistMedley.Stats, len);
    SetLength(PlaylistMedley.Stats[len - 1].Player, num);
    PlaylistMedley.Stats[len-1].SongArtist := CurrentSong.Artist;
    PlaylistMedley.Stats[len-1].SongTitle := CurrentSong.Title;
  }

  if PlaylistMedley.CurrentMedleySong <= PlaylistMedley.NumMedleySongs then
    for I := 0 to num - 1 do
      PlaylistMedley.Stats[len - 1].Player[I] := Player[I];

  if medley_end and not PlaylistMedley.ApplausePlayed and
    (PlaylistMedley.CurrentMedleySong<=PlaylistMedley.NumMedleySongs) then
  begin
    PlaylistMedley.ApplausePlayed:=true;

    AudioPlayback.Fade(CurrentSong.Medley.FadeOut_time, 0.1);
    AudioPlayback.PlaySound(SoundLib.Applause);
  }
}


void TScreenSingController.AutoSendScore;
var
  SendInfo: TSendInfo;
  SendStatus: byte;
  Send: bool;
  TotalScore: int;
  PlayerIndex, IndexWeb, IndexUser: int;
begin
  for PlayerIndex := 1 to PlayersPlay do
  begin
    for IndexWeb := 0 to High(DataBase.NetworkUser) do
    begin
      for IndexUser := 0 to High(DataBase.NetworkUser[IndexWeb].Userlist) do
      begin
        Send := false;
        TotalScore := player[PlayerIndex - 1].ScoreInt + player[PlayerIndex - 1].ScoreLineInt + player[PlayerIndex - 1].ScoreGoldenInt;

        case (Act_Level) of
          0: if (TotalScore >= DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoScoreEasy)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoMode = 1)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoPlayer = PlayerIndex - 1) then
                Send := true;

          1: if (TotalScore >= DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoScoreMedium)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoMode = 1)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoPlayer = PlayerIndex - 1) then
                Send := true;

          2: if (TotalScore >= DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoScoreHard)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoMode = 1)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoPlayer = PlayerIndex - 1) then
                Send := true;
        }

        if (Send) then
        begin

          DllMan.LoadWebsite(IndexWeb);

          SendInfo.Username := DataBase.NetworkUser[IndexWeb].UserList[IndexUser].Username;
          SendInfo.Password := DataBase.NetworkUser[IndexWeb].UserList[IndexUser].Password;
          SendInfo.ScoreInt := player[PlayerIndex - 1].ScoreInt;
          SendInfo.ScoreLineInt := player[PlayerIndex - 1].ScoreLineInt;
          SendInfo.ScoreGoldenInt := player[PlayerIndex - 1].ScoreGoldenInt;
          SendInfo.MD5Song := Act_MD5Song;
          SendInfo.Level := Act_Level;

          SendStatus := DllMan.WebsiteSendScore(SendInfo);

          case SendStatus of
            0: ScreenPopupError.ShowPopup(Language.Translate('WEBSITE_NO_CONNECTION'));
            2: ScreenPopupError.ShowPopup(Language.Translate('WEBSITE_LOGIN_ERROR'));
            3: ScreenPopupInfo.ShowPopup(Language.Translate('WEBSITE_OK_SEND'));
            4: ScreenPopupError.ShowPopup(Language.Translate('WEBSITE_ERROR_SCORE'));
            5: ScreenPopupError.ShowPopup(Language.Translate('WEBSITE_ERROR_SCORE_DUPLICATED'));
            7: ScreenPopupError.ShowPopup(Language.Translate('WEBSITE_ERROR_SONG'));
          }

       }
      }
    }
  }
}

void TScreenSingController.AutoSaveScore;
var
  SendInfo: TSendInfo;
  ScoreFile: TextFile;
  EncryptText: string;
  WebName: UTF8String;
  Save: bool;
  TotalScore: int;
  PlayerIndex, IndexWeb, IndexUser: int;
begin
  for PlayerIndex := 1 to PlayersPlay do
  begin
    for IndexWeb := 0 to High(DataBase.NetworkUser) do
    begin
      for IndexUser := 0 to High(DataBase.NetworkUser[IndexWeb].Userlist) do
      begin
        Save := false;
        TotalScore := player[PlayerIndex - 1].ScoreInt + player[PlayerIndex - 1].ScoreLineInt + player[PlayerIndex - 1].ScoreGoldenInt;

        case (Act_Level) of
          0: if (TotalScore >= DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoScoreEasy)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoMode = 2)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoPlayer = PlayerIndex - 1) then
                Save := true;

          1: if (TotalScore >= DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoScoreMedium)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoMode = 2)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoPlayer = PlayerIndex - 1) then
                Save := true;

          2: if (TotalScore >= DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoScoreHard)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoMode = 2)
              and (DataBase.NetworkUser[IndexWeb].UserList[IndexUser].AutoPlayer = PlayerIndex - 1) then
                Save := true;
        }

        if (Save) then
        begin

          DllMan.LoadWebsite(IndexWeb);

          SendInfo.Username := DataBase.NetworkUser[IndexWeb].UserList[IndexUser].Username;
          SendInfo.Password := DataBase.NetworkUser[IndexWeb].UserList[IndexUser].Password;
          SendInfo.ScoreInt := player[PlayerIndex - 1].ScoreInt;
          SendInfo.ScoreLineInt := player[PlayerIndex - 1].ScoreLineInt;
          SendInfo.ScoreGoldenInt := player[PlayerIndex - 1].ScoreGoldenInt;
          SendInfo.MD5Song := Act_MD5Song;
          SendInfo.Level := Act_Level;

          WebName := DataBase.NetworkUser[IndexWeb].Website;
          EncryptText := DllMan.WebsiteEncryptScore(SendInfo);

          AssignFile(ScoreFile, WebScoresPath.Append(WebName + '.usc').ToNative);

          if FileExists(WebScoresPath.Append(WebName + '.usc').ToNative) then
            Append(ScoreFile)
          else
            Rewrite(ScoreFile);

          WriteLn(ScoreFile, DatetoStr(Now) + '|' + TimetoStr(Now) + '|' + EncryptText);

          Flush(ScoreFile);
          Close(ScoreFile);

          ScreenPopupInfo.ShowPopup(Language.Translate('WEBSITE_SAVE_SCORE'));

       }
      }
    }
  }
}
}