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

#include "../base/types.hpp"

#include "UMenuBackground.hpp"
#include "../base/UTexture.h"

namespace UMenuBackgroundFade
{
/*uses
  UCommon,
  UThemes,
  UTexture,
  UMenuBackground,
  UPath;*/

//TMenuBackgroundFade - Background Fade In for Overlay screens
//--------

//type
    class TMenuBackgroundFade : public UMenuBackground::TMenuBackground
    {
    private:

	    UTexture::TextureWrapper::SPtr Tex;
	    UCommon::TRGBA Color;

        bool useTexture;

		std::chrono::system_clock::time_point FadeTimeStart;

    public:

        TMenuBackgroundFade(const UThemes::TThemeBackground ThemedSettings);
        ~TMenuBackgroundFade() override = default;

        void OnShow() override;
        void Draw() override;
    };

	constexpr auto FADEINTIME = std::chrono::milliseconds(1500); //Time the bg fades in
}