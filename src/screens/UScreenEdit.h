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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/screens/UScreenEdit.pas $
 * $Id: UScreenEdit.pas 2246 2010-04-18 13:43:36Z tobigun $
 */
#include <SDL2/SDL_keyboard.h>

#include "../switches.h"

#include "../menu/UMenu.h"

namespace UScreenEdit
{
    /*uses
      UMenu,
      UThemes,
      sdl2;*/

    class TScreenEdit : public UMenu::TMenu
    {
    public:
        int TextDescription;
        int TextDescriptionLong;

        TScreenEdit();
        ~TScreenEdit() override = default;

        void OnShow() override;
        bool ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown) override;
        void SetInteraction(int Num) override;
        void SetAnimationProgress(double Progress) override;
    };

    const std::string ID = "ID_060";   //for help system

    /*implementation

        uses
        UGraphic,
        UHelp,
        ULog,
        UMusic,
        USkins,
        UUnicodeUtils,
        SysUtils;*/

    bool TScreenEdit::ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown)
        //var
          //  SDL_ModState : word;
    {
    Result: = true;

        int SDL_ModState = SDL_GetModState() & (KMOD_LSHIFT | KMOD_RSHIFT | KMOD_LCTRL | KMOD_RCTRL | KMOD_LALT | KMOD_RALT);

        if (PressedDown)
        { // Key Down
              // check normal keys
            switch (UCS4UpperCase(CharCode))
            {
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
                if (Interaction == 0)
                {
                    AudioPlayback.PlaySound(SoundLib.Start);
                    FadeTo(@ScreenEditConvert);
                }

                if (Interaction == 1)
                {
                    AudioPlayback.PlaySound(SoundLib.Back);
                    FadeTo(@ScreenMain);
                }
                break;
            }
            case SDLK_DOWN:
                InteractInc();
                break;
            case SDLK_UP:
                InteractDec();
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

    TScreenEdit::TScreenEdit()
    {
        inherited Create;

        TextDescription = AddText(Theme.Edit.TextDescription);

        LoadFromTheme(Theme.Edit);

        AddButton(Theme.Edit.ButtonConvert);
        /*TODO:? Some ideas for more:
        AddButton(Theme.Edit.ButtonEditHeaders);
        AddButton(Theme.Edit.ButtonAdjustGap);
        */
        AddButton(Theme.Edit.ButtonExit);

        Interaction = 0;
    }

    void TScreenEdit::OnShow()
    {
        inherited;

        if (!Help.SetHelpID(ID))
            ULog::Log.LogWarn('No Entry for Help-ID ' + ID, 'ScreenEdit');

        // continue possibly stopped bg-music (stopped in midi import screen)
        SoundLib.StartBgMusic;
    }

    void TScreenEdit::SetInteraction(int Num)
    {
        inherited SetInteraction(Num);
        Text[TextDescription].Text = Theme.Edit.Description[Interaction];

        // long description not used atm // IMPROVE: Theme
        //Text[TextDescriptionLong].Text := Theme.Edit.DescriptionLong[Interaction];
    }

    void TScreenEdit::SetAnimationProgress(double Progress)
    {
        Statics[0].Texture.ScaleW = Progress;
        Statics[0].Texture.ScaleH = Progress;
    }
}