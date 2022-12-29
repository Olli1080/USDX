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
#include <cstdint>
#include <vector>
#include <array>
#include <memory>

#include <gl/GL.h>

#include "ULog.h"
#include "UPathUtils.h"
#include "IniFileHelper.hpp"

namespace TextGL
{
/*uses
  dglOpenGL,
  sdl2,
  Classes,
  UTexture,
  UFont,
  UPath,
  ULog;
  */

struct TGLFont
{
	TScalableFont Font;
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

std::vector<std::vector<TGLFont>> Fonts; // 1. dimension: font family, 2. dimesion: font style (regular, bold, outline, boldhighres)
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

/*uses
  UTextEncoding,
  SysUtils,
  IniFiles,
  UCommon,
  UMain,
  UPathUtils;
  */
const std::array<std::string, 4> FONT_STYLES = {
	"Regular", "Bold", "Outline", "BoldHighRes"
};

/**
 * Returns either Filename if it is absolute or a path relative to FontPath.
 */
std::filesystem::path FindFontFile(const std::string Filename)
{
	auto tmp = UPathUtils::FontPath / Filename;
	// if path does not exist, try as an absolute path
	if (!std::filesystem::is_regular_file(tmp))
		return Filename;
}

/*
 * This part of the code is in his own void as we need the font family list
 * early on to read/write from the user"s configuration file in UIni.
 */
void LoadFontFamilyNames()
{
	CurrentFont.FontFamily = 0;
	CurrentFont.FontStyle = 0;
	const std::string prefix = "Font_";

	auto FontIni = TMemIniFile(UPathUtils::FontPath / "fonts.ini");

	// each section describes one font family
	FontIni.ReadSections(FontSections);

	// read font family names into FontFamilyNamesArray (to be used in ScreenOptionsLyrics)
	FontFamilyNames.resize(0);
	for (const auto& fontSection : FontSections)
	{
		if (fontSection.starts_with(prefix))
		{
			FontFamilyNames.emplace_back(
				FontIni.ReadString(fontSection, "Name", fontSection.substr(prefix.size()))
			);
		}
	}
	// set font array size: Fonts[available font families (defined in fonts.ini)][possible font styles (fixed, see FONT_STYLES)]
	Fonts = std::vector<std::vector<TGLFont>>(FontFamilyNames.size(), std::vector<TGLFont>(FONT_STYLES.size()));
	// close ini-file
}

void BuildFonts()
	/*var
	  FontNameIndex, FontStyleIndex, FallbackIndex: int;
	  FontIni: TMemIniFile;
	  FontFile: std::filesystem::path;
	  FontMaxResolution: Integer;
	  FontPreCache: Integer;
	  Outline: single;
	  Embolden: single;
	  OutlineFont: TFTScalableOutlineFont;
	  SectionName: string;*/
{
	auto FontIni = TMemIniFile(UPathUtils::FontPath / "fonts.ini");

	try
	{
		std::list<std::string>::iterator SectionName = FontSections.begin();
		for (int FontNameIndex = 0; FontNameIndex < FontSections.size(); ++FontNameIndex, ++SectionName)
		{
			for (int FontStyleIndex = 0; FontStyleIndex < FONT_STYLES.size(); ++FontStyleIndex)
			{
				//std::string SectionName =  FontSections[FontNameIndex];
				auto FontFile = FindFontFile(FontIni.ReadString(*SectionName, FONT_STYLES[FontStyleIndex] + "File", ""));
				if (FontFile.empty())
					continue;;

				int FontMaxResolution = FontIni.ReadInteger(*SectionName, FONT_STYLES[FontStyleIndex] + "MaxResolution", 64);
				float Embolden = FontIni.ReadFloat(*SectionName, FONT_STYLES[FontStyleIndex] + "Embolden", 0.0);
				float Outline = FontIni.ReadFloat(*SectionName, FONT_STYLES[FontStyleIndex] + "Outline", 0.0);
				int FontPreCache = FontIni.ReadInteger(*SectionName, FONT_STYLES[FontStyleIndex] + "PreCache", 1);

				if (FONT_STYLES[FontStyleIndex] != "Outline")
				{
					// normal (non-outlined) font
					Fonts[FontNameIndex][FontStyleIndex].Font = TFTScalableFont(
						FontFile,
						FontMaxResolution,
						Embolden,
						true,
						(FontPreCache != 0)
					);
					Fonts[FontNameIndex][FontStyleIndex].Outlined = false;
				}
				else
				{
					// outlined font
					auto OutlineFont = TFTScalableOutlineFont(
						FontFile,
						FontMaxResolution,
						Outline,
						true,
						(FontPreCache != 0)
					);
					OutlineFont.SetOutlineColor(
						FontIni.ReadFloat(*SectionName, "OutlineColorR", 0.0),
						FontIni.ReadFloat(*SectionName, "OutlineColorG", 0.0),
						FontIni.ReadFloat(*SectionName, "OutlineColorB", 0.0),
						FontIni.ReadFloat(*SectionName, "OutlineColorA", -1.0)
					);
					Fonts[FontNameIndex][FontStyleIndex].Font = OutlineFont;
					Fonts[FontNameIndex][FontStyleIndex].Outlined = true;
				}

				Fonts[FontNameIndex][FontStyleIndex].Font.GlyphSpacing = FontIni.ReadFloat(*SectionName, FONT_STYLES[FontStyleIndex] + "GlyphSpacing", 0.0);
				Fonts[FontNameIndex][FontStyleIndex].Font.Stretch = FontIni.ReadFloat(*SectionName, FONT_STYLES[FontStyleIndex] + "Stretch", 1.0);

				for (int FallbackIndex = 1; FallbackIndex <= 25; ++FallbackIndex)
				{
					FontFile = FindFontFile(FontIni.ReadString(*SectionName, FONT_STYLES[FontStyleIndex] + "FallbackFile" + std::to_string(FallbackIndex), ""));
					if (FontFile.empty())
						continue;
					try
					{
						Fonts[FontNameIndex][FontStyleIndex].Font.AddFallback(FontFile);
					}
					catch (const std::exception& e)
					{
						ULog::Log.LogError("Setting font fallback \"" + FontFile.string() + "\" failed: " + e.what());
					}
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		ULog::Log.LogCritical(e.what(), "BuildFont");
	}

	FontSections.resize(0);
}


// Deletes the font
void KillFonts()
/*var
  I, J: int;*/
{
	//TODO:: do we need this?
	/*
  for I = 0 to High(Fonts) do
    for J = 0 to High(Fonts[I]) do
      Fonts[I][J].Font.Free;
      */
}

double glTextWidth(const std::string text)
/*var
  Bounds: TBoundsDbl;*/
{
  auto Bounds = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.BBox(text, true);
  return Bounds.Right;
}

// Custom GL "Print" Routine
void glPrint(const std::string Text)
/*var
  GLFont: PGLFont;*/
{
  // if there is no text do nothing
  if (Text.empty()) 
    return;

  GLFont = @Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle];

  glPushMatrix();
    // set font position
    glTranslatef(GLFont.X, GLFont.Y + GLFont.Font.Ascender, GLFont.Z);
    // draw string
    GLFont.Font.Print(Text);
  glPopMatrix();
}

void ResetFont()
{
  SetFontPos(0, 0);
  SetFontZ(0);
  SetFontItalic(false);
  SetFontReflection(false, 0);
  SetOutlineColor(0,0,0,-1);
}

void SetFontPos(X, Y: double);
{
  Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].X = X;
  Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Y = Y;
}

void SetFontZ(Z: double);
{
  Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Z = Z;
}

void SetFontSize(Size: double);
{
  Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Height = Size;
}

void SetFontFamily(FontFamily: int);
{
  CurrentFont.FontFamily = FontFamily;
}

void SetFontStyle(FontStyle: int);
{
  CurrentFont.FontStyle = FontStyle;
}

void SetFont(Family, Style: int);
{
  SetFontFamily(Family);
  SetFontStyle(Style);
}

void SetFont(Font: TFont);
{
  SetFontFamily(Font.FontFamily);
  SetFontStyle(Font.FontStyle);
}

void SetFontItalic(Enable: bool);
{
  if (Enable) 
    Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Style = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Style + [Italic]
  else
    Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Style = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Style - [Italic]
}

void SetFontReflection(Enable: bool; Spacing: double);
{
  if (Enable) 
    Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Style = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Style + [Reflect]
  else
    Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Style = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Style - [Reflect];
  Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.ReflectionSpacing = Spacing - Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Descender;
}

void SetOutlineColor(R, G, B, A: GLFloat);
{
  if (Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Outlined) 
    TFTScalableOutlineFont(Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font).SetOutlineColor(R, G, B, A);
}

};