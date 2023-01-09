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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/USong.pas $
 * $Id: USong.pas 3135 2015-09-12 00:48:54Z basisbit $
 */

#include "../switches.h"

#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <array>

#include "IniFileHelper.hpp"
#include "UTexture.h"


namespace UFiles
{
    class SongIO;
}

namespace USong
{
enum TSingMode
{
  smNormal, smPartyClassic, smPartyFree, smPartyChallenge, smPartyTournament, smJukebox, smPlaylistRandom , smMedley
};

enum TSongMode
{
  smAll, smCategory, smPlaylist
};

enum TMedleySource
{
  msNone, msCalculated, msTag
};

struct TMedley 
{
    TMedleySource Source;  //source of the information
    int StartBeat;        //start beat of medley
    int EndBeat;        //end beat of medley
    SecDouble FadeIn_time;           //FadeIn-Time in seconds
    SecDouble FadeOut_time;           //FadeOut-Time in seconds
};

struct TBPM
{
    double BPM;
    double StartBeat;
};

struct TScore
{
    std::string Name;
    int Score;
    std::string Date;
};

class TSong
{
private:

    friend UFiles::SongIO;

    //int FileLineNo;  // line, which is read last, for error reporting

    std::filesystem::path DecodeFilename(const std::string& Filename);
    void ParseNote(int Track, char TypeP, int StartP, int DurationP, int NoteP, std::string LyricS);
    void NewSentence(int LineNumberP, int Param1, int Param2);
    void FindRefrain(); // tries to find a refrain for the medley mode and preview start

    std::string ParseLyricStringParam(const std::string Line, int& LinePos);
    int ParseLyricIntParam(const std::string Line, int& LinePos);
    float ParseLyricFloatParam(const std::string Line, int& LinePos);
    char ParseLyricCharParam(const std::string Line, int& LinePos);
    std::string ParseLyricText(const std::string Line, int& LinePos);

    bool ReadTXTHeader(TTextFileStream SongFile, bool ReadCustomTags);
    bool ReadXMLHeader(const std::filesystem::path aFileName);

    std::string GetFolderCategory(const std::filesystem::path aFileName);
    std::filesystem::path FindSongFile(const std::filesystem::path Dir; std::string Mask);

  public:
    //std::filesystem::path Path; // just path component of file (only set if file was found)
    //std::string Folder; // for sorting by folder (only set if file was found)
    //std::filesystem::path FileName; // just name component of file (only set if file was found)

    UTexture::TTexture CoverTex;

    double VideoGAP;
    int NotesGAP;
    SecDouble Start; // in seconds
    std::chrono::milliseconds Finish; // in miliseconds
    bool Relative;
    int Resolution;
    //std::vector<TBPM> BPM;
    MiliSecDouble GAP; // in miliseconds

    //std::string Encoding;
    //TEncoding Encoding;
    
    TMedley Medley;  // medley params

    bool isDuet;
    std::vector<std::string> DuetNames; // duet singers name

    bool hasRap;

    

    std::array<std::vector<TScore>, 3> Score;

    // these are used when sorting is enabled
    bool Visible; // false if hidden, true if visible
    bool Main; // false for songs, true for category buttons
    int OrderNum; // has a number of category for category buttons and songs
    int OrderTyp; // type of sorting for this button (0=name)
    int CatNumber; // Count of Songs in Category for Cats and Number of Song in Category for Songs
    int VisibleIndex;

    std::array<int, 2> Base;

    std::string LastError;
    int GetErrorLineNo();
    //property    ErrorLineNo: int read GetErrorLineNo;


    TSong();
    TSong(const std::filesystem::path aFileName);

    bool LoadSong(bool DuetChange);
    bool LoadXMLSong();
    bool Analyse(const bool ReadCustomTags = false, bool DuetChange = false);
    bool AnalyseXML();
    void SetMedleyMode();
    void Clear();
    std::string MD5SongFile(TTextFileStream SongFileR);
};

class TSongOptions
{
public:
  VideoRatioAspect:        int;
  VideoWidth :             int;
  VideoHeight:             int;
  LyricPosition:           int;
  LyricAlpha:              int;
  LyricSingFillColor:      string;
  LyricActualFillColor:    string;
  LyricNextFillColor:      string;
  LyricSingOutlineColor:   string;
  LyricActualOutlineColor: string;
  LyricNextOutlineColor:   string;

