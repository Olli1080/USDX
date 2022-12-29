#include "UDataBase.h"

namespace UDataBase
{
    TDataBaseSystem::TDataBaseSystem(std::filesystem::path Filename) :
        fFilename(Filename)
    {
        storage = std::make_unique<Storage>(createOrmStorage(fFilename));
    }

    /**
     * Open database && create tables if they do not exist
     */
    void TDataBaseSystem::Init(const std::filesystem::path Filename)
    {
        ULog::Log.LogStatus("Initializing database: \"" + Filename.string() + "\"", "TDataBaseSystem::Init");

        try
        {
            // open database
            fFilename = Filename;

            int Version = storage->pragma.user_version();

            // add Table cUS_Statistics_Info
            // needed in the conversion from 1.01 to 1.1
            bool statInfoExists = storage->table_exists(cUS_Statistics_Info);

            /*
            *  sync_schema to ensure that statinfo exists now!
            */
            storage->sync_schema();

            if (!statInfoExists)
                storage->insert<StatInfo>(StatInfo{ std::chrono::system_clock::now().time_since_epoch().count() });

            // convert data from 1.01 to 1.1
            // part #1 - prearrangement
            /*
            bool finalizeConversion = false;
            if (Version == 0 && storage->table_exists(cUS_Scores))
            {
              // rename old tables - to be able to insert new table structures
              ScoreDB->exec("ALTER TABLE US_Scores RENAME TO us_scores_101;");
              ScoreDB->exec("ALTER TABLE US_Songs RENAME TO us_songs_101;");
              finalizeConversion = true; // means: conversion has to be done!
            }
            */
            // Set version number after creation
            if (Version != cDBVersion)
                storage->pragma.user_version(cDBVersion);

            // convert data from previous versions
            // part #2 - accomplishment
            /*
            if (finalizeConversion)
            {
              //convert data from 1.01 to 1.1
              if (ScoreDB->tableExists("us_scores_101"))
                ConvertFrom101To110();
            }
            */
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogError(e.what(), "TDataBaseSystem::Init");;
            std::filesystem::remove(Filename);
            ULog::Log.LogCritical(e.what(), "TDataBaseSystem::Init");
        }
    }

