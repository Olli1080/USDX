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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UXMLSong.pas $
 * $Id: UXMLSong.pas 1939 2009-11-09 00:27:55Z s_alexander $
 */

#include "switches.h"

#include <string>
#include <vector>
#include <list>
#include <filesystem>
#include <cstdint>

namespace UXMLSong
{
/*
uses
  Classes,
  UPath,
  UUnicodeUtils;
*/
struct TNote
{
  uint32_t Start;
  uint32_t Duration;
  int Tone;
  uint8_t NoteTyp;
  std::string Lyric;
};

typedef std::vector<TNote> ANote;

struct TSentence
{
  uint8_t Singer;
  uint32_t Duration;
  ANote Notes;
};

typedef std::vector<TSentence> ASentence;

struct TSongInfo
{
  uint32_t ID;
  bool DualChannel;
  struct Heading
  {
    std::string Artist;
    std::string Title;
    uint32_t Gap;
    double BPM;
    uint8_t Resolution;
    std::string Edition;
    std::string Genre;
    std::string Year;
    std::string Language;
  } Header;
  uint32_t CountSentences;
  ASentence Sentences;
};

class TParser
{
private:
  std::list<std::string> SSFile;

  uint8_t ParserState;
  uint32_t CurPosInSong; // Cur Beat Pos in the Song
  uint8_t CurDuetSinger;     // Who sings this Part?
  bool BindLyrics;  // Should the Lyrics be bind to the last Word (no Space)
  bool FirstNote;  // Is this the First Note found? For Gap calculating

  bool ParseLine(std::string Line);

public:
  TSongInfo SongInfo;
  std::string ErrorMessage;
  std::string Edition;
  std::string SingstarVersion;

  struct Settings
  {
    char DashReplacement;
  };

  TParser();

  std::string ParseConfigForEdition(const std::filesystem::path Filename);

  bool ParseSongHeader(const std::filesystem::path Filename); //Parse Song Header only
  bool ParseSong (const std::filesystem::path Filename);      //Parse whole Song
};

const int PS_None = 0;
const int PS_Melody = 1;
const int PS_Sentence = 2;

const int NT_Normal = 1;
const int NT_Freestyle = 0;
const int NT_Golden = 2;
const int NT_Rap = 3;
const int NT_RapGolden = 4;

const int DS_Player1 = 1;
const int DS_Player2 = 2;
const int DS_Both    = 3;

/*uses
  SysUtils,
  StrUtils;
*/

}