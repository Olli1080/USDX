#include "UImage.h"

#include "ULog.h"

namespace UImage
{
    bool hasRGB(const SDL_PixelFormat& pixelFmt)
    {
        return (pixelFmt.Rmask == 0x0000FF) &&
            (pixelFmt.Gmask == 0x00FF00) &&
            (pixelFmt.Bmask == 0xFF0000);
    }

    bool hasBGR(const SDL_PixelFormat& pixelFmt)
    {
	    return (pixelFmt.Bmask == 0x0000FF) &&
            (pixelFmt.Gmask == 0x00FF00) &&
            (pixelFmt.Rmask == 0xFF0000);
    }

    bool hasAlpha(const SDL_PixelFormat& pixelFmt)
    {
        return pixelFmt.Amask == 0xFF000000;
    }

    template<int N>
    bool hasChannels(const SDL_PixelFormat& pixelFmt)
    {
        return pixelFmt.BytesPerPixel == N;
    }

    bool IsRGBSurface(const SDL_PixelFormat& pixelFmt)
    {
        return hasChannels<3>(pixelFmt) &&
            hasRGB(pixelFmt);
    }

    bool IsRGBASurface(const SDL_PixelFormat& pixelFmt)
    {
        return hasChannels<4>(pixelFmt) &&
            hasRGB(pixelFmt) &&
            hasAlpha(pixelFmt);
    }

    bool IsBGRSurface(const SDL_PixelFormat& pixelFmt)
    {
        return hasChannels<3>(pixelFmt) &&
            hasBGR(pixelFmt);
    }

    bool IsBGRASurface(const SDL_PixelFormat& pixelFmt)
    {
        return hasChannels<4>(pixelFmt) &&
            hasBGR(pixelFmt) &&
            hasAlpha(pixelFmt);
    }

    // Converts alpha-formats to BGRA, non-alpha to BGR, and leaves BGR(A) as is
    // sets converted to true if the surface needed to be converted
    USDX::SDL_Surface_SPtr ConvertToBGR_BGRASurface(USDX::SDL_Surface_SPtr& Surface, bool& Converted)
    {
        const SDL_PixelFormat& pixelFmt = *Surface->format;
        if (IsBGRSurface(pixelFmt) || IsBGRASurface(pixelFmt))
        {
            Converted = false;
            return Surface;
        }

        // invalid format -> needs conversion
        Converted = true;
        if (pixelFmt.Amask != 0)
            return USDX::make_shared(SDL_ConvertSurfaceFormat(Surface.get(), SDL_PIXELFORMAT_BGRA8888, 0));
        return USDX::make_shared(SDL_ConvertSurfaceFormat(Surface.get(), SDL_PIXELFORMAT_BGR24, 0));
    }

    // Converts alpha-formats to RGBA, non-alpha to RGB, and leaves RGB(A) as is
    // sets converted to true if the surface needed to be converted
    USDX::SDL_Surface_SPtr ConvertToRGB_RGBASurface(USDX::SDL_Surface_SPtr& Surface, bool& Converted)
    {
        const SDL_PixelFormat& pixelFmt = *Surface->format;
        if (IsRGBSurface(pixelFmt) || IsRGBASurface(pixelFmt))
        {
            Converted = false;
            return Surface;
        }

        // invalid format -> needs conversion
        Converted = true;
        if (pixelFmt.Amask != 0)
            return USDX::make_shared(SDL_ConvertSurfaceFormat(Surface.get(), SDL_PIXELFORMAT_ARGB8888, SDL_SWSURFACE));
        return USDX::make_shared(SDL_ConvertSurfaceFormat(Surface.get(), SDL_PIXELFORMAT_RGB24, SDL_SWSURFACE));
    }

    /*******************************************************
     * Image saving
     *******************************************************/

     /***************************
      * BMP section
      *****************************/

#ifdef HaveBMP
  /*
   * ImageData must be in BGR-format
   */
    bool WriteBMPImage(const std::filesystem::path FileName, const USDX::SDL_Surface_SPtr& Surface)
    {
        const int result = IMG_SavePNG(Surface.get(), FileName.string().c_str());

        if (result == 0)
            return true;

        ULog::Log.LogErrorLocation("Could not write file : \"" + FileName.string() + "\"");
        return false;
    }
#endif

#ifdef HaveJPG
    /***************************
     * JPG section
     *****************************/
    

    bool WriteJPGImage(const std::filesystem::path FileName, const USDX::SDL_Surface_SPtr& Surface, int Quality)
    {
        const int result = IMG_SaveJPG(Surface.get(), FileName.string().c_str(), Quality);

        if (result == 0)
            return true;

        ULog::Log.LogErrorLocation("Could not write file : \"" + FileName.string() + "\"");
        return false;
    }
#endif

    /*******************************************************
     * Image loading
     *******************************************************/

