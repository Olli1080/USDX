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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UDisplay.pas $
 * $Id: UDisplay.pas 3150 2015-10-20 00:07:57Z basisbit $
 */
#include "../switches.h"

#include <string>
#include <array>
#include <gl/GL.h>

namespace UDisplay
{
    /*uses
      UCommon,
      Math,
      sdl2,
      dglOpenGL,
      SysUtils,
      UMenu,
      UPath,
      UMusic,
      UHookableEvent;*/

    class TDisplay
    {
    private:

        THookableEvent ePreDraw;
        THookableEvent eDraw;

        // fade-to-black
        bool BlackScreen;

        bool FadeEnabled;  // true if fading is enabled
        bool FadeFailed;  // true if fading is possible (enough memory, etc.)
        uint32_t FadeStartTime; // time when fading starts, 0 means that the fade texture must be initialized
        bool DoneOnShow;  // true if passed onShow after fading

        std::array<GLuint, 2> FadeTex;
        uint32_t TexW, TexH;

        uint32_t FPSCounter;
        uint32_t NextFPSSwap;

        std::string OSD_LastError;

        // software cursor data
        double Cursor_X;
        double Cursor_Y;
        bool Cursor_Pressed;

        // used for cursor fade out when there is no movement
        bool Cursor_Visible;
        std::chrono::system_clock::time_point Cursor_LastMove;
        bool Cursor_Fade;

        bool Cursor_Update;

        bool Console_Draw;
        int Console_ScrollOffset;
        void DrawDebugInformation();
        void DrawDebugConsole();

        // Handles parsing of inputs when console is opened. Called from ParseInput
        bool ConsoleParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown);
        // Handles parsing of inputs when console is opened. Called from ParseMouse
        bool ConsoleParseMouse(int MouseButton, bool BtnDown, int X, int Y);

        // called by MoveCursor and OnMouseButton to update last move and start fade in
        void UpdateCursorFade();

        static inline double CURSOR_ALPHA_VISIBLE = 0.7;

    public:

        bool Cursor_HiddenByScreen; // hides software cursor and deactivate auto fade in, must be public for access in UMenuButton

        PMenu NextScreen;
        PMenu CurrentScreen;

        // popup data
        Pmenu NextScreenWithCheck;
        bool CheckOK;

        // FIXME: Fade is set to 0 in UMain and other files but not used here anymore.
        double Fade;

        TDisplay();
        virtual ~TDisplay();

        void InitFadeTextures();

        void ToggleConsole();

        void SaveScreenShot();

        bool Draw();

        // TODO rewrite ParseInput to include handling/suppressing input as return, use KeepGoing as by-reference
        // checks if display is handling the passed key in ParseInput. calls HandleInput of cur or next Screen if assigned
        bool ShouldHandleInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown; bool& SuppressKey);

        // calls ParseInput of cur or next Screen if assigned
        bool ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown);

        // calls ParseMouse of cur or next Screen if assigned
        bool ParseMouse(int MouseButton, bool BtnDown, int X, int Y);

        // sets SDL_ShowCursor depending on options set in Ini
        void SetCursor();

        // called when cursor moves, positioning of software cursor
        void MoveCursor(double X, double Y);

        // called when left or right mousebutton is pressed or released 
        void OnMouseButton(bool Pressed);
        // fades to specific screen (playing specified sound)
        PMenu FadeTo(PMenu Screen, const TAudioPlaybackStream aSound = nullptr);

        // abort fading to the current screen, may be used in OnShow, or during fade process
        void AbortScreenChange();

        // draws software cursor
        void DrawCursor();

        // forces to update cursor (position and set visibility)
        void UpdateCursor();

        // returns whether this display is requesting an cursor update
        bool NeedsCursorUpdate();

        // called when the window has been resized
        virtual void OnWindowResized();

    };

    inline TDisplay Display;
    inline bool SupportsNPOT;
    uint32_t LastFPS;


    /* constants for screen transition
      time in milliseconds */
    constexpr auto FADE_DURATION = std::chrono::milliseconds(400);
    /* constants for software cursor effects
      time in milliseconds */
    constexpr auto CURSOR_FADE_IN_TIME = std::chrono::milliseconds(500);      // seconds the fade in effect lasts
    constexpr auto CURSOR_FADE_OUT_TIME = std::chrono::milliseconds(2000);    // seconds the fade out effect lasts
    constexpr auto CURSOR_AUTOHIDE_TIME = std::chrono::milliseconds(5000);   // seconds until auto fade out starts if there is no mouse movement
}