  constructor Create(RatioAspect, Width, Height, Position, Alpha: int;
            SingFillColor, ActualFillColor, NextFillColor, SingOutlineColor, ActualOutlineColor, NextOutlineColor: string);
}


class USong
{
public:
/*
uses
  {$IFDEF MSWINDOWS}
    Windows,
  {$ELSE}
    {$IFNDEF DARWIN}
      syscall,
    {$ENDIF}
    baseunix,
    UnixType,
  {$ENDIF}
  MD5,
  SysUtils,
  Classes,
  {$IFDEF DARWIN}
    cthreads,
  {$ENDIF}
  {$IFDEF USE_PSEUDO_THREAD}
    PseudoThread,
  {$ENDIF}
  UCatCovers,
  UCommon,
  UFilesystem,
  ULog,
  UPath,
  UPlatform,
  UTexture,
  UTextEncoding,
  UUnicodeStringHelper,
  UUnicodeUtils,
  UXMLSong;
*/
type

  

implementation

uses
  StrUtils,
  TextGL,
  UIni,
  UPathUtils,
  USongs,
  UMusic,  //needed for Tracks
  UNote;   //needed for Player


constructor TSongOptions.Create(RatioAspect, Width, Height, Position, Alpha: int;
                SingFillColor, ActualFillColor, NextFillColor, SingOutlineColor, ActualOutlineColor, NextOutlineColor: string);
begin
  inherited Create();

  VideoRatioAspect := RatioAspect;
  VideoWidth := Width;
  VideoHeight := Height;
  LyricPosition := Position;
  LyricAlpha := Alpha;
  LyricSingFillColor := SingFillColor;
  LyricActualFillColor := ActualFillColor;
  LyricNextFillColor := NextFillColor;
  LyricSingOutlineColor := SingOutlineColor;
  LyricActualOutlineColor := ActualOutlineColor;
  LyricNextOutlineColor := NextOutlineColor;

end;

constructor TSong.Create();
begin
  inherited;

  // to-do : special create for category "songs"
  //dirty fix to fix folders=on
  Self.Path     := PATH_NONE();
  Self.FileName := PATH_NONE();
  Self.Cover    := PATH_NONE();
  Self.Mp3      := PATH_NONE();
  Self.Background:= PATH_NONE();
  Self.Video    := PATH_NONE();
end;

// This may be changed, when we rewrite song select code.
// it is some kind of dirty, but imho the best possible
// solution as we do atm not support nested categorys.
// it works like the folder sorting in 1.0.1a
// folder is set to the first folder under the songdir
// so songs ~/.ultrastardx/songs/punk is in the same
// category as songs in shared/ultrastardx/songs are.
// note: folder is just the name of a category it has
//       nothing to do with the path used for file loading
function TSong.GetFolderCategory(const aFileName: IPath): UTF8String;
var
  I: Integer;
  CurSongPath: IPath;
  CurSongPathRel: IPath;
begin
  Result := 'Unknown'; //default folder category, if we can't locate the song dir

  for I := 0 to SongPaths.Count-1 do
  begin
    CurSongPath := SongPaths[I] as IPath;
    if (aFileName.IsChildOf(CurSongPath, false)) then
    begin
      if (aFileName.IsChildOf(CurSongPath, true)) then
      begin
        // songs are in the "root" of the songdir => use songdir for the categorys name
        Result := CurSongPath.RemovePathDelim.ToUTF8;
      end
      else
      begin
        // use the first subdirectory below CurSongPath as the category name
        CurSongPathRel := aFileName.GetRelativePath(CurSongPath.AppendPathDelim);
        Result := CurSongPathRel.SplitDirs[0].RemovePathDelim.ToUTF8;
      end;
      Exit;
    end;
  end;
end;

constructor TSong.Create(const aFileName: IPath);
begin
  inherited Create();

