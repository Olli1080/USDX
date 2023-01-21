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
#include "../switches.h"

#include <string>
#include <vector>
#include <optional>
#include <random>
#include <numeric>
#include <cstdint>

#include <boost/algorithm/string/case_conv.hpp>

namespace UParty
{
/*uses
  ULua;*/

//type
  /* array holds ids of modes or Party_Round_Random
    its length defines the number of rounds
    it is used as argument for TPartyGame::StartParty */
  typedef std::vector<int> ARounds;

  /* element of APartyTeamRanking returned by TPartyGame::GetTeamRanking
    and parameter for TPartyGame::SetWinner */
  struct TParty_TeamRanking
  {
    int Team; //< id of team
    int Rank; //< 1 to Length(Teams) e.g. 1 is for placed first
  };
  typedef std::vector<TParty_TeamRanking> AParty_TeamRanking; //< returned by TPartyGame::GetTeamRanking

  struct TParty_RoundList
  {
    int Index;
    std::string Name;
  };
  typedef std::vector<TParty_RoundList> AParty_ModeList;

  /* record used by TPartyGame to store round specific data */
  struct TParty_Round
  {
    TParty_Round(int mode)
      : Mode(mode)
      {}

    int Mode;
    bool AlreadyPlayed = false; //< true if round was already played
    AParty_TeamRanking Ranking;
    bool RankingSet = false; //< true if Self.Ranking is already set
  };

  struct TParty_ModeInfo
  {
    std::string Name; // name of this mode
    int Parent;   // Id of owning plugin

    bool CanNonParty; //< is playable when not in party mode
    bool CanParty;    //< is playable in party mode

    // one bit in the following settings stands for
    // a player or team count
    // PlayerCount = 2 or 4 indicates that the mode is playable with 2 and 3 players per team
    // TeamCount = 1 or 2 or 4 or 8 or 16 or 32 indicates that the mode is playable with 1 to 6 teams
    int PlayerCount;   //< playable with one, two, three etc. players per team
    int TeamCount;     //< playable with one, two, three etc. different teams


    struct 
    { // lua functions that will be called at specific events
      std::string BeforeSongSelect; // default actions are executed if functions = nil
      std::string AfterSongSelect;

      std::string BeforeSing;
      std::string OnSing;
      std::string AfterSing;
    } Functions;
  };

  /* used by TPartyGame to store player specific data */
  struct TParty_PlayerInfo
  {
    TParty_PlayerInfo(const std::string& name)
      : Name(name)
      {}

    std::string Name;         //< Playername
    int TimesPlayed = 0; //< How often this Player has Sung
  };

  /* used by TPartyGame to store team specific data */
  struct TParty_TeamInfo
  {
    TParty_TeamInfo(const std::string& name)
      : Name(name)
      {}

    std::string Name;        //< name of the Team
    uint16_t Score = 0;          //< current score
    int JokersLeft = Party_Count_Jokers;  //< amount of jokers this team has left

    int NextPlayer = -1;  //Id of the player that plays the next (the current) song

    std::vector<TParty_PlayerInfo> Players;
  };

  class TPartyGame
  {
  private:

    static inline std::random_device rd;

    int CurRound;   //< indicates which of the elements of Rounds is played next (at the moment)

    bool bPartyStarted;

    std::vector<int> TimesPlayed; //< times every mode was played in current party game (for random mode calculation)

    void GenScores();
    int GetRandomMode();
    int GetRandomPlayer(int Team);

    /* returns true if a mode is playable with current playerconfig */
    bool ModePlayable(int I);

    bool CallLua(int Parent, std::string Func);

    void SetRankingByScore();

  public:

    bool bPartyGame; //< are we playing party or standard mode

    //Teams: TTeamInfo;
    std::vector<TParty_Round> Rounds;    //< holds info which modes are played in this party game (if started)
    std::vector<TParty_TeamInfo> Teams;  //< holds info of teams playing in current round (private for easy manipulation of lua functions)

