#pragma once

/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it &&/or
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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UFont.pas $
 * $Id: UFont.pas 2675 2010-10-17 17:00:23Z tobigun $
 */

#define NOMINMAX

#include <array>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <filesystem>
#include <exception>
#include <sstream>
#include <locale>
#include <optional>
#include <fstream>
#include <map>
#include <set>

#include <gl/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>

namespace UFont
{
#ifndef  FREETYPE_DEMO
  // Flip direction of y-axis.
  // Default is a cartesian coordinate system with y-axis in upper direction
  // but with USDX the y-axis is in lower direction.
  #define  FLIP_Y_AXIS
  #define  BITMAP_FONT
#endif


// Enables the Freetype font cache
#define ENABLE_FT_FACE_CACHE

/*uses
  {$IF Defined(MSWINDOWS)}
  LazUTF8,
  LazUTF8Classes,
  {$IFEND}
  FreeType,
  dglOpenGL,
  sdl2,
  Math,
  Classes,
  SysUtils,
  UUnicodeUtils,
  {$IFDEF BITMAP_FONT}
  UTexture,
  {$ENDIF}
  UPath;
  */
typedef std::array<GLubyte, std::numeric_limits<int>::max() / sizeof(GLubyte)> TGLubyteArray;
typedef std::shared_ptr<TGLubyteArray> PGLubyteArray;
typedef std::vector<GLubyte> TGLubyteDynArray;
typedef std::vector<std::u32string> TUCS4StringArray;

struct TGLColor
{
    TGLColor() : vals{0, 0, 0, 0}
    {}

    TGLColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) : vals{ r, g, b, a}
    {}

    GLfloat vals[4];

    GLfloat& r()
	{
        return vals[0];
    }

    GLfloat& g()
    {
        return vals[1];
    }

    GLfloat& b()
    {
        return vals[2];
    }

    GLfloat& a()
    {
        return vals[3];
    }
};

struct TBoundsDbl
{
    double Left, Right;
    double Bottom, Top;
};

struct TPositionDbl
{
    double X, Y;
};

struct TTextureSize {
    int Width, Height;
};

struct TBitmapCoords {
    double Left, Top;
    int Width, Height;
};

class EFontError : public std::exception
{
public:
    EFontError(char const* const Message) noexcept : std::exception(Message)
    {}
};

/**
* Abstract base class representing a glyph.
*/
class TGlyph
{
protected:
    
public:

    virtual ~TGlyph();

    virtual void Render(bool UseDisplayLists) = 0;
    virtual void RenderReflection() = 0;

    /** Distance to next glyph (in pixels) */
    [[nodiscard]] virtual TPositionDbl Advance() const = 0;

    /** Glyph bounding box (in pixels) */
    [[nodiscard]] virtual TBoundsDbl Bounds() const = 0;
};

enum FontStyles
{
    Italic, Underline, Reflect
};
/**
* Font styles used by TFont::Style
*/
typedef std::set<FontStyles> TFontStyle;

/**
* Base font class.
*/
class TFont
{
private:
    /** Non-virtual reset-method used in Create() && Reset() */
    // ReSharper disable once CppHiddenFunction
    void ResetIntern();

protected:
    std::filesystem::path fFilename;
    TFontStyle fStyle;
    bool fUseKerning;
    float fLineSpacing;       // must be inited by subclass
    float fReflectionSpacing; // must be inited by subclass to -2*Descender
    float fGlyphSpacing;
    bool fReflectionPass;

    /**
     * Splits lines in Text seperated by newline (char-code #13).
     * @param Text   UCS-4 encoded string
     * @param Lines  splitted std::u32string lines
     */
    void SplitLines(const std::u32string Text, TUCS4StringArray& Lines);

    /**
     * Print an array of UCS4Strings. Each array-item is a line of text.
     * Lines of text are separated by the line-spacing.
     * This is the base function for all text drawing.
     */
    virtual void Print(const TUCS4StringArray Text);

    /**
     * Draws an underline.
     */
    virtual void DrawUnderline(const std::u32string Text);

    /**
     * Renders (one) line of text.
     */
    virtual void Render(const std::u32string Text) = 0;

