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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UMenuText.pas $
 * $Id: UMenuText.pas 2293 2010-04-23 22:39:26Z tobigun $
 */
#include "../switches.h"

#include <string>
#include <vector>
#include <cstdint>

#include "UMenuInteract.hpp"

namespace UMenuText
{
/*uses
  math,
  SysUtils,
  dglOpenGL,
  sdl2,
  TextGL,
  UMenuInteract,
  UTexture;*/


	class TText
	{
	private:

		bool SelectBool;
		std::string TextString;
		std::vector<std::string> TextTiles;

		std::chrono::milliseconds STicks;
		bool SelectBlink;

	public:

		double X;
		double Y;
		double Z;
		double MoveX;       // some modifier for x - position that don't affect the double X
		double MoveY;       // some modifier for y - position that don't affect the double Y
		double W;       // text wider than W is wrapped
		double H;       // text higher than H is decreased in font size
		double Size;
		double ColR;
		double ColG;
		double ColB;

		double Alpha;
		double Int;
		int Font;
		int Style;
		bool Visible;
		int Align; // 0 = left, 1 = center, 2 = right

		// reflection
		bool Reflection;
		double ReflectionSpacing;

		bool Writable;

		void SetSelect(bool Value);
		//property Selected: bool read SelectBool write SetSelect;

		void SetText(const std::string& Value);
		//property  Text: UTF8String read TextString write SetText;

		void DeleteLastLetter(); //< Deletes the rightmost letter

		void Draw();

		TText();
		TText(double X, double Y, const std::string& Text);
		TText(double ParX, double ParY, double Par);
		TText(double ParX, double ParY, double ParW, double ParH, int ParFont, int ParStyle, double ParSize, double ParColR, double ParColG, double ParColB, int ParAlign, const std::string& ParText, bool ParReflection, double ParReflectionSpacing, double ParZ, bool Writable);

		UMenuInteract::TMouseOverRect GetMouseOverArea();
	};
}
