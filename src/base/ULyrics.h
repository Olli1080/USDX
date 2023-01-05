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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/ULyrics.pas $
 * $Id: ULyrics.pas 1939 2009-11-09 00:27:55Z s_alexander $
 */
#include "../switches.h"

#include <string>
#include <memory>
#include <vector>

#include "UCommon.h"
#include "UIni.h"
#include "UTexture.h"
#include "UThemes.h"
#include "UMusic.h"

namespace ULyrics
{
    /*
    uses
        dglOpenGL,
        UCommon,
        UIni,
        UTexture,
        UThemes,
        UMusic;
        */

        // stores two textures for enabled/disabled states
    typedef std::array<UTexture::TTexture, 2> TPlayerIconTex;

    enum TLyricsEffect
    {
        lfxSimple, lfxZoom, lfxSlide, lfxBall, lfxShift
    };


    struct TLyricWord
    {
        double X;     // left corner
        double Width;     // width
        int32_t Start; // start of the word in quarters (beats)
        int32_t Length; // length of the word in quarters
        std::string Text; // text
        bool Freestyle;  // is freestyle?
    };
    typedef std::shared_ptr<TLyricWord> PLyricWord;
    typedef std::vector<TLyricWord> TLyricWordArray;

    class TLyricLine
    {
    public:
        std::string Text;   // text
        double Width;         // width
        double Height;         // height
        TLyricWordArray Words;   // words in this line
        int CurWord;      // current active word idx (only valid if line is active)
        int Start;      // start of this line in quarters (Note: negative start values are possible due to gap)
        int StartNote;      // start of the first note of this line in quarters
        int Length;      // length in quarters (from start of first to the end of the last note)
        uint8_t Players;         // players that should sing that line (bitset, Player1: 1, Player2: 2, Player3: 4)
        bool LastLine;      // is this the last line of the song?

        TLyricLine();
        ~TLyricLine();
        void Reset();
    };

    class TLyricEngine
    {
    private:
        double LastDrawBeat;
        TLyricLine UpperLine;    // first line displayed (top)
        TLyricLine LowerLine;    // second lind displayed (bottom)
        TLyricLine QueueLine;    // third line (will be displayed when lower line is finished)

        UTexture::TTexture IndicatorTex;      // texture for lyric indikator
        UTexture::TTexture BallTex;      // texture of the ball for the lyric effect

        bool QueueFull;       // set to true if the queue is full and a line will be replaced with the next AddLine
        int LCounter;       // line counter

        // duet mode - textures for player icons
        std::array<TPlayerIconTex, UIni::IMaxPlayerCount> PlayerIconTex;

        // Some helper voids for lyric drawing
        void DrawLyrics(double Beat);
        void UpdateLineMetrics(TLyricLine LyricLine);
        void DrawLyricsWords(TLyricLine LyricLine, double X, double Y, int StartWord, int EndWord);
        void DrawLyricsLine(double X, double W, double Y, double H, TLyricLine Line, double Beat);
        void DrawPlayerIcon(uint8_t Player, bool Enabled, double X, double Y, double Size, double Alpha);
        void DrawBall(double XBall, double YBall, double Alpha);

    public:
        // positions, line specific settings
        double UpperLineX;       // X start-pos of UpperLine
        double UpperLineW;       // Width of UpperLine with icon(s) and text
        double UpperLineY;       // Y start-pos of UpperLine
        double UpperLineH;       // Max. font-size of lyrics text in UpperLine

        double LowerLineX;       // X start-pos of LowerLine
        double LowerLineW;       // Width of LowerLine with icon(s) and text
        double LowerLineY;       // Y start-pos of LowerLine
        double LowerLineH;       // Max. font-size of lyrics text in LowerLine

        // display propertys
        UCommon::TRGBA LineColor_en;      // Color of words in an enabled line
        UCommon::TRGBA LineColor_dis;      // Color of words in a disabled line
        UCommon::TRGBA LineColor_act;      // Color of the active word

        /*{
        TRGBA LineOColor_en;      // Color of outline words in an enabled line
        TRGBA LineOColor_dis;      // Color of outline words in a disabled line
        TRGBA LineOColor_act;      // Color of outline the active word
        }*/

        uint8_t FontFamily;       // Font family for the lyric text
        uint8_t FontStyle;       // Font style for the lyric text

        /*{ // currently not used
        FadeInEffect:   uint8_t;       // Effect for line fading in: 0: No Effect; 1: Fade Effect; 2: Move Upwards from Bottom to Pos
        FadeOutEffect:  uint8_t;       // Effect for line fading out: 0: No Effect; 1: Fade Effect; 2: Move Upwards
        }*/

        // song specific settings
        double BPM;
        int Resolution;

        // properties to easily read options of this class
        [[nodiscard]] bool IsQueueFull() const { return QueueFull; }  // line in queue?
        [[nodiscard]] int LineCounter() const { return LCounter; }  // lines that were progressed so far (after last clear)

        void AddLine(UMusic::PLine Line);              // adds a line to the queue, if there is space
        void Draw(double Beat);                 // draw the current (active at beat) lyrics

        // clears all cached song specific information
        void Clear(double cBPM = 0, int cResolution = 0);

        [[nodiscard]] TLyricLine GetUpperLine() const;
        [[nodiscard]] TLyricLine GetLowerLine() const;

        [[nodiscard]] int GetUpperLineIndex() const;

        TLyricEngine(double ULX, double ULY, double ULW, double ULH, double LLX, double LLY, double LLW, double LLH);
        ~TLyricEngine();

        void LoadTextures();
    };
    /*
implementation

uses
SysUtils,
USkins,
TextGL,
UGraphic,
UDisplay,
ULog,
math;
*/
//{ TLyricLine }

}