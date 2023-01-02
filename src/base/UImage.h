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

#include <array>
#include <filesystem>
#include <cstdint>

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>

#include "ULog.h"

namespace UImage
{
    /*uses
      sdl2,
      UPath;*/

#define HaveBMP
#define HaveJPG
      //const
        //PixelFmt_RGBA
        //PixelFmt_RGB
        //PixelFmt_BGRA
        //PixelFmt_BGR

    enum TImagePixelFmt
    {
        ipfRGBA, ipfRGB, ipfBGRA, ipfBGR
    };

    /*******************************************************
     * Image saving
     *******************************************************/
#ifdef HaveBMP
    bool WriteBMPImage(const std::filesystem::path FileName, SDL_Surface Surface);
#endif
#ifdef HaveJPG
    bool WriteJPGImage(const std::filesystem::path FileName, SDL_Surface Surface, int Quality);
#endif

    /*******************************************************
     * Image loading
     *******************************************************/

    std::shared_ptr<SDL_Surface> ImageLoad(const std::filesystem::path Filename);

    /*******************************************************
     * Image manipulation
     *******************************************************/

    bool PixelFormatEquals(SDL_PixelFormat fmt1, SDL_PixelFormat fmt2);
    void ScaleImage(SDL_Surface& ImgSurface, uint32_t Width, uint32_t Height);
    void FitImage(SDL_Surface& ImgSurface, uint32_t Width, uint32_t Height);
    void ColorizeImage(SDL_Surface& ImgSurface, uint32_t NewColor);

    /*uses
      SysUtils,
      Classes,
      Math,*/
      /*$IFDEF MSWINDOWS*/
      //Windows,
      /*$ENDIF*/
      /*$IFDEF HaveJPG*/
        /*$IFDEF Delphi*/
        //Graphics,
        //jpeg,
        /*$ELSE*/
        /*jpeglib,
        jerror,
        jcparam,
        jdatadst, jcapimin, jcapistd,*/
        /*$ENDIF*/
      /*$ENDIF*/
      /*zlib,
      SDL2_image,
      UCommon,
      ULog;*/

    bool IsRGBSurface(SDL_PixelFormat pixelFmt)
    {
        return (pixelFmt.BitsPerPixel == 24) &&
            (pixelFmt.Rmask == 0x0000FF) &&
            (pixelFmt.Gmask == 0x00FF00) &&
            (pixelFmt.Bmask == 0xFF0000);
    }

    bool IsRGBASurface(SDL_PixelFormat pixelFmt)
    {
        return (pixelFmt.BitsPerPixel == 32) &&
            (pixelFmt.Rmask == 0x000000FF) &&
            (pixelFmt.Gmask == 0x0000FF00) &&
            (pixelFmt.Bmask == 0x00FF0000) &&
            (pixelFmt.Amask == 0xFF000000);
    }

    bool IsBGRSurface(SDL_PixelFormat pixelFmt)
    {
        return (pixelFmt.BitsPerPixel == 24) &&
            (pixelFmt.Bmask == 0x0000FF) &&
            (pixelFmt.Gmask == 0x00FF00) &&
            (pixelFmt.Rmask == 0xFF0000);
    }

    bool IsBGRASurface(SDL_PixelFormat pixelFmt)
    {
        return (pixelFmt.BitsPerPixel == 32) &&
            (pixelFmt.Bmask == 0x000000FF) &&
            (pixelFmt.Gmask == 0x0000FF00) &&
            (pixelFmt.Rmask == 0x00FF0000) &&
            (pixelFmt.Amask == 0xFF000000);
    }

    // Converts alpha-formats to BGRA, non-alpha to BGR, and leaves BGR(A) as is
    // sets converted to true if the surface needed to be converted
    std::shared_ptr<SDL_Surface> ConvertToBGR_BGRASurface(std::shared_ptr<SDL_Surface>& Surface, bool& Converted)
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
            return std::shared_ptr<SDL_Surface>(SDL_ConvertSurfaceFormat(Surface.get(), SDL_PIXELFORMAT_BGRA8888, 0));
        else
            return std::shared_ptr<SDL_Surface>(SDL_ConvertSurfaceFormat(Surface.get(), SDL_PIXELFORMAT_BGR24, 0));
    }

    // Converts alpha-formats to RGBA, non-alpha to RGB, and leaves RGB(A) as is
    // sets converted to true if the surface needed to be converted
    std::shared_ptr<SDL_Surface> ConvertToRGB_RGBASurface(std::shared_ptr<SDL_Surface>& Surface, bool& Converted)
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
            return std::shared_ptr<SDL_Surface>(SDL_ConvertSurfaceFormat(Surface.get(), SDL_PIXELFORMAT_ARGB8888, SDL_SWSURFACE));
        else
            return std::shared_ptr<SDL_Surface>(SDL_ConvertSurfaceFormat(Surface.get(), SDL_PIXELFORMAT_RGB24, SDL_SWSURFACE));
    }

    /*******************************************************
     * Image saving
     *******************************************************/

     /***************************
      * BMP section
      *****************************/

