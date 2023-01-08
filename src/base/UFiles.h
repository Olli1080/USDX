#pragma once
/** UltraStar Deluxe - Karaoke Game
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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UFiles.pas $
 * $Id: UFiles.pas 2510 2010-06-13 09:03:10Z tobigun $
 **/

#include "../switches.h"

#include <vector>
#include <variant>
#include <fstream>

#include <hash-library/md5.h>

#include "UMusic.h"
#include "USong.h"

namespace UFiles
{
/*uses
  SysUtils,
  Classes,
  ULog,
  UMusic,
  USongs,
  USong,
  UPath;*/


constexpr SecDouble DEFAULT_FADE_IN_TIME = SecDouble(8);   // for medley fade-in
constexpr SecDouble DEFAULT_FADE_OUT_TIME = SecDouble(2);  // for medley fade-out

constexpr int Mult = 1; // accuracy of measurement of note
constexpr int MultBPM = 4; // multiply beat-count of note by 4

//--------------------
// Resets the temporary Sentence Arrays for each Player and some other Variables
//--------------------
void ResetSingTemp();

enum TSaveSongResult
{
    ssrOK, ssrFileError, ssrEncodingError
};

static std::optional<std::string> fileHash(const std::filesystem::path& path);

/***
 * Throws a TEncodingException if the song"s fields cannot be encoded in the
 * requested encoding.
 **/
struct SongPayLoad
{
    USong::TSong Song;
    UMusic::TrackVec Tracks; //during loading size in [1, 2]
};

enum class TMedleySource
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

/*{ used to hold header tags that are not supported by this version of
  usdx (e.g. some tags from ultrastar 0.7.0) when songs are loaded in
  songeditor. They will be written the end of the song header }*/
struct TCustomHeaderTag
{
    std::string Tag;
    std::string Content;
};

struct TBPM
{
    double BPM;
    double StartBeat;
};

struct SongPaths
{
    // filenames
    std::filesystem::path Cover;
    std::filesystem::path Mp3;
    std::filesystem::path Background;
    std::filesystem::path Video;
};

struct SongHeader
{
    std::vector<TCustomHeaderTag> CustomTags;

    bool CalcMedley = true;  // if true => do not calc medley for that song

    std::string Encoding;
    std::string Language;

    std::string Title;
    std::string Artist;
    std::string Edition;
    std::string Genre;
    std::string Creator;

    SongPaths paths;

    int Year;

    double VideoGAP;
    int Resolution;
    int NotesGAP;

    SecDouble Start; // in seconds
    std::chrono::milliseconds Finish; // in milliseconds

    bool Relative = false;
    TMedley medley;

    std::vector<TBPM> BPM;
    MiliSecDouble GAP;

    std::array<std::string, 2> DuetNames = { "P1", "P2" };

    SecDouble PreviewStart;   // in seconds
    bool HasPreview;  // set if a valid PreviewStart was read
};

struct SongData
{
    std::filesystem::path Path;
    //std::filesystem::path Filename;
    std::string md5;
    SongHeader header;
    SongPayLoad payload;
};

class EUSDXParseException : public std::exception
{
public:

    EUSDXParseException(const char* str) : std::exception(str)
    {}
};

class SongIO
{
public:

    static TSaveSongResult SaveSongTXT(const SongData& SongData, const std::vector<UMusic::TLines>& Tracks, const std::filesystem::path Name, bool Relative);
    static std::optional<SongPayLoad> LoadSongTXTPayload(std::ifstream& SongFile, SongHeader& header, size_t& FileLineNo);
    static std::optional<SongData> LoadSongTXT(const std::filesystem::path& FileNamePath);

private:
    
    template<typename T>
    static T ParseLyricParam(std::string_view Line, size_t& LinePos, size_t FileLineNo = -1);
    static std::string_view ParseLyricText(std::string_view Line, size_t& LinePos);
    static void ParseNote(UMusic::TLines& track, char TypeP, int StartP, int DurationP, int NoteP, std::string_view LyricS);
    
    static void NewSentence(UMusic::TLines& track, int StartBeatP, int RelativeP, bool relative, bool isDuet, size_t FileLineNo = -1);

    static std::optional<SongHeader> ReadTXTHeader(std::ifstream& SongFile, size_t& FileLineNo, const std::filesystem::path& Path, bool ReadCustomTags = false);

    static int calcStartBeat(int progress, int mult, int rel_p = 0);
};
/*uses
  TextGL,
  UIni,
  UNote,
  UPlatform,
  UUnicodeUtils,
  UTextEncoding;*/

/*
RawByteString EncodeToken(const std::string& Str)
{
	bool Success = EncodeStringUTF8(Str, Result, Song.Encoding);
    if (not Success) then
        SaveSong = ssrEncodingError;
}
*/

void WriteCustomTags();

}