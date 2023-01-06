#pragma once
/** UltraStar Deluxe - Karaoke Game
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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UImage.pas $
 * $Id: UImage.pas 1939 2009-11-09 00:27:55Z s_alexander $
 **/

#include "../switches.h"

#include <filesystem>
#include <cstdint>

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>

#include "SDL_Helper.hpp"

#define HaveBMP
#define HaveJPG

namespace UImage
{
    enum TImagePixelFmt
    {
        ipfRGBA, ipfRGB, ipfBGRA, ipfBGR
    };

    bool hasRGB(const SDL_PixelFormat& pixelFmt);
    bool hasBGR(const SDL_PixelFormat& pixelFmt);
    bool hasAlpha(const SDL_PixelFormat& pixelFmt);

    template<int N>
    bool hasChannels(const SDL_PixelFormat& pixelFmt);

    bool IsRGBSurface(const SDL_PixelFormat& pixelFmt);
    bool IsRGBASurface(const SDL_PixelFormat& pixelFmt);
    bool IsBGRSurface(const SDL_PixelFormat& pixelFmt);
    bool IsBGRASurface(const SDL_PixelFormat& pixelFmt);


    /*******************************************************
     * Image saving
     *******************************************************/
#ifdef HaveBMP
    bool WriteBMPImage(const std::filesystem::path FileName, const USDX::SDL_Surface_SPtr& Surface);
#endif
#ifdef HaveJPG
    bool WriteJPGImage(const std::filesystem::path FileName, const USDX::SDL_Surface_SPtr& Surface, int Quality);
#endif

    /*******************************************************
     * Image loading
     *******************************************************/

    std::shared_ptr<SDL_Surface> ImageLoad(const std::filesystem::path Filename);

    /*******************************************************
     * Image manipulation
     *******************************************************/

    bool PixelFormatEquals(SDL_PixelFormat fmt1, SDL_PixelFormat fmt2);
    void ScaleImage(SDL_Surface& ImgSurface, int Width, int Height);
    void FitImage(SDL_Surface& ImgSurface, int Width, int Height);
    void ColorizeImage(SDL_Surface& ImgSurface, uint32_t NewColor);
    uint16_t ColorToHue(uint8_t r, uint8_t g, uint8_t b);
    uint8_t ColorToGrayScale(uint8_t r, uint8_t g, uint8_t b);
}