    /**
     * Convert Database from 1.01 to 1.1
     */
     /*
     void TDataBaseSystem::ConvertFrom101To110()
     {
       auto columnsUSScores = getColumns("us_scores_101");
       if (std::find(columnsUSScores.begin(), columnsUSScores.end(), "Date") == columnsUSScores.end())
       {
         ULog::Log.LogInfo(
           std::string("Outdated song database found - ") +
           std::string("{ conversion from V1.01 to V1.1"), "TDataBaseSystem::Convert101To110");

         // insert old values into new db-schemes (/tables)
         ScoreDB->exec(
           "INSERT INTO " + cUS_Scores +
           " SELECT  SongID, Difficulty, Player, Score, ""NULL"" FROM us_scores_101;");
       }
       else
       {
         ULog::Log.LogInfo(
           std::string("Outdated song database  found - ") +
           std::string("{ conversion from V1.01 Challenge Mod to V1.1", "TDataBaseSystem::Convert101To110"));

         // insert old values into new db-schemes (/tables)
         ScoreDB->exec(
           "INSERT INTO " + cUS_Scores +
           " SELECT  SongID, Difficulty, Player, Score, Date FROM us_scores_101;");
       }
       ScoreDB->exec(
         "INSERT INTO " + cUS_Songs +
         " SELECT  ID, Artist, Title, TimesPlayed, ""NULL"" FROM us_songs_101;");

       /*
       // now we have to convert all the texts for unicode support:

       // player names
       TableData = nil;
       try
       {
         TableData = ScoreDB.GetUniTable(
           "SELECT [rowid], [Player] " +
           "FROM [" + cUS_Scores + "];");

         // Go through all Entrys
         while (!TableData.EOF) do
         {
           // Convert name into UTF8 && alter all entrys
           DecodeStringUTF8(TableData.FieldByName["Player"], tempUTF8String, encCP1252);
           ScoreDB->exec(
             "UPDATE [" + cUS_Scores + "] " +
             "SET [Player] = ? " +
             "WHERE [rowid] = ? ",
             [tempUTF8String,
             TableData.FieldAsInteger(TableData.FieldIndex["rowid"])]);

           TableData.Next;
         } // while
       }
       catch (const std::exception& e)
       {
         ULog::Log.LogError(e.what(), "TDataBaseSystem::Convert101To110");
       }

       TableData.Free;

       // song artist && song title
       TableData = nil;
       try
       {
         auto TableData = SQLite::Statement(*ScoreDB,
           std::string("SELECT [ID], [Artist], [Title] ") +
           "FROM [" + cUS_Songs + "];");

         // Go through all Entrys
         while (TableData.executeStep())
         {
           // Convert Artist into UTF8 && alter all entrys
           std::string artist = TableData.getColumn("Artist");
           //DecodeStringUTF8(, tempUTF8String, encCP1252);
           //ULog::Log.LogError(TableData.FieldByName["Artist"]+" -> "+tempUTF8String+" (encCP1252)");
           SQLite::Statement update(*ScoreDB,
             "UPDATE [" + cUS_Songs + "] " +
             "SET [Artist] = ? " +
             "WHERE [ID] = ?" );
           update.bind(1, artist);
           update.bind(2, TableData.getColumn("ID").getInt());

           // Convert Title into UTF8 && alter all entrys
           //DecodeStringUTF8(TableData.FieldByName["Title"], tempUTF8String, encCP1252);
           std::string title = TableData.getColumn("Title");
           SQLite::updateTitle
           ScoreDB->exec(
             "UPDATE [" + cUS_Songs + "] " +
             "SET [Title] = ? " +
             "WHERE [ID] = ? ",
             [tempUTF8String,
             TableData.FieldAsInteger(TableData.FieldIndex["ID"])]);

           TableData.Next;
         } // while
       }
       catch (const std::exception& e)
       {
         ULog::Log.LogError(e.what(), "TDataBaseSystem::Convert101To110");
       }
     */
     //now drop old tables
   //  ScoreDB->exec("DROP TABLE us_scores_101;");
   //  ScoreDB->exec("DROP TABLE us_songs_101;");
   //}

   /**
    * Frees Database
    */
    TDataBaseSystem::~TDataBaseSystem()
    {
        ULog::Log.LogInfo("TDataBaseSystem::Free", "TDataBaseSystem::Destroy");
        storage.reset();
    }

    /**
     * Format a UNIX-Timestamp into DATE (If 0 then "")
     */
    std::string TDataBaseSystem::FormatDate(const std::unique_ptr<std::chrono::system_clock::rep>& time_stamp)
        /*var
          Year, Month, Day: uint16_t;*/
    {
        if (!time_stamp)
            return "";

        using namespace std::chrono;

        return std::format(
            ULanguage::Language.Translate("STAT_FORMAT_DATE"),
            system_clock::time_point(system_clock::duration(*time_stamp))
        );

        //ULog::Log.LogError("Error Parsing FormatString \"STAT_FORMAT_DATE\": " + std::string(e.what()));
    }


