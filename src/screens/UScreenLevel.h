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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/screens/UScreenLevel.pas $
 * $Id: UScreenLevel.pas 1975 2009-12-06 14:40:10Z s_alexander $
 */
#include <SDL2/SDL.h>

#include "../menu/UMenu.h"
#include "../base/UIni.h"
#include "../base/ULog.h"

namespace UScreenLevel
{
  /*
uses
  UDisplay,
  UFiles,
  UMenu,
  UMusic,
  UThemes,
  sdl2,
  SysUtils;
*/

  class TScreenLevel : public UMenu::TMenu
  {
  public:
  
      bool ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown) override;
      void OnShow() override;
      void SetAnimationProgress(double Progress) override;
  };

const std::string ID = "ID_011";   //for help system
/*
implementation

uses
  UGraphic,
  UHelp,
  UIni,
  ULog,
  UMain,
  USong,
  UTexture,
  UUnicodeUtils;
*/
bool TScreenLevel::ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown)
{
  if (!PressedDown) 
  return true;
  // Key Down
  // check normal keys
  switch (UCS4UpperCase(CharCode))
  {
    case Ord("Q"):
      {
        return false;
      }
  }

  // check special keys
  switch (PressedKey)
  {
    case SDLK_ESCAPE: [[fallthrough]]
    case SDLK_BACKSPACE:
    {
      AudioPlayback.PlaySound(SoundLib.Back);

      if (UIni::Ini.OnSongClick == sSelectPlayer)
        FadeTo(@ScreenMain);
      else
        FadeTo(@ScreenName);
      break;
    }
    case SDLK_RETURN:
    {
      UIni::Ini.Difficulty = Interaction;
      UIni::Ini.SaveLevel();
      AudioPlayback.PlaySound(SoundLib.Start);
      //Set Standard Mode
      ScreenSong.Mode = smNormal;
      FadeTo(@ScreenSong);
      break;
    }

    // Up and Down could be done at the same time,
    // but I don"t want to declare variables inside
    // functions like this one, called so many times
    case SDLK_DOWN:    
      InteractNext();
      break;
    case SDLK_UP:      
      InteractPrev();
      break;
    case SDLK_RIGHT:   
      InteractNext();
      break;
    case SDLK_LEFT:    
      InteractPrev();
      break;
  }
  return true;
}

TScreenLevel::TScreenLevel()
{

  LoadFromTheme(Theme.Level);

  AddButton(Theme.Level.ButtonEasy);
  AddButton(Theme.Level.ButtonMedium);
  AddButton(Theme.Level.ButtonHard);

  Interaction = 0;
}

void TScreenLevel::OnShow()
{
  inherited;

  Interaction = UIni::Ini.Difficulty;
  if (!Help.SetHelpID(ID)) 
    ULog::Log.LogWarn("No Entry for Help-ID " + ID, "ScreenLevel");
}

void TScreenLevel::SetAnimationProgress(double Progress)
{
  Button[0].Texture.ScaleW = Progress;
  Button[1].Texture.ScaleW = Progress;
  Button[2].Texture.ScaleW = Progress;
}
}