    std::vector<TParty_ModeInfo> Modes;  //< holds info of registred party modes //TODO:: same size as timesplayed

    std::vector<int> SungPartySongs;

    //property CurrentRound: int read CurRound;

    TPartyGame();
    ~TPartyGame() = default;

    /* set the attributes of Info to default values */
    void DefaultModeInfo(TParty_ModeInfo& Info);

    /* registers a new mode, returns true on success
      (mode name does not already exist) */
    bool RegisterMode(TParty_ModeInfo Info);

    /* returns true if modes are available for
      players and teams that are currently set
      up. if there are no teams set up it returns
      if there are any party modes available */
    bool ModesAvailable();

    /* returns an array with the name of all available modes (that
      are playable with current player configuration */
    AParty_ModeList GetAvailableModes();

    /* clears all party specific data previously stored */
    void Clear();

    /* adds a team to the team array, returning its id
      can only be called when game is not already started */
    std::optional<int> AddTeam(std::string Name);

    /* adds a player to the player array, returning its id
      can only be called when game is not already started */
    std::optional<int> AddPlayer(int Team, std::string Name);

    /* starts a new PartyGame, returns true on success
      before a call of this function teams and players
      has to be added by AddTeam and AddPlayer */

    bool StartGame(ARounds Rounds);

    /* sets the winner(s) of current round
      returns true on success */
    bool SetRanking(AParty_TeamRanking Ranking);

    /* increases players TimesPlayed value */
    void IncTimesPlayed();

    /* increases round counter by 1 and clears all round specific information;
      returns the number of the current round or -1 if last round has already
      been played */
    int NextRound();

    /* indicates that current round has already been played */
    void RoundPlayed();

    /* true if in a Party Game (not in standard mode) */
    //property PartyGame: bool read BPartyGame;


    /* returns true if last round was already played */
    bool GameFinished();

    /* call plugins defined function and/or default void
      only default void is called when no function is defined by plugin
      if plugins function returns true then default is called after plugins
      function was executed*/
    void CallBeforeSongSelect();
    void CallAfterSongSelect();
    void CallBeforeSing();
    void CallOnSing();
    void CallAfterSing();

    /* returns an array[1..6] of TParty_TeamRanking.
      the index stands for the placing,
      team is the team number (in the team array)
      rank is correct rank if some teams have the
      same score.
      */
    AParty_TeamRanking GetTeamRanking();

    /* returns a string like "Team 1 (and Team 2) win" */
    std::string GetWinnerString(int Round);

    void SaveSungPartySong(int ID);
    bool SongNotSungAndNotDuet(int ID, int N_DuetSongs);
  };


  /* minimal amount of teams for party mode */
const int Party_Teams_Min = 2;

  /* maximal amount of teams for party mode */
  const int Party_Teams_Max = 3;

  /* minimal amount of players for party mode */
  const int Party_Players_Min = 1;

  /* maximal amount of players for party mode */
  const int Party_Players_Max = 4;

  /* amount of jokers each team gets at the beginning of the game */
  const int Party_Count_Jokers = 5;

  /* to indicate that element (mode) should set randomly in ARounds array */
  const int Party_Round_Random = -1;

  /* values for TParty_TeamRanking.Rank */
  const int PR_First = 1;
  const int PR_Second = 2;
  const int PR_Third = 3;

  const int StandardModus = 0; //Modus Id that will be played in non-party mode

