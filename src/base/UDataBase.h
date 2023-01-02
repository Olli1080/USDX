#pragma once
/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it &&/or
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
 * along with this program; see the file COPYING. If !, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UDataBase.pas $
 * $Id: UDataBase.pas 2574 2010-07-07 19:52:32Z brunzelchen $
 */
#include "../switches.h"

#include <string>
#include <vector>
#include <list>
#include <filesystem>
#include <memory>
#include <cstdint>
#include <type_traits>
#include <chrono>

#include <sqlite_orm/sqlite_orm.h>

#include "UDLLManager.h"
#include "USong.h"
#include "ULog.h"
#include "UIni.h"
#include "ULanguage.h"

namespace UDataBase
{
/*uses
  Classes,
  SQLiteTable3,
  UDllManager,
  UIni,
  UPath,
  USong,
  USongs,
  UTextEncoding;*/

//--------------------
//DataBaseSystem - Class including all DB methods
//--------------------

enum TStatType
{
  stBestScores,   // Best scores
  stBestSingers,  // Best singers
  stMostSungSong, // Most sung songs
  stMostPopBand   // Most popular band
};

// abstract super-class for statistic results
class TStatResult
{
public:
  TStatType Typ;
};

class TStatResultBestScores : public TStatResult
{
public:
  std::string Singer;
  uint16_t Score;
  uint8_t Difficulty;
  std::string SongArtist;
  std::string SongTitle;
  std::string Date;
};

class TStatResultBestSingers : public TStatResult
{
public:
  std::string Player;
  uint16_t AverageScore;
};

class TStatResultMostSungSong : public TStatResult
{
public:
  std::string Artist;
  std::string Title;
  uint16_t TimesSung;
};

class TStatResultMostPopBand : public TStatResult
{
public:
  std::string ArtistName;
  uint16_t TimesSungTot;
};

struct TUserInfo
{
  std::string Username;
  std::string Password;
  int SendSavePlayer;
  int AutoMode;
  int AutoPlayer;
  int AutoScoreEasy;
  int AutoScoreMedium;
  int AutoScoreHard;
  bool Save;
};

typedef std::shared_ptr<TUserInfo> PUserInfo;

struct TNetworkUser
{
  std::string Website;
  std::vector<TUserInfo> UserList;
};

typedef std::shared_ptr<TNetworkUser> PNetworkUser;

/*{
 cDBVersion - history
 0 = USDX 1.01 or no Database
 01 = USDX 1.1
}*/
const int cDBVersion = 01; // 0.1
const std::string cUS_Scores = "us_scores";
const std::string cUS_Songs = "us_songs";
const std::string cUS_Statistics_Info = "us_statistics_info";
const std::string cUS_Users_Info = "us_users_info";
const std::string cUS_Webs = "us_webs";
const std::string cUS_Webs_Stats = "us_webs_stats";

struct DBScore
{
    int songId;
    int difficulty;
    std::string player;
    int score;
    std::unique_ptr<std::chrono::system_clock::rep> date;
};

struct DBSong
{
    int id;
    std::string artist;
    std::string title;
    int timesPlayed;
    std::unique_ptr<int> rating;
    std::unique_ptr<int> videoRatioAspect;
    std::unique_ptr<int> videoWidth;
    std::unique_ptr<int> videoHeight;
    std::unique_ptr<int> lyricPosition;
    std::unique_ptr<int> lyricAlpha;
    std::unique_ptr<std::string> lyricSingFillColor;
    std::unique_ptr<std::string> lyricActualFillColor;
    std::unique_ptr<std::string> lyricNextFillColor;
    std::unique_ptr<std::string> lyricSingOutlineColor;
    std::unique_ptr<std::string> lyricActualOutlineColor;
    std::unique_ptr<std::string> lyricNextOutlineColor;
};

struct Web
{
    int id;
    std::string name;
};

struct WebStat
{
    int webId;
    int songId;
    std::unique_ptr<int> maxScore0;
    std::unique_ptr<int> mediaScore0;
    std::unique_ptr<std::string> userScore0;
    std::unique_ptr<int> maxScore1;
    std::unique_ptr<int> mediaScore1;
    std::unique_ptr<std::string> userScore1;
    std::unique_ptr<int> maxScore2;
    std::unique_ptr<int> mediaScore2;
    std::unique_ptr<std::string> userScore2;
};

struct StatInfo
{
    std::chrono::system_clock::rep resetTime;
};

struct UserInfo
{
    int webID;
    std::string username;
    std::string password;
    int sendSavePlayer;
    int autoMode;
    int autoPlayer;
    int autoScoreEasy;
    int autoScoreMedium;
    int autoScoreHard;
};

inline auto createOrmStorage(std::filesystem::path Filename)
{
    using namespace sqlite_orm;
	return make_storage(Filename.string(), 
		make_table(cUS_Songs, 
		  make_column("ID", &DBSong::id, primary_key(), autoincrement()),
		  make_column("Artist", &DBSong::artist),
		  make_column("Title", &DBSong::title),
		  make_column("TimesPlayed", &DBSong::timesPlayed, default_value(0)),
		  make_column("Rating", &DBSong::rating),
		  make_column("VideoRatioAspect", &DBSong::videoRatioAspect),
		  make_column("VideoWidth", &DBSong::videoWidth),
		  make_column("VideoHeight", &DBSong::videoHeight),
		  make_column("LyricPosition", &DBSong::lyricPosition),
		  make_column("LyricAlpha", &DBSong::lyricAlpha),
		  make_column("LyricSingFillColor", &DBSong::lyricSingFillColor),
		  make_column("LyricActualFillColor", &DBSong::lyricActualFillColor),
		  make_column("LyricNextFillColor", &DBSong::lyricNextFillColor),
		  make_column("LyricSingOutlineColor", &DBSong::lyricSingOutlineColor),
		  make_column("LyricActualOutlineColor", &DBSong::lyricActualOutlineColor),
		  make_column("LyricNextOutlineColor", &DBSong::lyricNextOutlineColor)
		),
		make_table(cUS_Scores,
		  make_column("SongID", &DBScore::songId),
		  make_column("Difficulty", &DBScore::difficulty),
		  make_column("Player", &DBScore::player),
		  make_column("Score", &DBScore::score),
		  make_column("Date", &DBScore::date),
		  foreign_key(&DBScore::songId).references(&DBSong::id)
		),
		make_table(cUS_Webs,
		  make_column("ID", &Web::id, primary_key(), autoincrement()),
		  make_column("Name", &Web::name)
		),
		make_table(cUS_Webs_Stats,
		  make_column("WebID", &WebStat::webId),
		  make_column("SongID", &WebStat::songId),
		  make_column("Max_Score_0", &WebStat::maxScore0),
		  make_column("Media_Score_0", &WebStat::mediaScore0),
		  make_column("User_Score_0", &WebStat::userScore0),
		  make_column("Max_Score_1", &WebStat::maxScore1),
		  make_column("Media_Score_1", &WebStat::mediaScore1),
		  make_column("User_Score_1", &WebStat::userScore1),
		  make_column("Max_Score_2", &WebStat::maxScore2),
		  make_column("Media_Score_2", &WebStat::mediaScore2),
		  make_column("User_Score_2", &WebStat::userScore2),
		  foreign_key(&WebStat::webId).references(&Web::id),
		  foreign_key(&WebStat::songId).references(&DBSong::id)
		),
		make_table(cUS_Users_Info,
		  make_column("WebID", &UserInfo::webID),
		  make_column("Username", &UserInfo::username),
		  make_column("Password", &UserInfo::password),
		  make_column("SendSavePlayer", &UserInfo::sendSavePlayer, default_value(0)),
		  make_column("AutoMode", &UserInfo::autoMode, default_value(0)),
		  make_column("AutoPlayer", &UserInfo::autoPlayer, default_value(0)),
		  make_column("AutoScoreEasy", &UserInfo::autoScoreEasy, default_value(0)),
		  make_column("AutoScoreMedium", &UserInfo::autoScoreMedium, default_value(0)),
		  make_column("AutoScoreHard", &UserInfo::autoScoreHard, default_value(0)),
		  foreign_key(&UserInfo::webID, &Web::id)
		),
		make_table(cUS_Statistics_Info, 
		  make_column("ResetTime", &StatInfo::resetTime)
		)
	);
}

typedef std::invoke_result<decltype(&createOrmStorage), std::filesystem::path>::type Storage;

class TDataBaseSystem
{
private:
  std::filesystem::path fFilename;
  std::unique_ptr<Storage> storage;

public:
  // Network
  std::vector<TNetworkUser> NetworkUser;

