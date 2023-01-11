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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UMenuStatic.pas $
 * $Id: UMenuStatic.pas 1692 2009-04-24 18:43:12Z k-m_schindler $
 */
#include "../switches.h"

#include "../base/UTexture.h"
#include "UMenuInteract.hpp"

namespace UMenuStatic
{
    class TStatic
    {
    public:

	    UTexture::TextureWrapper::SPtr Texture; // Button Screen position and size
        bool Visible;

        // for list item
	    UTexture::TextureWrapper::SPtr TextureSelect;
	    UTexture::TextureWrapper::SPtr TextureDeselect; // Button Screen position and size

        //Reflection Mod
        bool Reflection;
        double Reflectionspacing;

        void Draw();
        TStatic(const UTexture::TextureWrapper::SPtr& Texture);
	    [[nodiscard]] std::optional<UMenuInteract::TMouseOverRect> GetMouseOverArea() const;
    };
}