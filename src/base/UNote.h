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
 * $URL$
 * $Id$
 */

#include "../switches.h"

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include "UFiles.h"
#include "UMusic.h"
#include "USong.h"
#include "UIni.h"

namespace UNote
{
struct TPlayerNote
{
  int Start;
  int Duration;
  double Detect;
  double Tone;
  bool Perfect;
  bool Hit;
  UMusic::TNoteType NoteType;
};

typedef std::shared_ptr<TPlayerNote> PPLayerNote;

struct TPlayer
{
  std::string Name;

  // Index in Teaminfo record
  uint8_t TeamID;
  uint8_t PlayerID;

  // Scores
  double Score;
  double ScoreLine;
  double ScoreGolden;

  int ScoreInt;
  int ScoreLineInt;
  int ScoreGoldenInt;
  int ScoreTotalInt;

  // LineBonus
  double ScoreLast;    // Last Line Score

  // PerfectLineTwinkle (effect)
  bool LastSentencePerfect;

  int HighNote; // index of last note (= High(Note)?)
  int LengthNote; // number of notes (= Length(Note)?).
  std::vector<TPlayerNote> Note;
};

typedef std::shared_ptr<TPlayer> PPLayer;

struct TStats
{
  std::vector<TPlayer> Player;
  std::string SongArtist;
  std::string SongTitle;
};

struct TMedleyPlaylist
{
  std::vector<int> Song;
  int NumMedleySongs;
  int CurrentMedleySong;
  bool ApplausePlayed;
  std::vector<TStats> Stats;
  int NumPlayer;
};

/*uses
  SysUtils,
  Classes,
  sdl2,
  dglOpenGL,
  UDisplay,
  UIni,
  ULog,
  ULyrics,
  UMusic,
  URecord,
  UScreenSingController,
  UScreenJukebox,
  USong,
  UTime;
*/

/* Player and music info */
  
/**
 * Player info and state for each player.
 * The amount of players is given by PlayersPlay.
 */
inline std::vector<TPlayer> Player;

/**
 * Number of players or teams playing.
 * Possible values: 1 - 6 //TODO::should be IMaxPlayer from UIni
 */
int PlayersPlay;

/**
 * Selected song for singing.
 */
std::shared_ptr<USong::TSong> CurrentSong;

TMedleyPlaylist PlaylistMedley;  // playlist medley

const int MAX_SONG_SCORE = 10000;     // max. achievable points per song
const int MAX_SONG_LINE_BONUS = 1000; // max. achievable line bonus per song

void Sing(TScreenSingController Screen);
void NewSentence(int CP, TScreenSingController Screen);
void NewBeatClick(TScreenSingController Screen);  // executed when on then new beat for click
void NewBeatDetect(TScreenSingController Screen); // executed when on then new beat for detection
void NewNote(int CP, TScreenSingController Screen);       // detect note
double GetMidBeat(double Time);
SecDouble GetTimeFromBeat(int Beat, const std::vector<UFiles::TBPM>& BPM, const MiliSecDouble& GAP);

void SingJukebox(TScreenJukebox Screen);

/*uses
  Math,
  StrUtils,
  UCatCovers,
  UCommandLine,
  UCommon,
  UConfig,
  UCovers,
  UDataBase,
  UGraphic,
  UGraphicClasses,
  UJoystick,
  ULanguage,
  UParty,
  UPathUtils,
  UPlatform,
  UPlaylist,
  USkins,
  USongs,
  UThemes;
*/
SecDouble GetTimeForBeats(double BPM, double Beats);

double GetBeats(double BPM, SecDouble Time);

static void GetMidBeatSub(size_t BPMNum, SecDouble& Time, double& CurBeat, const std::vector<USong::TBPM>& BPM);


double GetMidBeat(SecDouble Time);


void Sing(TScreenSingController Screen)
/*var
  LineIndex:  int;
  CountGr:    int;
  TrackIndex: int;
  PetGr:      int;*/
{
    LyricsState.UpdateBeats();

PetGr: = 0;
    if (CurrentSong.isDuet) and (PlayersPlay != 1)
        PetGr : = 1;

    // sentences routines
    for CountGr = 0 to PetGr do //High(Tracks)
    {
TrackIndex = CountGr;

    // old parts
    LyricsState.OldLine = Tracks[TrackIndex].CurrentLine;

    // choose current parts
    for LineIndex : = 0 to Tracks[TrackIndex].High do
    {
        if LyricsState.CurrentBeat >= Tracks[TrackIndex].Lines[LineIndex].StartBeat
            Tracks[TrackIndex].CurrentLine : = LineIndex;
}

    // clean player note if there is a new line
    // (optimization on halfbeat time)
    if Tracks[TrackIndex].CurrentLine != LyricsState.OldLine
        NewSentence(TrackIndex, Screen);

    } // for CountGr

    // make some operations on clicks
    if {(LyricsState.CurrentBeatC >= 0) && }(LyricsState.OldBeatC != LyricsState.CurrentBeatC)
        NewBeatClick(Screen);

    // make some operations when detecting new voice pitch
    if (LyricsState.CurrentBeatD >= 0) && (LyricsState.OldBeatD != LyricsState.CurrentBeatD)
        NewBeatDetect(Screen);
}

void SingJukebox(TScreenJukebox Screen);
var
  Count:   int;
  CountGr: int;
  CP:      int;
begin
  LyricsState.UpdateBeats();