#ifdef HaveBMP
#ifndef _WIN32

        /* constants for the biCompression field */
    const int BI_RGB = 0;
    const int BI_RLE8 = 1;
    const int BI_RLE4 = 2;
    const int BI_BITFIELDS = 3;
    const int BI_JPEG = 4;
    const int BI_PNG = 5;
#endif
    /*
    struct BITMAPINFOHEADER
    {
        uint32_t biSize;
        long int biWidth;
        long int biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        long int biXPelsPerMeter;
        long int biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    };

    typedef std::shared_ptr<BITMAPINFOHEADER> LPBITMAPINFOHEADER;
    typedef BITMAPINFOHEADER TBITMAPINFOHEADER;
    typedef std::shared_ptr<BITMAPINFOHEADER> PBITMAPINFOHEADER;

    struct RGBTRIPLE
    {
        uint8_t rgbtBlue;
        uint8_t rgbtGreen;
        uint8_t rgbtRed;
    };
    typedef RGBTRIPLE tagRGBTRIPLE;
    typedef RGBTRIPLE TRGBTRIPLE;
    typedef std::shared_ptr<RGBTRIPLE> PRGBTRIPLE;

    struct RGBQUAD
    {
        uint8_t rgbBlue;
        uint8_t rgbGreen;
        uint8_t rgbRed;
        uint8_t rgbReserved;
    };
    typedef RGBQUAD tagRGBQUAD;
    typedef RGBQUAD TRGBQUAD;
    typedef std::shared_ptr<RGBQUAD> PRGBQUAD;

    struct BITMAPINFO
    {
        BITMAPINFOHEADER bmiHeader;
        std::array<RGBQUAD, 1> bmiColors;
    };
    typedef std::shared_ptr<BITMAPINFO> LPBITMAPINFO;
    typedef std::shared_ptr<BITMAPINFO> PBITMAPINFO;
    typedef BITMAPINFO TBITMAPINFO;

    //$PACKRECORDS 2
    struct BITMAPFILEHEADER
    {
        uint16_t bfType;
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;
    };
    //$PACKRECORDS DEFAULT
    //$ENDIF
    */

    /*
     * ImageData must be in BGR-format
     */
    bool WriteBMPImage(const std::filesystem::path FileName, std::shared_ptr<SDL_Surface> Surface)
    {
        if (SDL_MUSTLOCK(Surface))
            SDL_LockSurface(Surface.get());

        int result = SDL_SaveBMP(Surface.get(), FileName.string().c_str());

        if (SDL_MUSTLOCK(Surface))
            SDL_UnlockSurface(Surface.get());

        if (result != 0)
        {
            ULog::Log.LogError("Could not write file : \"" + FileName.string() + "\"", "WriteBMPImage");
            return false;
        }
        return true;
    }

