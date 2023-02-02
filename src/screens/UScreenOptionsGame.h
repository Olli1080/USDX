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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/screens/UScreenOptionsGame.pas $
 * $Id: UScreenOptionsGame.pas 2203 2010-03-16 19:25:13Z brunzelchen $
 */
#include <string>
#include <functional>

#include <SDL2/SDL_keycode.h>

#include "../menu/UMenu.h"

namespace UScreenOptionsGame
{
    /*uses
      UDisplay,
      UFiles,
      UIni,
      UMenu,
      UMusic,
      UScreensong,
      USongs,
      UThemes,
      sdl2;*/

    class TScreenOptionsGame : public UMenu::TMenu
    {
    private:
        int old_Language;
        int old_SongMenu;
        int old_Sorting;
        int old_Tabs;
        int AVDelayOptInt;
        int MicDelayOptInt;
        int AVDelaySelectNum;
        int MicDelaySelectNum;

        void Leave();
        void ReloadCurrentScreen();
        void ReloadAllScreens();
        void ReloadSongMenu();
        void UpdateCalculatedSelectSlides(bool Init);

    public:

        TScreenOptionsGame();
        ~TScreenOptionsGame() override = default;
        bool ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown) override;
        void OnShow() override;
    };

    const std::string ID = "ID_071";   //for help system

    /*implementation

        uses
        UConfig,
        UGraphic,
        UHelp,
        ULanguage,
        ULog,
        UUnicodeUtils,
        SysUtils;*/

    typedef std::function<bool(int& Param, int Offset, bool Modify, const std::optional<std::string&> OptText)> TGetTextFunc;

    bool TScreenOptionsGame::ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown)
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
            Leave();
            break;
        case SDLK_TAB:
        {
            ScreenPopupHelp.ShowPopup();
            break;
        }
        case SDLK_RETURN:
        {
            if (SelInteraction == 8)
                Leave();
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
            if (SelInteraction >= 0 && SelInteraction <= 7)
            {
                AudioPlayback.PlaySound(SoundLib.Option);
                InteractInc();
            }
            UpdateCalculatedSelectSlides(false);
            if (SelInteraction == 0)
            {
                ReloadCurrentScreen();
            }
            break;
        }
        case SDLK_LEFT:
        {
            if (SelInteraction >= 0 && SelInteraction <= 7)
            {
                AudioPlayback.PlaySound(SoundLib.Option);
                InteractDec();
            }
            UpdateCalculatedSelectSlides(false);
            if (SelInteraction == 0)
            {
                ReloadCurrentScreen();
            }
            break;
        }
        }
        return true;
    }

    void CalculateSelectSlide(bool Init, TGetTextFunc GetText, int& Param, int& OptInt, std::vector<std::string>& Texts)
    {
        if (!GetText(Param, 0, true, std::nullopt))
        {
            Texts.clear();
            return;
        }
        int Idx = GetText(Param, -1, false, std::nullopt) ? 1 : 0;
        if (!Init)
        {
            if (OptInt == Idx)
                return;
            GetText(Param, OptInt - Idx, true, std::nullopt);
            Idx = GetText(Param, -1, false, std::nullopt) ? 1 : 0;
        }
        OptInt = Idx;
        const int NumOpts = Idx + (GetText(Param, 1, false, std::nullopt) ? 2 : 1);

        Texts.resize(NumOpts);
        for (int Idx = 0; Idx < Texts.size(); ++Idx)
            GetText(Param, Idx - OptInt, false, Texts[Idx]);
    }

    bool GetAVDelayOptText(int& Param, int Offset, bool Modify, std::optional<std::string&> OptText)
    {
        if (OptText.has_value())
            OptText^ = Format('%d ms', [Param + Offset * 10]);
        if (Modify)
            Param = Param + Offset * 10;
        return true;
    }

    bool GetMicDelayOptText(int& Param, int Offset, bool Modify, std::optional<std::string&> OptText)
    {
        if (Param + Offset * 10 < 0)
            return false;
        else
        {
            if (OptText.has_value())
                OptText^ = Format('%d ms', [Param + Offset * 10]);
            if (Modify)
                Param = Param + Offset * 10;
            return true;
        }
    }

    void TScreenOptionsGame::UpdateCalculatedSelectSlides(bool Init)
    {
        CalculateSelectSlide(Init, @GetAVDelayOptText, Ini.AVDelay, AVDelayOptInt, IAVDelay);
        CalculateSelectSlide(Init, @GetMicDelayOptText, Ini.MicDelay, MicDelayOptInt, IMicDelay);
        if (Init)
        {
            AVDelaySelectNum = AddSelectSlide(Theme.OptionsGame.SelectAVDelay, AVDelayOptInt, IAVDelay);
            MicDelaySelectNum = AddSelectSlide(Theme.OptionsGame.SelectMicDelay, MicDelayOptInt, IMicDelay);
        }
        else
        {
            UpdateSelectSlideOptions(Theme.OptionsGame.SelectAVDelay, AVDelaySelectNum, IAVDelay, AVDelayOptInt);
            UpdateSelectSlideOptions(Theme.OptionsGame.SelectMicDelay, MicDelaySelectNum, IMicDelay, MicDelayOptInt);
        }
    }

    TScreenOptionsGame::TScreenOptionsGame()
    {
        inherited Create;

        LoadFromTheme(Theme.OptionsGame);

        Theme.OptionsGame.SelectLanguage.showArrows = true;
        Theme.OptionsGame.SelectLanguage.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGame.SelectLanguage, Ini.Language, ILanguageTranslated);

        Theme.OptionsGame.SelectSongMenu.showArrows = true;
        Theme.OptionsGame.SelectSongMenu.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGame.SelectSongMenu, Ini.SongMenu, ISongMenuTranslated);

        Theme.OptionsGame.SelectTabs.showArrows = true;
        Theme.OptionsGame.SelectTabs.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGame.SelectTabs, Ini.Tabs, ITabsTranslated);

        Theme.OptionsGame.SelectSorting.showArrows = true;
        Theme.OptionsGame.SelectSorting.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGame.SelectSorting, Ini.Sorting, ISortingTranslated);

        Theme.OptionsGame.SelectShowScores.showArrows = true;
        Theme.OptionsGame.SelectShowScores.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGame.SelectShowScores, Ini.ShowScores, IShowScoresTranslated);

        Theme.OptionsGame.SelectDebug.showArrows = true;
        Theme.OptionsGame.SelectDebug.oneItemOnly = true;
        AddSelectSlide(Theme.OptionsGame.SelectDebug, Ini.Debug, IDebugTranslated);

        Theme.OptionsGame.SelectAVDelay.showArrows = true;
        Theme.OptionsGame.SelectAVDelay.oneItemOnly = true;
        Theme.OptionsGame.SelectMicDelay.showArrows = true;
        Theme.OptionsGame.SelectMicDelay.oneItemOnly = true;
        UpdateCalculatedSelectSlides(true);

        AddButton(Theme.OptionsGame.ButtonExit);
        if (Button[0].Text.empty())
            AddButtonText(20, 5, Theme.Options.Description[OPTIONS_DESC_INDEX_BACK]);
    }

    void TScreenOptionsGame::OnShow()
    {
        inherited;

        // save current settings in order to determine if a refresh is required
        old_Language = Ini.Language;
        old_SongMenu = Ini.SongMenu;
        old_Sorting = Ini.Sorting;
        old_Tabs = Ini.Tabs;

        Interaction = 0;

        if (!Help.SetHelpID(ID))
            ULog::Log.LogWarn('No Entry for Help-ID ' + ID, 'ScreenOptionsGame');
    }

    void TScreenOptionsGame::Leave()
    {
        Ini.Save;
        ReloadAllScreens;
        ReloadSongMenu;

        AudioPlayback.PlaySound(SoundLib.Back);
        FadeTo(@ScreenOptions);
    }

    void TScreenOptionsGame::ReloadCurrentScreen()
    {
        ScreenOptionsGame.Free;
        Language.ChangeLanguage(ILanguage[Ini.Language]);
        Ini.TranslateOptionValues;
        Theme.LoadTheme(Ini.Theme, Ini.Color);
        ScreenOptionsGame = TScreenOptionsGame::Create;
    }

    void TScreenOptionsGame::ReloadAllScreens()
    {
        // Reload all screens after language changed
        if (old_Language != Ini.Language)
        {
            UGraphic.UnloadScreens;

            Theme.LoadTheme(Ini.Theme, Ini.Color);
            UGraphic.LoadScreens(USDXVersionStr);

            old_Language = Ini.Language;
            old_SongMenu = Ini.SongMenu;
            old_Sorting = Ini.Sorting;
            old_Tabs = Ini.Tabs;
        }
    }

    void TScreenOptionsGame::ReloadSongMenu()
    {
        if (Ini.Sorting != old_Sorting || Ini.Tabs != old_Tabs || old_SongMenu != Ini.SongMenu)
        {
            Theme.ThemeSongLoad;

            ScreenSong.Free;
            ScreenSong = TScreenSong.Create;
        }
    }
}