  // sentences routines
  for CountGr := 0 to 0 do //High(Tracks)
  begin;
    CP := CountGr;
    // old parts
    LyricsState.OldLine := Tracks[CP].CurrentLine;

    // choose current parts
    for Count := 0 to Tracks[CP].High do
    begin
      if LyricsState.CurrentBeat >= Tracks[CP].Lines[Count].StartBeat then
        Tracks[CP].CurrentLine := Count;
    end;
  end; // for CountGr

  // on sentence change...
  Screen.onSentenceChange(Tracks[0].CurrentLine);
end;

void NewSentence(int CP, TScreenSingController Screen);
var
  I: int;
begin
  // clean note of player
  for I := 0 to High(Player) do
  begin
    if (not CurrentSong.isDuet) or (I mod 2 = CP) then
    begin
      Player[I].LengthNote := 0;
      Player[I].HighNote := -1;
      SetLength(Player[I].Note, 0);
    end;
  end;

  Screen.onSentenceChange(CP, Tracks[CP].CurrentLine)
end;

void NewBeatClick();

void NewBeatDetect(Screen: TScreenSingController);
  var
    MaxCP, CP, SentenceEnd: int;
    I, J: uint32_t;
begin
  // check for sentence end
  // we check all lines here because a new sentence may
  // have been started even before the old one finishes
  // due to corrupt lien breaks
  // checking only current line works to, but may lead to
  // weird ratings for the song files w/ the mentioned
  // errors
  // To-Do Philipp : check current and last line should
  // do it for most corrupt txt and for lines in
  // non-corrupt txts that start immediatly after the prev.
  // line ends
  MaxCP := 0;
  if (CurrentSong.isDuet) and (PlayersPlay <> 1) then
    MaxCP := 1;

  if (assigned(Screen)) then
  begin
    for J := 0 to MaxCP do
    begin
      CP := J;

      NewNote(CP, Screen);

      for I := 0 to Tracks[CP].High do
      begin
        with Tracks[CP].Lines[I] do
        begin
          if (HighNote >= 0) then
          begin
            SentenceEnd := Notes[HighNote].StartBeat + Notes[HighNote].Duration;

            if (LyricsState.OldBeatD < SentenceEnd) and (LyricsState.CurrentBeatD >= SentenceEnd) then
              Screen.OnSentenceEnd(CP, I);
          end;
        end;
      end;
    end;
  end;
end;

void NewNote(CP: int; Screen: TScreenSingController);
var
  LineFragmentIndex:   int;
  CurrentLineFragment: PLineFragment;
  PlayerIndex:         int;
  CurrentSound:        TCaptureBuffer;
  CurrentPlayer:       PPlayer;
  LastPlayerNote:      PPlayerNote;
  Line: 	       PLine;
  SentenceIndex:       int;
  SentenceMin:         int;
  SentenceMax:         int;
  SentenceDetected:    int; // sentence of detected note
  ActualBeat:          int;
  ActualTone:          int;
  NoteAvailable:       bool;
  NewNote:             bool;
  Range:               int;
  NoteHit:             bool;
  MaxSongPoints:       int; // max. points for the song (without line bonus)
  CurNotePoints:       double;    // Points for the cur. Note (PointsperNote * ScoreFactor[CurNote])
  CurrentNoteType:     TNoteType;
begin
  ActualTone := 0;
  NoteHit := false;

  // TODO: add duet mode support
  // use Tracks[LineSetIndex] with LineSetIndex depending on the current player