#endif

    /***************************
     * JPG section
     *****************************/

     /*$IFDEF HaveJPG*/

    bool WriteJPGImage(const std::filesystem::path FileName, SDL_Surface& Surface, int Quality)
        /*var
          //$IFDEF Delphi
          Bitmap:     TBitmap;
          BitmapInfo: TBitmapInfo;
          Jpeg:       TJpegImage;
          row:        int;
          FileStream: TBinaryFileStream;
          //$ELSE
          cinfo:     jpeg_compress_struct;
          jerr :     jpeg_error_mgr;
          jpgFile:   TBinaryFileStream;
          rowPtr:    array[0..0] of JSAMPROW;
          //$ENDIF
          converted:  bool;*/
    {
        bool Result = false;

        /*$IFDEF Delphi*/
          // only 24bit (BGR) data is supported, so convert to it
        if (IsBGRSurface(Surface.format)) then
            converted = false
        else
        {
            Surface = SDL_ConvertSurface(Surface, @PixelFmt_BGR, SDL_SWSURFACE);
            converted = true;
        }

        // create and setup bitmap
        Bitmap = TBitmap.Create;
        Bitmap.PixelFormat = pf24bit;
        Bitmap.Width = Surface.w;
        Bitmap.Height = Surface.h;

        // setup bitmap info on source image (Surface parameter)
        ZeroMemory(@BitmapInfo, SizeOf(BitmapInfo));
        with BitmapInfo.bmiHeader do
        {
            biSize = SizeOf(BITMAPINFOHEADER);
            biWidth = Surface.w;
            biHeight = Surface.h;
            biPlanes = 1;
            biBitCount = 24;
            biCompression = BI_RGB;
        }

        if (SDL_MUSTLOCK(Surface)) then
            SDL_LockSurface(Surface);

        // use fast Win32-API functions to copy data instead of Bitmap.Canvas.Pixels
        if (Surface.pitch mod 4 == 0) then
        {
            // if the image is aligned (to a 4-uint8_t boundary) -> copy all data at once
            // Note: surfaces created with SDL (e.g. with SDL_ConvertSurface) are aligned
            SetDIBits(0, Bitmap.Handle, 0, Bitmap.Height, Surface.pixels, BitmapInfo, DIB_RGB_COLORS);
        }
        else
        {
            // wrong alignment -> copy each line separately.
            // Note: for the last line unassigned memory (> last Surface.pixels element)
            //   will be copied to the padding area (last bytes of a row),
            //   but we do not care because the content of padding data is ignored anyhow.
            for (row = 0 to Surface.h)
            {
                SetDIBits(0, Bitmap.Handle, row, 1, @PChar(Surface.pixels)[row * Surface.pitch],
                    BitmapInfo, DIB_RGB_COLORS);
            }
        }

        if (SDL_MUSTLOCK(Surface)) then
            SDL_UnlockSurface(Surface);

        // assign Bitmap to JPEG and store the latter
        try
        {
            // init with nil so Free() will not fail if an exception occurs
            Jpeg = nil;
            Bitmap = nil;
            FileStream = nil;

            try
            {
                Jpeg = TJPEGImage.Create;
                Jpeg.Assign(Bitmap);

                // compress image (don't forget this line, otherwise it won't be compressed)
                Jpeg.CompressionQuality = Quality;
                Jpeg.Compress();

                // Note: FileStream needed for unicode filename support
                FileStream = TBinaryFileStream.Create(Filename, fmCreate);
                Jpeg.SaveToStream(FileStream);
            }
            catch (...)
            {
                FileStream.Free;
            }
            Bitmap.Free;
            Jpeg.Free;
        }
        catch (...)
        {
            Log.LogError('Could not save file: "' + FileName.ToNative + '"', 'WriteJPGImage');
            Exit;
        }

        /*$ELSE*/
          // based on example.pas in FPC's packages/base/pasjpeg directory

          // only 24bit (RGB) data is supported, so convert to it
        if (IsRGBSurface(Surface.format)) then
            converted = false
        else
        {
            Surface = SDL_ConvertSurfaceFormat(Surface, SDL_PIXELFORMAT_RGB24, SDL_SWSURFACE);
            converted = true;
        }

        // allocate and initialize JPEG compression object
        cinfo.err = jpeg_std_error(jerr);
        // msg_level that will be displayed. (Nomssi)
        //jerr.trace_level = 3;
        // initialize the JPEG compression object
        jpeg_create_compress(@cinfo);

        // open file for writing
        try
        {
            jpgFile = TBinaryFileStream.Create(FileName, fmCreate);
        }
        catch (...)
        {
            ULog::Log.LogError('Could not open file: "' + FileName.ToNative + '"', 'WriteJPGImage');
            Exit;
        }

        // specify data destination
        jpeg_stdio_dest(@cinfo, @jpgFile);

        // set parameters for compression
        cinfo.image_width = Surface.w;
        cinfo.image_height = Surface.h;
        cinfo.in_color_space = JCS_RGB;
        cinfo.input_components = 3;
        cinfo.data_precision = 8;

        // set default compression parameters
        jpeg_set_defaults(@cinfo);
        jpeg_set_quality(@cinfo, quality, true);

        // start compressor
        jpeg_start_compress(@cinfo, true);

        if (SDL_MUSTLOCK(Surface)) then
            SDL_LockSurface(Surface);

        while (cinfo.next_scanline < cinfo.image_height)
        {
            // Note: the uint8_t-count of a row is pitch (which is not width*bitsPerPixel if the image is aligned)
            rowPtr[0] = JSAMPROW(@PChar(Surface.pixels)[(Surface.h - cinfo.next_scanline - 1) * Surface.pitch]);
            jpeg_write_scanlines(@cinfo, JSAMPARRAY(@rowPtr), 1);
        }

        if (SDL_MUSTLOCK(Surface)) then
            SDL_UnlockSurface(Surface);

        // finish compression
        jpeg_finish_compress(@cinfo);
        // close the output file
        jpgFile.Free;

        // release JPEG compression object
        jpeg_destroy_compress(@cinfo);
        /*$ENDIF*/

        if (converted) then
            SDL_FreeSurface(Surface);

        Result = true;
    }

