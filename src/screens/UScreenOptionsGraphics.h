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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/screens/UScreenOptionsGraphics.pas $
 * $Id: UScreenOptionsGraphics.pas 2338 2010-05-03 21:58:30Z k-m_schindler $
 */
#include <string>
#include <vector>
#include <SDL2/SDL_keycode.h>

#include "../menu/UMenu.h"

namespace UScreenOptionsGraphics
{
    /*uses
      UDisplay,
      UFiles,
      UIni,
      UMenu,
      UMusic,
      UThemes,
      sdl2;*/

    class TScreenOptionsGraphics : public UMenu::TMenu
    {
    private:
        uint32_t SelectWindowMode;
        uint32_t SelectResolution;

        std::vector<std::string> IResolutionEmpty;
        int ResolutionEmpty; // not used, only to prevent changing original by-ref passed variable

        int OldWindowMode;

        void UpdateWindowMode();
        void UpdateResolution();

    public:

        TScreenOptionsGraphics();
        ~TScreenOptionsGraphics() override = default;

        bool ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown) override;
        void OnShow() override;
        void OnHide() override;
        void OnWindowResized() override;
    };

    const std::string ID = "ID_072";   //for help system
    /*
    implementation

        uses
        UGraphic,
        UHelp,
        ULog,
        UUnicodeUtils,
        SysUtils;*/

    bool TScreenOptionsGraphics::ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown)
    {
        if (!PressedDown)
            return true;
        // Key Down
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
            if (SelInteraction == 6)
            {
                UIni::Ini.Save();
                AudioPlayback.PlaySound(SoundLib.Back);

                if (OldWindowMode != Ini.FullScreen)
                    UGraphic.UpdateVideoMode();
                else
                    UGraphic.UpdateResolution();

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
            if (SelInteraction >= 0 && SelInteraction < 6)
            {
                AudioPlayback.PlaySound(SoundLib.Option);
                InteractInc();
            }

            if (Interaction == SelectWindowMode)
                UpdateResolution();

            break;

        }
        case SDLK_LEFT:
        {
            if (SelInteraction >= 0 && SelInteraction < 6)
            {
                AudioPlayback.PlaySound(SoundLib.Option);
                InteractDec();
            }

            if (Interaction == SelectWindowMode)
                UpdateResolution();

            break;
        }
        }
        return true;
    }

    TScreenOptionsGraphics::TScreenOptionsGraphics()
    {
        inherited Create;
        LoadFromTheme(Theme.OptionsGraphics);

        ResolutionEmpty = 0;
        SetLength(IResolutionEmpty, 1);
        IResolutionEmpty[0] = '---';

        Theme.OptionsGraphics.SelectFullscreen.showArrows = true;
        Theme.OptionsGraphics.SelectFullscreen.oneItemOnly = true;
        SelectWindowMode = AddSelectSlide(Theme.OptionsGraphics.SelectFullscreen, Ini.Fullscreen, IFullScreenTranslated);

        Theme.OptionsGraphics.SelectResolution.showArrows = true;
        Theme.OptionsGraphics.SelectResolution.oneItemOnly = true;
        SelectResolution = AddSelectSlide(Theme.OptionsGraphics.SelectResolution, Ini.Resolution, IResolution);

        Theme.OptionsGraphics.SelectDepth.showArrows = true;
        Theme.OptionsGraphics.SelectDepth.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGraphics.SelectDepth, Ini.Depth, IDepth);

        Theme.OptionsGraphics.SelectVisualizer.showArrows = true;
        Theme.OptionsGraphics.SelectVisualizer.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGraphics.SelectVisualizer, Ini.VisualizerOption, IVisualizerTranslated);

        Theme.OptionsGraphics.SelectOscilloscope.showArrows = true;
        Theme.OptionsGraphics.SelectOscilloscope.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGraphics.SelectOscilloscope, Ini.Oscilloscope, IOscilloscopeTranslated);

        Theme.OptionsGraphics.SelectMovieSize.showArrows = true;
        Theme.OptionsGraphics.SelectMovieSize.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGraphics.SelectMovieSize, Ini.MovieSize, IMovieSizeTranslated);

        // TODO: Add apply button
        AddButton(Theme.OptionsGraphics.ButtonExit);
        if (Length(Button[0].Text) == 0)
            AddButtonText(20, 5, Theme.Options.Description[OPTIONS_DESC_INDEX_BACK]);

    }

    void TScreenOptionsGraphics::OnShow()
        //var
          //  i : int;
    {
        inherited;

        if (CurrentWindowMode == Mode_Windowed)
            Ini.SetResolution(ScreenW, ScreenH);

        UpdateWindowMode();
        UpdateResolution();

        Interaction = 0;

        if (!Help.SetHelpID(ID))
            ULog::Log.LogWarn('No Entry for Help-ID ' + ID, 'ScreenOptionsGraphics');
    }

    void TScreenOptionsGraphics::OnHide()
    {
        inherited;
        Ini.ClearCustomResolutions();
    }

    void TScreenOptionsGraphics::OnWindowResized()
    {
        inherited;

        UpdateWindowMode();

        if (CurrentWindowMode == Mode_Windowed)
            Ini.SetResolution(ScreenW, ScreenH);
        UpdateResolution();
    }

    void TScreenOptionsGraphics::UpdateWindowMode()
    {
        UpdateSelectSlideOptions(Theme.OptionsGraphics.SelectFullscreen, SelectWindowMode, IFullScreenTranslated, Ini.FullScreen);
        OldWindowMode = int(Ini.FullScreen);
    }

    void TScreenOptionsGraphics::UpdateResolution()
    {

        if (Ini.Fullscreen == 2)
            UpdateSelectSlideOptions(Theme.OptionsGraphics.SelectResolution, SelectResolution, IResolutionEmpty, ResolutionEmpty);
        else if (Ini.Fullscreen == 1)
            UpdateSelectSlideOptions(Theme.OptionsGraphics.SelectResolution, SelectResolution, IResolutionFullScreen, Ini.ResolutionFullscreen);
        else
            UpdateSelectSlideOptions(Theme.OptionsGraphics.SelectResolution, SelectResolution, IResolution, Ini.Resolution);

    }
}