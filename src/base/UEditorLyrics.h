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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UEditorLyrics.pas $
 * $Id: UEditorLyrics.pas 2488 2010-06-11 15:54:50Z whiteshark0 $
 */

#include "../switches.h"

#include <string>
#include <vector>

#include "TextGL.h"

namespace UEditorLyrics
{
 /*   uses
        SysUtils,
        dglOpenGL,
        UMusic,
        UTexture;*/

    struct TWord
    {
        double X;
        double Y;
        double Width;
        std::string Text;
        double ColR;
        double ColG;
        double ColB;
    };

    class TEditorLyrics
    {
    private:
        int AlignI;
        double XR;
        double YR;
        double SizeR;
        int SelectedI;
        int FontFamilyI;         // font number
        int FontStyleI;         // font style number
        std::vector<TWord> Words;
        
        void SetAlign(int Value);
        [[nodiscard]] double GetSize() const;
        void SetSize(double Value);
        void SetSelected(int Value);
        void AddWord(std::string Text);
        void Refresh();
    public:
        double DColR;
        double DColG;
        double DColB;
        double ColR;
        double ColG;
        double ColB;
        bool Italic;

        void AddLine(int CurrentTrack, int CurrentLine);

        void Clear();
        void Draw();

        void X(double Value);
        void Y(double Value);
        [[nodiscard]] double ClientX() const;
        void Align(int Value);

        void Size(double size) { SetSize(size); }
        [[nodiscard]] double Size() const { return GetSize(); }

        void Selected(int select) { SetSelected(select); }
        [[nodiscard]] int Selected() const { return SelectedI; }

        void FontFamily(int Value);
        void FontStyle(int Value);
    };
}
