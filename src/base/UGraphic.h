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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UGraphic.pas $
 * $Id: UGraphic.pas 2339 2010-05-05 07:22:17Z canni0 $
 */
#include "../switches.h"

#include <string>

#include <SDL2/SDL_video.h>

#include "types.hpp"
#include "UIni.h"
#include "UImage.h"

namespace UGraphic
{
    /*uses
      sdl2,
      dglOpenGL,
      UTexture,
      TextGL,
      UConfig,
      UCommon,
      ULog,
      UIni,
      SysUtils,
      UImage,
      UCatCovers,
      USongs,
      UAvatars,
      UCovers,
      UMusic,
      UScreenLoading,
      UScreenMain,
      UScreenName,
      UScreenLevel,
      UScreenOptions,
      UScreenOptionsGame,
      UScreenOptionsGraphics,
      UScreenOptionsSound,
      UScreenOptionsInput,
      UScreenOptionsLyrics,
      UScreenOptionsThemes,
      UScreenOptionsRecord,
      UScreenOptionsAdvanced,
      UScreenOptionsNetwork,
      UScreenOptionsWebcam,
      UScreenOptionsJukebox,
      UScreenSong,
      UScreenSingController,
      UScreenJukebox,
      UScreenJukeboxOptions,
      UScreenJukeboxPlaylist,
      UScreenScore,
      UScreenTop5,
      UScreenEditSub,
      UScreenEdit,
      UScreenEditConvert,
      UScreenOpen,
      UScreenAbout,
      USkins,
      UScreenSongMenu,
      UScreenSongJumpto,
      {Party Screens}
      UScreenPartyNewRound,
      UScreenPartyScore,
      UScreenPartyOptions,
      UScreenPartyWin,
      UScreenPartyPlayer,
      UScreenPartyRounds,
      UScreenPartyTournamentRounds,
      UScreenPartyTournamentPlayer,
      UScreenPartyTournamentOptions,
      UScreenPartyTournamentWin,
      {Stats Screens}
      UScreenStatMain,
      UScreenStatDetail,
      {CreditsScreen}
      UScreenCredits,
      {Popup for errors, etc.}
      UScreenPopup;*/

    struct TRecR
    {
        double Top;
        double Left;
        double Right;
        double Bottom;
    };

    enum class Mode //TODO:: redundant
    {
        WINDOWED,
        BORDERLESS,
        FULLSCREEN
    };

    typedef int FullscreenModes;

    struct WindowDeleter
    {
        void operator()(SDL_Window* window)
        {
            SDL_DestroyWindow(window);
        }
    };

    struct ContextDeleter
    {
	    void operator()(SDL_GLContext* ctx)
	    {
            SDL_GL_DeleteContext(ctx);
	    }
    };

    struct TScreen
    {
        Size2D<int> size;
        int screens;
        int Act;
        int X;
        std::unique_ptr<SDL_Window, WindowDeleter> window;
        std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, ContextDeleter> glcontext;

        void SetTitle(const std::string& str);
        void SetIcon(SDL_Surface& icon);
        void CreateContext();
    };    

    std::unique_ptr<std::thread> LoadingThread;
    std::mutex Mutex;

    FullscreenModes CurrentWindowMode;
    bool WindowModeDirty;

    Size2D<int> Render;
    TScreen screen;
    Rect<int> Last;
    bool HasValidPosition;
    bool HasValidSize;

    TScreenLoading ScreenLoading;
    TScreenMain ScreenMain;
    TScreenName ScreenName;
    TScreenLevel ScreenLevel;
    TScreenSong ScreenSong;
    TScreenSingController ScreenSing;

    TScreenJukebox ScreenJukebox;
    TScreenJukeboxOptions ScreenJukeboxOptions;
    TScreenJukeboxPlaylist ScreenJukeboxPlaylist;

