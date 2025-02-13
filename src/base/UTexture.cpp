#include "UTexture.h"

#include <gl/glext.h>
//#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "ULog.h"
#include "UImage.h"

namespace UTexture
{
    void AdjustPixelFormat(USDX::SDL_Surface_SPtr& TexSurface, TTextureType Typ)
    {
        uint32_t NeededPixFmt = SDL_PIXELFORMAT_RGB24;
        if (Typ == TTextureType::TEXTURE_TYPE_TRANSPARENT || Typ == TTextureType::TEXTURE_TYPE_COLORIZED)
            NeededPixFmt = SDL_PIXELFORMAT_RGBA32;

        if (TexSurface->format->format != NeededPixFmt)
            TexSurface = USDX::make_shared(SDL_ConvertSurfaceFormat(TexSurface.get(), NeededPixFmt, 0));
    }

    /* TTextureDatabase */
    void TTextureDatabase::AddTexture(TextureWrapper::SPtr& Tex, TTextureType Typ, uint32_t Color, bool Cache)
    {
        auto maybeTexture = FindTexture(Tex->Name, Typ, Color);
        if (!maybeTexture)
        {
            auto id = TextureEntryID{ Tex->Name, Typ, Color };
            maybeTexture = Texture.emplace(id, std::make_shared<TTextureEntry>(id)).first->second;
        }
        if (Cache)
            maybeTexture->TextureCache = Tex;
        else
            maybeTexture->Texture = Tex;
    }

    PTextureEntry TTextureDatabase::FindTexture(const std::filesystem::path& Name, TTextureType Typ, uint32_t Color) const
    {
        const auto it = Texture.find({ Name, Typ, Color });
        return (it != Texture.end()) ? it->second : nullptr;
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

    void TTextureUnit::AddTexture(TextureWrapper::SPtr& Tex, TTextureType Typ, uint32_t Color, bool Cache)
    {
        TextureDatabase.AddTexture(Tex, Typ, Color, Cache);
    }

    TextureWrapper::SPtr TTextureUnit::LoadTexture(const std::filesystem::path& Identifier, TTextureType Typ, uint32_t Col)
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
        //basisbit ToDo make images scale in size and keep ratio?
        int newWidth = std::min(TexSurface->w, Limit);
        int newHeight = std::min(TexSurface->h, Limit);

        if (TexSurface->w > newWidth || TexSurface->h > newHeight)
            UImage::ScaleImage(*TexSurface, newWidth, newHeight);

        // now we might colorize the whole thing
        if (Typ == TTextureType::TEXTURE_TYPE_COLORIZED)
            UImage::ColorizeImage(*TexSurface, Col);

        // save actual dimensions of our texture
        const int oldWidth = newWidth;
        const int oldHeight = newHeight;

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
        if (Typ == TTextureType::TEXTURE_TYPE_TRANSPARENT || Typ == TTextureType::TEXTURE_TYPE_COLORIZED)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TexSurface->pixels);
        }
        else //if Typ == TEXTURE_TYPE_PLAIN then
        {
            glTexImage2D(GL_TEXTURE_2D, 0, 3, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, TexSurface->pixels);
        }

        // setup texture struct
        return std::make_shared<TextureWrapper>(ActTex, Identifier, 
            Size2D{ (double)oldWidth, (double)oldHeight },
            Size2D{ (double)newWidth, (double)newHeight });
    }

    TextureWrapper::SPtr TTextureUnit::GetTexture(const std::filesystem::path& Name, TTextureType Typ, uint32_t Col, bool FromCache)
    {
        if (Name.empty())
            return nullptr;

        if (FromCache)
        {
            // use texture
            if (const auto maybeTexture = TextureDatabase.FindTexture(Name, Typ, Col); maybeTexture)
                return maybeTexture->TextureCache;
        }

        // find texture entry in database
        auto maybeTexture = TextureDatabase.FindTexture(Name, Typ, Col);
        if (!maybeTexture)
        {
            auto id = TextureEntryID(Name, Typ, Col);
            maybeTexture = TextureDatabase.Texture.emplace(id, std::make_shared<TTextureEntry>(id)).first->second;
            maybeTexture->Texture = std::make_shared<TextureWrapper>(0, Name);
            maybeTexture->TextureCache = std::make_shared<TextureWrapper>(0, Name);
        }
        auto& texture = maybeTexture->Texture;
        if (texture->TexNum == 0)
            texture = LoadTexture(Name, Typ, Col);

        // use texture
        return texture;
    }

    TextureWrapper::SPtr TTextureUnit::CreateTexture(const std::vector<uint8_t>& Data, const std::filesystem::path& Name, uint16_t Width, uint16_t Height)
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

        return std::make_shared<TextureWrapper>(ActTex, Name);
    }

    void TTextureUnit::UnloadTexture(const std::filesystem::path& Name, TTextureType Typ, uint32_t Col, bool FromCache)
    {
        if (Name.empty())
            return;

        const auto maybeTexture = TextureDatabase.FindTexture(Name, Typ, Col);
        if (!maybeTexture)
            return;

        GLuint* TexNum;
        if (!FromCache)
            TexNum = &maybeTexture->Texture->TexNum;
        else
            TexNum = &maybeTexture->TextureCache->TexNum;
        
        if (TexNum <= 0)
            return;

        glDeleteTextures(1, TexNum);
        *TexNum = 0;
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
        return TextureTypeStr[static_cast<size_t>(TexType)];
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