  Mult    := 1;
  MultBPM := 4;

  LastError := '';

  Self.Path     := aFileName.GetPath;
  Self.FileName := aFileName.GetName;
  Self.Folder   := GetFolderCategory(aFileName);

  /*
  if (aFileName.IsFile) then
  begin
    if ReadTXTHeader(aFileName) then
    begin
      LoadSong();
    end
    else
    begin
      Log.LogError('Error Loading SongHeader, abort Song Loading');
      Exit;
    end;
  end;
  */
end;

function TSong.FindSongFile(Dir: IPath; Mask: UTF8String): IPath;
var
  Iter: IFileIterator;
  FileInfo: TFileInfo;
  FileName: IPath;
begin
  Iter := FileSystem.FileFind(Dir.Append(Mask), faDirectory);
  if (Iter.HasNext) then
    Result := Iter.Next.Name
  else
    Result := PATH_NONE;
end;

function TSong.DecodeFilename(Filename: RawByteString): IPath;
begin
  Result := UPath.Path(DecodeStringUTF8(Filename, Encoding));
end;

type
  EUSDXParseException = class(Exception);


function TSong.ReadXMLHeader(const aFileName : IPath): bool;
var
  Done        : byte;
  Parser      : TParser;
  FileNamePath: IPath;
begin
  Result := true;
  Done   := 0;

  //Parse XML
  Parser := TParser.Create;
  Parser.Settings.DashReplacement := '~';

  FileNamePath := Self.Path.Append(Self.FileName);
  if Parser.ParseSong(FileNamePath) then
  begin
    //-----------
    //Required Attributes
    //-----------

    //Title
    self.Title := Parser.SongInfo.Header.Title;
    self.TitleNoAccent := LowerCase(GetStringWithNoAccents(UTF8Decode(Parser.SongInfo.Header.Title)));

    //Add Title Flag to Done
    Done := Done or 1;

    //Artist
    self.Artist := Parser.SongInfo.Header.Artist;
    self.ArtistNoAccent := LowerCase(GetStringWithNoAccents(UTF8Decode(Parser.SongInfo.Header.Artist)));

    //Add Artist Flag to Done
    Done := Done or 2;

    //MP3 File //Test if Exists
    Self.Mp3 := FindSongFile(Self.Path, '*.mp3');
    //Add Mp3 Flag to Done
    if (Self.Path.Append(Self.Mp3).IsFile()) then
      Done := Done or 4;

    //Beats per Minute
    SetLength(self.BPM, 1);
    self.BPM[0].StartBeat := 0;

    self.BPM[0].BPM := (Parser.SongInfo.Header.BPM * Parser.SongInfo.Header.Resolution/4  ) * Mult * MultBPM;

    //Add BPM Flag to Done
    if self.BPM[0].BPM <> 0 then
      Done := Done or 8;

    //---------
    //Additional Header Information
    //---------

    // Gap
    self.GAP := Parser.SongInfo.Header.Gap;

    //Cover Picture
    self.Cover := FindSongFile(Path, '*[CO].jpg');

    //Background Picture
    self.Background := FindSongFile(Path, '*[BG].jpg');

    // Video File
    //    self.Video := Value

    // Video Gap
    //  self.VideoGAP := StrtoFloatI18n( Value )

    //Genre Sorting
    self.Genre := Parser.SongInfo.Header.Genre;
    self.GenreNoAccent := LowerCase(GetStringWithNoAccents(UTF8Decode(self.Genre)));

    //Edition Sorting
    self.Edition := Parser.SongInfo.Header.Edition;
    self.EditionNoAccent := LowerCase(GetStringWithNoAccents(UTF8Decode(self.Edition)));

    //Year Sorting
    //self.Year := Parser.SongInfo.Header.Year

    //Language Sorting
    self.Language := Parser.SongInfo.Header.Language;
    self.LanguageNoAccent := LowerCase(GetStringWithNoAccents(UTF8Decode(self.Language)));
  end
  else
    Log.LogError('File incomplete or not SingStar XML (A): ' + aFileName.ToNative);

  Parser.Free;