    TScreenScore ScreenScore;
    TScreenTop5 ScreenTop5;
    TScreenOptions ScreenOptions;
    TScreenOptionsGame ScreenOptionsGame;
    TScreenOptionsGraphics ScreenOptionsGraphics;
    TScreenOptionsSound ScreenOptionsSound;
    TScreenOptionsInput ScreenOptionsInput;
    TScreenOptionsLyrics ScreenOptionsLyrics;
    TScreenOptionsThemes ScreenOptionsThemes;
    TScreenOptionsRecord ScreenOptionsRecord;
    TScreenOptionsAdvanced ScreenOptionsAdvanced;
    TScreenOptionsNetwork ScreenOptionsNetwork;
    TScreenOptionsWebcam ScreenOptionsWebcam;
    TScreenOptionsJukebox ScreenOptionsJukebox;
    TScreenEditSub ScreenEditSub;
    TScreenEdit ScreenEdit;
    TScreenEditConvert ScreenEditConvert;
    TScreenOpen ScreenOpen;
    TScreenAbout ScreenAbout;

    TScreenSongMenu ScreenSongMenu;
    TScreenSongJumpto ScreenSongJumpto;

    //Party Screens
    //TScreenSingModi ScreenSingModi;
    TScreenPartyNewRound ScreenPartyNewRound;
    TScreenPartyScore ScreenPartyScore;
    TScreenPartyWin ScreenPartyWin;
    TScreenPartyOptions ScreenPartyOptions;
    TScreenPartyPlayer ScreenPartyPlayer;
    TScreenPartyRounds ScreenPartyRounds;

    // Tournament
    TScreenPartyTournamentRounds ScreenPartyTournamentRounds;
    TScreenPartyTournamentPlayer ScreenPartyTournamentPlayer;
    TScreenPartyTournamentOptions ScreenPartyTournamentOptions;
    TScreenPartyTournamentWin ScreenPartyTournamentWin;

    //StatsScreens
    TScreenStatMain ScreenStatMain;
    TScreenStatDetail ScreenStatDetail;

    //CreditsScreen
    TScreenCredits ScreenCredits;

    //popup mod
    TScreenPopupCheck ScreenPopupCheck;
    TScreenPopupError ScreenPopupError;
    TScreenPopupInfo ScreenPopupInfo;
    TScreenPopupInsertUser ScreenPopupInsertUser;
    TScreenPopupSendScore ScreenPopupSendScore;
    TScreenPopupScoreDownload ScreenPopupScoreDownload;

    //popup help system
    TScreenPopupHelp ScreenPopupHelp;

    //Notes
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Left;   //rename to tex_note_left
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Mid;   //rename to tex_note_mid
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Right;   //rename to tex_note_right

    std::array<TTexture, UIni::IMaxPlayerCount> Tex_plain_Left;   //rename to tex_notebg_left
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_plain_Mid;   //rename to tex_notebg_mid
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_plain_Right;   //rename to tex_notebg_right

    std::array<TTexture, UIni::IMaxPlayerCount> Tex_BG_Left;   //rename to tex_noteglow_left
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_BG_Mid;   //rename to tex_noteglow_mid
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_BG_Right;   //rename to tex_noteglow_right

    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Left_Rap;   //rename to tex_note_left
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Mid_Rap;   //rename to tex_note_mid
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Right_Rap;   //rename to tex_note_right

    std::array<TTexture, UIni::IMaxPlayerCount> Tex_plain_Left_Rap;   //rename to tex_notebg_left
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_plain_Mid_Rap;   //rename to tex_notebg_mid
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_plain_Right_Rap;   //rename to tex_notebg_right

    std::array<TTexture, UIni::IMaxPlayerCount> Tex_BG_Left_Rap;   //rename to tex_noteglow_left
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_BG_Mid_Rap;   //rename to tex_noteglow_mid
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_BG_Right_Rap;   //rename to tex_noteglow_right

    TTexture Tex_Note_Star;
    TTexture Tex_Note_Perfect_Star;


    TTexture Tex_Ball;
    TTexture Tex_Lyric_Help_Bar;
    bool FullScreen;