    /**
     * Returns the bounds of text-lines contained in Text.
     * @param(Advance if true the right bound is set to the advance instead
     *   of the minimal right bound.)
     */
    virtual TBoundsDbl BBox(const TUCS4StringArray Text, bool Advance) = 0;

    /**
     * Resets all user settings to default values.
     * Override methods should always call the inherited version.
     */
    virtual void Reset();

    /** Returns true if the current render-pass is used to draw the reflection }*/
    virtual void ReflectionPass(bool Enable);

public:

    TFont(const std::filesystem::path Filename);
    virtual ~TFont();

    /**
     * Prints a text.
     */
    template<typename CharType>
    void Print(const std::basic_string<CharType> Text);

    /**
     * Calculates the bounding box (width && height) around Text.
     * Works with Italic && Underline styles but reflections created
     * with the Reflect style are not considered.
     * Note that the width might differ due to kerning with appended text,
     * e.g. Width('VA') <== Width('V') + Width('A').
     * @param Advance  if set to true, Result.Right is set to the advance of
     * the given text rather than the min. right border. The advance width is
     * bigger than the text's width as it additionally contains the advance
     * && glyph-spacing of the last character.
     */

    template<typename TextType>
    TBoundsDbl BBox(const std::basic_string<TextType> Text, bool Advance = true);

    /**
     * Adds a new font that is used if the default font misses a glyph
     * @raises EFontError  if the fallback could not be initialized
     */
    virtual void AddFallback(const std::filesystem::path Filename) = 0;

    /** Font height */
    virtual void Height(float Height) = 0;
    [[nodiscard]] virtual float Height() const = 0;

    /** Vertical distance from baseline to top of glyph */
    [[nodiscard]] virtual float Ascender() const = 0;
    /** Vertical distance from baseline to bottom of glyph */
    [[nodiscard]] virtual float Descender() const = 0;

    /** Vertical distance between two baselines */
    virtual void LineSpacing(float Spacing);
    [[nodiscard]] virtual float LineSpacing() const;

    /** Space between end and start of next glyph added to the advance width */
    virtual void GlyphSpacing(float Spacing);
    [[nodiscard]] virtual float GlyphSpacing() const;

    /** Distance between normal baseline && baseline of the reflection */
    virtual void ReflectionSpacing(float Spacing);
    [[nodiscard]] virtual float ReflectionSpacing() const;

    /** Font style (italic/underline/...) */
    virtual void Style(TFontStyle Style);
    [[nodiscard]] virtual TFontStyle Style() const;
    [[nodiscard]] virtual float UnderlinePosition() const = 0;
    [[nodiscard]] virtual float UnderlineThickness() const = 0;

    /** If set to true (default) kerning will be used if available */
    virtual void UseKerning(bool Enable);
    [[nodiscard]] virtual bool UseKerning() const;
    
    /** Filename */
    [[nodiscard]] std::filesystem::path Filename() const;

    virtual bool ReflectionPass() const;
};

//** Max. number of mipmap levels that a TScalableFont can contain
const int cMaxMipmapLevel = 5;

/**
 * Wrapper around TFont to allow font size changes.
 * The font is scaled to the requested size by a modelview matrix
 * transformation (glScale) && not by rescaling the internal bitmap
 * representation. This way changing the size is really fast but the result
 * may lack quality on large or small scale factors.
 */
class TScalableFont : public TFont
{
private:
	// ReSharper disable once CppHidingFunction
	void ResetIntern();

protected:
    float fScale;        //**< current height to base-font height ratio
	float fStretch;      //**< stretch factor for width (Width * fStretch)
    std::shared_ptr<TFont> fBaseFont;      //**< shortcut for fMipmapFonts[0]
    bool fUseMipmaps;  //**< true if mipmap fonts are generated
    /// Mipmap fonts (size[level+1] == size[level]/2)
	std::array<std::shared_ptr<TFont>, cMaxMipmapLevel + 1> fMipmapFonts;

    void Render(const std::u32string Text) override;
    void Print(const TUCS4StringArray Text) override;
    TBoundsDbl BBox(const TUCS4StringArray Text, bool Advance) override;

    /**
     * Callback called for creation of each mipmap font.
     * Must be defined by the subclass.
     * Mipmaps created by this method are managed && freed by TScalableFont.
     */
    virtual std::shared_ptr<TFont> CreateMipmap(int Level, float Scale) = 0;