  // count min and max sentence range for checking
  // (detection is delayed to the notes we see on the screen)
  SentenceMin := Tracks[CP].CurrentLine-1;
  if (SentenceMin < 0) then
    SentenceMin := 0;
  SentenceMax := Tracks[CP].CurrentLine;

  for ActualBeat := LyricsState.OldBeatD+1 to LyricsState.CurrentBeatD do
  begin
    // analyze player signals
    for PlayerIndex := 0 to PlayersPlay-1 do
    begin
      if (not CurrentSong.isDuet) or (PlayerIndex mod 2 = CP) then
      begin
        // check for an active note at the current time defined in the lyrics
        NoteAvailable := false;
        SentenceDetected := SentenceMin;
        for SentenceIndex := SentenceMin to SentenceMax do
        begin
          Line := @Tracks[CP].Lines[SentenceIndex];
          for LineFragmentIndex := 0 to Line.HighNote do
          begin
            CurrentLineFragment := @Line.Notes[LineFragmentIndex];
            // check if line is active
            if ((CurrentLineFragment.StartBeat <= ActualBeat) and
              (CurrentLineFragment.StartBeat + CurrentLineFragment.Duration-1 >= ActualBeat)) and
              (CurrentLineFragment.NoteType <> ntFreestyle) and       // but ignore FreeStyle notes
              (CurrentLineFragment.Duration > 0) then                   // and make sure the note length is at least 1
            begin
              SentenceDetected := SentenceIndex;
              NoteAvailable := true;
              Break;
            end;
          end;
          // TODO: break here, if NoteAvailable is true? We would then use the first instead
          // of the last note matching the current beat if notes overlap. But notes
          // should not overlap at all.
          //if (NoteAvailable) then
          //  Break;
        end;

        CurrentPlayer := @Player[PlayerIndex];
        CurrentSound := AudioInputProcessor.Sound[PlayerIndex];

        // at the beginning of the song there is no previous note
        if (Length(CurrentPlayer.Note) > 0) and (CurrentPlayer.HighNote > -1) then
          LastPlayerNote := @CurrentPlayer.Note[CurrentPlayer.HighNote]
        else
          LastPlayerNote := nil;

        // analyze buffer
        CurrentSound.AnalyzeBuffer;

        // add some noise
        // TODO: do we need this?
        //LyricsState.Tone := LyricsState.Tone + Round(Random(3)) - 1;

        // add note if possible
        if (CurrentSound.ToneValid and NoteAvailable) then
        begin
          CurrentNoteType := ntNormal;
          Line := @Tracks[CP].Lines[SentenceDetected];
          // process until last note
          for LineFragmentIndex := 0 to Line.HighNote do
          begin
            CurrentLineFragment := @Line.Notes[LineFragmentIndex];
            if (CurrentLineFragment.StartBeat <= ActualBeat) and
              (CurrentLineFragment.StartBeat + CurrentLineFragment.Duration > ActualBeat) then
            begin
              // set the current note type
              CurrentNoteType := CurrentLineFragment.NoteType;
              // compare notes (from song-file and from player)

              // move players tone to proper octave
              while (CurrentSound.Tone - CurrentLineFragment.Tone > 6) do
                CurrentSound.Tone := CurrentSound.Tone - 12;

              while (CurrentSound.Tone - CurrentLineFragment.Tone < -6) do
                CurrentSound.Tone := CurrentSound.Tone + 12;

              // half size notes patch
              NoteHit := false;
              ActualTone := CurrentSound.Tone;
              if (ScreenSong.Mode = smNormal) then
                Range := 2 - Ini.PlayerLevel[PlayerIndex]
              else
                Range := 2 - Ini.Difficulty;

              // check if the player hit the correct tone within the tolerated range
              if (Abs(CurrentLineFragment.Tone - CurrentSound.Tone) <= Range) or (CurrentLineFragment.NoteType = ntRap) or (CurrentLineFragment.NoteType = ntRapGolden) then
              begin
                // adjust the players tone to the correct one
                // TODO: do we need to do this?
                // Philipp: I think we do, at least when we draw the notes.
                //          Otherwise the notehit thing would be shifted to the
                //          correct unhit note. I think this will look kind of strange.
                ActualTone := CurrentLineFragment.Tone;

                // half size notes patch
                NoteHit := true;

                if (Ini.LineBonus > 0) then
                  MaxSongPoints := MAX_SONG_SCORE - MAX_SONG_LINE_BONUS
                else
                  MaxSongPoints := MAX_SONG_SCORE;

