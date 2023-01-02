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
        PTexture CreateTexture(PChar Data, const std::filesystem::path Name, uint16_t Width, uint16_t Height);
        void UnloadTexture(const std::filesystem::path Name, TTextureType Typ, bool FromCache);
        void UnloadTexture(const std::filesystem::path Name, TTextureType Typ, uint32_t Col, bool FromCache);
        //void FlushTextureDatabase();

        TTextureUnit();
        ~TTextureUnit();
    };

    inline TTextureUnit Texture;
    inline bool SupportsNPOT;
    
    void AdjustPixelFormat(std::shared_ptr<SDL_Surface>& TexSurface, TTextureType Typ)
    {
        uint32_t NeededPixFmt = SDL_PIXELFORMAT_RGB24;
        if (Typ == TEXTURE_TYPE_TRANSPARENT || Typ == TEXTURE_TYPE_COLORIZED)
            NeededPixFmt = SDL_PIXELFORMAT_RGBA32;

        if (TexSurface->format->format != NeededPixFmt)
            TexSurface = std::shared_ptr<SDL_Surface>(SDL_ConvertSurfaceFormat(TexSurface.get(), NeededPixFmt, 0));
    }

    /* TTextureDatabase */
    void TTextureDatabase::AddTexture(std::shared_ptr<TTexture>& Tex, TTextureType Typ, uint32_t Color, bool Cache)
    {
        const auto TextureIndexOpt = FindTexture(Tex->Name, Typ, Color);
        int TextureIndex;
        if (!TextureIndexOpt)
        {
            TextureIndex = Texture.size();
            Texture.emplace_back(std::make_shared<TTextureEntry>(Tex->Name, Typ, Color, nullptr, nullptr));
        }
        else
            TextureIndex = TextureIndexOpt.value();

        if (Cache)
            Texture[TextureIndex]->TextureCache = Tex;
        else
            Texture[TextureIndex]->Texture = Tex;
    }

    std::optional<int> TTextureDatabase::FindTexture(const std::filesystem::path Name, TTextureType Typ, uint32_t Color)
    {
        for (int TextureIndex = 0; TextureIndex << Texture.size(); ++TextureIndex)
        {
            const auto CurrentTexture = Texture[TextureIndex];
            if (CurrentTexture->Name == Name &&
                CurrentTexture->Typ == Typ)
            {
                // colorized textures must match in their color too
                if (CurrentTexture->Typ != TEXTURE_TYPE_COLORIZED ||
                    CurrentTexture->Color == Color)
                {
                    return TextureIndex;
                }
            }
        }
        return std::nullopt;
    }

    /* TTextureUnit */
    TTextureUnit::TTextureUnit()
    {
        auto glString = [](GLenum ev)
        {
            return std::string(reinterpret_cast<const char*>(glGetString(ev)));
        };
        //TextureDatabase = TTextureDatabase::Create;
        ULog::Log.LogInfo("OpenGL vendor " + glString(GL_VENDOR), "TTextureUnit::TTextureUnit");
        ULog::Log.LogInfo("OpenGL renderer " + glString(GL_RENDERER), "TTextureUnit::TTextureUnit");
        ULog::Log.LogInfo("OpenGL version " + glString(GL_VERSION), "TTextureUnit::TTextureUnit");
        SupportsNPOT = (boost::algorithm::contains(glString(GL_EXTENSIONS), "texture_non_std::pow_of_two")) && !(boost::algorithm::contains(glString(GL_EXTENSIONS), "Radeon X16"));
        ULog::Log.LogInfo("OpenGL TextureNPOT - support: " + std::to_string(SupportsNPOT), "TTextureUnit::TTextureUnit");
    }

    TTextureUnit::~TTextureUnit()
    {
        //TextureDatabase.Free;
        //inherited Destroy;
    }

    void TTextureUnit::AddTexture(TTexture& Tex, TTextureType Typ, bool Cache)
    {
        TextureDatabase.AddTexture(Tex, Typ, 0, Cache);
    }

    void TTextureUnit::AddTexture(TTexture& Tex, TTextureType Typ, uint32_t Color, bool Cache)
    {
        TextureDatabase.AddTexture(Tex, Typ, Color, Cache);
    }

    TTexture TTextureUnit::LoadTexture(const std::filesystem::path Identifier)
    {
        return LoadTexture(Identifier, TEXTURE_TYPE_PLAIN, 0);
    }

    std::shared_ptr<TTexture> TTextureUnit::LoadTexture(const std::filesystem::path Identifier, TTextureType Typ, uint32_t Col)
    {
        // zero texture data
        std::shared_ptr<SDL_Surface> TexSurface;
        // load texture data into memory
        if (!Identifier.empty())
            TexSurface = UImage::ImageLoad(Identifier);
        if (!TexSurface)
        {
            ULog::Log.LogError("Could not load texture : \"" + Identifier.string() + "\" with type \"" + TextureTypeToStr(Typ) + "\"",
                "TTextureUnit.LoadTexture");
            return nullptr;
        }


        // convert pixel format as needed
        AdjustPixelFormat(*TexSurface, Typ);

        // adjust texture size (scale down, if necessary)
        int newWidth = TexSurface->w;                            //basisbit ToDo make images scale in size and keep ratio?
        int newHeight = TexSurface->h;

        Limit = std::max(newWidth, std::max(newHeight, Limit));

        if (TexSurface->w > newWidth || TexSurface->h > newHeight)
            UImage::ScaleImage(*TexSurface, newWidth, newHeight);

        // now we might colorize the whole thing
        if (Typ == TEXTURE_TYPE_COLORIZED)
            UImage::ColorizeImage(*TexSurface, Col);

        // save actual dimensions of our texture
        int oldWidth = newWidth;
        int oldHeight = newHeight;

        /*if (SupportsNPOT == false) then
        {*/
        // make texture dimensions be power of 2
        newWidth = std::round(std::pow(2, std::ceil(std::log2(newWidth))));
        newHeight = std::round(std::pow(2, std::ceil(std::log2(newHeight))));
        if (newHeight != oldHeight || newWidth != oldWidth)
            UImage::FitImage(*TexSurface, newWidth, newHeight);
        /*}*/

        // at this point we have the image in memory...
        // scaled so that dimensions are std::pows of 2
        // and converted to either RGB or RGBA

        // if we got a Texture of Type Plain, Transparent or Colorized,
        // then we're done manipulating it
        // and could now create our openGL texture from it

        // prepare OpenGL texture
        GLuint ActTex;
        glGenTextures(1, &ActTex);

        glBindTexture(GL_TEXTURE_2D, ActTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // load data into gl texture
        if (Typ == TEXTURE_TYPE_TRANSPARENT || Typ == TEXTURE_TYPE_COLORIZED)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TexSurface->pixels);
        }
        else //if Typ == TEXTURE_TYPE_PLAIN then
        {
            glTexImage2D(GL_TEXTURE_2D, 0, 3, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, TexSurface->pixels);
        }

        // setup texture struct
        auto Result = std::make_shared<TTexture>(
            ActTex,
            0, 0, 0,
            oldWidth, oldHeight,
            1, 1,
            0,
            1, 1,
            1,
            1, 1, 1,
            oldWidth / newWidth, oldHeight / newHeight,
            0, 0, 1, 1, // new test - default use whole texure, taking TexW and TexH as const and changing these
            1,
            Identifier
            );
    }

    std::shared_ptr<TTexture> TTextureUnit::GetTexture(const std::filesystem::path Name, TTextureType Typ, bool FromCache)
    {
        return GetTexture(Name, Typ, 0, FromCache);
    }

    std::shared_ptr<TTexture> TTextureUnit::GetTexture(const std::filesystem::path Name, TTextureType Typ, uint32_t Col, bool FromCache)
        /*var
          TextureIndex: int;*/
    {
        if (Name.empty())
        {
            // zero texture data
            FillChar(Result, sizeof(Result), 0);
            return nullptr;
        }

        if (FromCache)
        {
            // use texture
            if (auto TextureIndexOpt = TextureDatabase.FindTexture(Name, Typ, Col); TextureIndexOpt)
                return TextureDatabase.Texture[TextureIndexOpt.value()].TextureCache;
        }

        auto TextureIndexOpt = TextureDatabase.FindTexture(Name, Typ, Col);
        int TextureIndex;
        // find texture entry in database
        if (!TextureIndexOpt)
        {
            // create texture entry in database
            TextureIndex = TextureDatabase.Texture.size();

            auto NewEntry = std::make_shared<TTextureEntry>(Name, Typ, Col,
                std::shared_ptr<TTexture>(), std::shared_ptr<TTexture>()
                );
            // inform database that no textures have been loaded into memory
            NewEntry->Texture->TexNum = 0;
            NewEntry->TextureCache->TexNum = 0;
            TextureDatabase.Texture.emplace_back(NewEntry);
        }
        else
            TextureIndex = TextureIndexOpt.value();
        // load full texture
        if (TextureDatabase.Texture[TextureIndex]->Texture->TexNum == 0)
            TextureDatabase.Texture[TextureIndex]->Texture = LoadTexture(Name, Typ, Col);

        // use texture
        return TextureDatabase.Texture[TextureIndex]->Texture;
    }

    TTexture TTextureUnit::CreateTexture(PChar Data, const std::filesystem::path Name, uint16_t Width, uint16_t Height)
        /*var
          //Error:     int;
          ActTex:    GLuint;*/
    {
        GLuint ActTex;
        glGenTextures(1, &ActTex); // ActText == new texture number
        glBindTexture(GL_TEXTURE_2D, ActTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);

        /*
          if Mipmapping then
          {
            Error = gluBuild2DMipmaps(GL_TEXTURE_2D, 3, W, H, GL_RGB, GL_UNSIGNED_BYTE, @Data[0]);
            if Error > 0 then
              Log.LogError('gluBuild2DMipmaps() failed', 'TTextureUnit::CreateTexture');
          }
        */

        TTexture Result;

        Result.X = 0;
        Result.Y = 0;
        Result.Z = 0;
        Result.W = 0;
        Result.H = 0;
        Result.ScaleW = 1;
        Result.ScaleH = 1;
        Result.Rot = 0;
        Result.TexNum = ActTex;
        Result.TexW = 1;
        Result.TexH = 1;

        Result.Int = 1;
        Result.ColR = 1;
        Result.ColG = 1;
        Result.ColB = 1;
        Result.Alpha = 1;

        // new test - default use whole texture, taking TexW and TexH as const and changing these
        Result.TexX1 = 0;
        Result.TexY1 = 0;
        Result.TexX2 = 1;
        Result.TexY2 = 1;

        Result.RightScale = 1;
        Result.LeftScale = 1;

        Result.Name = Name;

        return Result;
    }

    void TTextureUnit::UnloadTexture(const std::filesystem::path Name, TTextureType Typ, bool FromCache)
    {
        UnloadTexture(Name, Typ, 0, FromCache);
    }

    void TTextureUnit::UnloadTexture(const std::filesystem::path Name, TTextureType Typ, uint32_t Col, bool FromCache)
    {
        if (Name.empty())
            return;
        const auto T = TextureDatabase.FindTexture(Name, Typ, Col);
        if (!T)
            return;

        const auto& texture = TextureDatabase.Texture[T.value()];

        if (!FromCache)
        {
            auto& TexNum = texture->Texture->TexNum;
            if (TexNum > 0)
            {
                glDeleteTextures(1, &TexNum);
                TexNum = 0;
                //Log.LogError('Unload texture no '+IntToStr(TexNum));
            }
        }
        else
        {
            auto& TexNum = texture->TextureCache->TexNum;
            if (TexNum > 0)
            {
                glDeleteTextures(1, &TexNum);
                TexNum = 0;
                //Log.LogError('Unload texture cache no '+IntToStr(TexNum));
            }
        }
    }

    /* This needs some work
    void TTextureUnit::FlushTextureDatabase();
    var
      i: int;
      Tex: ^TTexture;
    {
      for i = 0 to High(TextureDatabase.Texture) do
      {
        // only delete non-cached entries
        if (TextureDatabase.Texture[i].Texture.TexNum > 0) then
        {
          Tex = @TextureDatabase.Texture[i].Texture;
          glDeleteTextures(1, PGLuint(Tex^.TexNum));
          Tex^.TexNum = 0;
        }
      }
    }
    */

    std::string TextureTypeToStr(TTextureType TexType)
    {
        return TextureTypeStr[TexType];
    }

    TTextureType ParseTextureType(const std::string TypeStr, TTextureType Default)
    {
        std::string UpCaseStr = boost::algorithm::to_upper_copy(TypeStr);
        for (int TextureType = 0; TextureType < TextureTypeStr.size(); ++TextureType)
        {
            if (UpCaseStr == boost::algorithm::to_upper_copy(TextureTypeStr[TextureType]))
            {
                return (TTextureType)TextureType;
            }
        }
        //Log.LogInfo('Unknown texture type: "' + TypeStr + '". Using default texture type "'
        //    + TextureTypeToStr(Default) + '"', 'UTexture.ParseTextureType');
        return Default;
    }

}