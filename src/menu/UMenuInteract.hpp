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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UMenuInteract.pas $
 * $Id: UMenuInteract.pas 1950 2009-11-18 14:42:34Z whiteshark0 $
 */
#include "../switches.h"

#include "../base/types.hpp"

namespace UMenuInteract
{
    struct TInteract // for moving through menu
    {
		int Typ;  // 0 - button, 1 - select, 2 - Text, 3 - Select SLide, 5 - ButtonCollection Child
		int Num;  // number of this item in proper list like buttons, selects
    };

    // to handle the area where the mouse is over a control
    typedef Rect<double> TMouseOverRect;
    /*{
        double X, Y;
        double W, H;
    };*/

    // to handle the on click action
    enum class TMouseClickAction { maNone, maReturn, maLeft, maRight };
}