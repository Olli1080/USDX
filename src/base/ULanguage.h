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

/**
 * Load the specified LanguageFile
 */
void TLanguage.ChangeLanguage(const Language: AnsiString);
var
  IniFile:    TUnicodeMemIniFile;
  E:          integer; // entry
  S:          TStringList;
begin
  SetLength(Entry, 0);
  IniFile := TUnicodeMemIniFile.Create(LanguagesPath.Append(Language + '.ini'));
  S := TStringList.Create;

  IniFile.ReadSectionValues('Text', S);
  SetLength(Entry, S.Count);
  for E := 0 to high(Entry) do
  begin
    if S.Names[E] = 'IMPLODE_GLUE1' then
      Implode_Glue1 := S.ValueFromIndex[E]+ ' '
    else if S.Names[E] = 'IMPLODE_GLUE2' then
      Implode_Glue2 := ' ' + S.ValueFromIndex[E] + ' ';

    Entry[E].ID := S.Names[E];
    Entry[E].Text := S.ValueFromIndex[E];
  end;

  S.Free;
  IniFile.Free;
end;

/**
 * Find the index of ID an array of language entries.
 * @returns the index on success, -1 otherwise.
 */
function TLanguage.FindID(const ID: AnsiString; const EntryList: TLanguageEntryArray): integer;
var
  Index: integer;
begin
  for Index := 0 to High(EntryList) do
  begin
    if ID = EntryList[Index].ID then
    begin
      Result := Index;
      Exit;
    end;
  end;
  Result := -1;
end;

/**
 * Translate the Text.
 * If Text is an ID, text will be translated according to the current language
 * setting. If Text is not a known ID, it will be returned as is. 
 * @param Text either an ID or an UTF-8 encoded string 
 */
function TLanguage.Translate(const Text: RawByteString): UTF8String;
var
  ID: AnsiString;
  EntryIndex: integer;
begin
  // fallback result in case Text is not a known ID
  Result := Text;

  // normalize ID case
  ID := UpperCase(Text);

  // Check if ID exists

  //Const Mod
  EntryIndex := FindID(ID, EntryConst);
  if (EntryIndex >= 0) then
  begin
    Result := EntryConst[EntryIndex].Text;
    Exit;
  end;

  EntryIndex := FindID(ID, Entry);
  if (EntryIndex >= 0) then
  begin
    Result := Entry[EntryIndex].Text;
    Exit;
  end;

  //Standard Language (If a Language File is Incomplete)
  //Then use Standard Language
  EntryIndex := FindID(ID, EntryDefault);
  if (EntryIndex >= 0) then
  begin
    Result := EntryDefault[EntryIndex].Text;
    Exit;
  end;
end;

/**
 * Add a Constant ID that will be Translated but not Loaded from the LanguageFile
 */
void TLanguage.AddConst(const ID: AnsiString; const Text: UTF8String);
begin
  SetLength (EntryConst, Length(EntryConst) + 1);
  EntryConst[high(EntryConst)].ID := ID;
  EntryConst[high(EntryConst)].Text := Text;
end;

/**
 * Change a Constant Value by ID
 */
void TLanguage.ChangeConst(const ID: AnsiString; const Text: UTF8String);
var
  I: Integer;
begin
  for I := 0 to high(EntryConst) do
  begin
    if EntryConst[I].ID = ID then
    begin
     EntryConst[I].Text := Text;
     Break;
    end;
  end;
end;

/**
 * Connect an array of strings with ' and ' or ', ' to one string
 */
function TLanguage.Implode(const Pieces: array of UTF8String): UTF8String;
var
  I: Integer;
begin
  Result := '';
  //Go through Pieces
  for I := 0 to high(Pieces) do
  begin
    //Add Value
    Result := Result + Pieces[I];

    //Add Glue
    if (I < high(Pieces) - 1) then
      Result := Result + Implode_Glue1
    else if (I < high(Pieces)) then
      Result := Result + Implode_Glue2;
  end;
end;

end.
};