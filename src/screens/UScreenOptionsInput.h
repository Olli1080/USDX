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
 * $URL: $
 * $Id: $
 */

#include <SDL2/SDL.h>

#include "../base/UIni.h"
#include "../base/ULog.h"
#include "../menu/UMenu.h"

namespace UScreenOptionsInput
{
/*uses
  UCommon,
  UIni,
  UJoystick,
  UMenu,
  UMusic,
  UThemes,
  sdl2;

type
*/
  typedef setOfByte TByteSet;

  class TScreenOptionsInput : public UMenu::TMenu
  {
    protected:
      // interaction IDs
      int ButtonExitIID;
      int SelectMouse;
      int SelectJoyPad;

      // values
      int ActualMouse;

      TByteSet SoundInteractions;

      bool WasJoy;
      bool WasMouse;

    protected:
      void CheckOption();

    public:

      TScreenOptionsInput();
      bool ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown) override;

      void OnShow() override;
      void OnHide() override;
  };

const std::string ID = "ID_074";   //for help system
/*
implementation

uses
  UDisplay,
  UGraphic,
  UHelp,
  ULog,
  UUnicodeUtils,
  SysUtils;
*/
bool TScreenOptionsInput::ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown)
{
  if (!PressedDown)
    return true;
  // Key Down
  // check normal keys
  switch (UCS4UpperCase(CharCode))
  {
    Ord("Q"):
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
      UIni::Ini.Save();
      AudioPlayback.PlaySound(SoundLib.Back);
      FadeTo(@ScreenOptions);
      break;
    }
    case SDLK_TAB:
    {
      ScreenPopupHelp.ShowPopup();
      break;
    }
    case SDLK_RETURN:
    {
      if (Interaction == ButtonExitIID)
      {
        UIni::Ini.Save();
        AudioPlayback.PlaySound(SoundLib.Back);
        FadeTo(@ScreenOptions);
      }
    }
    case SDLK_DOWN:
      InteractNext();
      break;
    case SDLK_UP:
      InteractPrev();
      break;
    case SDLK_RIGHT:
    {
      if (Interaction in SoundInteractions) 
      {
        AudioPlayback.PlaySound(SoundLib.Option);
        InteractInc();
      }
      CheckOption();
      break;
    }
    case SDLK_LEFT:
    {
      if (Interaction in SoundInteractions) 
      {
        AudioPlayback.PlaySound(SoundLib.Option);
        InteractDec();
      }
      CheckOption();
      break;
    }
  }
}

TScreenOptionsInput::TScreenOptionsInput()
{
  //inherited Create;
  SoundInteractions = [];

  LoadFromTheme(Theme.OptionsInput);

  Theme.OptionsInput.SelectMouse.showArrows = true;
  Theme.OptionsInput.SelectMouse.oneItemOnly = true;
  SelectMouse = AddSelectSlide(Theme.OptionsInput.SelectMouse, ActualMouse, IMouseTranslated);
  Include(SoundInteractions, SelectMouse);

  Theme.OptionsInput.SelectJoypad.showArrows = true;
  Theme.OptionsInput.SelectJoypad.oneItemOnly = true;
  SelectJoyPad = AddSelectSlide(Theme.OptionsInput.SelectJoypad, UIni::Ini.Joypad, IJoypad);
  Include(SoundInteractions, SelectJoyPad);


  AddButton(Theme.OptionsInput.ButtonExit);
  ButtonExitIID = High(Interactions);
  if (Length(Button[0].Text)=0) 
    AddButtonText(20, 5, Theme.Options.Description[OPTIONS_DESC_INDEX_BACK]);

}

void TScreenOptionsInput::OnShow()
{
  //inherited;

  if (!Help.SetHelpID(ID)) 
    ULog::Log.LogError("No Entry for Help-ID " + ID + " (ScreenOptionsInput)");

  ActualMouse = UIni::Ini.Mouse;
  UpdateSelectSlideOptions(Theme.OptionsInput.SelectMouse, SelectMouse, IMouseTranslated, ActualMouse);

  WasMouse = UIni::Ini.Mouse > 0;
  WasJoy = HasJoyStick;
  Interaction = 0;
}

void TScreenOptionsInput::OnHide()
{
  //inherited;

  // TODO: query confirm whether to disable mouse
  // TODO: RattleSN4K3: query confirm whether to disable Joypad (requires interaction mode, Controller/Mouse/Keyboard)

  // update actual Mouse configuration
  UIni::Ini.Mouse = ActualMouse;
  Display.SetCursor;

  // re-initialize or remove joypad support
  if (WasJoy && UIni::Ini.Joypad == 0) 
  {
    FinalizeJoyStick;
  } 
  else if (!WasJoy && UIni::Ini.Joypad == 1)
    InitializeJoystick;
}

void TScreenOptionsInput::CheckOption()
{
  //inherited;

  if (Interaction == SelectMouse) 
  {
    if (ActualMouse > 0)
      UIni::Ini.Mouse = ActualMouse;
    else 
      UIni::Ini.Mouse = 1;
    Display.SetCursor();
    Display.UpdateCursor();
  }
}
}