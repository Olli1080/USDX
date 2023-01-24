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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/screens/UScreenOpen.pas $
 * $Id: UScreenOpen.pas 1939 2009-11-09 00:27:55Z s_alexander $
 */
#include <string>
#include <filesystem>

#include <SDL2/SDL.h>

#include "../menu/UMenu.h"

namespace UScreenOpen
{
  /*
uses
  UMenu,
  UMusic,
  UFiles,
  UIni,
  ULyrics,
  UMenuText,
  UPath,
  USongs,
  UTexture,
  UThemes,
  UTime,
  dglOpenGL,
  Math,
  sdl2,
  SysUtils;
*/
class TScreenOpen : public UMenu::TMenu
{
  private:
    //fTextF:      array[0..1] of int;
    int FileNameID; // button ID of filename
    std::filesystem::path fFilename;
    UMenu::TMenu fBackScreen;

  public:

    TScreenOpen();
    void OnShow() override;
    bool ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown) override;

    /**
     * Set by the caller to provide a default filename.
     * Set to the selected filename after calling this screen or to PATH_NONE
     * if the screen was aborted.
     * TODO: maybe pass this value with a callback OnValueChanged()
     */
    
    //TODO:: property Filename: IPath READ fFilename WRITE fFilename;
    /** The screen that is shown after this screen is closed (set by the caller) */
    //TODO:: property BackScreen: PMenu READ fBackScreen WRITE fBackScreen;
};

const std::string ID = "ID_062";   //for help system
/*
implementation

uses
  UDraw,
  UGraphic,
  UHelp,
  ULog,
  UMain,
  USkins,
  UUnicodeUtils;
*/
bool TScreenOpen::ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown)
{
  if (!PressedDown)  // Key Down
    return true;

  // check normal keys
  if (IsPrintableChar(CharCode)) 
  {
    if (Interaction == 0) 
    {
      Button[FileNameID].Text[0].Text = Button[FileNameID].Text[0].Text + UCS4ToUTF8String(CharCode);
      return true;
    }
  }

  // check special keys
  switch (PressedKey)
  {
    case SDLK_BACKSPACE: // del
    {
      if (Interaction == 0)
      {
        Button[FileNameID].Text[0].DeleteLastLetter;
      }
      break;
    }
    case SDLK_ESCAPE:
    {
      //Empty Filename and go to last Screen
      fFilename.clear();
      AudioPlayback.PlaySound(SoundLib.Back);
      FadeTo(fBackScreen);
      break;
    }
    case SDLK_RETURN:
    {
      if (Interaction = 0) 
      {
        InteractNext;
      }
      else if (Interaction = 1) 
      {
        //Update Filename and go to last Screen
        fFilename = Button[0].Text[0].Text;
        AudioPlayback.PlaySound(SoundLib.Back);
        FadeTo(fBackScreen);
      }
      else if (Interaction = 2) 
      {
        //Empty Filename and go to last Screen
        fFilename.clear();
        AudioPlayback.PlaySound(SoundLib.Back);
        FadeTo(fBackScreen);
      }
      break;
    }
    case SDLK_TAB:
    {
      ScreenPopupHelp.ShowPopup();
      break;
    }
    case SDLK_LEFT: [[fallthrough]]
    case SDLK_UP:
    {
      InteractPrev();
      break;
    }
    case SDLK_RIGHT: [[fallthrough]]
    case SDLK_DOWN:
    {
      InteractNext();
      break;
    }
  }
  return true;
}

TScreenOpen::TScreenOpen()
{
  inherited Create;

  LoadFromTheme(Theme.EditOpen);

  // button with editable text for filename
  FileNameID = AddButton(Theme.EditOpen.ButtonFileName);
  Button[FileNameID].Text[0].Writable = true;

  // buttons
  AddButton(Theme.EditOpen.ButtonLoad);
  AddButton(Theme.EditOpen.ButtonBack);
}

void TScreenOpen::OnShow()
{
  inherited;

  if not Help.SetHelpID(ID) 
    Log.LogWarn("No Entry for Help-ID " + ID, "ScreenOpen");

  Interaction = 0;
  Button[FileNameID].Text[0].Text = fFilename.string();
}
}