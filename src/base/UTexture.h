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
#include <map>
#include <filesystem>
#include <memory>
#include <cstdint>

#include <GL/gl.h>
#include <SDL2/SDL_surface.h>
#include <boost/algorithm/string/predicate.hpp>

#include "UCommon.h"
#include "types.hpp"

namespace UTexture
{
    enum class TTextureType
    {
        TEXTURE_TYPE_PLAIN,        // Plain (alpha == 1)
        TEXTURE_TYPE_TRANSPARENT,  // Alpha is used
        TEXTURE_TYPE_COLORIZED     // Alpha is used; Hue of the HSV color-model will be replaced by a new value
    };
    const std::array<std::string, 3> TextureTypeStr = { "Plain", "Transparent", "Colorized" };

    std::string TextureTypeToStr(TTextureType TexType);
    TTextureType ParseTextureType(const std::string& TypeStr, TTextureType Default);

    struct SizeScale
    {
        double W = 1.0; // for dynamic scaling while leaving width constant 
        double H = 1.0; // for dynamic scaling while leaving height constant
    };

    struct ExtentScale
    {
        double right = 1.0;
        double left = 1.0;
    };

    struct TTextureCoords
    {
        double X1 = 0.0;
        double Y1 = 0.0;
        double X2 = 1.0;
        double Y2 = 1.0;
    };

    struct SizeRatio
    {
        double W = 1.0; // percentage of width to use [0..1]
        double H = 1.0; // percentage of height to use [0..1]
    };

    struct TTexture
    {
        Position2D position;
        double Z;
        Size2D size;
        UCommon::TRGBA Color = {{1.0, 1.0, 1.0}, 1.0 };
        TTextureCoords coords;

        typedef std::shared_ptr<TTexture> SPtr;
    };

    /*struct TextureID
    {
        std::filesystem::path Name; // experimental for handling cache images. maybe it's useful for dynamic skins
        TTextureType Typ;

    };*/

    struct TextureWrapper : TTexture
    {
        TextureWrapper(GLuint TexNum, std::filesystem::path name)
	        : TexNum(TexNum), Name(std::move(name))
        {}

        TextureWrapper(GLuint TexNum, std::filesystem::path name, Size2D actualSize, Size2D scaledSize)
	        : TexNum(TexNum), Name(std::move(name))
        {
            size = actualSize;
            ratio = { size.W / scaledSize.W, size.H / scaledSize.H };
        }

        GLuint TexNum;
        SizeScale sizeScale;
        double Rot = 0.0; // 0 - 2*pi
        ExtentScale extentScale;
        double Int = 1.0; // intensity
        
        SizeRatio ratio;
        std::filesystem::path Name; // experimental for handling cache images. maybe it's useful for dynamic skins

        typedef std::shared_ptr<TextureWrapper> SPtr;
    };

    //typedef std::shared_ptr<TTexture> PTexture;
    void AdjustPixelFormat(SDL_Surface& TexSurface, TTextureType Typ);

    struct TextureEntryID
    {
        std::filesystem::path Name;
        TTextureType Typ;
        uint32_t Color;

        bool operator<(const TextureEntryID& rhs) const
        {
            if (Name > rhs.Name)
                return false;
            if (static_cast<size_t>(Typ) > static_cast<size_t>(rhs.Typ))
                return false;
            if (Typ == TTextureType::TEXTURE_TYPE_COLORIZED && Color > rhs.Color)
                return false;
            return true;
        }
    };

    struct TTextureEntry : TextureEntryID
    {
        TTextureEntry(TextureEntryID id) : TextureEntryID(std::move(id))
        {}

        //[[nodiscard]] std::filesystem::path Name() const { return Texture->Name; };

        // we use normal TTexture, it's easier to implement and if needed - we copy ready data
        TextureWrapper::SPtr Texture = nullptr; // Full-size texture
        TextureWrapper::SPtr TextureCache = nullptr; // Thumbnail texture
    };
    typedef std::shared_ptr<TTextureEntry> PTextureEntry;

    class TTextureDatabase
    {
    private:

        friend class TTextureUnit;

        //std::vector<PTextureEntry> Texture;
        std::map<TextureEntryID, PTextureEntry> Texture;

    public:

        void AddTexture(TextureWrapper::SPtr& Tex, TTextureType Typ, uint32_t Color, bool Cache);
        [[nodiscard]] PTextureEntry FindTexture(const std::filesystem::path& Name, TTextureType Typ, uint32_t Color) const;
    };

    class TTextureUnit
    {
    private:

        TTextureDatabase TextureDatabase;

    public:

        int Limit;
        
        void AddTexture(TextureWrapper::SPtr& Tex, TTextureType Typ, uint32_t Color = 0, bool Cache = false);
        TextureWrapper::SPtr GetTexture(const std::filesystem::path& Name, TTextureType Typ, uint32_t Col = 0, bool FromCache = false);
        TextureWrapper::SPtr LoadTexture(const std::filesystem::path& Identifier, TTextureType Typ = TTextureType::TEXTURE_TYPE_PLAIN, uint32_t Col = 0);
        void UnloadTexture(const std::filesystem::path& Name, TTextureType Typ, uint32_t Col = 0, bool FromCache = false);

        TextureWrapper::SPtr CreateTexture(const std::vector<uint8_t>& Data, const std::filesystem::path& Name, uint16_t Width, uint16_t Height);
        //void FlushTextureDatabase();

        TTextureUnit();
    };

    inline TTextureUnit Texture;
    inline bool SupportsNPOT;
}