/*$ENDIF*/

/*******************************************************
 * Image loading
 *******************************************************/

/*
 * Loads an image from the given file
 */
std::shared_ptr<SDL_Surface> ImageLoad(const std::filesystem::path Filename)
/*var
  FilenameCaseAdj: std::filesystem::path;*/
{
    // try to adjust filename's case and check if it exists
    auto FilenameCaseAdj = Filename; //Filename.AdjustCase(false);
    if (!std::filesystem::exists(FilenameCaseAdj) || !std::filesystem::is_regular_file(FilenameCaseAdj))
    {
        ULog::Log.LogError("Image - File does not exist \"" + FilenameCaseAdj.string() + "\"", "LoadImage");
        return nullptr;
    }

    // load from file
    try
    {        
        return std::shared_ptr<SDL_Surface>(IMG_Load(FilenameCaseAdj.string().c_str())); //SDL2 uses wants UTF-8 strings according to documentation
        // Note: TBinaryFileStream is freed by SDLStream. SDLStream by IMG_Load_RW().
    }
    catch (...)
    {
        ULog::Log.LogError("Could not load from file \"" + FilenameCaseAdj.string() + "\"", "LoadImage");
        return nullptr;
    }
}

/*******************************************************
 * Image manipulation
 *******************************************************/

bool PixelFormatEquals(SDL_PixelFormat fmt1, SDL_PixelFormat fmt2)
{
  return 
    (fmt1.BitsPerPixel  == fmt2.BitsPerPixel) &&
    (fmt1.BytesPerPixel == fmt2.BytesPerPixel) &&
    (fmt1.Rloss == fmt2.Rloss) && (fmt1.Gloss == fmt2.Gloss) && (fmt1.Bloss == fmt2.Bloss) &&
    (fmt1.Rmask == fmt2.Rmask) && (fmt1.Gmask == fmt2.Gmask) && (fmt1.Bmask == fmt2.Bmask)   &&
    (fmt1.Rshift == fmt2.Rshift) && (fmt1.Gshift == fmt2.Gshift) && (fmt1.Bshift == fmt2.Bshift)
  ;
}

void ScaleImage(SDL_Surface& ImgSurface, uint32_t Width, uint32_t Height)
/*var
   newstretchRect: PSDL_Rect;
   origRect: PSDL_Rect;*/
{
  origRect = new(PSDL_Rect);
  origRect.x=0;
  origRect.y=0;
  origRect.w=ImgSurface.w;
  origRect.h=ImgSurface.h;
  newstretchRect = new(PSDL_Rect);
  newstretchRect.x = 0;
  newstretchRect.y = 0;
  newstretchRect.w = Width;
  newstretchRect.h = Height;
  SDL_UpperBlitScaled( ImgSurface, origRect, ImgSurface, newstretchRect );
}