  //Check if all Required Values are given
  if (Done <> 15) then
  begin
    Result := false;
    if (Done and 8) = 0 then      //No BPM Flag
      Log.LogError('BPM tag missing: ' + self.FileName.ToNative)
    else if (Done and 4) = 0 then //No MP3 Flag
      Log.LogError('MP3 tag/file missing: ' + self.FileName.ToNative)
    else if (Done and 2) = 0 then //No Artist Flag
      Log.LogError('Artist tag missing: ' + self.FileName.ToNative)
    else if (Done and 1) = 0 then //No Title Flag
      Log.LogError('Title tag missing: ' + self.FileName.ToNative)
    else //unknown Error
      Log.LogError('File incomplete or not SingStar XML (B - '+ inttostr(Done) +'): ' + aFileName.ToNative);
  end;

end;

/**
 * "International" StrToFloat variant. Uses either ',' or '.' as decimal
 * separator.
 */
function StrToFloatI18n(const Value: string): extended;
var
  TempValue : string;
begin
  TempValue := Value;
  if (Pos(',', TempValue) <> 0) then
    TempValue[Pos(',', TempValue)] := '.';
  Result := StrToFloatDef(TempValue, 0);
end;

function  TSong.GetErrorLineNo: int;
begin
  if (LastError = 'ERROR_CORRUPT_SONG_ERROR_IN_LINE') then
    Result := FileLineNo
  else
    Result := -1;
end;

/* new void for preview
   tries find out the beginning of a refrain
   and the end... */
void TSong.FindRefrain();
Const
  MEDLEY_MIN_DURATION = 40;   //minimum duration of a medley-song in seconds

Type
  TSeries = record
    start:    int; //Start sentence of series
    end_:     int; //End sentence of series
    len:      int; //Length of sentence series
  end;

var
  I, J, K, num_lines:   int;
  sentences:            array of UTF8String;
  series:               array of TSeries;
  temp_series:          TSeries;
  max:                  int;
  len_lines, len_notes: int;
  found_end:            bool;
begin
  if self.Medley.Source = msTag then
    Exit;

  if not self.CalcMedley then
    Exit;

  //relative is not supported for medley!
  if self.Relative then
  begin
    Log.LogError('Song '+self.Artist+'-' + self.Title + ' contains #Relative, this is not supported by medley-function!');
    self.Medley.Source := msNone;
    Exit;
  end;

  num_lines := Length(Tracks[0].Lines);
  SetLength(sentences, num_lines);

  //build sentences array
  for I := 0 to num_lines - 1 do
  begin
    sentences[I] := '';
    for J := 0 to High(Tracks[0].Lines[I].Notes) do
    begin
      if (Tracks[0].Lines[I].Notes[J].NoteType <> ntFreestyle) then
        sentences[I] := sentences[I] + Tracks[0].Lines[I].Notes[J].Text;
    end;
  end;

  //find equal sentences series
  SetLength(series, 0);

  for I := 0 to num_lines - 2 do
  begin
    for J := I + 1 to num_lines - 1 do
    begin
      if sentences[I] = sentences[J] then
      begin
        temp_series.start := I;
        temp_series.end_  := I;

        if (J + J - I - 1 > num_lines - 1) then
          max:=num_lines-1-J
        else
          max:=J-I-1;

        for K := 1 to max do
        begin
          if sentences[I+K] = sentences[J+K] then
            temp_series.end_ := I+K
          else
            break;
        end;
        temp_series.len := temp_series.end_ - temp_series.start + 1;
        SetLength(series, Length(series)+1);
        series[Length(series)-1] := temp_series;
      end;
    end;
  end;

  //search for longest sequence
  max := 0;
  if Length(series) > 0 then
  begin
    for I := 0 to High(series) do
    begin
      if series[I].len > series[max].len then
        max := I;
    end;
  end;

  len_lines := length(Tracks[0].Lines);