                // Note: ScoreValue is the sum of all note values of the song
                // (MaxSongPoints / ScoreValue) is the points that a player
                // gets for a hit of one beat of a normal note
                // CurNotePoints is the amount of points that is meassured
                // for a hit of the note per full beat
                CurNotePoints := (MaxSongPoints / Tracks[CP].ScoreValue) * ScoreFactor[CurrentLineFragment.NoteType];

                case CurrentLineFragment.NoteType of
                  ntNormal:    CurrentPlayer.Score       := CurrentPlayer.Score       + CurNotePoints;
                  ntGolden:    CurrentPlayer.ScoreGolden := CurrentPlayer.ScoreGolden + CurNotePoints;
                  ntRap:       CurrentPlayer.Score       := CurrentPlayer.Score       + CurNotePoints;
                  ntRapGolden: CurrentPlayer.ScoreGolden := CurrentPlayer.ScoreGolden + CurNotePoints;
                end;

                // a problem if we use floor instead of round is that a score of
                // 10000 points is only possible if the last digit of the total points
                // for golden and normal notes is 0.
                // if we use round, the max score is 10000 for most songs
                // but a score of 10010 is possible if the last digit of the total
                // points for golden and normal notes is 5
                // the best solution is to use round for one of these scores
                // and round the other score in the opposite direction
                // so we assure that the highest possible score is 10000 in every case.
                CurrentPlayer.ScoreInt := round(CurrentPlayer.Score / 10) * 10;

                if (CurrentPlayer.ScoreInt < CurrentPlayer.Score) then
                  //normal score is floored so we have to ceil golden notes score
                  CurrentPlayer.ScoreGoldenInt := ceil(CurrentPlayer.ScoreGolden / 10) * 10
                else
                  //normal score is ceiled so we have to floor golden notes score
                  CurrentPlayer.ScoreGoldenInt := floor(CurrentPlayer.ScoreGolden / 10) * 10;


                CurrentPlayer.ScoreTotalInt := CurrentPlayer.ScoreInt +
                                               CurrentPlayer.ScoreGoldenInt +
                                               CurrentPlayer.ScoreLineInt;
              end;
            end; // operation
          end; // for

          // check if we have to add a new note or extend the note's length
          if (SentenceDetected = SentenceMax) then
          begin
            // we will add a new note
            NewNote := true;

            // if previous note (if any) was the same, extend previous note
            if ((CurrentPlayer.LengthNote > 0) and
                (LastPlayerNote <> nil) and
                (LastPlayerNote.Tone = ActualTone) and
                ((LastPlayerNote.Start + LastPlayerNote.Duration) = ActualBeat)) then
            begin
              NewNote := false;
            end;

            // if is not as new note to control
            for LineFragmentIndex := 0 to Line.HighNote do
            begin
              if (Line.Notes[LineFragmentIndex].StartBeat = ActualBeat) then
                NewNote := true;
            end;

            // add new note
            if NewNote then
            begin
              // new note
              Inc(CurrentPlayer.LengthNote);
              Inc(CurrentPlayer.HighNote);
              SetLength(CurrentPlayer.Note, CurrentPlayer.LengthNote);

              // update player's last note
              LastPlayerNote := @CurrentPlayer.Note[CurrentPlayer.HighNote];
              with LastPlayerNote^ do
              begin
                Start    := ActualBeat;
                Duration := 1;
                Tone     := ActualTone; // Tone || ToneAbs
                //Detect := LyricsState.MidBeat; // Not used!
                Hit      := NoteHit; // half note patch
                NoteType := CurrentNoteType;
              end;
            end
            else
            begin
              // extend note length
              if (LastPlayerNote <> nil) then
                Inc(LastPlayerNote.Duration);
            end;

            // check for perfect note and then light the star (on Draw)
            for LineFragmentIndex := 0 to Line.HighNote do
            begin
              CurrentLineFragment := @Line.Notes[LineFragmentIndex];
              if (CurrentLineFragment.StartBeat  = LastPlayerNote.Start) and
                (CurrentLineFragment.Duration = LastPlayerNote.Duration) and
                (CurrentLineFragment.Tone   = LastPlayerNote.Tone) then
              begin
                LastPlayerNote.Perfect := true;
              end;
            end;
          end; // if SentenceDetected = SentenceMax

        end; // if Detected
      end;
    end; // for PlayerIndex
  end; // for ActualBeat
  //Log.LogStatus('EndBeat', 'NewBeat');
end;

};