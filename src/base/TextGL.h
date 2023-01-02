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
 * along with this program; see the file COPYING. If !, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/TextGL.pas $
 * $Id: TextGL.pas 2675 2010-10-17 17:00:23Z tobigun $
 */

#include "../switches.h"

#include <string>
#include <vector>
#include <array>
#include <memory>

#include <gl/GL.h>

#include "ULog.h"
#include "IniFileHelper.hpp"
#include "UFont.h"

namespace TextGL
{
struct TGLFont
{
	std::shared_ptr<UFont::TScalableFont> Font;
	bool Outlined;
	double X, Y, Z;
};
typedef std::shared_ptr<TGLFont> PGLFont;
struct TFont
{
	int FontFamily;
	int FontStyle;
};

const int ftRegular = 0;
const int ftBold = 1;
const int ftOutline = 2;
const int ftBoldHighRes = 3;
/*{ftNormal   == 0;
ftBold     == 1;
ftOutline1 == 2;
ftOutline2 == 3;
ftBoldHighRes == 4;}*/

std::vector<std::vector<PGLFont>> Fonts; // 1. dimension: font family, 2. dimesion: font style (regular, bold, outline, boldhighres)
TFont CurrentFont;
std::vector<std::string> FontFamilyNames;
std::list<std::string> FontSections;

void LoadFontFamilyNames();
void BuildFonts();                         // builds all fonts
void KillFonts();                          // deletes all font
double glTextWidth(const std::string text); // returns text width
void glPrint(const std::string text);    // custom GL "Print" routine
void ResetFont();                        // reset font settings of active font
void SetFontPos(double X, double Y);             // sets X and Y
void SetFontZ(double Z);                  // sets Z
void SetFontSize(double Size);
void SetFontFamily(int FontFamily); // sets active font family
void SetFontStyle(int FontStyle);   // sets active font style (regular, bold, outline, boldhighres)
void SetFont(int Family, int Style);   // sets active font (family + style)
void SetFont(TFont Font);              // sets active font (family + style)
void SetFontItalic(bool Enable);     // sets italic type letter (works for all fonts)
void SetFontReflection(bool Enable, double Spacing); // enables/disables text reflection
void SetOutlineColor(GLfloat R, GLfloat G, GLfloat B, GLfloat A); // set outline color

const std::array<std::string, 4> FONT_STYLES = {
	"Regular", "Bold", "Outline", "BoldHighRes"
};
};