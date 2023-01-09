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
 * $URL$
 * $Id$
 */
#include "../switches.h"

#include <sol/state.hpp>

#include "../base/TextGL.h"

namespace ULuaTextGL
{
	//{ TextGl.Pos(X, Y: Float) : sets font position }
	//{ TextGl.Size(Size: Float) : sets font size }
	//{ TextGl.Style(Style: int) : sets font style (from 0 to 3) }
	//{ TextGl.Italic(isItalic: boolean) : sets if font is italic }
	//{ TextGl.Width(Text: String) : returns width of Text if printed
	  //w/ current settings in pixels }

	//{ TextGl.Print(Text: String) : prints text to screen w/ current
	  //settings}

    inline void registerTextGl(sol::state& state)
    {
        state["TextGl"] = state.create_table_with
        (
            "Pos", TextGL::SetFontPos,
	        "Size", TextGL::SetFontSize,
            "Style", TextGL::SetFontStyle,
            "Italic", TextGL::SetFontItalic,
            "Width", TextGL::glTextWidth,
            "Print", TextGL::glPrint
        );
    }
}