void FitImage(SDL_Surface& ImgSurface, int Width, int Height)
{
  auto TempSurface = ImgSurface;

  // create a new surface with given width and height
  auto ImgFmt = TempSurface.format;
  ImgSurface = SDL_CreateRGBSurface(
    SDL_SWSURFACE, Width, Height, ImgFmt->BitsPerPixel,
    ImgFmt->Rmask, ImgFmt->Gmask, ImgFmt->Bmask, ImgFmt->Amask);

  // copy image from temp- to new surface
  SDL_SetSurfaceBlendMode(&TempSurface, SDL_BLENDMODE_NONE);
  SDL_BlitSurface(&TempSurface, nullptr, &ImgSurface, nullptr);
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
    auto ColorToHue = [](const uint32_t Color)->uint32_t
        // returns hue within the range [0.0-6.0] but << 10, ie. times 1024
        /*var
        Red, Green, Blue: long int;
        Min, Max, Delta:  long int;
        Hue: double;*/
    {
        // extract the colors
        // division by 255 is omitted, since it is implicitly done
        // when dividing by delta
        long int Red = ((Color & 0xff0000) >> 16); // R
        long int Green = ((Color & 0xff00) >> 8); // G
        long int Blue = (Color & 0xff); // B

        long int Min = std::min(Red, std::min(Green, Blue));
        long int Max = std::max(Red, std::max(Green, Blue));

        // calc hue
        long int Delta = Max - Min;     // This gives a type size mismatch warning, because Delta is uint32_t, ie. >= 0
        // But the assignments above are easy enough to be sure, that Max - Min is >= 0.
        if (Delta == 0)
            return 0;
        else
        {
            // The division by Delta is done separately afterwards.
            // Necessary because Delphi did not do the type conversion from
            // uint32_t to double as expected.
            // After the change to long int, we may not need it, but left for now
            // Something to check
            double Hue;
            if (Max == Red)
                Hue = Green - Blue;
            else if (Max == Green)
                Hue = 2.0 * Delta + Blue - Red;
            else //if (Max == Blue)
                Hue = 4.0 * Delta + Red - Green;
            Hue /= Delta;
            if (Hue < 0.0)
                Hue += 6.0;
            return static_cast<uint32_t>(Hue * 1024);           // '*1024' is << 10
            //     if NewColor == $000000 then
            //       Log.LogError ('Hue: ' +  FloatToStr(Hue), 'ColorToHue');
        }
    };
    auto Pixel = (uint8_t*)ImgSurface.pixels;

    // check of the size of a pixel in bytes.
    // It should be always 4, but this
    // additional safeguard will show,
    // whether something went wrong up to here.

    if (ImgSurface.format->BytesPerPixel != 4)
    {
        ULog::Log.LogError("ColorizeImage: The pixel size should be 4, but it is "
            + std::to_string(ImgSurface.format->BytesPerPixel));
    }

    // Check whether the new color is white, grey or black, 
    // because a greyscale must be created in a different
    // way.

    long int Red = ((NewColor & 0xff0000) >> 16); // R
    long int Green = ((NewColor & 0xff00) >> 8); // G
    long int Blue = (NewColor & 0xff); // B

    if (Red == Green && Green == Blue) // greyscale image
    {
        // According to these recommendations (ITU-R BT.709-5)
        // the conversion parameters for rgb to greyscale are
        // 0.299, 0.587, 0.114
        for (size_t PixelIndex = 0; PixelIndex < ImgSurface.w * ImgSurface.h; ++PixelIndex)
        {
            auto PixelColors = Pixel;
            auto GreyReal = 0.299 * PixelColors[0] + 0.587 * PixelColors[1] + 0.114 * PixelColors[2];
            //       PixelColors[3] is alpha and remains untouched
            auto Grey = std::round(GreyReal);
            PixelColors[0] = Grey;
            PixelColors[1] = Grey;
            PixelColors[2] = Grey;
            //       PixelColors[3] is alpha and remains untouched
            Pixel += ImgSurface.format->BytesPerPixel;
        }
        return; // we are done with a greyscale image.
    }

    auto Hue = ColorToHue(NewColor);   // Hue is << 10
    auto f = Hue & 0x3ff;           // f is the decimal part of hue
    auto HueInteger = Hue >> 10;

    for (size_t PixelIndex = 0; PixelIndex < ImgSurface.w * ImgSurface.h; ++PixelIndex)
    {
        auto PixelColors = Pixel;
        // inlined colorize per pixel

        // uses fixed point math
        // << 10 is used for divisions

        // get color values

        uint32_t Red = PixelColors[0];
        uint32_t Green = PixelColors[1];
        uint32_t Blue = PixelColors[2];
        //       PixelColors[3] is alpha and remains untouched

        //calculate luminance and saturation from rgb

        uint32_t Max = std::max(Red, std::max(Green, Blue));

        if (Max == 0)               // the color is black
        {
            PixelColors[0] = 0;
            PixelColors[1] = 0;
            PixelColors[2] = 0;
        }
        else
        {
            uint32_t Min = std::min(Red, std::min(Green, Blue));

            if (Min == 255)           // the color is white
            {
                PixelColors[0] = 255;
                PixelColors[1] = 255;
                PixelColors[2] = 255;
            }
            else                          // all colors except black and white
            {
                uint32_t Delta = Max - Min;         // This gives a type size mismatch warning, because Delta is uint32_t, ie. >= 0
                // But the assignments above are easy enough to be sure, that Max - Min is >= 0.
                uint32_t Sat = (Delta << 10) / Max;  // << 10

                // >> 10 corrects that Sat and f are << 10
                // the resulting p, q and t are unshifted

                uint32_t p = (Max * (1024 - Sat)) >> 10;
                uint32_t q = (Max * (1024 - (Sat * f) >> 10)) >> 10;
                uint32_t t = (Max * (1024 - (Sat * (1024 - f)) >> 10)) >> 10;

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

                PixelColors[0] = static_cast<uint8_t>(Red);
                PixelColors[1] = static_cast<uint8_t>(Green);
                PixelColors[2] = static_cast<uint8_t>(Blue);

            }
        }
        Pixel += ImgSurface.format->BytesPerPixel;
    }
}
};