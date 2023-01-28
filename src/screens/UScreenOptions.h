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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/screens/UScreenOptions.pas $
 * $Id: UScreenOptions.pas 2649 2010-10-10 10:34:20Z tobigun $
 */
#include <string>
#include <vector>

#include <SDL2/SDL_keycode.h>

#include "../menu/UMenu.h"

namespace UScreenOptions
{
  /*uses
    UDisplay,
    UFiles,
    UIni,
    UMenu,
    UMusic,
    USongs,
    UThemes,
    sdl2,
    SysUtils;*/

  class TScreenOptions : public UMenu::TMenu
  {
  private:
    uint32_t ButtonGameIID;
    uint32_t ButtonGraphicsIID;
    uint32_t ButtonSoundIID;
    uint32_t ButtonInputIID;
    uint32_t ButtonLyricsIID;
    uint32_t ButtonThemesIID;
    uint32_t ButtonRecordIID;
    uint32_t ButtonAdvancedIID;
    uint32_t ButtonNetworkIID;
    uint32_t ButtonWebcamIID;
    uint32_t ButtonJukeboxIID;
    uint32_t ButtonExitIID;

    std::vector<int> MapIIDtoDescID;

    virtual void UpdateTextDescriptionFor(int IID);

  public:
    TScreenOptions();