    TTexture Tex_TimeProgress;
    TTexture Tex_JukeboxTimeProgress;

    //Sing Bar Mod
    TTexture Tex_SingBar_Back;
    TTexture Tex_SingBar_Bar;
    TTexture Tex_SingBar_Front;
    //end Singbar Mod

    //PhrasenBonus - Line Bonus Mod
    std::array<TTexture, 9> Tex_SingLineBonusBack;
    //End PhrasenBonus - Line Bonus Mod

    //ScoreBG Texs
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_ScoreBG;

    //Score Screen Textures
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Score_NoteBarLevel_Dark;
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Score_NoteBarRound_Dark;

    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Score_NoteBarLevel_Light;
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Score_NoteBarRound_Light;

    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Score_NoteBarLevel_Lightest;
    std::array<TTexture, UIni::IMaxPlayerCount> Tex_Score_NoteBarRound_Lightest;

    std::array<TTexture, 8> Tex_Score_Ratings;  //stores all possible rating result images

    // arrows for SelectSlide
    TTexture Tex_SelectS_ArrowL;
    TTexture Tex_SelectS_ArrowR;

    // textures for software mouse cursor
    TTexture Tex_Cursor_Unpressed;
    TTexture Tex_Cursor_Pressed;


    bool PboSupported;
    bool SoftwareRendering;

    const int Skin_BGColorR = 1;
    const int Skin_BGColorG = 1;
    const int Skin_BGColorB = 1;

    const int Skin_SpectrumR = 0;
    const int Skin_SpectrumG = 0;
    const int Skin_SpectrumB = 0;

    const double Skin_Spectograph1R = 0.6;
    const double Skin_Spectograph1G = 0.8;
    const double Skin_Spectograph1B = 1;

    const double Skin_Spectograph2R = 0;
    const double Skin_Spectograph2G = 0;
    const double Skin_Spectograph2B = 0.2;

    const double Skin_FontR = 0;
    const double Skin_FontG = 0;
    const double Skin_FontB = 0;

    const double Skin_FontHighlightR = 0.3; // 0.3
    const double Skin_FontHighlightG = 0.3; // 0.3
    const double Skin_FontHighlightB = 1;   // 1

    const double Skin_TimeR = 0.25; //0,0,0
    const double Skin_TimeG = 0.25;
    const double Skin_TimeB = 0.25;

    const double Skin_OscR = 0;
    const double Skin_OscG = 0;
    const double Skin_OscB = 0;

    const double Skin_SpectrumT = 470;
    const double Skin_SpectrumBot = 570;
    const double Skin_SpectrumH = 100;

    const double Skin_P1_LinesR = 0.5;  // 0.6 0.6 1
    const double Skin_P1_LinesG = 0.5;
    const double Skin_P1_LinesB = 0.5;

    const double Skin_P2_LinesR = 0.5; // 1 0.6 0.6
    const double Skin_P2_LinesG = 0.5;
    const double Skin_P2_LinesB = 0.5;

    const double Skin_P1_NotesB = 250;
    const double Skin_P2_NotesB = 430; // 430 / 300

    const double Skin_P1_ScoreT = 50;
    const double Skin_P1_ScoreL = 20;

    const double Skin_P2_ScoreT = 50;
    const double Skin_P2_ScoreL = 640;

    void Initialize3D(const std::string& Title);
    void Finalize3D();
    void Reinitialize3D();
    void SwapBuffers();

    void LoadTextures();
    void InitializeScreen();
    void LoadLoadingScreen();
    void LoadScreens(const std::string& Title);
    void UnloadScreens();

    int LoadingThreadFunction();

    void UpdateResolution();
    void UpdateVideoMode();

    void SetVideoMode(FullscreenModes Mode);
    FullscreenModes SwitchVideoMode(FullscreenModes Mode);
    bool HasWindowState(int Flag);

    // events
    void OnWindowMoved(int x, int y);
    void OnWindowResized(int w, int h);
}