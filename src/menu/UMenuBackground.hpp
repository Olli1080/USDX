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

namespace UThemes
{
	struct TThemeBackground;
}

namespace UMenuBackground
{
    /*uses
  SysUtils,
  UThemes;*/

  //TMenuBackground - abstraction class for MenuBackgrounds
  //this is a class, not an interface because of the constructors
  //and destructors
  //--------
  
        class EMenuBackgroundError : public std::exception {};
        class TMenuBackground
        {
        public:

            TMenuBackground() = default;
            virtual void OnShow() = 0;
            virtual void Draw() = 0;
            virtual void OnFinish() = 0;

            virtual ~TMenuBackground() = default;
        };
    //cMenuBackground = class of TMenuBackground;
}