    int TextDescription;
    bool ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown) override;
    void OnShow() override;
    void SetInteraction(int Num) override;
    void SetAnimationProgress(double Progress) override;
  };

  const std::string ID = "ID_070"; // for help system

  /*implementation

  uses
    UDatabase,
    UGraphic,
    UHelp,
    ULanguage,
    ULog,
    UWebcam,
    UUnicodeUtils;*/

  bool TScreenOptions::ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown)
  {
    Result = true;
    if (PressedDown)
    { // Key Down
      // check normal keys
      switch (UCS4UpperCase(CharCode))
      {
      case Ord('G'):
      {
        FadeTo(@ScreenOptionsGame, SoundLib.Start);
        return true;
      }

      case Ord('H'):
      {
        FadeTo(@ScreenOptionsGraphics, SoundLib.Start);
        return true;
      }

      case Ord('S'):
      {
        FadeTo(@ScreenOptionsSound, SoundLib.Start);
        return true;
      }

      case Ord('I'):
      {
        FadeTo(@ScreenOptionsInput, SoundLib.Start);
        return true;
      }

      case Ord('L'):
      {
        FadeTo(@ScreenOptionsLyrics, SoundLib.Start);
        return true;
      }

      case Ord('T'):
      {
        FadeTo(@ScreenOptionsThemes, SoundLib.Start);
        return true;
      }

      case Ord('R'):
      {
        FadeTo(@ScreenOptionsRecord, SoundLib.Start);
        return true;
      }

      case Ord('A'):
      {
        FadeTo(@ScreenOptionsAdvanced, SoundLib.Start);
        return true;
      }

      case Ord('N'):
      {
        if (High(DataBase.NetworkUser) = -1)
          ScreenPopupError.ShowPopup(Language.Translate('SING_OPTIONS_NETWORK_NO_DLL'));
        else
        {
          AudioPlayback.PlaySound(SoundLib.Back);
          FadeTo(@ScreenOptionsNetwork);
        }
        return true;
      }

      case Ord('W'):
      {
        FadeTo(@ScreenOptionsWebcam, SoundLib.Start);
        return true;
      }

      case Ord('J'):
      {
        FadeTo(@ScreenOptionsJukebox, SoundLib.Start);
        return true;
      }

      case Ord('Q'):
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
        Ini.Save;
        AudioPlayback.PlaySound(SoundLib.Back);
        FadeTo(@ScreenMain);
        break;
      }
      case SDLK_TAB:
      {
        ScreenPopupHelp.ShowPopup();
        break;
      }
      case SDLK_RETURN:
      {
        if (Interaction == ButtonGameIID)
        {
          AudioPlayback.PlaySound(SoundLib.Start);
          FadeTo(@ScreenOptionsGame);
        }

        if (Interaction == ButtonGraphicsIID)
        {
          AudioPlayback.PlaySound(SoundLib.Start);
          FadeTo(@ScreenOptionsGraphics);
        }

        if (Interaction == ButtonSoundIID)
        {
          AudioPlayback.PlaySound(SoundLib.Start);
          FadeTo(@ScreenOptionsSound);
        }

        if (Interaction == ButtonInputIID)
        {
          AudioPlayback.PlaySound(SoundLib.Start);
          FadeTo(@ScreenOptionsInput);
        }

        if (Interaction == ButtonLyricsIID)
        {
          AudioPlayback.PlaySound(SoundLib.Start);
          FadeTo(@ScreenOptionsLyrics);
        }

        if (Interaction == ButtonThemesIID)
        {
          AudioPlayback.PlaySound(SoundLib.Start);
          FadeTo(@ScreenOptionsThemes);
        }

        if (Interaction == ButtonRecordIID)
        {
          AudioPlayback.PlaySound(SoundLib.Start);
          FadeTo(@ScreenOptionsRecord);
        }

        if (Interaction == ButtonAdvancedIID)
        {
          AudioPlayback.PlaySound(SoundLib.Start);
          FadeTo(@ScreenOptionsAdvanced);
        }

        if (Interaction == ButtonNetworkIID)
        {
          if (High(DataBase.NetworkUser) = -1)
            ScreenPopupError.ShowPopup(Language.Translate('SING_OPTIONS_NETWORK_NO_DLL')) else
            {
              AudioPlayback.PlaySound(SoundLib.Back);
              FadeTo(@ScreenOptionsNetwork);
            }
        }

        if (Interaction == ButtonWebcamIID)
        {
          AudioPlayback.PlaySound(SoundLib.Back);
          FadeTo(@ScreenOptionsWebcam);
        }

        if (Interaction == ButtonJukeboxIID)
        {
          if (Songs.SongList.Count >= 1)
          {
            AudioPlayback.PlaySound(SoundLib.Start);
            FadeTo(@ScreenOptionsJukebox);
          }
          else // show error message, No Songs Loaded
            ScreenPopupError.ShowPopup(Language.Translate('ERROR_NO_SONGS'));
        }

        if (Interaction == ButtonExitIID)
        {
          Ini.Save;
          AudioPlayback.PlaySound(SoundLib.Back);
          FadeTo(@ScreenMain);
        }
        break;
      }
      case SDLK_DOWN:
        InteractNextRow();
        break;
      case SDLK_UP:
        InteractPrevRow();
        break;
      case SDLK_RIGHT:
        InteractNext();
        break;
      case SDLK_LEFT:
        InteractPrev();
        break;
      }
    }
  }

  TScreenOptions::TScreenOptions()
  {
    inherited Create;

    TextDescription = AddText(Theme.Options.TextDescription);

    LoadFromTheme(Theme.Options);

    // TODO: Generalize method and implement it into base code (to be used by every screen/menu)
    auto AddButtonChecked = [this](TThemeButton Btn, uint8_t DescIndex, uint32_t &IIDvar, double AddX = 14, double AddY = 20) -> uint32_t
    // var OldPos: integer;
    {
      size_t OldPos = Button.size();
      auto Result = AddButton(Btn);
      if (Button.size() != OldPos) // check if button was succesfully added // TODO: RattleSN4K3: Improve AddButton interface returning properly index to be used by interaction check
      {
        IIDvar = Interactions.size() - 1;

        // update mapping, IID to Desc index
        MapIIDtoDescID.emplace_back(IIDvar, DescIndex);

        if (Button[Result].Text.empty()) // update text if not already set
          AddButtonText(AddX, AddY, Theme.Options.Description[DescIndex]);
      }
      return Result;
    };

    // Order is irrelevant to the represenatation, however InteractNext/Prev is not working with a different order // TODO: RattleSN4K3: allow InteractNext etc. work with themes having a different button layout
    AddButtonChecked(Theme.Options.ButtonGame, OPTIONS_DESC_INDEX_GAME, ButtonGameIID);
    AddButtonChecked(Theme.Options.ButtonGraphics, OPTIONS_DESC_INDEX_GRAPHICS, ButtonGraphicsIID);
    AddButtonChecked(Theme.Options.ButtonSound, OPTIONS_DESC_INDEX_SOUND, ButtonSoundIID);
    AddButtonChecked(Theme.Options.ButtonInput, OPTIONS_DESC_INDEX_INPUT, ButtonInputIID);

    AddButtonChecked(Theme.Options.ButtonLyrics, OPTIONS_DESC_INDEX_LYRICS, ButtonLyricsIID);
    AddButtonChecked(Theme.Options.ButtonThemes, OPTIONS_DESC_INDEX_THEMES, ButtonThemesIID);
    AddButtonChecked(Theme.Options.ButtonRecord, OPTIONS_DESC_INDEX_RECORD, ButtonRecordIID);
    AddButtonChecked(Theme.Options.ButtonAdvanced, OPTIONS_DESC_INDEX_ADVANCED, ButtonAdvancedIID);
    AddButtonChecked(Theme.Options.ButtonNetwork, OPTIONS_DESC_INDEX_NETWORK, ButtonNetworkIID);

    AddButtonChecked(Theme.Options.ButtonWebcam, OPTIONS_DESC_INDEX_WEBCAM, ButtonWebcamIID);
    AddButtonChecked(Theme.Options.ButtonJukebox, OPTIONS_DESC_INDEX_JUKEBOX, ButtonJukeboxIID);

    AddButtonChecked(Theme.Options.ButtonExit, OPTIONS_DESC_INDEX_BACK, ButtonExitIID);

    Interaction = 0;
  }

  void TScreenOptions::OnShow()
  {
    inherited;

    if (!Help.SetHelpID(ID))
      ULog::Log.LogWarn("No Entry for Help-ID " + ID, "ScreenOptions");

    // continue possibly stopped bg-music (stopped in record options)
    SoundLib.StartBgMusic;
  }

  void TScreenOptions::SetInteraction(int Num)
  {
    UMenu::SetInteraction(Num);
    UpdateTextDescriptionFor(Interaction);
  }

  void TScreenOptions::SetAnimationProgress(double Progress)
  {
    // update all buttons
    for (auto &button : Button)
      button.Texture.ScaleW = Progress;
  }

  void TScreenOptions::UpdateTextDescriptionFor(int IID)
  {
    // Sanity check
    if (IID < 0 || IID >= MapIIDtoDescID.size())
      return;

    Text[TextDescription].Text = Theme.Options.Description[MapIIDtoDescID[IID]];
  }
}