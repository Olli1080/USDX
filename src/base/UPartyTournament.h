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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UParty.pas $
 * $Id: UParty.pas 2581 2010-07-18 10:33:05Z whiteshark0 $
 */

#include <string>
#include <vector>
#include <array>

namespace UPartyTournament
{
    struct NextPlayers
    {
        int Player1;
        int Player2;
        std::string NamePlayer1;
        std::string NamePlayer2;
    };

    struct PosResult
    {
        double X;
        double Y;
        std::string Text;
    };

    class TPartyTournament
    {

    private:


    public:

        int PlayersCount;
        std::array<int, 4> Rounds;
        std::string Winner;

        int LastPlayer;
        int Phase;
        std::vector<int> EliminatedPlayers;
        NextPlayers Next;
        std::array<std::array<PosResult, 16>, 4> ResultPlayer; //TODO:: replace with round and player values from Ini

        bool TournamentFinish;

        TPartyTournament();
        void Clear();
    };

    inline TPartyTournament PartyTournament;
    /*
  implementation

  uses
    UGraphic,
    ULanguage,
    ULog,
    ULuaCore,
    UDisplay,
    USong,
    UNote,
    SysUtils;
    */

    //-------------
    // Just the constructor
    //-------------
    TPartyTournament::TPartyTournament()
    {
        Clear();
    }

    // clears all party specific data previously stored
    void TPartyTournament::Clear()
    {

        Next.Player1 = -1;
        Next.Player2 = -1;

        LastPlayer = -1;
        EliminatedPlayers.clear();
        Phase = 0;
        TournamentFinish = false;
        Winner = "";

        for (auto& round : ResultPlayer)
        {
            for (auto& player : round)
            {
                player.Text = "";
            }
        }
    }
}