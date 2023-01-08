#include "UTexture.h"

#include <gl/glext.h>
//#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "ULog.h"
#include "UImage.h"

namespace UTexture
{
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
        size_t TextureIndex;
        if (const auto TextureIndexOpt = FindTexture(Tex->Name, Typ, Color); TextureIndexOpt)
            TextureIndex = TextureIndexOpt.value();
        else
        {
            TextureIndex = Texture.size();
            Texture.emplace_back(std::make_shared<TTextureEntry>(Tex->Name, Typ, Color, nullptr, nullptr));
        }
        if (Cache)
            Texture[TextureIndex]->TextureCache = Tex;
        else
            Texture[TextureIndex]->Texture = Tex;
    }

    std::optional<size_t> TTextureDatabase::FindTexture(const std::filesystem::path Name, TTextureType Typ, uint32_t Color) const
    {
        for (size_t TextureIndex = 0; TextureIndex << Texture.size(); ++TextureIndex)
        {
            const auto CurrentTexture = Texture[TextureIndex];
            if (
                CurrentTexture->Name == Name && CurrentTexture->Typ == Typ &&
                (
                    // colorized textures must match in their color too
                    CurrentTexture->Typ != TEXTURE_TYPE_COLORIZED ||
                    CurrentTexture->Color == Color)
                )
            {
                return TextureIndex;
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
        ULog::Log.LogInfoLocation("OpenGL vendor " + glString(GL_VENDOR));
        ULog::Log.LogInfoLocation("OpenGL renderer " + glString(GL_RENDERER));
        ULog::Log.LogInfoLocation("OpenGL version " + glString(GL_VERSION));
        SupportsNPOT = boost::algorithm::contains(glString(GL_EXTENSIONS), "GL_OES_texture_npot");
        ULog::Log.LogInfoLocation("OpenGL TextureNPOT - support: " + std::to_string(SupportsNPOT));
    }

    void TTextureUnit::AddTexture(PTexture& Tex, TTextureType Typ, bool Cache)
    {
        TextureDatabase.AddTexture(Tex, Typ, 0, Cache);
    }

    void TTextureUnit::AddTexture(PTexture& Tex, TTextureType Typ, uint32_t Color, bool Cache)
    {
        TextureDatabase.AddTexture(Tex, Typ, Color, Cache);
    }

    PTexture TTextureUnit::LoadTexture(const std::filesystem::path& Identifier)
    {
        return LoadTexture(Identifier, TEXTURE_TYPE_PLAIN, 0);
    }

    PTexture TTextureUnit::LoadTexture(const std::filesystem::path& Identifier, TTextureType Typ, uint32_t Col)
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
        newWidth = std::roundl(std::pow(2, std::ceil(std::log2(newWidth))));
        newHeight = std::roundl(std::pow(2, std::ceil(std::log2(newHeight))));
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
        return std::make_shared<TTexture>(
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

    PTexture TTextureUnit::GetTexture(const std::filesystem::path Name, TTextureType Typ, bool FromCache)
    {
        return GetTexture(Name, Typ, 0, FromCache);
    }

    PTexture TTextureUnit::GetTexture(const std::filesystem::path Name, TTextureType Typ, uint32_t Col, bool FromCache)
    {
        if (Name.empty())
            return nullptr;

        if (FromCache)
        {
            // use texture
            if (const auto TextureIndexOpt = TextureDatabase.FindTexture(Name, Typ, Col); TextureIndexOpt)
                return TextureDatabase.Texture[TextureIndexOpt.value()]->TextureCache;
        }

        size_t TextureIndex;
        // find texture entry in database
        if (const auto& TextureIndexOpt = TextureDatabase.FindTexture(Name, Typ, Col); TextureIndexOpt)
            TextureIndex = TextureIndexOpt.value();
        else
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
        auto& texture = TextureDatabase.Texture[TextureIndex]->Texture;
        // load full texture
        if (texture->TexNum == 0)
            texture = LoadTexture(Name, Typ, Col);

        // use texture
        return texture;
    }

    PTexture TTextureUnit::CreateTexture(const std::vector<uint8_t>& Data, const std::filesystem::path Name, uint16_t Width, uint16_t Height)
    {
        GLuint ActTex;
        glGenTextures(1, &ActTex); // ActText == new texture number
        glBindTexture(GL_TEXTURE_2D, ActTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data.data());

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

        return std::make_shared<TTexture>(std::move(Result));
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

        GLuint* TexNum;
        if (!FromCache)
            TexNum = &texture->Texture->TexNum;
        else
            TexNum = &texture->TextureCache->TexNum;
        
        if (*TexNum > 0)
        {
            glDeleteTextures(1, TexNum);
            *TexNum = 0;
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
        const std::string UpCaseStr = boost::algorithm::to_upper_copy(TypeStr);
        for (int TextureType = 0; TextureType < TextureTypeStr.size(); ++TextureType)
        {
            if (UpCaseStr == boost::algorithm::to_upper_copy(TextureTypeStr[TextureType]))
            {
                return static_cast<TTextureType>(TextureType);
            }
        }
        //Log.LogInfo('Unknown texture type: "' + TypeStr + '". Using default texture type "'
        //    + TextureTypeToStr(Default) + '"', 'UTexture.ParseTextureType');
        return Default;
    }
}