  TDataBaseSystem(std::filesystem::path Filename);
  ~TDataBaseSystem();

  void Init(const std::filesystem::path Filename);
  void ConvertFrom101To110();
  void ReadScore(USong::TSong& Song);
  void AddScore(USong::TSong Song, int Level, const std::string Name, int Score);

  void ReadUsers();
  void UpdateUsers();
  void DeleteUser(std::string Website, std::string Username);
  void NewUser(std::string Website, std::string Username, std::string Password);

  void AddWebsite();

  void AddSong(USong::TSong Song);

  void AddMax_Score(USong::TSong Song, int WebID, int Receive_Max_Score, int Level);
  void AddMedia_Score(USong::TSong Song, int WebID, int Receive_Media_Score, int Level);
  void AddUser_Score(USong::TSong Song, int WebID, std::string Receive_User_Score, int Level);

  int ReadMax_Score(std::string Artist, std::string Title, int WebID, int Level);
  int ReadMedia_Score(std::string Artist, std::string Title, int WebID, int Level);
  std::string ReadUser_Score(std::string Artist, std::string Title, int WebID, int Level);

  int ReadMax_ScoreLocal(std::string Artist, std::string Title, int Level);
  int ReadMedia_ScoreLocal(std::string Artist, std::string Title, int Level);
  std::string ReadUser_ScoreLocal(std::string Artist, std::string Title, int Level);

