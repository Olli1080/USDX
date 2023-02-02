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
 * $URL: $
 * $Id: $
 */
#include <string>
#include <vector>

#include <SDL2/SDL_keycode.h>

#include "../menu/UMenu.h"

namespace UScreenJukeboxPlaylist
{
    /*uses
      UDisplay,
      UFiles,
      UMenu,
      UMusic,
      UNote,
      UThemes,
      sdl2,
      SysUtils;*/
    enum class PlayListSelection
    {
		ALL,
        CATEGORY,
        PLAYLIST,
        MANUAL
    };

    class TScreenJukeboxPlaylist : public UMenu::TMenu
    {
    private:

        uint32_t SelectPlayList;
        uint32_t SelectPlayList2;

        std::vector<std::string> IPlaylist;
        std::vector<std::string> IPlaylist2;

        PlayListSelection PlayList;
        int PlayList2;

        void SetPlaylists();

    public:

        TScreenJukeboxPlaylist();
        ~TScreenJukeboxPlaylist() override;

        bool ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown) override;
        void OnShow() override;
        void SetAnimationProgress(double Progress) override;

        void InitJukebox();
    };

    const std::string ID = "ID_040";   //for help system

    /*implementation

        uses
        UGraphic,
        UHelp,
        UMain,
        UIni,
        UTexture,
        ULanguage,
        ULog,
        UParty,
        USong,
        UPlaylist,
        USongs,
        UUnicodeUtils;*/

    bool TScreenJukeboxPlaylist::ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown)
        //var
          //  Report : string;
    //I: Integer;
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
            //Don'T start when Playlist is Selected and there are no Playlists
            if (Playlist == 3 && PlaylistMan.Playlists.empty())
                return true;

            try
            {
                InitJukebox();
            }
            catch (const std::exception& e)
            {
                std::string Report = "Starting jukebox failed.Most likely no folder / empty folder / playlist with not available songs was selected." + LineEnding +
                    "Stacktrace:" + LineEnding;
                if (E)
                {
                    Report += 'Exception class: ' + E.ClassName + LineEnding +
                        'Message: ' + e.what() + LineEnding;
                }
                Report += BackTraceStrFunc(ExceptAddr);
                for (I = 0 to ExceptFrameCount - 1)
                {
                    Report += LineEnding + BackTraceStrFunc(ExceptFrames[I]);
                }
                ULog::Log.LogWarnLocation(Report);
            }
            break;
        }
        // Up and Down could be done at the same time,
        // but I don't want to declare variables inside
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

            //Change Playlist2 if Playlist is Changed
            if (Interaction == SelectPlayList)
            {
                SetPlaylists();
            }
            break;
        }
        case SDLK_LEFT:
        {
            AudioPlayback.PlaySound(SoundLib.Option);
            InteractDec();

            //Change Playlist2 if Playlist is Changed
            if (Interaction == SelectPlayList)
            {
                SetPlaylists();
            }
            break;
        }
        }
        return true;
    }

    TScreenJukeboxPlaylist::TScreenJukeboxPlaylist()
    {
        inherited Create;

        //Clear all Selects
        PlayList = 0;
        PlayList2 = 0;

        // playlist modes
        IPlaylist2 = { "---" };
        IPlaylist = {
            Language.Translate('PARTY_PLAYLIST_ALL'),
            Language.Translate('PARTY_PLAYLIST_CATEGORY'),
            Language.Translate('PARTY_PLAYLIST_PLAYLIST'),
            Language.Translate('PARTY_PLAYLIST_MANUAL')
        };

        //Load Screen From Theme
        LoadFromTheme(Theme.JukeboxPlaylist);

        Theme.JukeboxPlaylist.SelectPlayList.oneItemOnly = true;
        Theme.JukeboxPlaylist.SelectPlayList.showArrows = true;
        SelectPlayList = AddSelectSlide(Theme.JukeboxPlaylist.SelectPlayList, PlayList, IPlaylist);

        Theme.JukeboxPlaylist.SelectPlayList2.oneItemOnly = true;
        Theme.JukeboxPlaylist.SelectPlayList2.showArrows = true;
        SelectPlayList2 = AddSelectSlide(Theme.JukeboxPlaylist.SelectPlayList2, PlayList2, IPlaylist2);

        Interaction = 0;
    }

    void TScreenJukeboxPlaylist::SetPlaylists()
    {
        switch (PlayList)
        {
        case PlayListSelection::ALL: [[fallthrough]]
        case PlayListSelection::MANUAL:
        {
            IPlaylist2 = { "---" };
            break;
        }
        case PlayListSelection::CATEGORY:
        {
            IPlaylist2.clear();
            for (const auto& song : CatSongs)
            {
                if (song.Main)
                    IPlaylist2.emplace_back(song.Artist);
            }

            if (IPlaylist2.empty())
                IPlaylist2 = { "No Categories found" };
            break;
        }
        case PlayListSelection::PLAYLIST:
        {
            if (!PlaylistMan.Playlists.empty())
            {
                SetLength(IPlaylist2, Length(PlaylistMan.Playlists));
                PlaylistMan.GetNames(IPlaylist2);
            }
            else
            {
                IPlaylist2 = { "No Play lists found" };
            }
            break;
        }
        }

        PlayList2 = 0;
        UpdateSelectSlideOptions(Theme.PartyOptions.SelectPlayList2, SelectPlayList2, IPlaylist2, PlayList2);
    }

    void TScreenJukeboxPlaylist::OnShow()
    {
        inherited;

        if (!Help.SetHelpID(ID))
            ULog::Log.LogError("No Entry for Help - ID " + ID + " (ScreenJukeboxPlaylist)");

    }

    void TScreenJukeboxPlaylist::InitJukebox()
    {
        ScreenSong.Mode = smJukebox;
        AudioPlayback.PlaySound(SoundLib.Start);

        SetLength(ScreenJukebox.JukeboxSongsList, 0);
        SetLength(ScreenJukebox.JukeboxVisibleSongs, 0);

        ScreenJukebox.ActualInteraction = 0;
        ScreenJukebox.CurrentSongList = 0;
        ScreenJukebox.ListMin = 0;
        ScreenJukebox.Interaction = 0;

        switch (PlayList)
        {
        case PlayListSelection::ALL:
            for (size_t I = 0; I < CatSongs.Song.size(); ++I)
            {
                if (!CatSongs.Song[I].Main)
                    ScreenJukebox.AddSongToJukeboxList(I);
            }
            ScreenJukebox.CurrentSongID = ScreenJukebox.JukeboxVisibleSongs[0];

            FadeTo(@ScreenJukebox);
            break;
        case PlayListSelection::CATEGORY:
            int J = -1;
            for (size_t I = 0; I < CatSongs.Song.size(); ++I)
            {
                if (CatSongs.Song[I].Main)
                    ++J;

                if (J == PlayList2)
                    ScreenJukebox.AddSongToJukeboxList(I);
            }
            ScreenJukebox.CurrentSongID = ScreenJukebox.JukeboxVisibleSongs[0];

            FadeTo(@ScreenJukebox);
            break;
        case PlayListSelection::PLAYLIST:
            const auto& PlayListItems = PlaylistMan.PlayLists[PlayList2].Items;

            if (!PlayListItems.empty())
            {
                for (size_t I = 0; I < PlayListItems.size(); ++I)
                    ScreenJukebox.AddSongToJukeboxList(PlayListItems[I].SongID);

                ScreenJukebox.CurrentSongID = ScreenJukebox.JukeboxVisibleSongs[0];

                FadeTo(@ScreenJukebox);
            }
            else
            {
                ULog::Log.LogWarnLocation("Can not play selected playlist in JukeBox because playlist is empty or no song found.");
            }
            break;
        case PlayListSelection::MANUAL:
            FadeTo(@ScreenSong);
            break;
        }
    }

    void TScreenJukeboxPlaylist::SetAnimationProgress(double Progress)
    {
        //for I := 0 to 6 do
        //  SelectS[I].Texture.ScaleW := Progress;
    }
}