     /*
      * Loads an image from the given file
      */
    USDX::SDL_Surface_SPtr ImageLoad(const std::filesystem::path Filename)
        /*var
          FilenameCaseAdj: std::filesystem::path;*/
    {
        // try to adjust filename's case and check if it exists
        auto FilenameCaseAdj = Filename; //Filename.AdjustCase(false);
        if (!exists(FilenameCaseAdj) || !is_regular_file(FilenameCaseAdj))
        {
            ULog::Log.LogErrorLocation("Image - File does not exist \"" + FilenameCaseAdj.string() + "\"");
            return nullptr;
        }

        // load from file
        try
        {
            return USDX::make_shared(IMG_Load(FilenameCaseAdj.string().c_str())); //SDL2 uses wants UTF-8 strings according to documentation
            // Note: TBinaryFileStream is freed by SDLStream. SDLStream by IMG_Load_RW().
        }
        catch (...)
        {
            ULog::Log.LogErrorLocation("Could not load from file \"" + FilenameCaseAdj.string() + "\"");
            return nullptr;
        }
    }

    /*******************************************************
     * Image manipulation
     *******************************************************/

    bool PixelFormatEquals(SDL_PixelFormat fmt1, SDL_PixelFormat fmt2)
    {
        return
            (fmt1.BitsPerPixel == fmt2.BitsPerPixel) &&
            (fmt1.BytesPerPixel == fmt2.BytesPerPixel) &&
            (fmt1.Rloss == fmt2.Rloss) && (fmt1.Gloss == fmt2.Gloss) && (fmt1.Bloss == fmt2.Bloss) &&
            (fmt1.Rmask == fmt2.Rmask) && (fmt1.Gmask == fmt2.Gmask) && (fmt1.Bmask == fmt2.Bmask) &&
            (fmt1.Rshift == fmt2.Rshift) && (fmt1.Gshift == fmt2.Gshift) && (fmt1.Bshift == fmt2.Bshift);
    }

    void ScaleImage(SDL_Surface& ImgSurface, int Width, int Height)
    {
        const SDL_Rect origRect{ 0, 0, ImgSurface.w, ImgSurface.h };
        SDL_Rect newStretchRect{ 0, 0, Width, Height };
        
        SDL_UpperBlitScaled(&ImgSurface, &origRect, &ImgSurface, &newStretchRect);
    }

    void FitImage(USDX::SDL_Surface_SPtr& ImgSurface, int Width, int Height)
    {
        // create a new surface with given width and height
        auto ImgFmt = ImgSurface->format;
        auto NewImgSurface = USDX::make_shared
    	(
            SDL_CreateRGBSurface(
            SDL_SWSURFACE, Width, Height, ImgFmt->BitsPerPixel,
            ImgFmt->Rmask, ImgFmt->Gmask, ImgFmt->Bmask, ImgFmt->Amask)
        );

        // copy image from temp- to new surface
        SDL_SetSurfaceBlendMode(ImgSurface.get(), SDL_BLENDMODE_NONE);
        SDL_BlitSurface(ImgSurface.get(), nullptr, NewImgSurface.get(), nullptr);
    }

    uint16_t ColorToHue(uint8_t r, uint8_t g, uint8_t b)
    {
        // extract the colors
        // division by 255 is omitted, since it is implicitly done
        // when dividing by delta
        uint8_t Min = std::min(r, std::min(g, b));
        uint8_t Max = std::max(r, std::max(g, b));

        // calc hue
        uint8_t Delta = Max - Min;     // This gives a type size mismatch warning, because Delta is uint32_t, ie. >= 0
        // But the assignments above are easy enough to be sure, that Max - Min is >= 0.
        if (Delta == 0)
            return 0;

        // The division by Delta is done separately afterwards.
        // Necessary because Delphi did not do the type conversion from
        // uint32_t to double as expected.
        // After the change to long int, we may not need it, but left for now
        // Something to check
        double Hue;
        if (Max == r)
            Hue = g - b;
        else if (Max == g)
            Hue = 2.0 * Delta + b - r;
        else //if (Max == Blue)
            Hue = 4.0 * Delta + r - g;
        Hue /= Delta;
        if (Hue < 0.0)
            Hue += 6.0;
        return static_cast<uint16_t>(Hue * 1024.0);           // '*1024' //6 * 1024 == 0b0001'1000'0000'0000 uint16_t
    }

    uint8_t ColorToGrayScale(uint8_t r, uint8_t g, uint8_t b)
    {
	    return static_cast<uint8_t>(std::lround(0.299 * r + 0.587 * g + 0.114 * b));
    }

    void ColorizeImage(SDL_Surface& ImgSurface, uint32_t NewColor)

        // First, the rgb colors are converted to hsv, second hue is replaced by
        // the NewColor, saturation and value remain unchanged, finally this
        // hsv color is converted back to rgb space.
        // For the conversion algorithms of colors from rgb to hsv space
        // and back simply check the wikipedia.
        // In order to speed up starting time of USDX the division of reals is 
        // replaced by division of longints, shifted by 10 bits to keep 
        // digits.

