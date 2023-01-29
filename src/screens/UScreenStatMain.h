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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/screens/UScreenStatMain.pas $
 * $Id: UScreenStatMain.pas 1939 2009-11-09 00:27:55Z s_alexander $
 */
#include <string>

#include <SDL2/SDL_keycode.h>

#include "../menu/UMenu.h"

namespace UScreenStatMain
{
  /*uses
    UDisplay,
    UIni,
    UMenu,
    UMusic,
    UThemes,
    sdl2,
    SysUtils;*/

  class TScreenStatMain : public UMenu::TMenu
  {
  private:
    // Some Stat Value that don't need to be calculated 2 times
    uint32_t SongsWithVid;
    std::string FormatOverviewIntro(std::string FormatStr);
    std::string FormatSongOverview(std::string FormatStr);
    std::string FormatPlayerOverview(std::string FormatStr);

  public:
    int TextOverview;
    TScreenStatMain();
    bool ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown) override;
    void OnShow() override;
    void SetAnimationProgress(double Progress) override;

    void SetOverview();
  };

  const std::string ID = "ID_050"; // for help system

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
    Classes;*/

  bool TScreenStatMain::ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown)
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
      // Exit Button Pressed
      if (Interaction == 4)
      {
        AudioPlayback.PlaySound(SoundLib.Back);
        FadeTo(@ScreenMain);
      }
      else // One of the Stats Buttons Pressed
      {
        AudioPlayback.PlaySound(SoundLib.Back);
        ScreenStatDetail.Typ = TStatType(Interaction);
        FadeTo(@ScreenStatDetail);
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
    return true;
  }

  TScreenStatMain::TScreenStatMain()
  // var
  // I:    int;
  {
    // inherited Create;

    TextOverview = AddText(Theme.StatMain.TextOverview);

    LoadFromTheme(Theme.StatMain);

    AddButton(Theme.StatMain.ButtonScores);
    if (Button[0].Text.empty())
      AddButtonText(14, 20, Theme.StatDetail.Description[0]);

    AddButton(Theme.StatMain.ButtonSingers);
    if (Button[1].Text.empty())
      AddButtonText(14, 20, Theme.StatDetail.Description[1]);

    AddButton(Theme.StatMain.ButtonSongs);
    if (Button[2].Text.empty())
      AddButtonText(14, 20, Theme.StatDetail.Description[2]);

    AddButton(Theme.StatMain.ButtonBands);
    if (Button[3].Text.empty())
      AddButtonText(14, 20, Theme.StatDetail.Description[3]);

    AddButton(Theme.StatMain.ButtonExit);
    if (Button[4].Text.empty())
      AddButtonText(14, 20, Theme.Options.Description[OPTIONS_DESC_INDEX_BACK]);

    Interaction = 0;

    // Set Songs with Vid
    SongsWithVid = 0;
    for (I = 0 to Songs.SongList.Count - 1)
      if (TSong(Songs.SongList[I]).Video.IsSet)
        ++SongsWithVid;
  }

  void TScreenStatMain::OnShow()
  {
    inherited;

    if (!Help.SetHelpID(ID))
      ULog::Log.LogWarn("No Entry for Help-ID " + ID, "ScreenStatMain");

    // Set Overview Text:
    SetOverview();
  }

  std::string TScreenStatMain::FormatOverviewIntro(std::string FormatStr)
  // var
  // Year, Month, Day: word;
  {
    /*Format:
      %0:d Ultrastar Version
      %1:d Day of Reset
      %2:d Month of Reset
      %3:d Year of Reset*/

    try
    {
      DecodeDate(Database.GetStatReset(), Year, Month, Day);
      return Format(FormatStr, [ Language.Translate("US_VERSION"), Day, Month, Year ]);
    }
    catch (const std::exception &e)
    {
      ULog::Log.LogError("Error Parsing FormatString \"STAT_OVERVIEW_INTRO\": " + e.what());
    }
    return "";
  }

  std::string TScreenStatMain::FormatSongOverview(std::string FormatStr)
  // var
  // CntSongs, CntSungSongs, CntVidSongs: int;
  // MostPopSongArtist, MostPopSongTitle: UTF8String;
  // StatList: TList;
  // MostSungSong: TStatResultMostSungSong;
  {
    /*Format:
      %0:d Count Songs
      %1:d Count of Sung Songs
      %2:d Count of UnSung Songs
      %3:d Count of Songs with Video
      %4:s Name of the most popular Song*/

    CntSongs = Songs.SongList.size();
    CntSungSongs = Database.GetTotalEntrys(stMostSungSong);
    CntVidSongs = SongsWithVid;

    StatList = Database.GetStats(stMostSungSong, 1, 0, false);
    if (StatList && !StatList.empty())
    {
      MostSungSong = StatList[0];
      MostPopSongArtist = MostSungSong.Artist;
      MostPopSongTitle = MostSungSong.Title;
    }
    else
    {
      MostPopSongArtist = '-';
      MostPopSongTitle = '-';
    }
    Database.FreeStats(StatList);

    try
    {
      return Format(FormatStr, [
        CntSongs, CntSungSongs, CntSongs - CntSungSongs, CntVidSongs,
        MostPopSongArtist,
        MostPopSongTitle
      ]);
    }
    catch (const std::exception &e)
    {
      ULog::Log.LogError("Error Parsing FormatString \"STAT_OVERVIEW_SONG\": " + e.what());
    }
    return "";
  }

  std::string TScreenStatMain::FormatPlayerOverview(std::string FormatStr)
  // var
  // CntPlayers: int;
  // BestScoreStat:    TStatResultBestScores;
  // BestSingerStat:   TStatResultBestSingers;
  // BestPlayer, BestScorePlayer: UTF8String;
  // BestPlayerScore, BestScore: int;
  // SingerStats, ScoreStats: TList;
  {
    /*Format:
      %0:d Count Players
      %1:s Best Player
      %2:d Best Players Score
      %3:s Best Score Player
      %4:d Best Score*/

    CntPlayers = Database.GetTotalEntrys(stBestSingers);

    SingerStats = Database.GetStats(stBestSingers, 1, 0, false);
    if (SingerStats && !SingerStats.empty())
    {
      BestSingerStat = SingerStats[0];
      BestPlayer = BestSingerStat.Player;
      BestPlayerScore = BestSingerStat.AverageScore;
    }
    else
    {
      BestPlayer = '-';
      BestPlayerScore = 0;
    }
    Database.FreeStats(SingerStats);

    ScoreStats = Database.GetStats(stBestScores, 1, 0, false);
    if (ScoreStats && !ScoreStats.empty())
    {
      BestScoreStat = ScoreStats[0];
      BestScorePlayer = BestScoreStat.Singer;
      BestScore = BestScoreStat.Score;
    }
    else
    {
      BestScorePlayer = '-';
      BestScore = 0;
    }
    Database.FreeStats(ScoreStats);

    try
    {
      return Format(Formatstr, [
        CntPlayers, BestPlayer, BestPlayerScore,
        BestScorePlayer,
        BestScore
      ]);
    }
    catch (const std::exception &e)
    {
      ULog::Log.LogError("Error Parsing FormatString \"STAT_OVERVIEW_PLAYER\": " + e.what());
    }
    return "";
  }

  void TScreenStatMain::SetOverview()
  // var
  // Overview: UTF8String;
  {
    // Format overview
    std::string Overview = FormatOverviewIntro(Language.Translate("STAT_OVERVIEW_INTRO")) + "\n \n" +
                           FormatSongOverview(Language.Translate("STAT_OVERVIEW_SONG")) + "\n \n" +
                           FormatPlayerOverview(Language.Translate("STAT_OVERVIEW_PLAYER"));
    Text[0].Text = Overview;
  }

  void TScreenStatMain::SetAnimationProgress(double Progress)
  // var
  // I: int;
  {
    for (I = 0 to high(Button))
      Button[I].Texture.ScaleW = Progress;
  }
}