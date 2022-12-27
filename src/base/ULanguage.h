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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/ULanguage.pas $
 * $Id: ULanguage.pas 1939 2009-11-09 00:27:55Z s_alexander $
 */
#include <string>
#include <vector>
#include <list>

#include "switches.h"
#include "UIni.h"

namespace ULanguage
{

//uses
 // UUnicodeUtils;

struct TLanguageEntry
{
  std::string ID;  //**< identifier (ASCII)
  std::string Text;  //**< translation (UTF-8)
};

struct TLanguageList
{
  std::string Name;  //**< language name (ASCII)
};

class TLanguage
{
private:

  std::vector<TLanguageList> List;

  std::vector<TLanguageEntry> Entry; //**< Entrys of Chosen Language
  std::vector<TLanguageEntry> EntryDefault; //**< Entrys of Standard Language
  std::vector<TLanguageEntry> EntryConst; //**< Constant Entrys e.g. Version

  std::string Implode_Glue1, Implode_Glue2;

  void LoadList();
  int FindID(const std::string ID, const std::vector<TLanguageEntry> EntryList);

public:

  TLanguage();
  std::string Translate(const std::string Text);
  void ChangeLanguage(const std::string Language);
  void AddConst(const std::string ID, const std::string Text);
  void ChangeConst(const std::string ID, const std::string Text);
  std::string Implode(const std::vector<std::string> Pieces);
};

TLanguage Language;
/*
uses
  UMain,
  UIni,
  IniFiles,
  Classes,
  SysUtils,
  ULog,
  UPath,
  UFilesystem,
  UPathUtils;
*/
};