    /**
     * Returns the mipmap level considering the current scale && projection
     * matrix.
     */
    int GetMipmapLevel();

    /**
     * Returns the scale applied to the given mipmap font.
     * fScale * fBaseFont.Height / fMipmapFont[Level].Height
     */
    [[nodiscard]] std::optional<float> GetMipmapScale(int Level) const;

    /**
     * Chooses the mipmap that looks nicest with current scale && projection
     * matrix.
     */
    std::shared_ptr<TFont> ChooseMipmapFont();

public:
    /**
     * Creates a wrapper to make the base-font Font scalable.
     * If UseMipmaps is set to true smaller fonts are created so that a
     * resized (Height property changed) font looks nicer.
     * The font passed is managed && freed by TScalableFont.
     */
    TScalableFont(std::shared_ptr<TFont> Font, bool UseMipmaps);

    /**
     * Frees memory. The fonts passed on Create() && mipmap creation
     * are freed too.
     */
    ~TScalableFont() override;

    /** @seealso TFont::Reset */
    void Reset() override;

    /** Font height */
    //property Height: float read Height write Height;
    /** Factor for font stretching (NewWidth == Width*Stretch), 1.0 by default */
    //property Stretch: float read Stretch write Stretch;

    void Height(float Height) override;
    [[nodiscard]] float Height() const override;

    [[nodiscard]] float Ascender() const override;
    [[nodiscard]] float Descender()const  override;

    void LineSpacing(float Spacing) override;
    [[nodiscard]] float LineSpacing()const  override;

    void GlyphSpacing(float Spacing) override;
    [[nodiscard]] float GlyphSpacing()const  override;

    void ReflectionSpacing(float Spacing) override;
    [[nodiscard]] float ReflectionSpacing() const override;

    void Style(TFontStyle Style) override;
    [[nodiscard]] TFontStyle Style() const override;

    [[nodiscard]] float UnderlinePosition() const override;
    [[nodiscard]] float UnderlineThickness() const override;

    void UseKerning(bool Enable) override;

    void Stretch(float Stretch);
    [[nodiscard]] float Stretch() const;
};

/**
* Table for storage of max. 256 glyphs.
* Used for the second cache level. Indexed by the LSB of the UCS4Char
* char-code.
*/
typedef std::array<TGlyph, 256> TGlyphTable;
typedef std::shared_ptr<TGlyphTable> PGlyphTable;

/**
* Cache for glyphs of a float font.
* The cached glyphs are stored inside a hash-list.
* Hashing is performed in two steps:
* 1. the least significant byte (LSB) of the UCS4Char character code
* is removed (shr 8) && the result (we call it BaseCode here) looked up in
* the hash-list.
* 2. Each entry of the hash-list contains a table with max. 256 entries.
* The LSB of the char-code of a glyph is the table-offset of that glyph.
*/
class TGlyphCache
{
private:
    std::map<char32_t, std::shared_ptr<TGlyph>> fGlyphs;

    /**
     * Finds a glyph-table storing cached glyphs with base-code BaseCode
     * (== upper char-code bytes) in the hash-list && returns the table &&
     * its index.
     * @param(InsertPos  the position of the tyble in the list if it was found,
     *                   otherwise the position the table should be inserted)
     */
    //PGlyphTable FindGlyphTable(uint32_t BaseCode, int& InsertPos);

public:

    TGlyphCache() = default;
    virtual ~TGlyphCache() = default;

    /**
     * Add glyph Glyph with char-code ch to the cache.
     * @returns @true on success, @false otherwise
     */
    bool AddGlyph(char32_t ch, const std::shared_ptr<TGlyph> Glyph);

    /**
     * Removes the glyph with char-code ch from the cache.
     */
    void DeleteGlyph(char32_t ch);

    /**
     * Removes the glyph with char-code ch from the cache.
     */
    std::shared_ptr<TGlyph> GetGlyph(char32_t ch);

    /**
     * Checks if a glyph with char-code ch is cached.
     */
    [[nodiscard]] bool HasGlyph(char32_t ch) const;