  int Delete_Score(USong::TSong Song, int WebID);

  std::list<> GetStats(TStatType Typ, uint8_t Count, uint32_t Page, bool Reversed);
  void FreeStats(std::list<> StatList);
  uint32_t GetTotalEntrys(TStatType Typ);
  TDateTime GetStatReset();
  std::string FormatDate(const std::unique_ptr<std::chrono::system_clock::rep>& time_stamp);

  void SaveSongOptions(USong::TSong Song, USong::TSongOptions Options);
  USong::TSongOptions GetSongOptions(USong::TSong Song);
};

inline TDataBaseSystem DataBase;



/**
 * Adds songs to DB
 */
void TDataBaseSystem::AddSong(USong::TSong Song)
/*var
  ID:        int;
  TableData: TSQLiteTable;*/
{
  TableData = nil;

  if (Song.Artist != "") && (Song.Title != "") then
  {
  try
    ID = ScoreDB.GetTableValue(
        "SELECT [ID] FROM ["+cUS_Songs+"] " +
        "WHERE [Artist] = ? && [Title] = ?",
        [Song.Artist, Song.Title]);

    if (ID = 0) then
    {
      // Create song if it does ! exist
      ScoreDB->exec(
          "INSERT INTO ["+cUS_Songs+"] " +
          "([Artist], [Title], [TimesPlayed]) VALUES " +
          "(?, ?, 0);",
          [Song.Artist, Song.Title]);
    }

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::AddSong");
  }
  }

  TableData.Free;
}

/**
 * Add Max_Score to Song
 */
void TDataBaseSystem::AddMax_Score (Song: USong::TSong; WebID: int; Receive_Max_Score: int; Level: int);
var
  Max_Score, ID, Count: int;
  TableData: TSQLiteTable;
{

  if ! ScoreDB then
    return;

  TableData = nil;

  try

    ID = ScoreDB.GetTableValue(
        "SELECT [ID] FROM ["+cUS_Songs+"] " +
        "WHERE [Artist] = ? && [Title] = ?",
        [Song.Artist, Song.Title]);

    Count = ScoreDB.GetTableValue(
        "SELECT COUNT/*/ FROM [" + cUS_Webs_Stats + "] " +
        "WHERE [WebID] = ? && [SongID] = ?",
        [WebID, ID]);

    if (Count = 0) then
    {
      // Insert Max Score
      ScoreDB->exec(
         "INSERT INTO ["+cUS_Webs_Stats+"] " +
         "([WebID], [SongID], [Max_Score_" + IntToStr(Level) + "]) VALUES(?,?,?);",
         [WebID, ID, Receive_Max_Score]);
    end
    else
    {

      Max_Score = ScoreDB.GetTableValue(
        "SELECT [Max_Score_" + IntToStr(Level) + "] FROM [" + cUS_Webs_Stats + "] " +
        "WHERE [WebID] = ? && [SongID] = ?",
        [WebID, ID]);

      if (Max_Score != Receive_Max_Score) then
      {
        // Update Max Score
        ScoreDB->exec(
            "UPDATE ["+cUS_Webs_Stats+"] " +
            "SET [Max_Score_" + IntToStr(Level) + "] = " + IntToStr(Receive_Max_Score) +
            " WHERE [WebID] = ? && [SongID] = ?;",
            [WebID, ID]);
      }
    }

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::AddMax_Score");
  }

  TableData.Free;

}