    /**
     * Read Scores into SongArray
     */
    void TDataBaseSystem::ReadScore(USong::TSong& Song)
        /*var
          TableData:  TSQLiteUniTable;
          Difficulty: int;
          I: int;
          PlayerListed: boolean;*/
    {
        try
        {
            using namespace sqlite_orm;
            // Search Song in DB

            const auto songs = storage->prepare(
                select(columns(&DBScore::difficulty, &DBScore::player, &DBScore::score, &DBScore::date), from<DBScore>(),
                    where(c(&DBScore::songId) ==
                        select(&DBSong::id, from<DBSong>(),
                            where(c(&DBSong::artist) == Song.Artist && c(&DBSong::title) == Song.Title),
                            limit(1)
                        )
                        ),
                    order_by(&DBScore::score).desc() //no LIMIT! see filter below!
                ));

            // Empty Old Scores
            Song.Score[0].resize(0); //easy
            Song.Score[1].resize(0); //medium
            Song.Score[2].resize(0); //hard

            for (const auto& [Difficulty, Player, Score, Date] : storage->execute(songs))
            {
                // Add one Entry to Array
                if (Difficulty >= 0 && Difficulty <= 2 &&
                    Song.Score[Difficulty].size() < 5)
                {
                    //filter player
                    bool PlayerListed = false;
                    if (UIni::Ini.TopScores == 1)
                    {
                        if (!Song.Score[Difficulty].empty())
                        {
                            for (auto& diff : Song.Score[Difficulty])
                            {
                                if (diff.Name == Player)
                                {
                                    PlayerListed = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (!PlayerListed)
                    {
                        Song.Score[Difficulty].emplace_back(USong::TScore
                            {
                                Player, Score, FormatDate(Date)
                            });
                    }
                }
            }
        }
        catch (...)
        {
            for (int Difficulty = 0; Difficulty <= 2; ++Difficulty)
            {
                Song.Score[Difficulty].resize(1);
                Song.Score[Difficulty][0].Name = "Error Reading ScoreDB";
                //Song.Score[Difficulty, 1].Name = "Error Reading ScoreDB";
            }
        }
    }

    /**
     * Adds one new score to DB
     */
    void TDataBaseSystem::AddScore(USong::TSong Song, int Level, const std::string Name, int Score)
        /*var
          ID:        int;
          TableData: TSQLiteTable;*/
    {
        // Prevent 0 Scores from being added EDIT: ==> UScreenTop5.pas!
        //if (Score <= 0) then
        //  return;

        try
        {
            using namespace sqlite_orm;

            //There can be at most one id
            const auto ids = storage->select(&DBSong::id, from<DBSong>(),
                where(c(&DBSong::artist) == Song.Artist && c(&DBSong::title) == Song.Title)
            );

            int id;

            if (ids.empty())
            {
                // Create song if it does not exist
                storage->insert(into<DBSong>(), columns(&DBSong::artist, &DBSong::title),
                    values(Song.Artist, Song.Title));
                id = storage->last_insert_rowid();
            }
            else
            {
                id = ids.front();
            }
            // Create new entry
            storage->insert(into<DBScore>(), columns(&DBScore::songId, &DBScore::difficulty, &DBScore::player, &DBScore::score, &DBScore::date),
                values(id, Level, Name, Score, std::chrono::system_clock::now().time_since_epoch().count()));
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogError(e.what(), "TDataBaseSystem::AddScore");
        }
    }

    /*
     * Add Website
     */
    void TDataBaseSystem::AddWebsite()
        /*var
          I, WebID: Integer;*/
    {
        using namespace sqlite_orm;
        try
        {
            for (int i = 0; i < UDLLManager::DLLMan.Websites.size(); ++i)
            {
                // load functions
                UDLLManager::DLLMan.LoadWebsite(i);

                auto WebIDs = storage->select(&Web::id, from<Web>(),
                    where(c(&Web::name) == UDLLManager::DLLMan.Websites[i].Name));

                if (WebIDs.empty())
                {
                    // Create website if it does not exist
                    storage->insert(into<Web>(), columns(&Web::id, &Web::name),
                        values(nullptr, UDLLManager::DLLMan.Websites[i].Name));
                    UDLLManager::DLLMan.Websites[i].ID = storage->last_insert_rowid();
                }
                else
                {
                    UDLLManager::DLLMan.Websites[i].ID = WebIDs.front();
                }
            }
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogError(e.what(), "TDataBaseSystem::AddWebsite");
        }
    }

    /**
     * Read Users Info to Array
     */
    void TDataBaseSystem::ReadUsers()
    {
        using namespace sqlite_orm;
        //NetworkUser = {};
        NetworkUser.resize(UDLLManager::DLLMan.Websites.size());

        for (int i = 0; i < UDLLManager::DLLMan.Websites.size(); ++i)
            NetworkUser[i].Website = UDLLManager::DLLMan.Websites[i].Name;

        try
        {
            // Search User's in DB
            auto TableData_User = storage->get_all<UserInfo>(order_by(c(&UserInfo::username)));

            // Go through all Entries
            for (const auto& dataUser : TableData_User)
            {
                // Add one Entry to Array of WebSite->UserList

                for (auto& user : NetworkUser)
                {
                    auto WebIDs = storage->select(&Web::id, from<Web>(),
                        where(c(&Web::name) == user.Website));

                    if (!WebIDs.empty() && WebIDs.back() == dataUser.webID)
                    {
                        user.UserList.emplace_back(TUserInfo
                            {
                                dataUser.username,
                                dataUser.password,
                                dataUser.sendSavePlayer,
                                dataUser.autoMode,
                                dataUser.autoPlayer,
                                dataUser.autoScoreEasy,
                                dataUser.autoScoreMedium,
                                dataUser.autoScoreHard
                            });
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogError(e.what(), "TDataBaseSystem::ReadUser");
        }
    }

    /**
     * Save User to DB
     */
    void TDataBaseSystem::NewUser(std::string Website, std::string Username, std::string Password)
        /*var
          WebID: int;*/
    {
        using namespace sqlite_orm;
        try
        {
            const auto WebID = storage->select(&Web::id, from<Web>(),
                where(c(&Web::name) == Website)).at(0);

            // Insert New entries
            storage->insert(into<UserInfo>(), columns(&UserInfo::webID, &UserInfo::username, &UserInfo::password,
                values(WebID, Username, Password)));
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogError(e.what(), "TDataBaseSystem::NewUser");
        }
    }

    /**
     * Save Users Info to DB
     */
    void TDataBaseSystem::UpdateUsers()
    {
        using namespace sqlite_orm;
        try
        {
            for (const auto& netUser : NetworkUser)
            {
                for (const auto& uL : netUser.UserList)
                {
                    if (uL.Save == true)
                    {
                        const int WebID = storage->select(&Web::id, from<Web>(),
                            where(c(&Web::name) == netUser.Website)).at(0);

                        // Update entries
                        storage->update_all(
                            set(
                                c(&UserInfo::sendSavePlayer) = uL.SendSavePlayer,
                                c(&UserInfo::autoMode) = uL.AutoMode,
                                c(&UserInfo::autoPlayer) = uL.AutoPlayer,
                                c(&UserInfo::autoScoreEasy) = uL.AutoScoreEasy,
                                c(&UserInfo::autoScoreMedium) = uL.AutoScoreMedium,
                                c(&UserInfo::autoScoreHard) = uL.AutoScoreHard
                            ),
                            where(c(&UserInfo::webID) == WebID && c(&UserInfo::username) == uL.Username)
                        );
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogError(e.what(), "TDataBaseSystem::UpdateUser");
        }
    }

    /**
     * Delete User Info on DB
     */
    void TDataBaseSystem::DeleteUser(std::string Website, std::string Username)
        /*var
          WebID: int;*/
    {
        using namespace sqlite_orm;
        try
        {
            const int WebID = storage->select(&Web::id, from<Web>(),
                where(c(&Web::name) == Website)).at(0);

            storage->remove_all<UserInfo>(
                where(c(&UserInfo::webID) == WebID && c(&UserInfo::webID) == Username));
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogError(e.what(), "TDataBaseSystem::DeleteUser");
        }
    }
}