    /**
     * Remove and free all cached glyphs. If KeepBaseSet is set to
     * true, cached characters in the range 0..255 will not be flushed.
     */
    void FlushCache(bool KeepBaseSet);
};

class TCachedFont : public TFont
{
protected:
    TGlyphCache fCache;

    /**
     * Retrieves a cached glyph with char-code ch from cache.
     * If the glyph is not already cached, it is loaded with LoadGlyph().
     */
    std::shared_ptr<TGlyph> GetGlyph(char32_t ch);

    /**
     * Callback to create (load) a glyph with char-code ch.
     * Implemented by subclasses.
     */
    virtual std::shared_ptr<TGlyph> LoadGlyph(char32_t ch) = 0;

public:

    TCachedFont(const std::filesystem::path Filename);
    ~TCachedFont() override;

    /**
     * Remove && free all cached glyphs. If KeepBaseSet is set to
     * true, the base glyphs are not be flushed.
     * @seealso TGlyphCache.FlushCache
     */
    void FlushCache(bool KeepBaseSet);
};

class TFTFontFace;
class TFTFontFaceCache;
enum TFontPart { fpNone, fpInner, fpOutline };
typedef std::vector<std::weak_ptr<TFTFontFace>> TFTFontFaceArray;

/**
* Freetype font class.
*/
class TFTFont : public TCachedFont
{
private:
	// ReSharper disable once CppHidingFunction
	void ResetIntern();
    static std::shared_ptr<TFTFontFaceCache> GetFaceCache();

protected:
    std::shared_ptr<TFTFontFace> fFace;           //**< Default font face
    int fSize;               //**< Font base size (in pixels)
    float fOutset;              //**< size of outset extrusion (in pixels)
    bool fPreCache;           //**< pre-load base glyphs
    FT_Int32 fLoadFlags;         //**< FT glpyh load-flags
    bool fUseDisplayLists;    //**< true: use display-lists, false: direct drawing
    TFontPart fPart;             //**< indicates the part of an outline font
    TFTFontFaceArray fFallbackFaces; //**< available fallback faces, ordered by priority

    /** @seealso TCachedFont::LoadGlyph */
    std::shared_ptr<TGlyph> LoadGlyph(char32_t ch) override;

    void Render(const std::u32string Text) override;
    TBoundsDbl BBox(const TUCS4StringArray Text, bool Advance) override;

public:
    /**
     * Creates a font of size Size (in pixels) from the file Filename.
     * If Outset (in pixels) is set to a value > 0 the glyphs will be extruded
     * at their borders. Use it for e.g. a bold effect.
     * @param  LoadFlags  flags passed to FT_Load_Glyph()
     * @raises EFontError  if the font-file could not be loaded
     */
    TFTFont(const std::filesystem::path Filename,
        int Size, float Outset = 0.f,
        bool PreCache = true,
        FT_Int32 LoadFlags = FT_LOAD_DEFAULT);

    /**
     * Frees all resources associated with the font.
     */
    ~TFTFont() override;

    /** @see also TFont::Reset */
    void Reset() override;

    void AddFallback(const std::filesystem::path Filename) override;

    /** Size of the base font */
    [[nodiscard]] int Size() const { return fSize; }
    /** Outset size */
    [[nodiscard]] float Outset() const { return fOutset; }
    /** The part (inner/outline/none) this font represents in a composite font */
    [[nodiscard]] TFontPart Part() const { return fPart; }
    void Part(TFontPart part) { fPart = part; }
    /** Freetype face of this font */
    [[nodiscard]] const std::shared_ptr<TFTFontFace> DefaultFace() const { return fFace; }
    /** Available freetype fallback faces, ordered by priority */
    [[nodiscard]] TFTFontFaceArray FallbackFaces() const { return fFallbackFaces; }
    [[nodiscard]] bool PreCache() const { return fPreCache; }

    [[nodiscard]] float Height() const override;
    [[nodiscard]] float Ascender() const override;
    [[nodiscard]] float Descender() const override;
    [[nodiscard]] float UnderlinePosition() const override;
    [[nodiscard]] float UnderlineThickness() const override;
};