  if (Length(series) > 0) and (series[max].len > 3) then
  begin
    self.Medley.StartBeat := Tracks[0].Lines[series[max].start].Notes[0].StartBeat;
    len_notes := length(Tracks[0].Lines[series[max].end_].Notes);
    self.Medley.EndBeat := Tracks[0].Lines[series[max].end_].Notes[len_notes - 1].StartBeat +
      Tracks[0].Lines[series[max].end_].Notes[len_notes - 1].Duration;

    found_end := false;

    //set end if duration > MEDLEY_MIN_DURATION
    if GetTimeFromBeat(self.Medley.StartBeat) + MEDLEY_MIN_DURATION >
      GetTimeFromBeat(self.Medley.EndBeat) then
    begin
      found_end := true;
    end;

    //estimate the end: just go MEDLEY_MIN_DURATION
    //ahead an set to a line end (if possible)
    if not found_end then
    begin
      for I := series[max].start + 1 to len_lines - 1 do
      begin
        len_notes := length(Tracks[0].Lines[I].Notes);
        for J := 0 to len_notes - 1 do
        begin
          if GetTimeFromBeat(self.Medley.StartBeat) + MEDLEY_MIN_DURATION >
            GetTimeFromBeat(Tracks[0].Lines[I].Notes[J].StartBeat +
            Tracks[0].Lines[I].Notes[J].Duration) then
          begin
            found_end := true;
            self.Medley.EndBeat := Tracks[0].Lines[I].Notes[len_notes-1].StartBeat +
              Tracks[0].Lines[I].Notes[len_notes - 1].Duration;
            break;
          end;
        end;
      end;
    end;

    if found_end then
    begin
      self.Medley.Source := msCalculated;

      //calculate fade time
      self.Medley.FadeIn_time := DEFAULT_FADE_IN_TIME;
      self.Medley.FadeOut_time := DEFAULT_FADE_OUT_TIME;
    end;
  end;

  //set PreviewStart if not set
  if self.PreviewStart = 0 then
  begin
    if self.Medley.Source = msCalculated then
      self.PreviewStart := GetTimeFromBeat(self.Medley.StartBeat);
  end;
end;

//sets a song to medley-mode:
//converts all unneeded notes into freestyle
//updates score values
void TSong.SetMedleyMode();
var
  TrackIndex: int;
  LineIndex:  int;
  NoteIndex:  int;
  cut_line:   array of int;
  foundcut:   array of bool;
  start:      int;
  end_:       int;

begin
  start := self.Medley.StartBeat;
  end_  := self.Medley.EndBeat;
  SetLength(cut_line, Length(Tracks));
  SetLength(foundcut, Length(Tracks));

  for TrackIndex := 0 to High(Tracks) do
  begin
    foundcut[TrackIndex] := false;
    cut_line[TrackIndex] := high(Integer);
    Tracks[TrackIndex].ScoreValue := 0;
    for LineIndex := 0 to High(Tracks[TrackIndex].Lines) do
    begin
      Tracks[TrackIndex].Lines[LineIndex].ScoreValue := 0;
      for NoteIndex := 0 to High(Tracks[TrackIndex].Lines[LineIndex].Notes) do
      begin
        if Tracks[TrackIndex].Lines[LineIndex].Notes[NoteIndex].StartBeat < start then      //check start
        begin
          Tracks[TrackIndex].Lines[LineIndex].Notes[NoteIndex].NoteType := ntFreeStyle;
        end else if Tracks[TrackIndex].Lines[LineIndex].Notes[NoteIndex].StartBeat >= end_ then  //check end
        begin
          Tracks[TrackIndex].Lines[LineIndex].Notes[NoteIndex].NoteType := ntFreeStyle;
          if not foundcut[TrackIndex] then
          begin
            if (NoteIndex = 0) then
              cut_line[TrackIndex] := LineIndex
            else
              cut_line[TrackIndex] := LineIndex + 1;
          end;
          foundcut[TrackIndex] := true;
        end
	else
        begin
          //add this notes value ("notes length" * "notes scorefactor") to the current songs entire value
          Inc(Tracks[TrackIndex].ScoreValue, Tracks[TrackIndex].Lines[LineIndex].Notes[NoteIndex].Duration * ScoreFactor[Tracks[TrackIndex].Lines[LineIndex].Notes[NoteIndex].NoteType]);
          //and to the current lines entire value
          Inc(Tracks[TrackIndex].Lines[LineIndex].ScoreValue, Tracks[TrackIndex].Lines[LineIndex].Notes[NoteIndex].Duration * ScoreFactor[Tracks[TrackIndex].Lines[LineIndex].Notes[NoteIndex].NoteType]);
        end;
      end;
    end;
  end;