/**
 * Add Media_Score to Song
 */
void TDataBaseSystem::AddMedia_Score(Song: USong::TSong; WebID: int; Receive_Media_Score: int; Level: int)
/*var
  Media_Score, ID: int;
  TableData: TSQLiteTable;*/
{

  if ! ScoreDB then
    return;

  TableData = nil;

  try

    ID = ScoreDB.GetTableValue(
        "SELECT [ID] FROM ["+cUS_Songs+"] " +
        "WHERE [Artist] = ? && [Title] = ?",
        [Song.Artist, Song.Title]);

    Media_Score = ScoreDB.GetTableValue(
        "SELECT [Media_Score_" + IntToStr(Level) + "] FROM [" + cUS_Webs_Stats + "] " +
        "WHERE [WebID] = ? && [SongID] = ?",
        [WebID, ID]);

    if (Media_Score != Receive_Media_Score) then
    {
      // Update Max Score
      ScoreDB->exec(
          "UPDATE ["+cUS_Webs_Stats+"] " +
          "SET [Media_Score_" + IntToStr(Level) + "] = " + IntToStr(Receive_Media_Score) +
          " WHERE [WebID] = ? && [SongID] = ?;",
          [WebID, ID]);
    }

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::AddMedia_Score");
  }

  TableData.Free;

}


/**
 * Add User to Song
 */
void TDataBaseSystem::AddUser_Score (Song: USong::TSong; WebID: int; Receive_User_Score: string; Level: int);
var
  User_Score: string;
  ID: int;
  TableData: TSQLiteTable;
{

  if ! ScoreDB then
    return;

  TableData = nil;

  try

    ID = ScoreDB.GetTableValue(
        "SELECT [ID] FROM ["+cUS_Songs+"] " +
        "WHERE [Artist] = ? && [Title] = ?",
        [Song.Artist, Song.Title]);

    User_Score = ScoreDB.GetTableString(
        "SELECT [User_Score_" + IntToStr(Level) + "] FROM [" + cUS_Webs_Stats + "] " +
        "WHERE [WebID] = ? && [SongID] = ?",
        [WebID, ID]);

    if (User_Score != Receive_User_Score) then
    {
      // Update User Score
      ScoreDB->exec(
          "UPDATE ["+cUS_Webs_Stats+"] " +
          "SET [User_Score_" + IntToStr(Level) + "] = ? " +
          " WHERE [WebID] = ? && [SongID] = ?;",
          [UTF8Encode(Receive_User_Score), WebID, ID]);
    }

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::AddUser_Score");
  }

  TableData.Free;

}

/**
 * Read Max_Score
 */
function TDataBaseSystem::ReadMax_Score(Artist, Title: std::string; WebID, Level: int): int;
var
  Max_Score, SongID: int;
  TableData: TSQLiteTable;
{

  Max_Score = 0;

  if ! ScoreDB then
    return;

  TableData = nil;

  try
    SongID = ScoreDB.GetTableValue(
        "SELECT [ID] FROM ["+cUS_Songs+"] " +
        "WHERE [Artist] = ? && [Title] = ?",
        [Artist, Title]);

    Max_Score = ScoreDB.GetTableValue(
        "SELECT [Max_Score_" + IntToStr(Level) + "] FROM ["+cUS_Webs_Stats+"] " +
        "WHERE [WebID] = ? && [SongID] = ?",
        [WebID, SongID]);

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::ReadMax_Score");
  }

  TableData.Free;

  result = Max_Score;

}

/**
 * Read Media_Score
 */
function TDataBaseSystem::ReadMedia_Score(Artist, Title: std::string; WebID, Level: int): int;
var
  Media_Score, SongID: int;
  TableData: TSQLiteTable;
{
  Media_Score = 0;

  if ! ScoreDB then
    return;

  TableData = nil;

  try
    SongID = ScoreDB.GetTableValue(
        "SELECT [ID] FROM ["+cUS_Songs+"] " +
        "WHERE [Artist] = ? && [Title] = ?",
        [Artist, Title]);

    Media_Score = ScoreDB.GetTableValue(
        "SELECT [Media_Score_" + IntToStr(Level) + "] FROM ["+cUS_Webs_Stats+"] " +
        "WHERE [WebID] = ? && [SongID] = ?",
        [WebID, SongID]);

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::ReadMedia_Score");
  }

  TableData.Free;

  Result = Media_Score;

}

