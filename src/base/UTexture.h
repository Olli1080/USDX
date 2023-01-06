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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/UTexture.pas $
 * $Id: UTexture.pas 3150 2015-10-20 00:07:57Z basisbit $
 **/

#define NOMINMAX

#include "../switches.h"

#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <memory>
#include <cstdint>

#include <gl/GL.h>
#include <gl/glext.h>
#include <SDL2/SDL_surface.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>


#include "ULog.h"
#include "UImage.h"

namespace UTexture
{
    struct TTextureCoords
    {
        double X1;
        double Y1;
        double X2;
        double Y2;
    };

    struct TTexture
    {
        GLuint TexNum;
        double X;
        double Y;
        double Z;
        double W;
        double H;
        double ScaleW; // for dynamic scaling while leaving width constant
        double ScaleH; // for dynamic scaling while leaving height constant
        double Rot; // 0 - 2*pi
        double RightScale; //
        double LeftScale; //
        double Int; // intensity
        double ColR;
        double ColG;
        double ColB;
        double TexW; // percentage of width to use [0..1]
        double TexH; // percentage of height to use [0..1]
        double TexX1;
        double TexY1;
        double TexX2;
        double TexY2;
        double Alpha;
        std::filesystem::path Name; // experimental for handling cache images. maybe it's useful for dynamic skins
    };

    typedef std::shared_ptr<TTexture> PTexture;


    enum TTextureType
    {
        TEXTURE_TYPE_PLAIN,        // Plain (alpha == 1)
        TEXTURE_TYPE_TRANSPARENT,  // Alpha is used
        TEXTURE_TYPE_COLORIZED     // Alpha is used; Hue of the HSV color-model will be replaced by a new value
    };

    const std::array<std::string, 3> TextureTypeStr = { "Plain", "Transparent", "Colorized" };

    std::string TextureTypeToStr(TTextureType TexType);
    TTextureType ParseTextureType(const std::string TypeStr, TTextureType Default);

    void AdjustPixelFormat(SDL_Surface& TexSurface, TTextureType Typ);



    struct TTextureEntry
    {
        std::filesystem::path Name;
        TTextureType Typ;
        uint32_t Color;

        // we use normal TTexture, it's easier to implement and if needed - we copy ready data
        PTexture Texture; // Full-size texture
        PTexture TextureCache; // Thumbnail texture
    };
    typedef std::shared_ptr<TTextureEntry> PTextureEntry;

    class TTextureDatabase
    {
    private:
        friend class TTextureUnit;

        std::vector<PTextureEntry> Texture;
    public:
        void AddTexture(PTexture& Tex, TTextureType Typ, uint32_t Color, bool Cache);
        std::optional<int> FindTexture(const std::filesystem::path Name, TTextureType Typ, uint32_t Color);
    };

    class TTextureUnit
    {
    private:

        TTextureDatabase TextureDatabase;

    public:

        int Limit;

        void AddTexture(PTexture& Tex, TTextureType Typ, bool Cache = false);
        void AddTexture(PTexture& Tex, TTextureType Typ, uint32_t Color, bool Cache = false);
        PTexture GetTexture(const std::filesystem::path Name, TTextureType Typ, bool FromCache = false);
        PTexture GetTexture(const std::filesystem::path Name, TTextureType Typ, uint32_t Col, bool FromCache = false);
        PTexture LoadTexture(const std::filesystem::path Identifier, TTextureType Typ, uint32_t Col);
        PTexture LoadTexture(const std::filesystem::path Identifier);
        PTexture CreateTexture(const std::vector<uint8_t>& Data, const std::filesystem::path Name, uint16_t Width, uint16_t Height);
        void UnloadTexture(const std::filesystem::path Name, TTextureType Typ, bool FromCache);
        void UnloadTexture(const std::filesystem::path Name, TTextureType Typ, uint32_t Col, bool FromCache);
        //void FlushTextureDatabase();

        TTextureUnit();
    };

    inline TTextureUnit Texture;
    inline bool SupportsNPOT;
}