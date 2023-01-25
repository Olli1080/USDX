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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/screens/UScreenOptionsSound.pas $
 * $Id: UScreenOptionsSound.pas 3124 2015-08-23 03:31:49Z basisbit $
 */

#include <string>
#include <SDL2/SDL_keycode.h>

#include "../base/ULog.h"
#include "../menu/UMenu.h"

namespace UScreenOptionsSound
{
    /*
uses
  UDisplay,
  UFiles,
  UIni,
  UMenu,
  UMusic,
  UThemes,
  sdl2;
  */

    class TScreenOptionsSound : public UMenu::TMenu
    {
    public:

        TScreenOptionsSound();
        bool ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown) override;
        void OnShow() override;
    };

    const std::string ID = "ID_073";   //for help system
    /*
    implementation

        uses
        UGraphic,
        UHelp,
        ULog,
        UUnicodeUtils,
        SysUtils;
        */
    bool TScreenOptionsSound::ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown)
    {
        if (PressedDown)
        { // Key Down
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
                Ini.Save;
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
                if (SelInteraction == 7)
                {
                    Ini.Save;
                    AudioPlayback.PlaySound(SoundLib.Back);
                    FadeTo(@ScreenOptions);
                }
                break;
            }
            case SDLK_DOWN:
                InteractNext();
                break;
            case SDLK_UP:
                InteractPrev();
                break;
            case SDLK_RIGHT:
            {
                if (SelInteraction >= 0 && SelInteraction < 8)
                {
                    AudioPlayback.PlaySound(SoundLib.Option);
                    InteractInc();
                }
                break;
            }
            case SDLK_LEFT:
            {
                if (SelInteraction >= 0 && SelInteraction < 8)
                {
                    AudioPlayback.PlaySound(SoundLib.Option);
                    InteractDec();
                }
                break;
            }
            }
        }

        /**
        * Actually this one isn't pretty - but it does the trick of
        * turning the background music on / off in "real time"
        * bgm = background music
        * TODO: -Fetching the SelectInteraction via something more descriptive
        * -Obtaining the current value of a select is imho ugly
        */
        if (SelInteraction == 1)
        {
            if (TBackgroundMusicOption(SelectsS[1].SelectedOption) == bmoOn)
                SoundLib.StartBgMusic();
            else
                SoundLib.PauseBgMusic();
        }
        return true;
    }

    constructor TScreenOptionsSound::Create;
    {
        inherited Create;

        LoadFromTheme(Theme.OptionsSound);

        Theme.OptionsSound.SelectSlideVoicePassthrough.showArrows = true;
        Theme.OptionsSound.SelectSlideVoicePassthrough.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsSound.SelectSlideVoicePassthrough, Ini.VoicePassthrough, IVoicePassthroughTranslated);

        Theme.OptionsSound.SelectBackgroundMusic.showArrows = true;
        Theme.OptionsSound.SelectBackgroundMusic.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsSound.SelectBackgroundMusic, Ini.BackgroundMusicOption, IBackgroundMusicTranslated);

        Theme.OptionsSound.SelectClickAssist.showArrows = true;
        Theme.OptionsSound.SelectClickAssist.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsSound.SelectClickAssist, Ini.ClickAssist, IClickAssistTranslated);

        Theme.OptionsSound.SelectBeatClick.showArrows = true;
        Theme.OptionsSound.SelectBeatClick.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsSound.SelectBeatClick, Ini.BeatClick, IBeatClickTranslated);

        Theme.OptionsSound.SelectSlideMusicAutoGain.showArrows = true;
        Theme.OptionsSound.SelectSlideMusicAutoGain.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsSound.SelectSlideMusicAutoGain, Ini.MusicAutoGain, IMusicAutoGainTranslated);

        Theme.OptionsSound.SelectSlidePreviewVolume.showArrows = true;
        Theme.OptionsSound.SelectSlidePreviewVolume.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsSound.SelectSlidePreviewVolume, Ini.PreviewVolume, IPreviewVolumeTranslated);

        Theme.OptionsSound.SelectSlidePreviewFading.showArrows = true;
        Theme.OptionsSound.SelectSlidePreviewFading.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsSound.SelectSlidePreviewFading, Ini.PreviewFading, IPreviewFadingTranslated);

        AddButton(Theme.OptionsSound.ButtonExit);
        if (Length(Button[0].Text) = 0)
            AddButtonText(20, 5, Theme.Options.Description[OPTIONS_DESC_INDEX_BACK]);

        Interaction = 0;
    }

    void TScreenOptionsSound::OnShow()
    {
        //inherited;
        Interaction = 0;

        if (!Help.SetHelpID(ID))
            ULog::Log.LogWarn("No Entry for Help-ID " + ID, "ScreenOptionsSound");
    }
}