/**
 * Read User_Score
 */
function TDataBaseSystem::ReadUser_Score(Artist, Title: std::string; WebID, Level: int): string;
var
  User_Score: string;
  SongID: int;
  TableData: TSQLiteTable;
{

  if ! ScoreDB then
    return;

  TableData = nil;

  try
    SongID = ScoreDB.GetTableValue(
        "SELECT [ID] FROM ["+cUS_Songs+"] " +
        "WHERE [Artist] = ? && [Title] = ?",
        [Artist, Title]);

    User_Score = ScoreDB.GetTableString(
        "SELECT [User_Score_" + IntToStr(Level) + "] FROM ["+cUS_Webs_Stats+"] " +
        "WHERE [WebID] = ? && [SongID] = ?",
        [WebID, SongID]);

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::ReadUser_Score");
  }

  TableData.Free;

  Result = UTF8Decode(User_Score);

}


/**
 * Read Max_Score Local
 */
function TDataBaseSystem::ReadMax_ScoreLocal(Artist, Title: std::string; Level: int): int;
var
  Max_Score, ID: int;
  TableData: TSQLiteTable;
{

  Max_Score = 0;

  if ! ScoreDB then
    return;

  TableData = nil;

  try

    ID = ScoreDB.GetTableValue(
          "SELECT [ID] FROM ["+cUS_Songs+"] " +
          "WHERE [Artist] = ? && [Title] = ?",
          [Artist, Title]);

    Max_Score = ScoreDB.GetTableValue(
        "SELECT MAX([Score]) FROM ["+cUS_Scores+"] " +
        "WHERE [SongID] = ? && [Difficulty] = ?",
        [ID, Level]);

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::ReadMax_ScoreLocal");
 }

  TableData.Free;

  Result = Max_Score;

}

/**
 * Read Media_Score
 */
function TDataBaseSystem::ReadMedia_ScoreLocal(Artist, Title: std::string; Level: int): int;
var
  Media_Score, ID: int;
  TableData: TSQLiteTable;
{

  Media_Score = 0;

  if ! ScoreDB then
    return;

  TableData = nil;

  try
    ID = ScoreDB.GetTableValue(
          "SELECT [ID] FROM ["+cUS_Songs+"] " +
          "WHERE [Artist] = ? && [Title] = ?",
          [Artist, Title]);

    Media_Score = ScoreDB.GetTableValue(
        "SELECT AVG([Score]) FROM ["+cUS_Scores+"] " +
        "WHERE [SongID] = ? && [Difficulty] = ?",
        [ID, Level]);

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::ReadMedia_ScoreLocal");
  }

  TableData.Free;

  result = Media_Score;

}

/**
 * Read User_Score
 */
function TDataBaseSystem::ReadUser_ScoreLocal(Artist, Title: std::string; Level: int): string;
var
  User_Score: string;
  ID: int;
  TableData: TSQLiteTable;
{

  if ! ScoreDB then
    return;

  TableData = nil;

  try
    ID = ScoreDB.GetTableValue(
          "SELECT [ID] FROM ["+cUS_Songs+"] " +
          "WHERE [Artist] = ? && [Title] = ?",
          [Artist, Title]);

    User_Score = ScoreDB.GetTableString(
                 "SELECT [Player] FROM ["+cUS_Scores+"] " +
                 "WHERE [SongID] = ? && [Difficulty] = ? ORDER BY [Score] DESC LIMIT 1",
                 [ID, Level]);

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::ReadUser_Score");
  }

  TableData.Free;

  Result = User_Score;

}

/**
 * Delete Score (Maybe song note exist anymore (or update))
 */