  for LineIndex := 0 to High(Tracks) do
  begin
    if (foundcut[LineIndex]) and (Length(Tracks[LineIndex].Lines) > cut_line[LineIndex]) then
    begin
      SetLength(Tracks[LineIndex].Lines, cut_line[LineIndex]);
      Tracks[LineIndex].High := cut_line[LineIndex]-1;
      Tracks[LineIndex].Number := Tracks[LineIndex].High+1;
    end;
  end;
end;

void TSong.Clear();
begin
  //Main Information
  Title  := '';
  Artist := '';

  //Sortings:
  Genre    := 'Unknown';
  Edition  := 'Unknown';
  Language := 'Unknown';
  Year := 0;

  // set to default encoding
  Encoding := Ini.DefaultEncoding;

  // clear custom header tags
  SetLength(CustomTags, 0);

  //Required Information
  Mp3    := PATH_NONE;
  SetLength(BPM, 0);

  GAP    := 0;
  Start  := 0;
  Finish := 0;

  //Additional Information
  Background := PATH_NONE;
  Cover      := PATH_NONE;
  Video      := PATH_NONE;
  VideoGAP   := 0;
  NotesGAP   := 0;
  Resolution := 4;
  Creator    := '';
  PreviewStart := 0;
  CalcMedley := true;
  Medley.Source := msNone;

  isDuet := false;

  SetLength(DuetNames, 2);
  DuetNames[0] := 'P1';
  DuetNames[1] := 'P2';

  Relative := false;
end;

function TSong.Analyse(const ReadCustomTags: bool; DuetChange: bool): bool;
var
  SongFile: TTextFileStream;
begin
  Result := false;

  //Reset LineNo
  FileLineNo := 0;

  try
    //Open File and set File Pointer to the beginning
    SongFile := TMemTextFileStream.Create(Self.Path.Append(Self.FileName), fmOpenRead);
  except
    Log.LogError('Failed to open ' + Self.Path.Append(Self.FileName).ToUTF8(true));
    Exit;
  end;

  try

    //Clear old Song Header
    Self.clear;

    //Read Header
    Result := Self.ReadTxTHeader(SongFile, ReadCustomTags);

    //Load Song for Medley Tags
    CurrentSong := self;
    Result := Result and LoadSong(DuetChange);

    if Result then
    begin
      //Medley and Duet - is it possible? Perhaps later...
      if not Self.isDuet then
        Self.FindRefrain()
      else
        Self.Medley.Source := msNone;
    end;
  except
    Log.LogError('Reading headers from file failed. File incomplete or not Ultrastar txt?: ' + Self.Path.Append(Self.FileName).ToUTF8(true));
  end;
  SongFile.Free;
end;


function TSong.AnalyseXML(): bool;

begin
  Result := false;

  //Reset LineNo
  FileLineNo := 0;

  //Clear old Song Header
  self.clear;

  //Read Header
  Result := self.ReadXMLHeader( FileName );

end;

/*
function TSong.MD5SongFile(SongFileR: TTextFileStream): string;
var
  TextFile: string;
  Line: string;
  FileLineNo2: int;
begin

  TextFile := '';
  SongFileR.Position := 0;

  //Search for Note Beginning
  FileLineNo2 := 0;
  while (SongFileR.ReadLine(Line)) do
  begin
    Inc(FileLineNo2);

    if (Length(Line) > 0) and (Line[1] <> '#') then
    begin
      TextFile := TextFile + Line;
    end;
  end;
  {$IFDEF MSWINDOWS}
    Result := MD5Print(MD5String(TextFile)); //basisbit TODO
  {$ELSE}
    Result := 'unknown';
  {$ENDIF}
end;
};*/
}
