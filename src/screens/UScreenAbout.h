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
#include "../switches.h"

#include <string>
#include <boost/algorithm/string/case_conv.hpp>


#include <SDL2/SDL_keycode.h>

#include "../base/UIni.h"
#include "../base/ULog.h"
#include "../base/UThemes.h"
#include "../menu/UMenu.h"

namespace UScreenAbout
{
    /*
    uses
        UDisplay,
        UIni,
        UMenu,
        UMusic,
        UThemes,
        sdl2,
        SysUtils;
        */
        //type
    class TScreenAbout : public UMenu::TMenu
    {
    public:

        int TextOverview;

        TScreenAbout();
        ~TScreenAbout() override;

        bool ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown) override;
        void OnShow() override;
        void SetAnimationProgress(double Progress) override;

        void SetOverview();
    };

    constexpr std::string ID = "ID_002";   //for help system

    /*
    implementation

        uses
        UCommon,
        UDataBase,
        UGraphic,
        UHelp,
        ULanguage,
        ULog,
        USong,
        USongs,
        UUnicodeUtils,
        Classes;
*/

    bool TScreenAbout::ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown)
    {
        bool Result = true;
        if (PressedDown)
        { // Key Down
          // check normal keys
            switch (boost::algorithm::to_upper(CharCode))
            {
            case U'C':
            {
                FadeTo(@ScreenCredits, SoundLib.Start);
                return Result;
            }
            case U'Q':
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
                //Exit Button Pressed
                if (Interaction == 1)
                {
                    AudioPlayback.PlaySound(SoundLib.Back);
                    FadeTo(@ScreenMain);
                }

                // ultrastar deluxe team credits
                if (Interaction == 0)
                {
                    AudioPlayback.PlaySound(SoundLib.Back);
                    FadeTo(@ScreenCredits);
                }
                break;
            }
            case SDLK_LEFT:
            {
                InteractPrev();
                break;
            }
            case SDLK_RIGHT:
            {
                InteractNext();
                break;
            }
            case SDLK_UP:
            {
                InteractPrev();
                break;
            }
            case SDLK_DOWN:
            {
                InteractNext();
                break;
            }
            }
        }
    }

    TScreenAbout::TScreenAbout()
    {
        inherited Create;

        TextOverview = AddText(UThemes::Theme.AboutMain.TextOverview);

        LoadFromTheme(UThemes::Theme.AboutMain);

        AddButton(UThemes::Theme.AboutMain.ButtonCredits);
        AddButton(UThemes::Theme.AboutMain.ButtonExit);

        Interaction = 0;
    }

    void TScreenAbout::OnShow()
    {
        inherited;

        if (!Help.SetHelpID(ID))
            ULog::Log.LogWarnLocation("No Entry for Help-ID " + ID);

        //Set Overview Text:
        SetOverview();
    }

    void TScreenAbout::SetOverview()
    /*var
        Overview : UTF8String;*/
    {
        // Format overview
        Overview = Language.Translate("ABOUT_OVERVIEW");
        Text[0].Text = Overview;
    }

    void TScreenAbout::SetAnimationProgress(double Progress)
    {
        for (auto& button : Button)
            button.Texture.ScaleW = Progress;
    }
}
