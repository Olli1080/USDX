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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/branches/experimental/Lua/src/screens/UScreenPartyOptions.pas $
 * $Id: UScreenPartyOptions.pas 2036 2009-12-14 20:59:44Z whiteshark0 $
 */

#include <string>
#include <array>

#include <SDL2/SDL_keycode.h>

#include "../menu/UMenu.h"

namespace UScreenPartyTournamentOptions
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
    class TScreenPartyTournamentOptions : public UMenu::TMenu
    {
    private:

        uint32_t SelectRoundsFinal;
        uint32_t SelectRounds2Final;
        uint32_t SelectRounds4Final;
        uint32_t SelectRounds8Final;

        int RoundFinalCount;
        int Round2FinalCount;
        int Round4FinalCount;
        int Round8FinalCount;

    public:

        TScreenPartyTournamentOptions();
        void UpdateTournament();
        bool ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown) override;
        void OnShow() override;
        void SetAnimationProgress(double Progress) override;
    };

    const std::string ID = "ID_037";   //for help system
    const std::array<std::string, 3> IRoundCount = { "1", "3", "5" };
    /*
    implementation

        uses
        UGraphic,
        UHelp,
        UIni,
        ULanguage,
        ULog,
        UMain,
        UNote,
        UPartyTournament,
        UPlaylist,
        USong,
        USongs,
        UTexture,
        UUnicodeUtils;
        */
    void TScreenPartyTournamentOptions::UpdateTournament()
    {
        if (SelectsS[SelectRounds8Final].Visible)
            PartyTournament.Rounds[0] = StrToInt(IRoundCount[Round8FinalCount]);

        if (SelectsS[SelectRounds4Final].Visible)
            PartyTournament.Rounds[1] = StrToInt(IRoundCount[Round4FinalCount]);

        if (SelectsS[SelectRounds2Final].Visible)
            PartyTournament.Rounds[2] = StrToInt(IRoundCount[Round2FinalCount]);

        PartyTournament.Rounds[3] = StrToInt(IRoundCount[RoundFinalCount]);
    }

    bool TScreenPartyTournamentOptions::ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown)
        //var
          //  I, J : int;
    {
        Result = true;
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
                AudioPlayback.PlaySound(SoundLib.Back);
                FadeTo(@ScreenPartyTournamentPlayer);
                break;
            }
            case SDLK_TAB:
            {
                ScreenPopupHelp.ShowPopup();
                break;
            }
            case SDLK_RETURN:
            {
                UpdateTournament;
                FadeTo(@ScreenPartyTournamentRounds, SoundLib.Start);
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
            {
                AudioPlayback.PlaySound(SoundLib.Option);
                InteractInc();
                break;
            }
            case SDLK_LEFT:
            {
                AudioPlayback.PlaySound(SoundLib.Option);
                InteractDec();
            }
            }
        }
    }

    TScreenPartyTournamentOptions::TScreenPartyTournamentOptions()
        //var
          //  I : int;
    {
        inherited Create;

        //Load Screen From Theme
        LoadFromTheme(Theme.PartyTournamentOptions);

        Theme.PartyTournamentOptions.SelectRoundsFinal.oneItemOnly = true;
        Theme.PartyTournamentOptions.SelectRoundsFinal.showArrows = true;
        SelectRoundsFinal = AddSelectSlide(Theme.PartyTournamentOptions.SelectRoundsFinal, RoundFinalCount, IRoundCount);

        Theme.PartyTournamentOptions.SelectRounds2Final.oneItemOnly = true;
        Theme.PartyTournamentOptions.SelectRounds2Final.showArrows = true;
        SelectRounds2Final = AddSelectSlide(Theme.PartyTournamentOptions.SelectRounds2Final, Round2FinalCount, IRoundCount);

        Theme.PartyTournamentOptions.SelectRounds4Final.oneItemOnly = true;
        Theme.PartyTournamentOptions.SelectRounds4Final.showArrows = true;
        SelectRounds4Final = AddSelectSlide(Theme.PartyTournamentOptions.SelectRounds4Final, Round4FinalCount, IRoundCount);

        Theme.PartyTournamentOptions.SelectRounds8Final.oneItemOnly = true;
        Theme.PartyTournamentOptions.SelectRounds8Final.showArrows = true;
        SelectRounds8Final = AddSelectSlide(Theme.PartyTournamentOptions.SelectRounds8Final, Round8FinalCount, IRoundCount);

        Interaction = 0;
    }

    void TScreenPartyTournamentOptions::OnShow()
    {
        inherited;

        if (!Help.SetHelpID(ID))
            ULog::Log.LogError("No Entry for Help-ID " + ID + " (ScreenPartyTournamentOptions)");

        SelectsS[SelectRounds2Final].Visible = false;
        SelectsS[SelectRounds4Final].Visible = false;
        SelectsS[SelectRounds8Final].Visible = false;

        if (PartyTournament.PlayersCount > 2)
            SelectsS[SelectRounds2Final].Visible = true;

        if (PartyTournament.PlayersCount > 4)
            SelectsS[SelectRounds4Final].Visible = true;

        if (PartyTournament.PlayersCount > 8)
            SelectsS[SelectRounds8Final].Visible = true;

    }

    void TScreenPartyTournamentOptions::SetAnimationProgress(double Progress)
    {
        /*{ for I  =  0 to 6 do
          SelectS[I].Texture.ScaleW  =  Progress; }*/
    }
}