/**
* Freetype font face class.
*/
class TFTFontFace
{
private:
    std::filesystem::path fFilename;             //**< filename of the font-file
	std::vector<FT_Byte> byarr1;
    FT_Face fFace;               //**< Holds the height of the font
    TPositionDbl fFontUnitScale; //**< FT font-units to pixel ratio
	int fSize;

public:
    /**
     * @raises EFontError  if the glyph could not be initialized
     */
    TFTFontFace(const std::filesystem::path Filename, int Size);

    virtual ~TFTFontFace();

    //property Filename: std::filesystem::path read fFilename;
    [[nodiscard]] std::filesystem::path Filename() const { return fFilename;  }
    [[nodiscard]] FT_Face Data() const { return fFace; }
    [[nodiscard]] TPositionDbl FontUnitScale() const { return fFontUnitScale; }
    [[nodiscard]] int Size() const { return fSize; }
};

/**
* Loading font faces with freetype is a slow process.
* Especially loading a font (e.g. fallback fonts) more than once is a waste
* of time. Just cache already loaded faces here.
*/
class TFTFontFaceCache
{
private:
#ifdef ENABLE_FT_FACE_CACHE
    std::vector<std::weak_ptr<TFTFontFace>> fFaces;
	//std::vector<int> fFacesRefCnt;
#endif
public:
    /**
     * @raises EFontError  if the font could not be initialized
     */
    std::shared_ptr<TFTFontFace> LoadFace(const std::filesystem::path Filename, int Size);

    void UnloadFace(std::weak_ptr<TFTFontFace> Face);
};

/**
* Freetype glyph.
* Each glyph stores a texture with the glyph's image.
*/
class TFTGlyph : public TGlyph
{
private:
    char32_t fCharCode;     //**< Char code
    TFTFontFace fFace;       //**< Freetype face used for this glyph
    FT_UInt fCharIndex;      //**< Freetype specific char-index (!= char-code)
    GLuint fDisplayList;     //**< Display-list ID
	GLuint fTexture;         //**< Texture ID
    TBitmapCoords fBitmapCoords; //**< Left/Top offset && Width/Height of the bitmap (in pixels)
    TPositionDbl fTexOffset; //**< Right && bottom texture offset for removal of power-of-2 padding
    TTextureSize fTexSize;   //**< Texture size in pixels

    std::shared_ptr<TFTFont> fFont;           //**< Font associated with this glyph
    TPositionDbl fAdvance;   //**< Advance width of this glyph
    TBoundsDbl fBounds;      //**< Glyph bounds
	float fOutset;          //**< Extrusion outset

    /**
     * Extrudes the outline of a glyph's bitmap stored in TexBuffer with size
     * fTexSize by Outset pixels.
     * This is useful to create bold or outlined fonts.
     * TexBuffer must be 2*Ceil(Outset) pixels higher && wider than the
     * original glyph bitmap, otherwise the glyph borders cannot be extruded
     * correctly.
     * The bitmap must be 2* pixels wider && higher than the
     * original glyph's bitmap with the latter centered in it.
     */
    void StrokeBorder(FT_Glyph& Glyph);

    /**
     * Creates an OpenGL texture (&& display list) for the glyph.
     * The glyph's && bitmap's metrics are set correspondingly.
     * @param  LoadFlags  flags passed to FT_Load_Glyph()
     * @raises EFontError  if the glyph could not be initialized
     */
    void CreateTexture(FT_Int32 LoadFlags);

protected:

public:

    [[nodiscard]] TPositionDbl Advance() const override;
    [[nodiscard]] TBoundsDbl Bounds() const override;

    /**
     * Creates a glyph with char-code ch from font Font.
     * @param LoadFlags  flags passed to FT_Load_Glyph()
     */
    TFTGlyph(std::shared_ptr<TFTFont> Font, char32_t ch, float Outset, FT_Int32 LoadFlags);
    ~TFTGlyph() override;

    /** Renders the glyph (normal render pass) */
    void Render(bool UseDisplayLists) override;
    /** Renders the glyph's reflection */
    void RenderReflection() override;

    /** Freetype specific char-index (!= char-code) */
    [[nodiscard]] FT_UInt CharIndex() const { return fCharIndex; }

    /** Freetype face used for this glyph */
    [[nodiscard]] TFTFontFace Face() const { return fFace; }
};