        // The use of longwards leads to some type size mismatch warnings
        // whenever differences are formed.
        // This should not be a problem, since the results should all be positive.
        // replacing uint32_t by long int would probably resolve this cosmetic fault :-)

        /*var
          PixelIndex: uint32_t;
          Pixel: PByte;
          PixelColors: PByteArray;
          Red, Green, Blue: uint32_t;
          Hue, Sat: uint32_t;
          Min, Max, Delta: uint32_t;
          HueInteger: uint32_t;
          f, p, q, t: uint32_t;
          GreyReal: double;
          Grey: uint8_t;*/
    {
        // check of the size of a pixel in bytes.
        // It should be always 4, but this
        // additional safeguard will show,
        // whether something went wrong up to here.

        if (ImgSurface.format->BytesPerPixel != 4)
        {
            ULog::Log.LogError("ColorizeImage: The pixel size should be 4, but it is "
                + std::to_string(ImgSurface.format->BytesPerPixel));
        }
        auto Pixel = static_cast<uint8_t*>(ImgSurface.pixels);

        // Check whether the new color is white, grey or black, 
        // because a greyscale must be created in a different
        // way.

        const uint8_t RedNew = (NewColor & 0xff0000) >> 16; // R
        const uint8_t GreenNew = (NewColor & 0xff00) >> 8; // G
        const uint8_t BlueNew = NewColor & 0xff; // B

        if (RedNew == GreenNew && GreenNew == BlueNew) // greyscale image
        {
            // According to these recommendations (ITU-R BT.709-5)
            // the conversion parameters for rgb to greyscale are
            // 0.299, 0.587, 0.114
            for (size_t PixelIndex = 0; PixelIndex < ImgSurface.w * ImgSurface.h; ++PixelIndex, Pixel += 4)
            {
                //TODO:: check intention!
                const uint8_t Grey = ColorToGrayScale(Pixel[0], Pixel[1], Pixel[2]);
                Pixel[0] = Grey;
                Pixel[1] = Grey;
                Pixel[2] = Grey;
                //Pixel[3] is alpha and remains untouched
            }
            return; // we are done with a greyscale image.
        }

        const uint16_t Hue = ColorToHue(RedNew, GreenNew, BlueNew);   // Hue is << 10
        const uint16_t f = Hue & 0x3ff;           // f is the decimal part of hue
        const uint8_t HueInteger = Hue / 1024;

        for (size_t PixelIndex = 0; PixelIndex < ImgSurface.w * ImgSurface.h; ++PixelIndex, Pixel += 4)
        {
            // inlined colorize per pixel

            // uses fixed point math
            // << 10 is used for divisions

            // get color values

            uint8_t Red = Pixel[0];
            uint8_t Green = Pixel[1];
            uint8_t Blue = Pixel[2];
            //       Pixel[3] is alpha and remains untouched

            //calculate luminance and saturation from rgb

            const uint8_t Max = std::max(Red, std::max(Green, Blue));

            if (Max == 0)               // the color is black
            {
                Pixel[0] = 0;
                Pixel[1] = 0;
                Pixel[2] = 0;
            }
            else
            {
                const uint8_t Min = std::min(Red, std::min(Green, Blue));

                if (Min == 255)           // the color is white
                {
                    Pixel[0] = 255;
                    Pixel[1] = 255;
                    Pixel[2] = 255;
                }
                else                          // all colors except black and white
                {
                    const uint8_t Delta = Max - Min;         // This gives a type size mismatch warning, because Delta is uint8_t, ie. >= 0
                    // But the assignments above are easy enough to be sure, that Max - Min is >= 0.
                    const uint16_t Sat = (Delta * 1024) / Max;  // << 10

                    // >> 10 corrects that Sat and f are << 10
                    // the resulting p, q and t are unshifted

                    const uint8_t p = (Max * (1024 - Sat)) / 1024;
                    const uint8_t q = (Max * (1024 - (Sat * f) / 1024)) / 1024;
                    const uint8_t t = (Max * (1024 - (Sat * (1024 - f)) / 1024)) / 1024;

                    // The above 3 lines give type size mismatch warning, but all variables are uint32_t and the ranges should be ok.

                    switch (HueInteger)
                    {
                    case 0: { Red = Max; Green = t;   Blue = p; break; } // (v,t,p)
                    case 1: { Red = q;   Green = Max; Blue = p; break; } // (q,v,p)
                    case 2: { Red = p;   Green = Max; Blue = t; break; } // (p,v,t)
                    case 3: { Red = p;   Green = q;   Blue = Max; break; } // (p,q,v)
                    case 4: { Red = t;   Green = p;   Blue = Max; break; } // (t,p,v)
                    case 5: { Red = Max; Green = p;   Blue = q; break; } // (v,p,q)
                    }

                    Pixel[0] = Red;
                    Pixel[1] = Green;
                    Pixel[2] = Blue;
                }
            }
        }
    }
}