  inline TPartyGame Party;

/*implementation

uses
  UGraphic,
  ULanguage,
  ULog,
  ULuaCore,
  UDisplay,
  USong,
  USongs,
  UNote,
  SysUtils;
*/

//-------------
// Just the constructor
//-------------
TPartyGame::TPartyGame()
{
  Clear();
};

/* clears all party specific data previously stored */
void TPartyGame::Clear()
{
  bPartyGame = false; // no party game
  CurRound = low(int);

  bPartyStarted = false; //game not startet

  Teams.clear(); //remove team info
  Rounds.clear(); //remove round info

  // clear times played
  for I = 0 to High(TimesPlayed) do
    TimesPlayed[I] = 0;

  SungPartySongs.clear();
};

/* private: some intelligent randomnes for plugins */
int TPartyGame::GetRandomMode()
{
  int lowestTP = std::numeric_limits<int>::max();
  std::vector<int> potentials;

  // search for the plugins less played yet
  for (size_t i = 0; i < Modes.size(); ++i)
  {
    if (!ModePlayable(i) || TimesPlayed[i] > lowestTP)
      continue;
    
    if (TimesPlayed[i] < lowestTP)
    {
      lowestTP = TimesPlayed[i];
      potentials.clear();
    }
    potentials.emplace_back(i);
  }
  
  //If there are no matching modes, play first mode
  if (potentials.empty()) //TODO:: should we increase timesplayed for the default?
    return 0;

  // create random number
  int choosen = potentials[std::uniform_int_distribution(size_t(0), potentials.size() - 1)(rd)];
  ++TimesPlayed[choosen];
  return choosen;
}

/* private: GetRandomPlayer - returns a random player
                             that does not play to often ;) */
int TPartyGame::GetRandomPlayer(int Team)
{
  int lowestTP = std::numeric_limits<int>::max();
  std::vector<int> potentials;

  // search for players that have less played yet
  for (size_t i = 0; i < Teams[Team].Players.size(); ++i)
  {
    const auto& timesPlayed = Teams[Team].Players[i].TimesPlayed;
    if (timesPlayed > lowestTP)
      continue;

    if (timesPlayed < lowestTP)
    {
      lowestTP = timesPlayed;
      potentials.clear();
    }
    potentials.emplace_back(i);
  }

  if (potentials.empty())
    return 0;

  int choosen = potentials[std::uniform_int_distribution(size_t(0), potentials.size() - 1)(rd)];
  //++Teams[Team].Players[choosen].TimesPlayed; TODO:: no increment here; why?
  return choosen;
}

//----------
//GenScores - inc scores for cur. round
//----------
void TPartyGame::GenScores()
{
  if (Teams.size() == 2)
  { // score generation for 2 teams, winner gets 1 point
    for (const auto& rank : Rounds[CurRound].Ranking)
    {
      if (rank.Rank == PR_First)
        ++Teams[rank.Team].Score;
    }
  }
  else if (Teams.size() == 3)
  { // score generation for 3 teams,
    // winner gets 3 points 2nd gets 1 point
    for (const auto& rank : Rounds[CurRound].Ranking)
    {
      if (rank.Rank == PR_First)
        Teams[rank.Team].Score += 3;
      else if (rank.Rank == PR_Second)
        ++Teams[rank.Team].Score;
    }
  }
}

/* set the attributes of Info to default values */
void TPartyGame::DefaultModeInfo(TParty_ModeInfo& Info)
{
  Info.Name = "undefined";
  Info.Parent = -1; //< not loaded by plugin (e.g. Duell)
  Info.CanNonParty = false;
  Info.CanParty = false;
  Info.PlayerCount = std::numeric_limits<int>::max(); //< no restrictions either on player count
  Info.TeamCount = std::numeric_limits<int>::max();   //< nor on team count
  Info.Functions.BeforeSongSelect = ""; //< use default functions
  Info.Functions.AfterSongSelect  = "";
  Info.Functions.BeforeSing       = "";
  Info.Functions.OnSing           = "";
  Info.Functions.AfterSing        = "";
}

/* registers a new mode, returns true on success
  (mode name does not already exist) */
bool TPartyGame::RegisterMode(TParty_ModeInfo Info)
{
  if (Info.Name == "undefined")
    return false;

  // search for a plugin w/ same name
  auto LowerName = boost::algorithm::to_lower_copy(Info.Name); // case [in]sensitive search
  if (std::ranges::any_of(Modes, [&LowerName](const auto& mode){
    return LowerName == mode.Name;
  }))
    return false; //< no success (name already exist)

  // add new mode to array and append and clear a new TimesPlayed element
  Modes.emplace_back(Info);
  TimesPlayed.emplace_back(0);

  return true;
}

/* returns true if a mode is playable with current playerconfig */
bool TPartyGame::ModePlayable(int i)
{
  if (Teams.empty())
    return true;
  
  auto& mode = Modes[i];
  if ((mode.TeamCount & (1 << (Teams.size() - 1))) != 0)
  {
    return std::ranges::all_of(Teams, [&mode](const auto& team){
      return mode.PlayerCount & (1 << (team.Players.size() - 1)) != 0;
    });
  }
  else
    return false;
}

/* returns true if modes are available for
  players and teams that are currently set
  up. if there are no teams set up it returns
  if there are any party modes available */
bool TPartyGame::ModesAvailable()
{
  if (Teams.empty())
    return !Modes.empty();

  for (size_t i = 0; i < Modes.size(); ++i)
  {
    if (ModePlayable(i))
      return true;
  }
  return false;
}       

/* returns an array with the name of all available modes (that
  are playable with current player configuration */
AParty_ModeList TPartyGame::GetAvailableModes()
  /*var
    I: int;
    Len: int;*/
{
  AParty_ModeList res;
  res.emplace_back(Party_Round_Random, "MODE_RANDOM_NAME"); //TODO:: Language.Translate('MODE_RANDOM_NAME');

  for (size_t i = 0; i < Modes.size(); ++i)
    if (ModePlayable(i))
      res.emplace_back(i, "MODE_" + boost::algorithm::to_upper_copy(Modes[i].Name) + "_NAME");
      //TODO:: Language.Translate('MODE_' + Uppercase(Modes[I].Name) + '_NAME')
}

/* adds a team to the team array, returning its id
  can only be called when game is not already started */
std::optional<int> TPartyGame::AddTeam(std::string Name)
{
  if (!bPartyStarted && !Name.empty() && Teams.size() < Party_Teams_Max)
  {
    Teams.emplace_back(TParty_TeamInfo(Name));
    return Teams.size() - 1;
  }
  return std::nullopt;
}

/* adds a player to the player array, returning its id
  can only be called when game is not already started */
std::optional<int> TPartyGame::AddPlayer(int Team, std::string Name)
{
  if (!bPartyStarted && Team >= 0 && Team < Teams.size() && (Teams[Team].Players.size() < Party_Players_Max) && !Name.empty())
  {
    // append element to players array
    auto& players = Teams[Team].Players;
    players.emplace_back(TParty_PlayerInfo(Name));

    return players.size() - 1;
  }
  return std::nullopt;
}

/* starts a new PartyGame, returns true on success
  before a call of this function teams and players
  has to be added by AddTeam and AddPlayer */
bool TPartyGame::StartGame(ARounds Rounds)
{
  if (bPartyStarted || Rounds.empty() || Teams.size() < Party_Teams_Min)
    return false;

  // check teams for minimal player count
  if (std::ranges::any_of(Teams, [](const auto& team){
    return team.Players.size() < Party_Players_Min;
  }))
    return false;

  // create rounds array
  this->Rounds.clear();
  this->Rounds.reserve(Rounds.size());

  for (const auto& round : Rounds)
  {
    // copy round or select a random round
    if (round != Party_Round_Random && round >= 0 && round < Modes.size())
      this->Rounds.emplace_back(round);
    else
      this->Rounds.emplace_back(GetRandomMode());
  }

  // get the party started!11
  bPartyStarted = true;
  bPartyGame = true;
  CurRound = std::numeric_limits<int>::min(); //< set not to -1 to indicate that party game is not finished

  // first round
  NextRound();

  return true;
}

/* sets the winner(s) of current round
  returns true on success */
bool TPartyGame::SetRanking(AParty_TeamRanking Ranking)
{
  if (!bPartyStarted || CurRound < 0 || CurRound >= Rounds.size())
    return false;
  
  auto& curRound = Rounds[CurRound];
  auto& curRanking = curRound.Ranking;
  curRanking = Ranking;

  // look for teams that don't exist
  int TeamExists = 0;
  for (const auto& rank : curRanking)
    TeamExists |= (1 << (rank.Team - 1));

  // create teams that don't exist
  for (size_t i = 0; i < Teams.size(); ++i)
  {
    if (TeamExists & (1 << i) == 0)
      curRanking.emplace_back(i + 1, Teams.size());
  }
  // we may remove rankings from invalid teams here to
  // but at the moment this is not necessary, because the
  // functions this function is called from don't create
  // invalid rankings

  // sort rankings by team
  std::ranges::sort(curRanking, std::ranges::less(), &TParty_TeamRanking::Team);

  //set rounds RankingSet to true
  curRound.RankingSet = true;

  return true;
}

/* sets ranking of current round by score saved in players array */
void TPartyGame::SetRankingByScore()
{
  /*if (Player.size() == Teams.size())
  {
    SetLength(Ranking, Length(Teams));
    SetLength(Scores, Length(Teams));

    // fill ranking array
    for I = 0 to High(Ranking) do
    {
      Ranking[I].Team = I;
      Ranking[I].Rank = 0;
      Scores[I] = Player[I].ScoreTotalInt;
    }

    // bubble sort by score
    J = High(Ranking);
    repeat
      for I = 0 to J - 1 do
        if (Scores[I] < Scores[I+1]) then
        {
          TmpRanking = Ranking[I];
          Ranking[I] = Ranking[I+1];
          Ranking[I+1] = TmpRanking;

          TmpScore = Scores[I];
          Scores[I] = Scores[I+1];
          Scores[I+1] = TmpScore;
        }
      Dec(J);
    until J <= 0;

    // set rank field
    Rank = 1; //first rank has id 1
    for I = 0 to High(Ranking) do
    {
      Ranking[I].Rank = Rank;

      if (I < High(Ranking)) and (Scores[I] <> Scores[I+1]) then
        Inc(Rank); // next rank if next team has different score
    }
  }
  else
    SetLength(Ranking, 0);*/

  AParty_TeamRanking Ranking;
  Ranking.reserve(Teams.size());

  std::vector<std::vector<TParty_TeamInfo>::iterator> ranking(Teams.size());
  std::iota(ranking.begin(), ranking.end(), Teams.begin());

  std::ranges::sort(ranking, std::ranges::greater(), [](const std::vector<TParty_TeamInfo>::iterator& it){ return it->Score; });

  int prevRank = 0;
  int prevScore = std::numeric_limits<int>::max();
  for (const auto& sorted : ranking)
  {
    //idx is team
    int idx = std::distance(Teams.begin(), sorted);
    if (prevScore > sorted->Score)
    {
      ++prevRank;
      prevScore = sorted->Score;
    }
    Ranking.emplace_back(idx, prevRank);
  }
  SetRanking(Ranking);
}

/* increases players TimesPlayed value */
void TPartyGame::IncTimesPlayed()
{
  for (auto& team : Teams)
    ++team.Players[team.NextPlayer].TimesPlayed;
}

/* increases round counter by 1 and clears all round specific information;
  returns the number of the current round or -1 if last round has already
  been played */
int TPartyGame::NextRound()
{
  // some lines concerning the previous round
  if (CurRound >= 0)
  {
    IncTimesPlayed();
    Rounds[CurRound].AlreadyPlayed = true;
    GenScores();
  }

  // increase round counter
  ++CurRound;
  CurRound = std::max(0, CurRound); // we start first round

  if (CurRound >= Rounds.size())
    CurRound = -1; //< last round played

  // some lines concerning the next round
  if (CurRound >= 0)
  {
    // select player
    for (size_t i = 0; i < Teams.size(); ++i)
      Teams[i].NextPlayer = GetRandomPlayer(i);
  }
  return CurRound;
}

/* indicates that current round has already been played */
void TPartyGame::RoundPlayed()
{
  if (!bPartyStarted || CurRound < 0 || CurRound >= Rounds.size())
    return;
  
  // set rounds ranking by score if it was not set by plugin
  if (not Rounds[CurRound].RankingSet)
    SetRankingByScore();

  Rounds[CurRound].AlreadyPlayed = true;
}

/* returns true if last round was already played */
bool TPartyGame::GameFinished()
{
  return (bPartyStarted && CurRound == -1);
}

/* private: calls the specified function Func from lua plugin Parent
           if both exist.
           return true if default function should be called
           (function or plugin does not exist, or function returns
           true) */
bool TPartyGame::CallLua(int Parent, std::string Func)
  //var
    //P: TLuaPlugin;
{
  // call default function by default
  Result = true;

  // check for core plugin and empty function name
  if (Parent >= 0) and (Length(Func) > 0) then
  {
    // get plugin that registred the mode
    P = LuaCore.GetPluginById(Parent);

    if (P <> nil) then
    {
      if (P.CallFunctionByName(Func, 0, 1)) then
        // check result
        Result = (lua_toboolean(P.LuaState, 1));
    }
  }
}

/* call plugins defined function and/or default void
  only default void is called when no function is defined by plugin
  if plugins function returns true then default is called after plugins
  function was executed*/
void TPartyGame::CallBeforeSongSelect()
  //var
    //ExecuteDefault: bool;
{
  if not bPartyStarted then
    ExecuteDefault = true
  else if (CurRound >= 0) then
  {
    // we set screen song to party mode
    // plugin should not have to do this if it
    // don't want default void to be executed
    //ScreenSong.Mode = smPartyClassic;

    with Modes[Rounds[CurRound].Mode] do
      ExecuteDefault = (CallLua(Parent, Functions.BeforeSongSelect));
  }
  else
    ExecuteDefault = true;

  // execute default function:
  if ExecuteDefault then
  {
    // display song select screen
    Display.FadeTo(@ScreenSong);
  }
}

void TPartyGame::CallAfterSongSelect()
  var
    ExecuteDefault: bool; 
{
  if not bPartyStarted then
    ExecuteDefault = true
  else if (CurRound >= 0) then
  {
    with Modes[Rounds[CurRound].Mode] do
      ExecuteDefault = (CallLua(Parent, Functions.AfterSongSelect));
  }
  else
    ExecuteDefault = true;

  // execute default function:
  if ExecuteDefault then
  {
    // display sing screen
    ScreenSong.StartSong;
  }
}

void TPartyGame::CallBeforeSing()
  var
    ExecuteDefault: bool;
{
  if (!bPartyStarted)
    ExecuteDefault = true
  else if (CurRound >= 0)
  {
    with Modes[Rounds[CurRound].Mode] do
      ExecuteDefault = (CallLua(Parent, Functions.BeforeSing));
  }
  else
    ExecuteDefault = true;

  //set correct playersplay
  if (bPartyGame)
    PlayersPlay = Length(Teams);

  if (ScreenSong.Mode == smPartyTournament)
    PlayersPlay = 2;

  // execute default function:
  if (ExecuteDefault)
  {
    //nothing atm
    /* to-do : compartmentalize TSingScreen.OnShow into
              functions for init of a specific part of
              sing screen.
              these functions should be called here before
              sing screen is shown, or it should be called
              by plugin if it wants to define a custom
              singscreen start up. */
  }
}

void TPartyGame::CallOnSing()
  var
    ExecuteDefault: bool;
{
  if (!bPartyStarted)
    ExecuteDefault = true;
  else if (CurRound >= 0)
  {
    with Modes[Rounds[CurRound].Mode] do
      ExecuteDefault = (CallLua(Parent, Functions.OnSing));;
  }
  else
    ExecuteDefault = true;

  // execute default function:
  if ExecuteDefault then
  {
    //nothing atm
  }
}

void TPartyGame::CallAfterSing()
  var
    ExecuteDefault: bool;
{
  if not bPartyStarted then
    ExecuteDefault = true
  else if (CurRound >= 0) then
  {
    with Modes[Rounds[CurRound].Mode] do
      ExecuteDefault = (CallLua(Parent, Functions.AfterSing));
  }
  else
    ExecuteDefault = true;

  // execute default function:
  if (ExecuteDefault)
  {
    if (bPartyGame)
      // display party score screen
      Display.FadeTo(@ScreenPartyScore)
    else //display standard score screen
    {
      if (ScreenSong.Mode == smPartyTournament)
        Display.FadeTo(@ScreenPartyTournamentWin)
      else
        Display.FadeTo(@ScreenScore);
    }
  }
}

/* returns an array[1..6] of int. the index stands for the placing,
  value is the team number (in the team array) */
AParty_TeamRanking TPartyGame::GetTeamRanking()
  var
    I, J: int;
    Temp: TParty_TeamRanking;
    Rank: int;
{
  SetLength(Result, Length(Teams));

  // fill ranking array
  for I = 0 to High(Result) do
  {
    Result[I].Team = I;
    Result[I].Rank = 0;
  }

  // bubble sort by score
  J = High(Result);
  repeat
    for I = 0 to J - 1 do
      if (Teams[Result[I].Team].Score < Teams[Result[I+1].Team].Score) then
      {
        Temp = Result[I];
        Result[I] = Result[I+1];
        Result[I+1] = Temp;
      }
    Dec(J);
  until J <= 0;

  // set rank field
  Rank = 1; //first rank has id 1
  for I = 0 to High(Result) do
  {
    Result[I].Rank = Rank;

    if (I < High(Result)) and (Teams[Result[I].Team].Score <> Teams[Result[I+1].Team].Score) then
      Inc(Rank); // next rank if next team has different score 
  } 
}

/* returns a string like "Team 1 (and Team 2) win"
  if Round is in range from 0 to high(Rounds) then
  result is name of winners of specified round.
  if Round is -1 the result is name of winners of
  the whole party game*/
std::string TPartyGame::GetWinnerString(int Round)
var
  Winners: array of UTF8String;
  I: int;
  Ranking: AParty_TeamRanking;
{
  Result = "";
  Ranking = nil;
  
  if (Round >= 0) and (Round <= High(Rounds)) then
  {
    if (not Rounds[Round].AlreadyPlayed) then
      Result = Language.Translate('PARTY_NOTPLAYEDYET')
    else
      Ranking = Rounds[Round].Ranking;
  }
  else if (Round = -1) then
    Ranking = GetTeamRanking;


  if (Ranking <> nil) then
  {
    SetLength(Winners, 0);
    for I = 0 to High(Ranking) do
    {
      if (Ranking[I].Rank = PR_First) and (Ranking[I].Team >= 0) and (Ranking[I].Team <= High(Teams)) then
      {
        SetLength(Winners, Length(Winners) + 1);
        Winners[high(Winners)] = UTF8String(Teams[Ranking[I].Team].Name);
      }
    }

    if (Length(Winners) > 0) then
      Result = Language.Implode(Winners);
  }

  if (Length(Result) = 0) then
    Result = Language.Translate('PARTY_NOBODY');
}

void TPartyGame::SaveSungPartySong(int ID)
{
  SetLength(SungPartySongs, Length(SungPartySongs) + 1);
  SungPartySongs[High(SungPartySongs)] = ID;
}

bool TPartyGame::SongNotSungAndNotDuet(int ID, int N_DuetSongs)
var
  I: int;
  Sung: bool;
{
  Sung = false;

  if (CatSongs.Song[ID].isDuet)
  {
    Result = false;
    Exit;
  }

  for I = 0 to High(SungPartySongs) do
  {
    if (SungPartySongs[I] = ID)
      Sung = true;
  }

  if (Sung)
  {
    if (Songs.SongList.Count - High(Party.SungPartySongs) - N_DuetSongs <= 1) then
      Result = true
    else
      Result = false
  }
  else
    Result = true;

}
}