class TFTScalableFont : public TScalableFont
{
protected:
    [[nodiscard]] virtual float GetOutset() const;
    std::shared_ptr<TFont> CreateMipmap(int Level, float Scale) override;

public:
    /**
     * Creates a scalable font of size Size (in pixels) from the file Filename.
     * OutsetAmount is the ratio of the glyph extrusion.
     * The extrusion in pixels is Size*OutsetAmount
     * (0.0 -> no extrusion, 0.1 -> 10%).
     *
     * The memory size (in bytes) consumed by a scalable font
     * - with UseMipmaps==false:
     *  mem == size^2 * #cached_glyphs
     * - with UseMipmaps==true (all mipmap levels):
     *  mem == size^2 * #cached_glyphs * Sum[i==1..cMaxMipmapLevel](1/i^2)
     * - with UseMipmaps==true (5 <== cMaxMipmapLevel <== 10):
     *  mem ~== size^2 * #cached_glyphs * 1.5
     *
     * Examples (for 128 cached glyphs):
     * - Size: 64 pixels: 768 KB (mipmapped) or 512 KB (non-mipmapped).
     * - Size 128 pixels: 3 MB (mipmapped) or 2 MB (non-mipmapped)
     *
     * Note: once a glyph is cached there will
     */
    TFTScalableFont(const std::filesystem::path Filename,
		int Size, float OutsetAmount = 0.f,
		bool UseMipmaps= true,
		bool PreCache= true);

    void AddFallback(const std::filesystem::path Filename) override;

    /** @seealso TGlyphCache.FlushCache */
    void FlushCache(bool KeepBaseSet);

    /** Outset size (in pixels) of the scaled font */
    [[nodiscard]] float Outset() const { return GetOutset(); }
};

  
  /**
   * Represents a freetype font with an additional outline around its glyphs.
   * The outline size is passed on creation && cannot be changed later.
   */
class TFTOutlineFont : public TFont
{
private:
    int fSize;
    float fOutset;
    std::shared_ptr<TFTFont> fInnerFont, fOutlineFont;
    TGLColor fOutlineColor;
    bool fPreCache;

    // ReSharper disable once CppHidingFunction
    void ResetIntern();

protected:
    void DrawUnderline(const std::u32string Text) override;
    void Render(const std::u32string Text) override;
    TBoundsDbl BBox(const TUCS4StringArray Text, bool Advance) override;

    
    void LineSpacing(float Spacing) override;
    void GlyphSpacing(float Spacing) override;
    void ReflectionSpacing(float Spacing) override;
    void Style(TFontStyle Style) override;
    void UseKerning(bool Enable) override;
    void ReflectionPass(bool Enable) override;

public:
    TFTOutlineFont(const std::filesystem::path Filename,
		int Size, float Outset,
		bool PreCache = true,
        FT_Int32 LoadFlags = FT_LOAD_DEFAULT);
    ~TFTOutlineFont() override;

    /**
     * Sets the color of the outline.
     * If the alpha component is < 0, OpenGL's current alpha value will be
     * used.
     */
    void SetOutlineColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = -1.f);

    /** @seealso TGlyphCache.FlushCache */
    void FlushCache(bool KeepBaseSet);

    void AddFallback(const std::filesystem::path Filename) override;

    /** @seealso TFont::Reset */
    void Reset() override;

    /** Size of the base font */
    [[nodiscard]] int Size() const { return fSize; }
    /** Outset size */
    [[nodiscard]] float Outset() const { return fOutset; }

    [[nodiscard]] float Height() const override;
    [[nodiscard]] float Ascender() const override;
    [[nodiscard]] float Descender() const override;
    [[nodiscard]] TFontStyle Style() const override;
    [[nodiscard]] float UnderlinePosition() const override;
    [[nodiscard]] float UnderlineThickness() const override;
};

  /**
   * Wrapper around TOutlineFont to allow font resizing.
   * @seealso TScalableFont
   */
class TFTScalableOutlineFont : public TScalableFont
{
protected:
    
    std::shared_ptr<TFont> CreateMipmap(int Level, float Scale) override;

public:
    TFTScalableOutlineFont(const std::filesystem::path Filename,
		int Size, float OutsetAmount,
		bool UseMipmaps = true,
		bool PreCache = true);

