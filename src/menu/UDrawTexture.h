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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UDrawTexture.pas $
 * $Id: UDrawTexture.pas 1498 2008-11-03 14:53:17Z tobigun $
 */
#include "../switches.h"

#include "../base/UCommon.h"
#include "../base/UTexture.h"

namespace UDrawTexture
{
    void DrawLine(const UTexture::TTextureCoords& coords, const UCommon::TRGB<double>& Col);
    void DrawQuad(const UTexture::Position2D& position, const UTexture::Size& size, const UCommon::TRGB<double>& Col);
    void DrawTexture(const UTexture::TextureWrapper& texture);
}