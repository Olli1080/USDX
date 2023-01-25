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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/screens/UScreenPartyWin.pas $
 * $Id: UScreenPartyWin.pas 2246 2010-04-18 13:43:36Z tobigun $
 */
#include <string>

#include "../menu/UMenu.h"

namespace UScreenPartyTournamentWin
{
    /*
uses
  UCommon,
  UDisplay,
  UIni,
  UMenu,
  UMusic,
  UThemes,
  sdl2,
  SysUtils;
  */
  //type
    class TScreenPartyTournamentWin : public UMenu::TMenu
    {
    public:

        uint32_t TextScorePlayer1;
        uint32_t TextScorePlayer2;

        uint32_t TextNamePlayer1;
        uint32_t TextNamePlayer2;

        uint32_t StaticBGPlayer1;
        uint32_t StaticBGPlayer2;

        TScreenPartyTournamentWin();
        bool ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown) override;
        void OnShow() override;
        void UpdateRounds();
        void SetAnimationProgress(double Progress) override;
    };

    const std::string ID = "ID_039";   //for help system

    /*
    implementation

        uses
        UGraphic,
        UHelp,
        ULanguage,
        ULog,
        UMain,
        UNote,
        UPartyTournament,
        UScreenPartyTournamentRounds,
        UUnicodeUtils;
        */
    bool TScreenPartyTournamentWin::ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown)
    {
        Result = true;
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
        case SDLK_BACKSPACE: [[fallthrough]]
        case SDLK_RETURN:
        {
            AudioPlayback.PlaySound(SoundLib.Start);

            if ((StrToInt(Text[TextScorePlayer1].Text) > PartyTournament.Rounds[PartyTournament.Phase] / 2 || StrToInt(Text[TextScorePlayer2].Text) > PartyTournament.Rounds[PartyTournament.Phase] / 2))
            {
                if (PartyTournament.Phase != 3)
                {
                    SetLength(PartyTournament.EliminatedPlayers, Length(PartyTournament.EliminatedPlayers) + 1);

                    if (StrToInt(Text[TextScorePlayer1].Text) > StrToInt(Text[TextScorePlayer2].Text))
                        PartyTournament.EliminatedPlayers[High(PartyTournament.EliminatedPlayers)] = PartyTournament.Next.Player2;
                    else
                        PartyTournament.EliminatedPlayers[High(PartyTournament.EliminatedPlayers)] = PartyTournament.Next.Player1;
                }
                else
                {
                    if (StrToInt(Text[TextScorePlayer1].Text) > StrToInt(Text[TextScorePlayer2].Text))
                        PartyTournament.Winner = PartyTournament.Next.NamePlayer1;
                    else
                        PartyTournament.Winner = PartyTournament.Next.NamePlayer2;

                    PartyTournament.TournamentFinish = true;

                }
            }
            PartyTournament.LastPlayer = PartyTournament.Next.Player2;

            if (PartyTournament.Phase == 3)
            {
                if (!PartyTournament.TournamentFinish)
                    FadeTo(@ScreenSong);
                else
                    FadeTo(@ScreenPartyTournamentRounds);
            }
            else
                FadeTo(@ScreenPartyTournamentRounds);
            break;
        }
        case SDLK_TAB:
        {
            ScreenPopupHelp.ShowPopup();
            break;
        }
        }
    }

    TScreenPartyTournamentWin::TScreenPartyTournamentWin()
    {
        inherited Create;

        LoadFromTheme(Theme.PartyTournamentWin);

    TextScorePlayer1 = AddText(Theme.PartyTournamentWin.TextScorePlayer1);
    TextScorePlayer2 = AddText(Theme.PartyTournamentWin.TextScorePlayer2);

    TextNamePlayer1 = AddText(Theme.PartyTournamentWin.TextNamePlayer1);
    TextNamePlayer2 = AddText(Theme.PartyTournamentWin.TextNamePlayer2);

    StaticBGPlayer1 = AddStatic(Theme.PartyTournamentWin.StaticBGPlayer1);
    StaticBGPlayer2 = AddStatic(Theme.PartyTournamentWin.StaticBGPlayer2);
    }

    void TScreenPartyTournamentWin::OnShow()
        //var
          //  Col : TRGB;
    {
        inherited;

        if (!Help.SetHelpID(ID))
            ULog::Log.LogError("No Entry for Help-ID " + ID + " (ScreenPartyTournamentWin)");

		Col = GetPlayerColor(Ini.SingColor[0]);

        Statics[StaticBGPlayer1].Texture.ColR  = Col.R;
        Statics[StaticBGPlayer1].Texture.ColG  = Col.G;
        Statics[StaticBGPlayer1].Texture.ColB  = Col.B;

		Col = GetPlayerColor(Ini.SingColor[1]);

        Statics[StaticBGPlayer2].Texture.ColR  = Col.R;
        Statics[StaticBGPlayer2].Texture.ColG  = Col.G;
        Statics[StaticBGPlayer2].Texture.ColB  = Col.B;

        Text[TextNamePlayer1].Text  = PartyTournament.Next.NamePlayer1;
        Text[TextNamePlayer2].Text  = PartyTournament.Next.NamePlayer2;

        UpdateRounds();

        Text[TextScorePlayer1].Text  = PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player1].Text;
        Text[TextScorePlayer2].Text  = PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player2].Text;

    }

    void TScreenPartyTournamentWin::UpdateRounds()
    //var
      //  p : integer;
    {
        if (PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player1].Text = "")
            PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player1].Text  = "0";

        if (PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player2].Text = "")
            PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player2].Text  = "0";

        if (ScreenSing.SungToEnd)
        {
			if (player[0].ScoreTotalInt > player[1].ScoreTotalInt)
				PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player1].Text = IntToStr(StrToInt(PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player1].Text) + 1);

			if (player[0].ScoreTotalInt < player[1].ScoreTotalInt)
				PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player2].Text = IntToStr(StrToInt(PartyTournament.ResultPlayer[PartyTournament.Phase, PartyTournament.Next.Player2].Text) + 1);
        }

    }

    void TScreenPartyTournamentWin::SetAnimationProgress(double Progress)
    {
        /*if (ScreenSingModi.PlayerInfo.NumPlayers >= 1) then
          Statics[StaticTeam1].Texture.ScaleW := Progress * ScreenSingModi.PlayerInfo.Playerinfo[0].Score / maxScore;
        if (ScreenSingModi.PlayerInfo.NumPlayers >= 2) then
          Statics[StaticTeam2].Texture.ScaleW := Progress * ScreenSingModi.PlayerInfo.Playerinfo[1].Score / maxScore;
        if (ScreenSingModi.PlayerInfo.NumPlayers >= 3) then
          Statics[StaticTeam3].Texture.ScaleW := Progress * ScreenSingModi.PlayerInfo.Playerinfo[2].Score / maxScore;*/
    }
}