    /** @seealso TFTOutlineFont::SetOutlineColor */
    void OutlineColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = -1.f);

    /** @seealso TGlyphCache.FlushCache */
    void FlushCache(bool KeepBaseSet);

    void AddFallback(const std::filesystem::path Filename) override;

    /** Outset size */
    [[nodiscard]] virtual float Outset() const;
};

#ifdef BITMAP_FONT
  /**
   * A bitmapped font loads it's glyphs from a bitmap && stores them in a
   * texture. Unicode characters are not supported (but could be by supporting
   * multiple textures each storing a subset of unicode glyphs).
   * For backward compatibility only.
   */
class TBitmapFont : public TFont
{
private:
    TTexture fTex;
    int fTexSize;
    int fBaseline;
    int fAscender;
    int fDescender;
    std::array<uint8_t, 256> fWidths; //**< half widths
    int fOutline;
    TGLColor fTempColor; //**< colours for the reflection

    // ReSharper disable once CppHidingFunction
    void ResetIntern();

    void RenderChar(char32_t ch, double& AdvanceX);

    /**
     * Load font widths from an info file.
     * @param  InfoFile  the name of the info (.dat) file
     * @raises EFontError if the file is corrupted
     */
    void LoadFontInfo(const std::filesystem::path InfoFile);

protected:
    void Render(const std::u32string Text) override;
    TBoundsDbl BBox(const TUCS4StringArray Text, bool Advance) override;

    [[nodiscard]] float Height() const override;
    [[nodiscard]] float Ascender() const override;
    [[nodiscard]] float Descender() const override;
    [[nodiscard]] float UnderlinePosition() const override;
    [[nodiscard]] float UnderlineThickness() const override;

public:
    /**
     * Creates a bitmapped font from image Filename && font width info
     * loaded from the corresponding file with ending .dat.
     * @param Baseline  y-coord of the baseline given in cartesian coords
     *        (y-axis up) && from the lower edge of the glyphs bounding box
     * @param Ascender  pixels from baseline to top of highest glyph
     */
    TBitmapFont(const std::filesystem::path Filename, int Outline,
        int Baseline, int Ascender, int Descender);
    ~TBitmapFont() override;

    /**
     * Corrects font widths provided by the info file.
     * NewWidth = Width * WidthMult + WidthAdd
     */
    void CorrectWidths(double WidthMult, int WidthAdd);

    /** @seealso TFont::Reset */
    void Reset() override;

    void AddFallback(const std::filesystem::path Filename) override;
};

#endif

class TFreeType
{
public:

    /**
     * Creates a wrapper for the freetype library singleton
     * If non exists, freetype will be initialized.
     * @raises EFontError if initialization failed
     */
    TFreeType()
    {
		if (!LibraryInst)
		{
            // initialize freetype
            if (FT_Init_FreeType(&LibraryInst) != 0)
                throw EFontError("FT_Init_FreeType failed");
		}
        ++instances;
    }

    ~TFreeType()
    {
        --instances;
        if (instances == 0)
            FreeLibrary();
    }

    /**
     * Returns a pointer to the freetype library singleton.
     * If non exists, freetype will be initialized.
     * @raises EFontError if initialization failed
     */
    FT_Library GetLibrary() const;

private:

    inline static int instances = 0;
    static FT_Library LibraryInst;
    static void FreeLibrary();
};

inline TFreeType FreeTypeLib;

//uses Types;

//** shear factor used for the italic effect (bigger value -> more bending)
inline constexpr GLfloat cShearFactor = 0.25;
inline const std::array<GLfloat, 16> cShearMatrix =
{
  1,            0, 0, 0,
  cShearFactor, 1, 0, 0,
  0,            0, 1, 0,
  0,            0, 0, 1
};
inline const std::array<GLfloat, 16> cShearMatrixInv =
{
  1,             0, 0, 0,
  -cShearFactor, 1, 0, 0,
  0,             0, 1, 0,
  0,             0, 0, 1
};

/**
 * Returns the first power of 2 >= Value.
 */
inline int NextPowerOf2(int Value)
{
  int res = 1;
  while (res < Value)
    res <<= 1;
  return res;
}
}