function TDataBaseSystem::Delete_Score(Song: USong::TSong; WebID: int): int;
var
  Score: uint8_t;
  ID: int;
  TableData: TSQLiteTable;
{

  Score = 0;

  if ! ScoreDB then
    return;

  TableData = nil;

  try

    ID = ScoreDB.GetTableValue(
        "SELECT [ID] FROM ["+cUS_Songs+"] " +
        "WHERE [Artist] = ? && [Title] = ?",
        [Song.Artist, Song.Title]);

    Score = ScoreDB.GetTableValue(
        "SELECT COUNT/*/ FROM [" + cUS_Webs_Stats + "] " +
        "WHERE [WebID] = ? && [SongID] = ?",
        [WebID, ID]);

    if (Score != 0) then
    {
      // Delete Score
      ScoreDB->exec(
          "DELETE FROM ["+cUS_Webs_Stats+"] " +
          " WHERE [WebID] = ? && [SongID] = ?;",
          [WebID, ID]);
    }

  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::Delete_Score");
  }

  TableData.Free;

  Result = Score;
}

/*
 * Writes some stats to array.
 * Returns nil if the database is ! ready or a list with zero or more statistic
 * entries.
 * Free the result-list with FreeStats() after usage to avoid memory leaks.
 */
function TDataBaseSystem::GetStats(Typ: TStatType; Count: uint8_t; Page: uint32_t; Reversed: bool): TList;
var
  Query:     string;
  TableData: TSQLiteUniTable;
  Stat:      TStatResult;
{
  Result = nil;

  if ! ScoreDB then
    return;

  {Todo:  Add Prevention that only players with more than 5 scores are selected at type 2}

  // Create query
  case Typ of
    stBestScores: {
      Query = "SELECT [Player], [Difficulty], [Score], [Artist], [Title], [Date] FROM [" + cUS_Scores + "] " +
               "INNER JOIN [" + cUS_Songs + "] ON ([SongID] = [ID]) ORDER BY [Score]";
    }
    stBestSingers: {
      Query = "SELECT [Player], ROUND(AVG([Score])) FROM [" + cUS_Scores + "] " +
               "GROUP BY [Player] ORDER BY AVG([Score])";
    }
    stMostSungSong: {
      Query = "SELECT [Artist], [Title], [TimesPlayed] FROM [" + cUS_Songs + "] " +
               "ORDER BY [TimesPlayed]";
    }
    stMostPopBand: {
      Query = "SELECT [Artist], SUM([TimesPlayed]) FROM [" + cUS_Songs + "] " +
               "GROUP BY [Artist] ORDER BY SUM([TimesPlayed])";
    }
  }

  // Add order direction
  Query = Query + IfThen(Reversed, " ASC", " DESC");

  // Add limit
  Query = Query + " LIMIT " + InttoStr(Count * Page) + ", " + InttoStr(Count) + ";";

  // Execute query
  try
    TableData = ScoreDB.GetUniTable(Query);
  except
    on E: Exception do
    {
      ULog::Log.LogError(E.Message, "TDataBaseSystem::GetStats");
      return;
    }
  }

  Result = TList.Create;
  Stat = nil;

  // Copy result to stats array
  while ! TableData.EOF do
  {
    case Typ of
      stBestScores: {
        Stat = TStatResultBestScores.Create;
        with TStatResultBestScores(Stat) do
        {
          Singer = TableData.Fields[0];
          Difficulty = TableData.FieldAsInteger(1);
          Score = TableData.FieldAsInteger(2);
          SongArtist = TableData.Fields[3];
          SongTitle = TableData.Fields[4];
          Date = FormatDate(TableData.FieldAsInteger(5));
        }
      }
      stBestSingers: {
        Stat = TStatResultBestSingers.Create;
        with TStatResultBestSingers(Stat) do
        {
          Player = TableData.Fields[0];
          AverageScore = TableData.FieldAsInteger(1);
        }
      }
      stMostSungSong: {
        Stat = TStatResultMostSungSong.Create;
        with TStatResultMostSungSong(Stat) do
        {
          Artist = TableData.Fields[0];
          Title  = TableData.Fields[1];
          TimesSung  = TableData.FieldAsInteger(2);
        }
      }
      stMostPopBand: {
        Stat = TStatResultMostPopBand.Create;
        with TStatResultMostPopBand(Stat) do
        {
          ArtistName = TableData.Fields[0];
          TimesSungTot = TableData.FieldAsInteger(1);
        }
      end
      else
        ULog::Log.LogCritical("Unknown stat-type", "TDataBaseSystem::GetStats");
    }

    Stat.Typ = Typ;
    Result.Add(Stat);

    TableData.Next;
  }

  TableData.Free;
}

void TDataBaseSystem::FreeStats(StatList: TList)
/*var
  Index: int;*/
{
  if (StatList == nil) then
    return;
  for Index = 0 to StatList.Count-1 do
    TStatResult(StatList[Index]).Free;
}

/**
 * Gets total number of entries for a stats query
 */
uint32_t TDataBaseSystem::GetTotalEntrys(TStatType Typ)
{
	uint32_t Result = 0;

    std::string Query;
	try
	{
	    // Create query
	    switch (Typ)
		{
	    case stBestScores:
	        Query = "SELECT COUNT([SongID]) FROM [" + cUS_Scores + "];";
            break;
        case stBestSingers:
	        Query = "SELECT COUNT(DISTINCT [Player]) FROM [" + cUS_Scores + "];";
            break;
        case stMostSungSong:
	        Query = "SELECT COUNT([ID]) FROM [" + cUS_Songs + "];";
            break;
        case stMostPopBand:
	        Query = "SELECT COUNT(DISTINCT [Artist]) FROM [" + cUS_Songs + "];";
            break;
		}

	Result = ScoreDB.GetTableValue(Query);
	}
  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::GetTotalEntrys");
  }

}

/**
 * Gets reset date of statistic data
 */
TDateTime TDataBaseSystem::GetStatReset()
/*var
  Query: string;*/
{
  Result = 0;

  try
    Query = "SELECT [ResetTime] FROM [" + cUS_Statistics_Info + "];";
    Result = UnixToDateTime(ScoreDB.GetTableValue(Query));
  except on E: Exception do
    ULog::Log.LogError(E.Message, "TDataBaseSystem::GetStatReset");
  }
}

/**
 * SaveSongOptions to DB
 */
void TDataBaseSystem::SaveSongOptions(Song: USong::TSong; Options: TSongOptions);
{

  AddSong(Song);

  ScoreDB->exec(
          "UPDATE [" + cUS_Songs + "] " +
          "SET [VideoRatioAspect] = ?, " +
          "[VideoWidth] = ?, " +
          "[VideoHeight] = ?, " +
          "[LyricPosition] = ?, " +
          "[LyricAlpha] = ?, " +
          "[LyricSingFillColor] = ?, " +
          "[LyricActualFillColor] = ?, " +
          "[LyricNextFillColor] = ?, " +
          "[LyricSingOutlineColor] = ?, " +
          "[LyricActualOutlineColor] = ?, " +
          "[LyricNextOutlineColor] = ? " +
          "WHERE [Artist] = ? && [Title] = ?;",
          [Options.VideoRatioAspect, Options.VideoWidth, Options.VideoHeight,
          Options.LyricPosition, Options.LyricAlpha,
          Options.LyricSingFillColor, Options.LyricActualFillColor, Options.LyricNextFillColor,
          Options.LyricSingOutlineColor, Options.LyricActualOutlineColor, Options.LyricNextOutlineColor,
          Song.Artist, Song.Title]);

  }

function TDataBaseSystem::GetSongOptions(Song: USong::TSong): TSongOptions;
var
  TableData: TSQLiteUniTable;
  SongOptions: TSongOptions;
{
  Result = nil;

  if ! ScoreDB then
    return;

  // Execute query
  try
    TableData = ScoreDB.GetUniTable("SELECT VideoRatioAspect, VideoWidth, VideoHeight, LyricPosition, LyricAlpha, " +
                                            "LyricSingFillColor, LyricActualFillColor, LyricNextFillColor," +
                                            "LyricSingOutlineColor, LyricActualOutlineColor, LyricNextOutlineColor " +
                                     "FROM [" + cUS_Songs + "] " +
                                     "WHERE [Artist] = ? && [Title] = ?",
                                     [Song.Artist, Song.Title]);
  except
    on E: Exception do
    {
      ULog::Log.LogError(E.Message, "TDataBaseSystem::GetSongOptions");
      return;
    }
  }

  if (TableData.EOF = false) then
  {
    SongOptions = TSongOptions.Create(TableData.FieldAsInteger(0), TableData.FieldAsInteger(1), TableData.FieldAsInteger(2), TableData.FieldAsInteger(3), TableData.FieldAsInteger(4),
      TableData.FieldAsString(5), TableData.FieldAsString(6), TableData.FieldAsString(7), TableData.FieldAsString(8), TableData.FieldAsString(9), TableData.FieldAsString(10));

    Result = SongOptions;
  end
  else
    Result = nil;

  TableData.Free;
}

}