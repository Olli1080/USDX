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

#include <gl/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <freetype/freetype.h>

namespace UFont
{
/*
{$IFNDEF FREETYPE_DEMO}
  // Flip direction of y-axis.
  // Default is a cartesian coordinate system with y-axis in upper direction
  // but with USDX the y-axis is in lower direction.
  {$DEFINE FLIP_YAXIS}
  {$DEFINE BITMAP_FONT}
{$ENDIF}

// Enables the Freetype font cache
{$DEFINE ENABLE_FT_FACE_CACHE}
*/


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

union TGLColor
{
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

class EFontError : public std::exception {};

/**
* Abstract base class representing a glyph.
*/
class TGlyph
{
protected:
	virtual TPositionDbl GetAdvance() const = 0;
	virtual TBoundsDbl GetBounds() const = 0;
public:
    virtual void Render(bool UseDisplayLists) = 0;
    virtual void RenderReflection() = 0;

    /** Distance to next glyph (in pixels) */
    TPositionDbl Advance() const { return GetAdvance(); }
    /** Glyph bounding box (in pixels) */
    TBoundsDbl Bounds() const { return GetBounds(); }
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
    virtual void SetReflectionPass(bool Enable);

public:

    TFont(const std::filesystem::path Filename);
    virtual ~TFont();

    /**
     * Prints a text.
     */
    template<typename CharType>
    void Print(const std::basic_string<CharType> Text);

    //void Print(const std::u32string Text);
    /** UTF-16 version of @link(Print) */
    //void Print(const std::wstring Text);
    /** UTF-8 version of @link(Print) */
    //void Print(const std::u8string Text);

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


    //TBoundsDbl BBox(const std::u32string Text, bool Advance = true);
    /** UTF-16 version of @link(BBox) */
    //TBoundsDbl BBox(const std::wstring Text, bool Advance = true);
    /** UTF-8 version of @link(BBox) */
    //TBoundsDbl BBox(const std::u8string Text, bool Advance = true);

    /**
     * Adds a new font that is used if the default font misses a glyph
     * @raises EFontError  if the fallback could not be initialized
     */
    virtual void AddFallback(const std::filesystem::path Filename) = 0;

    /** Font height */
    virtual void SetHeight(float Height) = 0;
    [[nodiscard]] virtual float GetHeight() const = 0;

    /** Vertical distance from baseline to top of glyph */
    [[nodiscard]] virtual float GetAscender() const = 0;
    /** Vertical distance from baseline to bottom of glyph */
    [[nodiscard]] virtual float GetDescender() const = 0;

    /** Vertical distance between two baselines */
    virtual void SetLineSpacing(float Spacing);
    [[nodiscard]] virtual float GetLineSpacing() const;

    /** Space between end and start of next glyph added to the advance width */
    virtual void SetGlyphSpacing(float Spacing);
    [[nodiscard]] virtual float GetGlyphSpacing() const;

    /** Distance between normal baseline && baseline of the reflection */
    virtual void SetReflectionSpacing(float Spacing);
    [[nodiscard]] virtual float GetReflectionSpacing() const;

    /** Font style (italic/underline/...) */
    virtual void SetStyle(TFontStyle Style);
    [[nodiscard]] virtual TFontStyle GetStyle() const;
    [[nodiscard]] virtual float GetUnderlinePosition() const = 0;
    [[nodiscard]] virtual float GetUnderlineThickness() const = 0;

    /** If set to true (default) kerning will be used if available */
    virtual void SetUseKerning(bool Enable);
    [[nodiscard]] virtual bool GetUseKerning() const;
    
    /** Filename */
    [[nodiscard]] std::filesystem::path Filename() const;
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
     * Mipmaps created by this method are managed && freed by TScalableFont::
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
     * The font passed is managed && freed by TScalableFont::
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
    //property Height: float read GetHeight write SetHeight;
    /** Factor for font stretching (NewWidth == Width*Stretch), 1.0 by default */
    //property Stretch: float read GetStretch write SetStretch;

    void SetHeight(float Height) override;
    float GetHeight() override;
    void SetStretch(float Stretch);
    float GetStretch() override;
    float GetAscender() override;
    float GetDescender() override;
    void SetLineSpacing(float Spacing) override;
    float GetLineSpacing() override;
    void SetGlyphSpacing(float Spacing) override;
    float GetGlyphSpacing() override;
    void SetReflectionSpacing(float Spacing) override;
    float GetReflectionSpacing() override;
    void SetStyle(TFontStyle Style) override;
    TFontStyle GetStyle() override;
    float GetUnderlinePosition() override;
    float GetUnderlineThickness() override;
    void SetUseKerning(bool Enable) override;
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
    TList fHash;

    /**
     * Finds a glyph-table storing cached glyphs with base-code BaseCode
     * (== upper char-code bytes) in the hash-list && returns the table &&
     * its index.
     * @param(InsertPos  the position of the tyble in the list if it was found,
     *                   otherwise the position the table should be inserted)
     */
    PGlyphTable FindGlyphTable(uint32_t BaseCode, int& InsertPos);

public:

    TGlyphCache();
    virtual ~TGlyphCache();

    /**
     * Add glyph Glyph with char-code ch to the cache.
     * @returns @true on success, @false otherwise
     */
    bool AddGlyph(char32_t ch, const TGlyph Glyph);

    /**
     * Removes the glyph with char-code ch from the cache.
     */
    void DeleteGlyph(char32_t ch);

    /**
     * Removes the glyph with char-code ch from the cache.
     */
    TGlyph GetGlyph(char32_t ch);

    /**
     * Checks if a glyph with char-code ch is cached.
     */
    bool HasGlyph(char32_t ch);

    /**
     * Remove && free all cached glyphs. If KeepBaseSet is set to
     * true, cached characters in the range 0..255 will not be flushed.
     */
    void FlushCache(bool KeepBaseSet);
};

/**
* Entry of a glyph-cache's (TGlyphCache) hash.
* Stores a BaseCode (upper-bytes of a glyph's char-code) && a table
* with all glyphs cached at the moment with that BaseCode.
*/
class TGlyphCacheHashEntry
{
private:
    uint32_t fBaseCode;
public:
    TGlyphTable GlyphTable;

    TGlyphCacheHashEntry(uint32_t BaseCode);

    /** Base-code (upper-bytes) of the glyphs stored in this entry's table */
    //property BaseCode: uint32_t read fBaseCode;
    uint32_t BaseCode() const { return fBaseCode; };
};

class TCachedFont : public TFont
{
protected:
    TGlyphCache fCache;

    /**
     * Retrieves a cached glyph with char-code ch from cache.
     * If the glyph is not already cached, it is loaded with LoadGlyph().
     */
    TGlyph GetGlyph(char32_t ch);

    /**
     * Callback to create (load) a glyph with char-code ch.
     * Implemented by subclasses.
     */
    virtual TGlyph LoadGlyph(char32_t ch) = 0;

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

class TFTFont;

/**
* Freetype font face class.
*/
class TFTFontFace
{
private:
    std::filesystem::path fFilename;             //**< filename of the font-file
	std::vector<uint8_t> byarr1;
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
    std::filesystem::path Filename() const { return fFilename;  }
    FT_Face Data() const { return fFace; }
    TPositionDbl FontUnitScale() const { return fFontUnitScale; }
    int Size() const { return fSize; }
};

/**
* Loading font faces with freetype is a slow process.
* Especially loading a font (e.g. fallback fonts) more than once is a waste
* of time. Just cache already loaded faces here.
*/
class TFTFontFaceCache
{
private:
    std::vector<TFTFontFace> fFaces;
	std::vector<int> fFacesRefCnt;
public:
    /**
     * @raises EFontError  if the font could not be initialized
     */
    TFTFontFace LoadFace(const std::filesystem::path Filename, int Size);

    void UnloadFace(TFTFontFace Face);
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

    TFTFont fFont;           //**< Font associated with this glyph
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
    TPositionDbl GetAdvance() override;
    TBoundsDbl GetBounds() override;

public:
    /**
     * Creates a glyph with char-code ch from font Font.
     * @param LoadFlags  flags passed to FT_Load_Glyph()
     */
    TFTGlyph(TFTFont Font, char32_t ch, float Outset, FT_Int32 LoadFlags);
    ~TFTGlyph() override;

    /** Renders the glyph (normal render pass) */
    void Render(bool UseDisplayLists) override;
    /** Renders the glyph's reflection */
    void RenderReflection() override;

    /** Freetype specific char-index (!= char-code) */
    FT_UInt CharIndex() const { return fCharIndex; }

    /** Freetype face used for this glyph */
    TFTFontFace Face() const { return fFace; }
};

enum TFontPart { fpNone, fpInner, fpOutline };
typedef std::vector< TFTFontFace> TFTFontFaceArray;

/**
* Freetype font class.
*/
class TFTFont : public TCachedFont
{
private:
    void ResetIntern();
    static TFTFontFaceCache GetFaceCache();

protected:
    TFTFontFace fFace;           //**< Default font face
    int fSize;               //**< Font base size (in pixels)
    float fOutset;              //**< size of outset extrusion (in pixels)
    bool fPreCache;           //**< pre-load base glyphs
    FT_Int32 fLoadFlags;         //**< FT glpyh load-flags
    bool fUseDisplayLists;    //**< true: use display-lists, false: direct drawing
    TFontPart fPart;             //**< indicates the part of an outline font
    TFTFontFaceArray fFallbackFaces; //**< available fallback faces, ordered by priority

    /** @seealso TCachedFont.LoadGlyph */
    TGlyph LoadGlyph(char32_t ch) override;

    void Render(const std::u32string Text) override;
    TBoundsDbl BBox(const TUCS4StringArray Text , bool Advance) override;

    float GetHeight() override;
    float GetAscender() override;
    float GetDescender() override;
    float GetUnderlinePosition() override;
    float GetUnderlineThickness() override;

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
    property Size: int read fSize;
    /** Outset size */
    property Outset: float read fOutset;
    /** The part (inner/outline/none) this font represents in a composite font */
    property Part: TFontPart read fPart write fPart;
    /** Freetype face of this font */
    property DefaultFace: TFTFontFace read fFace;
    /** Available freetype fallback faces, ordered by priority */
    property FallbackFaces: TFTFontFaceArray read fFallbackFaces;
};

class TFTScalableFont : public TScalableFont
{
protected:
    function GetOutset() : float; virtual;
    function CreateMipmap(Level: int; Scale: float) : TFont; override;

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
    constructor Create(const Filename : std::filesystem::path;
		Size: int; OutsetAmount: float == 0.0;
		UseMipmaps: bool == true;
		PreCache: bool == true);

    void AddFallback(const Filename : std::filesystem::path); override;

    /** @seealso TGlyphCache.FlushCache */
    void FlushCache(KeepBaseSet: bool);

    /** Outset size (in pixels) of the scaled font */
    property Outset: float read GetOutset;
};

  
  /**
   * Represents a freetype font with an additional outline around its glyphs.
   * The outline size is passed on creation && cannot be changed later.
   */
class TFTOutlineFont : public TFont
{
private:
    fSize : int;
	fOutset: float;
	fInnerFont, fOutlineFont: TFTFont;
	fOutlineColor: TGLColor;
	fPreCache: bool;

    void ResetIntern();

protected:
    void DrawUnderline(const Text : std::u32string); override;
    void Render(const Text : std::u32string); override;
    function BBox(const Text : TUCS4StringArray; Advance: bool) : TBoundsDbl; override;

    function GetHeight() : float; override;
    function GetAscender() : float; override;
    function GetDescender() : float; override;
    void SetLineSpacing(Spacing: float); override;
    void SetGlyphSpacing(Spacing: float); override;
    void SetReflectionSpacing(Spacing: float); override;
    void SetStyle(Style: TFontStyle); override;
    function GetStyle() : TFontStyle; override;
    function GetUnderlinePosition() : float; override;
    function GetUnderlineThickness() : float; override;
    void SetUseKerning(Enable: bool); override;
    void SetReflectionPass(Enable: bool); override;

public:
    constructor Create(const Filename : std::filesystem::path;
	Size: int; Outset: float;
	PreCache: bool == true;
	LoadFlags: FT_Int32 == FT_LOAD_DEFAULT);
    destructor Destroy; override;

    /**
     * Sets the color of the outline.
     * If the alpha component is < 0, OpenGL's current alpha value will be
     * used.
     */
    void SetOutlineColor(r, g, b: GLfloat; a: GLfloat == -1.0);

    /** @seealso TGlyphCache.FlushCache */
    void FlushCache(KeepBaseSet: bool);

    void AddFallback(const Filename : std::filesystem::path); override;

    /** @seealso TFont::Reset */
    void Reset(); override;

    /** Size of the base font */
    property Size: int read fSize;
    /** Outset size */
    property Outset: float read fOutset;
};

  /**
   * Wrapper around TOutlineFont to allow font resizing.
   * @seealso TScalableFont
   */
class TFTScalableOutlineFont : public TScalableFont
{
protected:
    function GetOutset() : float; virtual;
    function CreateMipmap(Level: int; Scale: float) : TFont; override;

public:
    constructor Create(const Filename : std::filesystem::path;
	Size: int; OutsetAmount: float;
	UseMipmaps: bool == true;
	PreCache: bool == true);

    /** @seealso TFTOutlineFont.SetOutlineColor */
    void SetOutlineColor(r, g, b: GLfloat; a: GLfloat == -1.0);

    /** @seealso TGlyphCache.FlushCache */
    void FlushCache(KeepBaseSet: bool);

    void AddFallback(const Filename : std::filesystem::path); override;

    /** Outset size */
    property Outset: float read GetOutset;
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
    fTex : TTexture;
	fTexSize:   int;
	fBaseline:  int;
	fAscender:  int;
	fDescender: int;
	fWidths:    array[0..255] of byte; //**< half widths
	fOutline:   int;
	fTempColor: TGLColor; //**< colours for the reflection

    void ResetIntern();

    void RenderChar(ch: UCS4Char; var AdvanceX : real);

    /**
     * Load font widths from an info file.
     * @param  InfoFile  the name of the info (.dat) file
     * @raises EFontError if the file is corrupted
     */
    void LoadFontInfo(const InfoFile : std::filesystem::path);

protected:
    void Render(const Text : std::u32string); override;
    function BBox(const Text : TUCS4StringArray; Advance: bool) : TBoundsDbl; override;

    function GetHeight() : float; override;
    function GetAscender() : float; override;
    function GetDescender() : float; override;
    function GetUnderlinePosition() : float; override;
    function GetUnderlineThickness() : float; override;

public:
    /**
     * Creates a bitmapped font from image Filename && font width info
     * loaded from the corresponding file with ending .dat.
     * @param(Baseline  y-coord of the baseline given in cartesian coords
     *        (y-axis up) && from the lower edge of the glyphs bounding box)
     * @param(Ascender  pixels from baseline to top of highest glyph)
     */
    constructor Create(const Filename : std::filesystem::path; Outline: int;
    Baseline, Ascender, Descender: int);
    destructor Destroy(); override;

    /**
     * Corrects font widths provided by the info file.
     * NewWidth = Width * WidthMult + WidthAdd
     */
    void CorrectWidths(WidthMult: real; WidthAdd: int);

    /** @seealso TFont::Reset */
    void Reset(); override;

    void AddFallback(const Filename : std::filesystem::path); override;
};

#endif

class TFreeType
{
public:
    /**
     * Returns a pointer to the freetype library singleton.
     * If non exists, freetype will be initialized.
     * @raises EFontError if initialization failed
     */
    static FT_Library GetLibrary();
    static void FreeLibrary();
};

//uses Types;

//** shear factor used for the italic effect (bigger value -> more bending)
const GLfloat cShearFactor = 0.25;
std::array<GLfloat, 16> cShearMatrix =
{
  1,            0, 0, 0,
  cShearFactor, 1, 0, 0,
  0,            0, 1, 0,
  0,            0, 0, 1
};
std::array<GLfloat, 16> cShearMatrixInv =
{
  1,             0, 0, 0,
  -cShearFactor, 1, 0, 0,
  0,             0, 1, 0,
  0,             0, 0, 1
};


inline FT_Library LibraryInst;

TGLColor NewGLColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    return { r, g, b, a };
}

/**
 * Returns the first power of 2 >= Value.
 */
inline int NextPowerOf2(int Value)
{
  int res = 1;
  while (res < Value)
    res <<= 1;
}


/*
 * TFont
 */

TFont::TFont(const std::filesystem::path Filename)
{
  inherited Create();
  fFilename = Filename;
  ResetIntern();
}

inline TFont::~TFont()
{
    inherited
}

void TFont::ResetIntern()
{
    fStyle = {};
    fUseKerning = true;
    fGlyphSpacing = 0.0;
    fReflectionPass = false;

    // must be set by subclasses
    fLineSpacing = 0.0;
    fReflectionSpacing = 0.0;
}

void TFont::Reset()
{
  ResetIntern();
}

void TFont::SplitLines(const std::u32string Text, TUCS4StringArray& Lines)
{
    // split lines on newline
    Lines.resize(0);

    auto ststr = std::stringstream<char32_t>(Text);
    for (std::u32string line; std::getline(ststr, line, '\n');)
        Lines.push_back(line);
}

template<>
TBoundsDbl TFont::BBox(const std::u32string Text, bool Advance)
{
  TUCS4StringArray LineArray;
  SplitLines(Text, LineArray);
  return BBox(LineArray, Advance);
}

template<typename TextType>
TBoundsDbl TFont::BBox(const std::basic_string<TextType> Text, bool Advance)
{
  return BBox(std::codecvt<char32_t, TextType, std::mbstate_t>(Text), Advance);
}

std::filesystem::path TFont::Filename() const
{
    return fFilename;
}

void TFont::Print(const TUCS4StringArray Text)
/*var
  LineIndex: int;*/
{
    // recursively call this function to draw reflected text
    if (GetStyle().contains(Reflect) && !fReflectionPass)
    {
        SetReflectionPass(true);
        Print(Text);
        SetReflectionPass(false);
    }

    // store current color, enable-flags, matrix-mode
    glPushAttrib(GL_CURRENT_BIT or GL_ENABLE_BIT or GL_TRANSFORM_BIT);

    // set OpenGL state
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    {
        // TODO: just draw texels with alpha > 0 to avoid setting z-buffer for them?
        glAlphaFunc(GL_GREATER, 0);
        glEnable(GL_ALPHA_TEST);

        //TODO: Do we need depth-testing?
        if (fReflectionPass)
        {
            glDepthMask(0);
            glEnable(GL_DEPTH_TEST);
        }
    }

#ifdef FLIP_YAXIS
    glPushMatrix();
    glScalef(1, -1, 1);
#endif

    // display text
    for (int LineIndex = 0; LineIndex < Text.size(); ++LineIndex)
    {
        glPushMatrix();

        // move to baseline
        glTranslatef(0, -GetLineSpacing() * LineIndex, 0);

        if (GetStyle().contains(Underline) && !fReflectionPass)
        {
            glDisable(GL_TEXTURE_2D);
            DrawUnderline(Text[LineIndex]);
            glEnable(GL_TEXTURE_2D);
        }

        // draw reflection
        if (fReflectionPass)
        {
            // set reflection spacing
            glTranslatef(0, -GetReflectionSpacing(), 0);
            // flip y-axis
            glScalef(1, -1, 1);
        }

        // shear for italic effect
        if (GetStyle().contains(Italic))
            glMultMatrixf(cShearMatrix.data());

        // render text line
        Render(Text[LineIndex]);

        glPopMatrix();
    }

    // restore settings
#ifdef FLIP_YAXIS
    glPopMatrix();
#endif
    glPopAttrib();
}

template<>
void TFont::Print(const std::u32string Text)
{
    TUCS4StringArray LineArray;
    SplitLines(Text, LineArray);
    Print(LineArray);
}

template<typename CharType>
void TFont::Print(const std::basic_string<CharType> Text)
{
  Print(std::codecvt<char32_t, CharType, std::mbstate_t>(Text));
}

void TFont::DrawUnderline(const std::u32string Text)
{
  const float UnderlineY1 = GetUnderlinePosition();
  const float UnderlineY2 = UnderlineY1 + GetUnderlineThickness();
  const auto Bounds = BBox(Text, false);
  glRectf(Bounds.Left, UnderlineY1, Bounds.Right, UnderlineY2);
}

void TFont::SetStyle(TFontStyle Style)
{
  fStyle = Style;
}

TFontStyle TFont::GetStyle() const
{
  return fStyle;
}

void TFont::SetLineSpacing(float Spacing)
{
  fLineSpacing = Spacing;
}

float TFont::GetLineSpacing() const
{
  return fLineSpacing;
}

void TFont::SetGlyphSpacing(float Spacing)
{
  fGlyphSpacing = Spacing;
}

float TFont::GetGlyphSpacing() const
{
  return fGlyphSpacing;
}

void TFont::SetReflectionSpacing(float Spacing)
{
  fReflectionSpacing = Spacing;
}

float TFont::GetReflectionSpacing() const
{
  return fReflectionSpacing;
}

void TFont::SetUseKerning(bool Enable)
{
  fUseKerning = Enable;
}

bool TFont::GetUseKerning() const
{
  return fUseKerning;
}

void TFont::SetReflectionPass(bool Enable)
{
  fReflectionPass = Enable;
}


/*
 * TScalableFont
 */

TScalableFont::TScalableFont(std::shared_ptr<TFont> Font, bool UseMipmaps) : TFont(Font->Filename())
/*var
  MipmapLevel: int;*/
{  
  fBaseFont = Font;
  fMipmapFonts[0] = Font;
  fUseMipmaps = UseMipmaps;
  ResetIntern();

  // create mipmap fonts if requested
  if (fUseMipmaps)
  {
    for (int MipmapLevel = 1; MipmapLevel < cMaxMipmapLevel; ++MipmapLevel)
    {
      fMipmapFonts[MipmapLevel] = CreateMipmap(MipmapLevel, 1 / (1 << MipmapLevel));
      // stop if no smaller mipmap font is returned
      if (!fMipmapFonts[MipmapLevel])
        break;
    }
  }
}

TScalableFont::~TScaleableFont();
/*var
  Level: int;*/
{
  for Level = 0 to High(fMipmapFonts) do
    fMipmapFonts[Level].Free;
  inherited;
}

void TScalableFont::ResetIntern();
{
  fScale = 1.0;
  fStretch = 1.0;
}

void TScalableFont::Reset();
var
  Level: int;
{
  inherited;
  ResetIntern();
  for Level = 0 to High(fMipmapFonts) do
    if (fMipmapFonts[Level] != nil) then
      fMipmapFonts[Level].Reset();
}

/**
 * Returns the mipmap level to use with regard to the current projection
 * && modelview matrix, font scale && stretch.
 *
 * Note:
 * - for Freetype fonts, hinting && grid-fitting must be disabled, otherwise
 *   the glyph widths/heights ratios && advance widths of the mipmap fonts
 *   do not match as they are adjusted sligthly (e.g. an 'a' at size 12px has
 *   width 12px, but at size 6px width 8px).
 * - returned mipmap-level is used for all glyphs of the current text to print.
 *   This is faster, much easier to handle, since we just need to create
 *   multiple sized fonts && select the one we need for the mipmap-level &&
 *   it avoids that neighbored glyphs use different mipmap-level which might
 *   look odd because one glyph might look blurry && the other sharp.
 *
 * Motivation:
 *   We do not use OpenGL for mipmapping as the results are very bad. At least
 *   with automatic mipmap generation (gluBuildMipmaps) the fonts look rather
 *   blurry.
 *   Defining our own mipmaps by creating multiple textures with
 *   for different mimap levels is a pain, as the font size passed to freetype
 *   is not the size of the bitmaps created && it does not guarantee that a
 *   glyph bitmap of a font with font-size s/2 is half the size of the font with
 *   font-size s. If the bitmap size is just a float pixel bigger than the half
 *   we might need a texture of the next power-of-2 && the texture would not be
 *   half of the size of the next bigger mipmap. In addition we use a fixed one
 *   pixel sized border to smooth the texture (see cTexSmoothBorder) && maybe
 *   an outset that is added to the font, so creating a glyph mipmap that is
 *   exactly half the size of the next bigger one is a very difficult task.
 *
 * Solution:
 *   Use mipmap textures that are not exactly half the size of the next mipmap
 *   level. OpenGL does not support this (at least not without extensions).
 *   The trickiest task is to determine the mipmap to use by calculating the
 *   amount of minification that is performed in this function.
 */
int TScalableFont::GetMipmapLevel()
/*var
  ModelMatrix, ProjMatrix: TGLMatrixd4;
  ViewPortArray: TGLVectori4;
  Dist, Dist2, DistSum, PM15x2: double;
  WidthScale, HeightScale: double;*/
{
    // an offset to the mipmap-level to adjust the change-over of two consecutive
  // mipmap levels. If for example the bias is 0.1 && unbiased level is 1.9
  // the result level will be 2. A bias of 0.5 is equal to rounding.
  // With bias=0.1 we prefer larger mipmaps over smaller ones.
    const float cBias = 0.2f;
    glm::mat4 ModelMatrix;
    glm::mat4 ProjMatrix;
    glm::vec<4, int> ViewPortArray;

  // 1. retrieve current transformation matrices for gluProject
  glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(ModelMatrix));
  glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(ProjMatrix));
  glGetIntegerv(GL_VIEWPORT, glm::value_ptr(ViewPortArray));


  // See 6545ededd512ea7c7dc5c08a1ef96397afdcbe49 for the original
  // code using gluProject() that has been simplified.
  double PM15x2 = 2*abs(ModelMatrix[3][0] * ProjMatrix[0][3]
                 +ModelMatrix[3][1]*ProjMatrix[1][3]
                 +ModelMatrix[3][2]*ProjMatrix[2][3]
                 +ModelMatrix[3][3]*ProjMatrix[3][3]);

  double Dist = ViewPortArray[2]*(ModelMatrix[0][0]*ProjMatrix[0][0]
                            +ModelMatrix[0][1]*ProjMatrix[1][0]
                            +ModelMatrix[0][2]*ProjMatrix[2][0]
                            +ModelMatrix[0][3]*ProjMatrix[3][0]);
  double Dist2 = ViewPortArray[3]*(ModelMatrix[0][0]*ProjMatrix[0][1]
                            +ModelMatrix[0][1]*ProjMatrix[1][1]
                            +ModelMatrix[0][2]*ProjMatrix[2][1]
                            +ModelMatrix[0][3]*ProjMatrix[3][1]);
  double WidthScale = 1;
  double DistSum = Dist*Dist + Dist2*Dist2;
  if (DistSum > 0)
  {
    WidthScale = PM15x2 / std::sqrt(DistSum);
  }

  Dist  = ViewPortArray[2] * (ModelMatrix[1][0]*ProjMatrix[0][0]
                            +ModelMatrix[1][1]*ProjMatrix[1][0]
                            +ModelMatrix[1][2]*ProjMatrix[2][0]
                            +ModelMatrix[1][3]*ProjMatrix[3][0]);
  Dist2 = ViewPortArray[3] * (ModelMatrix[1][0]*ProjMatrix[0][1]
                            +ModelMatrix[1][1]*ProjMatrix[1][1]
                            +ModelMatrix[1][2]*ProjMatrix[2][1]
                            +ModelMatrix[1][3]*ProjMatrix[3][1]);

  double HeightScale = 1;
  DistSum = Dist*Dist + Dist2*Dist2;
  if (DistSum > 0)
  {
    HeightScale = PM15x2 / std::sqrt(DistSum);
  }

  //writeln(Format('Scale %f, %f', [WidthScale, HeightScale]));

  // 4. Now that we have got the scale, take the bigger minification scale
  // && get it to a logarithmic scale as each mipmap is 1/2 the size of its
  // predecessor (Mipmap_size[i] == Mipmap_size[i-1]/2).
  // The result is our mipmap-level == the index of the mipmap to use.

  // Level > 0: Minification; < 0: Magnification
  Result = Trunc(Log2(Max(WidthScale, HeightScale)) + cBias);

  // clamp to valid range
  if (Result < 0) then
    Result = 0;
  if (Result > High(fMipmapFonts)) then
    Result = High(fMipmapFonts);
}

std::optional<float> TScalableFont::GetMipmapScale(int Level) const
{
  if (!fMipmapFonts[Level])
      return std::nullopt;

  return fScale * fMipmapFonts[0]->GetHeight() / fMipmapFonts[Level]->GetHeight();
}

/**
 * Returns the correct mipmap font for the current scale && projection
 * matrix. The modelview scale is adjusted to the mipmap level, so
 * Result.Print() will display the font in the correct size.
 */
std::shared_ptr<TFont> TScalableFont::ChooseMipmapFont()
/*var
  DesiredLevel: int;
  Level: int;
  MipmapScale: float;*/
{
    std::shared_ptr<TFont> Result = nullptr;
    const int DesiredLevel = GetMipmapLevel();

    // get the smallest mipmap available for the desired level
    // as not all levels must be assigned to a font.
    for (int Level = DesiredLevel; Level >= 0; --Level)
    {
        if (fMipmapFonts[Level])
        {
            Result = fMipmapFonts[Level];
            break;
        }
    }

    // since the mipmap font (if level > 0) is smaller than the base-font
    // we have to scale to get its size right.
    const float MipmapScale = fMipmapFonts[0]->GetHeight() / Result->GetHeight();
    glScalef(MipmapScale, MipmapScale, 0);
}

void TScalableFont::Print(const TUCS4StringArray Text)
{
    glPushMatrix();

    // set scale && stretching
    glScalef(fScale * fStretch, fScale, 0);

    // print text
    if (fUseMipmaps)
        ChooseMipmapFont().Print(Text);
    else
        fBaseFont.Print(Text);

    glPopMatrix();
}

void TScalableFont::Render(const std::u32string Text)
{
  Assert(false, 'Unused TScalableFont::Render() was called');
}

TBoundsDbl TScalableFont::BBox(const TUCS4StringArray Text, bool Advance)
{
    auto res = fBaseFont.BBox(Text, Advance);
    res.Left *= fScale * fStretch;
    res.Right *= fScale * fStretch;
    res.Top *= fScale;
    res.Bottom *= fScale;

    return res;
}

void TScalableFont::SetHeight(float Height)
{
  fScale = Height / fBaseFont.GetHeight();
}

float TScalableFont::GetHeight()
{
  return fBaseFont.GetHeight() * fScale;
}

void TScalableFont::SetStretch(float Stretch)
{
  fStretch = Stretch;
}

float TScalableFont::GetStretch()
{
    return fStretch;
}

float TScalableFont::GetAscender()
{
    return fBaseFont.GetAscender() * fScale;
}

float TScalableFont::GetDescender()
{
    return fBaseFont.GetDescender() * fScale;
}

void TScalableFont::SetLineSpacing(float Spacing)
{
  for (int Level = 0; Level < fMipmapFonts.size(); ++Level)
    if (fMipmapFonts[Level])
      fMipmapFonts[Level]->SetLineSpacing(Spacing / GetMipmapScale(Level));
}

float TScalableFont::GetLineSpacing()
{
  Result = fBaseFont.GetLineSpacing() * fScale;
}

void TScalableFont::SetGlyphSpacing(float Spacing)
/*var
  Level: int;*/
{
  for Level = 0 to High(fMipmapFonts) do
    if (fMipmapFonts[Level] != nil) then
      fMipmapFonts[Level].SetGlyphSpacing(Spacing / GetMipmapScale(Level));
}

float TScalableFont::GetGlyphSpacing()
{
  Result = fBaseFont.GetGlyphSpacing() * fScale;
}

void TScalableFont::SetReflectionSpacing(float Spacing);
/*var
  Level: int;*/
{
  for Level = 0 to High(fMipmapFonts) do
    if ((fMipmapFonts[Level] != nil) AND (GetMipmapScale(Level) > 0)) then
      fMipmapFonts[Level].SetReflectionSpacing(Spacing / GetMipmapScale(Level));
}

function TScalableFont::GetReflectionSpacing(): float;
{
  Result = fBaseFont.GetLineSpacing() * fScale;
}

void TScalableFont::SetStyle(Style: TFontStyle);
var
  Level: int;
{
  for Level = 0 to High(fMipmapFonts) do
    if (fMipmapFonts[Level] != nil) then
      fMipmapFonts[Level].SetStyle(Style);
}

function TScalableFont::GetStyle(): TFontStyle;
{
  Result = fBaseFont.GetStyle();
}

function TScalableFont::GetUnderlinePosition(): float;
{
  Result = fBaseFont.GetUnderlinePosition();
}

function TScalableFont::GetUnderlineThickness(): float;
{
  Result = fBaseFont.GetUnderlinePosition();
}

void TScalableFont::SetUseKerning(Enable: bool);
var
  Level: int;
{
  for Level = 0 to High(fMipmapFonts) do
    if (fMipmapFonts[Level] != nil) then
      fMipmapFonts[Level].SetUseKerning(Enable);
}


/*
 * TCachedFont
 */

constructor TCachedFont.Create(const Filename: std::filesystem::path);
{
  inherited Create(Filename);
  fCache = TGlyphCache.Create();
}

destructor TCachedFont.Destroy();
{
  fCache.Free;
  inherited;
}

function TCachedFont.GetGlyph(ch: UCS4Char): TGlyph;
{
  Result = fCache.GetGlyph(ch);
  if (Result == nil) then
  {
    Result = LoadGlyph(ch);
    if (not fCache.AddGlyph(ch, Result)) then
      Result.Free;
  }
}

void TCachedFont.FlushCache(KeepBaseSet: bool);
{
  fCache.FlushCache(KeepBaseSet);
}

/*
 * TFTFontFaceCache
 */

/*
 * TFTFontFace
 */

 TFTFontFace::TFTFontFace(const std::filesystem::path Filename, int Size)
 /*var
   {$IF Defined(MSWINDOWS)}
   SourceFile: TFileStreamUTF8;
   {$IFEND}
   lengthvar: Integer;
   b1: Integer;
   arraylength: Int64;*/
 {
   {$IF Defined(MSWINDOWS)}
   SourceFile = nil;
   {$IFEND}
   b1=3;

   fFilename = Filename;
   fSize = Size;
   try
     {$IF Defined(MSWINDOWS)}
     SourceFile = TFileStreamUTF8.Create(Filename.ToUTF8(true), fmOpenRead);
     arraylength=SourceFile.Size;
     SetLength(byarr1, arraylength+1);
     SourceFile.Read(byarr1[0], arraylength);
     b1 = FT_New_Memory_Face(TFreeType.GetLibrary(),@byarr1[0],arraylength,0,fFace);
     {$ELSE}
     b1 = FT_New_Face(TFreeType.GetLibrary(), PChar(Filename.ToNative), 0, fFace);
     {$IFEND}
   except
     raise EFontError.Create("FT_New_Face: Error - Could not load font file to memory on Windows \""  + Filename.string() + "\"");
   }
   {$IF Defined(MSWINDOWS)}
   SourceFile.Destroy;
   {$IFEND}
   // load font information
   if (b1 != 0)
     raise EFontError.Create("FT_New_Face: Could not load font """  + Filename.ToNative + """");

  // support scalable fonts only
  if (!FT_IS_SCALABLE(fFace))
    raise EFontError.Create("Font is not scalable");

  if (FT_Set_Pixel_Sizes(fFace, 0, Size) != 0) then
    raise EFontError.Create("FT_Set_Pixel_Sizes failes");

  // get scale factor for font-unit to pixel-size transformation
  fFontUnitScale.X = fFace.size.metrics.x_ppem / fFace.units_per_EM;
  fFontUnitScale.Y = fFace.size.metrics.y_ppem / fFace.units_per_EM;
}

TFTFontFace::~TFTFontFace()
{
  // free face data
  FT_Done_Face(fFace);
  byarr1=nil;
  inherited;
}


/*
 * TFTFontFaceCache
 */

TFTFontFace TFTFontFaceCache::LoadFace(const Filename: std::filesystem::path; Size: int)
var
  I: Integer;
  Face: TFTFontFace;
{
  {$IFDEF ENABLE_FT_FACE_CACHE}
  for I = 0 to High(fFaces) do
  {
    Face = fFaces[I];
    // check if we have this file in our cache
    if ((Face.Filename.Equals(Filename)) && (Face.Size == Size)) then
    {
      // true -> return cached face && increment ref-count
      Inc(fFacesRefCnt[I]);
      Result = Face;
      Exit;
    }
  }
  {$ENDIF}

  // face not in cache -> load it
  Face = TFTFontFace::Create(Filename, Size);

  // add face to cache
  SetLength(fFaces, Length(fFaces)+1);
  SetLength(fFacesRefCnt, Length(fFaces)+1);
  fFaces[High(fFaces)] = Face;
  fFacesRefCnt[High(fFaces)] = 1;

  Result = Face;
}

void TFTFontFaceCache::UnloadFace(Face: TFTFontFace);
var
  I: Integer;
{
  for I = 0 to High(fFaces) do
  {
    // search face in cache
    if (fFaces[I] == Face) then
    {
      // decrement ref-count && free face if ref-count is 0
      Dec(fFacesRefCnt[I]);
      if (fFacesRefCnt[I] <== 0) then
        fFaces[I].Free;
      Exit;
    }
  }
}


/*
 * TFTFont
 */

constructor TFTFont::Create(
    const Filename: std::filesystem::path;
    Size: int; Outset: float;
    PreCache: bool;
    LoadFlags: FT_Int32);
var
  ch: UCS4Char;
{
  inherited Create(Filename);

  fSize = Size;
  fOutset = Outset;
  fPreCache = PreCache;
  fLoadFlags = LoadFlags;
  fUseDisplayLists = true;
  fPart = fpNone;

  fFace = GetFaceCache.LoadFace(Filename, Size);

  ResetIntern();

  // pre-cache some commonly used glyphs (' ' - '~')
  if (PreCache) then
  {
    for ch = 32 to 126 do
      fCache.AddGlyph(ch, TFTGlyph::Create(Self, ch, Outset, LoadFlags));
  }
}

destructor TFTFont::Destroy();
var
  I: int;
{
  // free faces
  GetFaceCache.UnloadFace(fFace);
  for I = 0 to High(fFallbackFaces) do
    GetFaceCache.UnloadFace(fFallbackFaces[I]);    

  inherited;
}

var
  FontFaceCache: TFTFontFaceCache == nil;

class function TFTFont::GetFaceCache(): TFTFontFaceCache;
{
  if (FontFaceCache == nil) then
    FontFaceCache = TFTFontFaceCache::Create;
  Result = FontFaceCache;
}

void TFTFont::ResetIntern();
{
  // Note: outset && non outset fonts use same spacing
  fLineSpacing = fFace.Data.height * fFace.FontUnitScale.Y;
  fReflectionSpacing = -2*fFace.Data.descender * fFace.FontUnitScale.Y;
}

void TFTFont::Reset();
{
  inherited;
  ResetIntern();
}

void TFTFont::AddFallback(const Filename: std::filesystem::path);
var
  FontFace: TFTFontFace;
{
  FontFace = GetFaceCache.LoadFace(Filename, Size);
  SetLength(fFallbackFaces, Length(fFallbackFaces) + 1);
  fFallbackFaces[High(fFallbackFaces)] = FontFace;
}

function TFTFont::LoadGlyph(ch: UCS4Char): TGlyph;
{
  Result = TFTGlyph::Create(Self, ch, Outset, fLoadFlags);
}

function TFTFont::BBox(const Text: TUCS4StringArray; Advance: bool): TBoundsDbl;
var
  Glyph, PrevGlyph: TFTGlyph;
  TextLine: std::u32string;
  LineYOffset: float;
  LineIndex, CharIndex: int;
  LineBounds: TBoundsDbl;
  KernDelta: FT_Vector;
  UnderlinePos: double;
{
  // Reset global bounds
  Result.Left   = Infinity;
  Result.Right  = 0;
  Result.Bottom = Infinity;
  Result.Top    = 0;

  // reset last glyph
  PrevGlyph = nil;

  // display text
  for LineIndex = 0 to High(Text) do
  {
    // get next text line
    TextLine = Text[LineIndex];
    LineYOffset = -LineSpacing * LineIndex;

    // reset line bounds
    LineBounds.Left   = Infinity;
    LineBounds.Right  = 0;
    LineBounds.Bottom = Infinity;
    LineBounds.Top    = 0;

    // for each glyph image, compute its bounding box
    for CharIndex = 0 to LengthUCS4(TextLine)-1 do
    {
      Glyph = TFTGlyph(GetGlyph(TextLine[CharIndex]));
      if (Glyph != nil) then
      {
        // get kerning
        if (fUseKerning && FT_HAS_KERNING(fFace.Data) && (PrevGlyph != nil)) then
        {
          FT_Get_Kerning(fFace.Data, PrevGlyph.CharIndex, Glyph.CharIndex,
                         FT_KERNING_UNSCALED, KernDelta);
          LineBounds.Right = LineBounds.Right + KernDelta.x * fFace.FontUnitScale.X;
        }

        // update left bound (must be done before right bound is updated)
        if (LineBounds.Right + Glyph.Bounds.Left < LineBounds.Left) then
          LineBounds.Left = LineBounds.Right + Glyph.Bounds.Left;

        // update right bound
        if (CharIndex < LengthUCS4(TextLine)-1) or  // not the last character
           (TextLine[CharIndex] == Ord(' ')) or      // on space char (Bounds.Right == 0)
           Advance then                             // or in advance mode
        {
          // add advance && glyph spacing
          LineBounds.Right = LineBounds.Right + Glyph.Advance.x + GlyphSpacing
        }
        else
        {
          // add glyph's right bound
          LineBounds.Right = LineBounds.Right + Glyph.Bounds.Right;
        }

        // update bottom && top bounds
        if (Glyph.Bounds.Bottom < LineBounds.Bottom) then
          LineBounds.Bottom = Glyph.Bounds.Bottom;
        if (Glyph.Bounds.Top > LineBounds.Top) then
          LineBounds.Top = Glyph.Bounds.Top;
      }

      PrevGlyph = Glyph;
    }

    // handle italic font style
    if (Italic in Style) then
    {
      LineBounds.Left = LineBounds.Left + LineBounds.Bottom * cShearFactor;
      LineBounds.Right = LineBounds.Right + LineBounds.Top * cShearFactor;
    }

    // handle underlined font style
    if (Underline in Style) then
    {
      UnderlinePos = GetUnderlinePosition();
      if (UnderlinePos < LineBounds.Bottom) then
        LineBounds.Bottom = UnderlinePos;
    }

    // add line offset
    LineBounds.Bottom = LineBounds.Bottom + LineYOffset;
    LineBounds.Top = LineBounds.Top + LineYOffset;

    // adjust global bounds
    if (Result.Left > LineBounds.Left) then
      Result.Left = LineBounds.Left;
    if (Result.Right < LineBounds.Right) then
      Result.Right = LineBounds.Right;
    if (Result.Bottom > LineBounds.Bottom) then
      Result.Bottom = LineBounds.Bottom;
    if (Result.Top < LineBounds.Top) then
      Result.Top = LineBounds.Top;
  }

  // if left or bottom bound was not set, set them to 0
  if (IsInfinite(Result.Left)) then
    Result.Left = 0.0;
  if (IsInfinite(Result.Bottom)) then
    Result.Bottom = 0.0;
}

void TFTFont::Render(const Text: std::u32string);
var
  CharIndex: int;
  Glyph, PrevGlyph: TFTGlyph;
  KernDelta: FT_Vector;
{
  // reset last glyph
  PrevGlyph = nil;

  // draw current line
  for CharIndex = 0 to LengthUCS4(Text)-1 do
  {
    Glyph = TFTGlyph(GetGlyph(Text[CharIndex]));
    if (Assigned(Glyph)) then
    {
      // get kerning
      if (fUseKerning && FT_HAS_KERNING(fFace.Data) && (PrevGlyph != nil)) then
      {
        FT_Get_Kerning(fFace.Data, PrevGlyph.CharIndex, Glyph.CharIndex,
                       FT_KERNING_UNSCALED, KernDelta);
        glTranslatef(KernDelta.x * fFace.FontUnitScale.X, 0, 0);
      }

      if (ReflectionPass) then
        Glyph.RenderReflection()
      else
        Glyph.Render(fUseDisplayLists);

      glTranslatef(Glyph.Advance.x + fGlyphSpacing, 0, 0);
    }

    PrevGlyph = Glyph;
  }
}

function TFTFont::GetHeight(): float;
{
  Result = Ascender - Descender;
}

function TFTFont::GetAscender(): float;
{
  Result = fFace.Data.ascender * fFace.FontUnitScale.Y + Outset*2;
}

function TFTFont::GetDescender(): float;
{
  // Note: outset is not part of the descender as the baseline is lifted
  Result = fFace.Data.descender * fFace.FontUnitScale.Y;
}

function TFTFont::GetUnderlinePosition(): float;
{
  Result = fFace.Data.underline_position * fFace.FontUnitScale.Y - Outset;
}

function TFTFont::GetUnderlineThickness(): float;
{
  Result = fFace.Data.underline_thickness * fFace.FontUnitScale.Y + Outset*2;
}


/*
 * TFTScalableFont
 */

TFTScalableFont::TFTScalableFont(const std::filesystem::path Filename,
    int Size, float OutsetAmount,
	bool UseMipmaps,
	bool PreCache)
{
  FT_Int32 LoadFlags = FT_LOAD_DEFAULT;
  // Disable hinting && grid-fitting to preserve font outlines at each font
  // size, otherwise the font widths/heights do not match resulting in ugly
  // text size changes during zooming.
  // A drawback is a reduced quality with smaller font sizes but it is not that
  // bad with gray-scaled rendering (at least it looks better than OpenGL's
  // linear downscaling on minification).
  if (UseMipmaps)
    LoadFlags = LoadFlags or FT_LOAD_NO_HINTING;
  inherited Create(
      TFTFont::Create(Filename, Size, Size * OutsetAmount, PreCache, LoadFlags),
      UseMipmaps);
}

std::shared_ptr<TFont> TFTScalableFont::CreateMipmap(int Level, float Scale)
/*var
  ScaledSize: int;
  BaseFont: TFTFont;*/
{
  Result = nil;
  BaseFont = TFTFont(fBaseFont);
  ScaledSize = Round(BaseFont.Size * Scale);
  // do not create mipmap fonts < 8 pixels
  if (ScaledSize < 8) then
    Exit;
  Result = TFTFont::Create(BaseFont.Filename,
      ScaledSize, BaseFont.Outset * Scale,
      BaseFont.fPreCache,
      FT_LOAD_DEFAULT or FT_LOAD_NO_HINTING);
}

function TFTScalableFont::GetOutset(): float;
{
  Result = TFTFont(fBaseFont).Outset * fScale;
}

void TFTScalableFont::AddFallback(const Filename: std::filesystem::path);
var
  Level: int;
{
  for Level = 0 to High(fMipmapFonts) do
    if (fMipmapFonts[Level] != nil) then
      TFTFont(fMipmapFonts[Level]).AddFallback(Filename);
}

void TFTScalableFont::FlushCache(KeepBaseSet: bool);
var
  Level: int;
{
  for Level = 0 to High(fMipmapFonts) do
    if (fMipmapFonts[Level] != nil) then
      TFTFont(fMipmapFonts[Level]).FlushCache(KeepBaseSet);
}


/*
 * TOutlineFont
 */

constructor TFTOutlineFont.Create(
    const Filename: std::filesystem::path;
    Size: int; Outset: float;
    PreCache: bool;
    LoadFlags: FT_Int32);
{
  inherited Create(Filename);

  fSize = Size;
  fOutset = Outset;
  fPreCache = PreCache;
  fInnerFont = TFTFont::Create(Filename, Size, 0.0, PreCache, LoadFlags);
  fInnerFont.Part = fpInner;
  fOutlineFont = TFTFont::Create(Filename, Size, Outset, PreCache, LoadFlags);
  fOutlineFont.Part = fpOutline;

  ResetIntern();
}

destructor TFTOutlineFont.Destroy;
{
  fOutlineFont.Free;
  fInnerFont.Free;
  inherited;
}

void TFTOutlineFont.ResetIntern();
{
  // TODO: maybe swap fInnerFont/fOutlineFont.GlyphSpacing to use the spacing
  // of the outline font?
  //fInnerFont.GlyphSpacing = fOutset*2;
  fOutlineFont.GlyphSpacing = -fOutset*2;

  fLineSpacing = fOutlineFont.LineSpacing;
  fReflectionSpacing = fOutlineFont.ReflectionSpacing;
  fOutlineColor = NewGLColor(0, 0, 0, -1);
}

void TFTOutlineFont.Reset();
{
  inherited;
  fInnerFont.Reset();
  fOutlineFont.Reset();
  ResetIntern();
}

void TFTOutlineFont.DrawUnderline(const Text: std::u32string);
var
  CurrentColor: TGLColor;
  OutlineColor: TGLColor;
{
  // save current color
  glGetFloatv(GL_CURRENT_COLOR, @CurrentColor.vals);

  // if the outline's alpha component is < 0 use the current alpha
  OutlineColor = fOutlineColor;
  if (OutlineColor.a < 0) then
    OutlineColor.a = CurrentColor.a;

  // draw underline outline (in outline color)
  glColor4fv(@OutlineColor.vals);
  fOutlineFont.DrawUnderline(Text);
  glColor4fv(@CurrentColor.vals);

  // draw underline inner part (in current color)
  glPushMatrix();
  glTranslatef(fOutset, 0, 0);
  fInnerFont.DrawUnderline(Text);
  glPopMatrix();
}

void TFTOutlineFont.Render(const Text: std::u32string);
var
  CurrentColor: TGLColor;
  OutlineColor: TGLColor;
{
  // save current color
  glGetFloatv(GL_CURRENT_COLOR, @CurrentColor.vals);

  // if the outline's alpha component is < 0 use the current alpha
  OutlineColor = fOutlineColor;
  if (OutlineColor.a < 0) then
    OutlineColor.a = CurrentColor.a;

  { setup && render outline font }

  glColor4fv(@OutlineColor.vals);
  glPushMatrix();
  fOutlineFont.Render(Text);
  glPopMatrix();
  glColor4fv(@CurrentColor.vals);

  { setup && render inner font }

  glPushMatrix();
  glTranslatef(fOutset, fOutset, 0);
  fInnerFont.Render(Text);
  glPopMatrix();
}

void TFTOutlineFont.SetOutlineColor(r, g, b: GLfloat; a: GLfloat);
{
  fOutlineColor = NewGLColor(r, g, b, a);
}

void TFTOutlineFont.FlushCache(KeepBaseSet: bool);
{
  fOutlineFont.FlushCache(KeepBaseSet);
  fInnerFont.FlushCache(KeepBaseSet);
}

void TFTOutlineFont.AddFallback(const Filename: std::filesystem::path);
{
  fOutlineFont.AddFallback(Filename);
  fInnerFont.AddFallback(Filename);
}

function TFTOutlineFont.BBox(const Text: TUCS4StringArray; Advance: bool): TBoundsDbl;
{
  Result = fOutlineFont.BBox(Text, Advance);
}

function TFTOutlineFont.GetHeight(): float;
{
  Result = fOutlineFont.Height;
}

function TFTOutlineFont.GetAscender(): float;
{
  Result = fOutlineFont.Ascender;
}

function TFTOutlineFont.GetDescender(): float;
{
  Result = fOutlineFont.Descender;
}

void TFTOutlineFont.SetLineSpacing(Spacing: float);
{
  inherited SetLineSpacing(Spacing);
  fInnerFont.LineSpacing = Spacing;
  fOutlineFont.LineSpacing = Spacing;
}

void TFTOutlineFont.SetGlyphSpacing(Spacing: float);
{
  inherited SetGlyphSpacing(Spacing);
  fInnerFont.GlyphSpacing = Spacing;
  fOutlineFont.GlyphSpacing = Spacing - Outset*2;
}

void TFTOutlineFont.SetReflectionSpacing(Spacing: float);
{
  inherited SetReflectionSpacing(Spacing);
  fInnerFont.ReflectionSpacing = Spacing;
  fOutlineFont.ReflectionSpacing = Spacing;
}

void TFTOutlineFont.SetStyle(Style: TFontStyle);
{
  inherited SetStyle(Style);
  fInnerFont.Style = Style;
  fOutlineFont.Style = Style;
}

function TFTOutlineFont.GetStyle(): TFontStyle;
{
  Result = inherited GetStyle();
}

function TFTOutlineFont.GetUnderlinePosition(): float;
{
  Result = fOutlineFont.GetUnderlinePosition();
}

function TFTOutlineFont.GetUnderlineThickness(): float;
{
  Result = fOutlineFont.GetUnderlinePosition();
}

void TFTOutlineFont.SetUseKerning(Enable: bool);
{
  inherited SetUseKerning(Enable);
  fInnerFont.fUseKerning = Enable;
  fOutlineFont.fUseKerning = Enable;
}

void TFTOutlineFont.SetReflectionPass(Enable: bool);
{
  inherited SetReflectionPass(Enable);
  fInnerFont.fReflectionPass = Enable;
  fOutlineFont.fReflectionPass = Enable;
}

/**
 * TScalableOutlineFont
 */

constructor TFTScalableOutlineFont.Create(
    const Filename: std::filesystem::path;
    Size: int; OutsetAmount: float;
    UseMipmaps: bool;
    PreCache: bool);
var
  LoadFlags: FT_Int32;
{
  LoadFlags = FT_LOAD_DEFAULT;
  // Disable hinting && grid-fitting (see TFTScalableFont::Create)
  if (UseMipmaps) then
    LoadFlags = LoadFlags or FT_LOAD_NO_HINTING;
  inherited Create(
      TFTOutlineFont.Create(Filename, Size, Size*OutsetAmount, PreCache, LoadFlags),
      UseMipmaps);
}

function TFTScalableOutlineFont.CreateMipmap(Level: int; Scale: float): TFont;
var
  ScaledSize: int;
  BaseFont: TFTOutlineFont;
{
  Result = nil;
  BaseFont = TFTOutlineFont(fBaseFont);
  ScaledSize = Round(BaseFont.Size*Scale);
  // do not create mipmap fonts < 8 pixels
  if (ScaledSize < 8) then
    Exit;
  Result = TFTOutlineFont.Create(BaseFont.fFilename,
      ScaledSize, BaseFont.fOutset*Scale,
      BaseFont.fPreCache,
      FT_LOAD_DEFAULT or FT_LOAD_NO_HINTING);
}

function TFTScalableOutlineFont.GetOutset(): float;
{
  Result = TFTOutlineFont(fBaseFont).Outset * fScale;
}

void TFTScalableOutlineFont.SetOutlineColor(r, g, b: GLfloat; a: GLfloat);
var
  Level: int;
{
  for Level = 0 to High(fMipmapFonts) do
    if (fMipmapFonts[Level] != nil) then
      TFTOutlineFont(fMipmapFonts[Level]).SetOutlineColor(r, g, b, a);
}

void TFTScalableOutlineFont.FlushCache(KeepBaseSet: bool);
var
  Level: int;
{
  for Level = 0 to High(fMipmapFonts) do
    if (fMipmapFonts[Level] != nil) then
      TFTOutlineFont(fMipmapFonts[Level]).FlushCache(KeepBaseSet);
}

void TFTScalableOutlineFont.AddFallback(const Filename: std::filesystem::path);
var
  Level: int;
{
  for Level = 0 to High(fMipmapFonts) do
    if (fMipmapFonts[Level] != nil) then
      TFTOutlineFont(fMipmapFonts[Level]).AddFallback(Filename);
}


/*
 * TFTGlyph
 */

const
  /**
   * Size of the transparent border surrounding the glyph image in the texture.
   * The border is necessary because OpenGL does not smooth texels at the
   * border of a texture with the GL_CLAMP or GL_CLAMP_TO_EDGE styles.
   * Without the border, magnified glyph textures look very ugly at their edges.
   * It looks edgy, as if some pixels are missing especially on the left edge
   * (just set cTexSmoothBorder to 0 to see what is meant by this).
   * With the border even the glyphs edges are blended to the border (transparent)
   * color && everything looks nice.
   *
   * Note:
   * OpenGL already supports texture border by setting the border parameter
   * of glTexImage*D() to 1 && using a texture size of 2^m+2b && setting the
   * border pixels to the border color. In some forums it is discouraged to use
   * the border parameter as only a few of the more modern graphics cards support
   * this feature. On an ATI Radeon 9700 card, the slowed down to 0.5 fps &&
   * the glyph's background got black.  So instead of using this feature we
   * handle it on our own. The only drawback is that textures might get bigger
   * because the border might require a higher power of 2 size instead of just
   * two additional pixels.
   */
  cTexSmoothBorder == 1;

void TFTGlyph::StrokeBorder(var Glyph: FT_Glyph);
var
  Outline: PFT_Outline;
  OuterStroker, InnerStroker:  FT_Stroker;
  OuterNumPoints, InnerNumPoints, GlyphNumPoints: FT_UInt;
  OuterNumContours, InnerNumContours, GlyphNumContours: FT_UInt;
  OuterBorder, InnerBorder: FT_StrokerBorder;
  OutlineFlags: FT_Int;
  UseStencil: bool;
{
  // It is possible to extrude the borders of a glyph with FT_Glyph_Stroke
  // but it will extrude the border to the outside && the inside of a glyph
  // although we just want to extrude to the outside.
  // FT_Glyph_StrokeBorder extrudes to the outside but also fills the interior
  // (this is what we need for bold fonts).
  // In both cases the inner font && outline font (border) will overlap.
  // Normally this does not matter but it does if alpha blending is active.
  // In this case if e.g. the inner color is set to white, the outline to red
  // && alpha to 0.5 the inner part will not be white it will be pink.

  InnerStroker = nil;
  OuterStroker = nil;

  // If we are to create the interior of an outlined font (fInner == true)
  // we have to create two borders:
  // - one extruded to the outside by fOutset pixels &&
  // - one extruded to the inside by almost 0 zero pixels.
  // The second one is used as a stencil for the first one, clearing the
  // interiour of the glyph.
  // The stencil is not needed to create bold fonts.
  UseStencil = (fFont.Part == fpInner);

  // we cannot extrude bitmaps, only vector based glyphs.
  // Check for FT_GLYPH_FORMAT_OUTLINE otherwise a cast to FT_OutlineGlyph is
  // invalid && FT_Stroker_ParseOutline() will crash
  if (Glyph.format != FT_GLYPH_FORMAT_OUTLINE) then
    Exit;

  Outline = @FT_OutlineGlyph(Glyph).outline;
  
  OuterBorder = FT_Outline_GetOutsideBorder(Outline);
  if (OuterBorder == FT_STROKER_BORDER_LEFT) then
    InnerBorder = FT_STROKER_BORDER_RIGHT
  else
    InnerBorder = FT_STROKER_BORDER_LEFT;

  { extrude outer border }

  if (FT_Stroker_New(Glyph.library_, OuterStroker) != 0) then
    raise EFontError.Create('FT_Stroker_New failed!');
  FT_Stroker_Set(
      OuterStroker,
      Round(fOutset * 64),
      FT_STROKER_LINECAP_ROUND,
      FT_STROKER_LINEJOIN_BEVEL,
      0);

  // similar to FT_Glyph_StrokeBorder(inner == FT_FALSE) but it is possible to
  // use FT_Stroker_ExportBorder() afterwards to combine inner && outer borders
  if (FT_Stroker_ParseOutline(OuterStroker, Outline, FT_FALSE) != 0) then
    raise EFontError.Create('FT_Stroker_ParseOutline failed!');

  FT_Stroker_GetBorderCounts(OuterStroker, OuterBorder, OuterNumPoints, OuterNumContours);

  { extrude inner border (== stencil) }

  if (UseStencil) then
  {
    if (FT_Stroker_New(Glyph.library_, InnerStroker) != 0) then
      raise EFontError.Create('FT_Stroker_New failed!');
    FT_Stroker_Set(
        InnerStroker,
        63, // extrude at most one pixel to avoid a black border
        FT_STROKER_LINECAP_ROUND,
        FT_STROKER_LINEJOIN_BEVEL,
        0);

    if (FT_Stroker_ParseOutline(InnerStroker, Outline, FT_FALSE) != 0) then
      raise EFontError.Create('FT_Stroker_ParseOutline failed!');

    FT_Stroker_GetBorderCounts(InnerStroker, InnerBorder, InnerNumPoints, InnerNumContours);
  } else {
    InnerNumPoints = 0;
    InnerNumContours = 0;
  }

  { combine borders (subtract: OuterBorder - InnerBorder) }

  GlyphNumPoints = InnerNumPoints + OuterNumPoints;
  GlyphNumContours = InnerNumContours + OuterNumContours;

  // save flags before deletion (TODO: set them on the resulting outline)
  OutlineFlags = Outline.flags;

  // resize glyph outline to hold inner && outer border
  FT_Outline_Done(Glyph.Library_, Outline);
  if (FT_Outline_New(Glyph.Library_, GlyphNumPoints, GlyphNumContours, Outline) != 0) then
    raise EFontError.Create('FT_Outline_New failed!');

  Outline.n_points = 0;
  Outline.n_contours = 0;

  // add points to outline. The inner-border is used as a stencil.
  FT_Stroker_ExportBorder(OuterStroker, OuterBorder, Outline);
  if (UseStencil) then
    FT_Stroker_ExportBorder(InnerStroker, InnerBorder, Outline);
  if (FT_Outline_Check(outline) != 0) then
    raise EFontError.Create('FT_Stroker_ExportBorder failed!');

  if (InnerStroker != nil) then
    FT_Stroker_Done(InnerStroker);
  if (OuterStroker != nil) then
    FT_Stroker_Done(OuterStroker);
}

void TFTGlyph::CreateTexture(FT_Int32 LoadFlags)
/*var
  X, Y:          int;
  Glyph:         FT_Glyph;
  BitmapGlyph:   FT_BitmapGlyph;
  Bitmap:        PFT_Bitmap;
  BitmapLine:    PByteArray;
  BitmapBuffer:  PByteArray;
  TexBuffer:     TGLubyteDynArray;
  TexLine:       PGLubyteArray;
  CBox:          FT_BBox;*/
{
  // we need vector data for outlined glyphs so do not load bitmaps.
  // This is necessary for mixed fonts that contain bitmap versions of smaller
  // glyphs, for example in CJK fonts.
  if (fOutset > 0) then
    LoadFlags = LoadFlags or FT_LOAD_NO_BITMAP;

  // load the Glyph for our character
  if (FT_Load_Glyph(fFace.Data, fCharIndex, LoadFlags) != 0) then
    raise EFontError.Create('FT_Load_Glyph failed');

  // move the face's glyph into a Glyph object
  if (FT_Get_Glyph(fFace.Data^.glyph, Glyph) != 0) then
    raise EFontError.Create('FT_Get_Glyph failed');

  if (fOutset > 0) then
    StrokeBorder(Glyph);

  // store scaled advance width/height in glyph-object
  fAdvance.X = fFace.Data^.glyph^.advance.x / 64 + fOutset*2;
  fAdvance.Y = fFace.Data^.glyph^.advance.y / 64 + fOutset*2;

  // get the contour's bounding box (in 1/64th pixels, not font-units)
  FT_Glyph_Get_CBox(Glyph, FT_GLYPH_BBOX_UNSCALED, CBox);
  // convert 1/64th values to double values
  fBounds.Left   = CBox.xMin / 64;
  fBounds.Right  = CBox.xMax / 64 + fOutset*2;
  fBounds.Bottom = CBox.yMin / 64;
  fBounds.Top    = CBox.yMax / 64 + fOutset*2;

  // convert the glyph to a bitmap (&& destroy original glyph image).
  // Request 8 bit gray level pixel mode. 
  FT_Glyph_To_Bitmap(Glyph, FT_RENDER_MODE_NORMAL, nil, 1);
  BitmapGlyph = FT_BitmapGlyph(Glyph);

  // get bitmap offsets
  fBitmapCoords.Left = BitmapGlyph^.left - cTexSmoothBorder;
  // Note: add 1*fOutset for lifting the baseline so outset fonts to not intersect
  // with the baseline; Ceil(fOutset) for the outset pixels added to the bitmap.
  fBitmapCoords.Top  = BitmapGlyph^.top + fOutset+Ceil(fOutset) + cTexSmoothBorder;

  // make accessing the bitmap easier
  Bitmap = @BitmapGlyph^.bitmap;
  // get bitmap dimensions
  fBitmapCoords.Width  = Bitmap.width + (Ceil(fOutset) + cTexSmoothBorder)*2;
  fBitmapCoords.Height = Bitmap.rows + (Ceil(fOutset) + cTexSmoothBorder)*2;

  // get power-of-2 bitmap widths
  fTexSize.Width  =
      NextPowerOf2(Bitmap.width + (Ceil(fOutset) + cTexSmoothBorder)*2);
  fTexSize.Height =
      NextPowerOf2(Bitmap.rows + (Ceil(fOutset) + cTexSmoothBorder)*2);

  // texture-widths ignoring empty (power-of-2) padding space
  fTexOffset.X = fBitmapCoords.Width / fTexSize.Width;
  fTexOffset.Y = fBitmapCoords.Height / fTexSize.Height;

  // allocate memory for texture data
  SetLength(TexBuffer, fTexSize.Width * fTexSize.Height);
  FillChar(TexBuffer[0], Length(TexBuffer), 0);

  // Freetype stores the bitmap with either upper (pitch is > 0) or lower
  // (pitch < 0) glyphs line first. Set the buffer to the upper line.
  // See http://freetype.sourceforge.net/freetype2/docs/glyphs/glyphs-7.html
  if (Bitmap.pitch > 0) then
    BitmapBuffer = @Bitmap.buffer[0]
  else
    BitmapBuffer = @Bitmap.buffer[(Bitmap.rows-1) * Abs(Bitmap.pitch)];

  // copy data to texture bitmap (upper line first).
  for Y = 0 to Bitmap.rows-1 do
  {
    // set pointer to first pixel in line that holds bitmap data.
    // Each line starts with a cTexSmoothBorder pixel && multiple outset pixels
    // that are added by Extrude() later.
    TexLine = @TexBuffer[(Y + cTexSmoothBorder + Ceil(fOutset)) * fTexSize.Width +
                          cTexSmoothBorder + Ceil(fOutset)];
    // get next lower line offset, use pitch instead of width as it tells
    // us the storage direction of the lines. In addition a line might be padded.
    BitmapLine = @BitmapBuffer[Y * Bitmap.pitch];

    // check for pixel mode && copy pixels
    // Should be 8 bit gray, but even with FT_RENDER_MODE_NORMAL, freetype
    // sometimes (e.g. 16px sized japanese fonts) fallbacks to 1 bit pixels.
    case (Bitmap.pixel_mode) of
      FT_PIXEL_MODE_GRAY: {  // 8 bit gray
        for X = 0 to Bitmap.width-1 do
          TexLine[X] = BitmapLine[X];
      }
      FT_PIXEL_MODE_MONO: {  // 1 bit mono
        for X = 0 to Bitmap.width-1 do
          TexLine[X] = High(GLubyte) * ((BitmapLine[X div 8] shr (7-(X mod 8))) && $1);
      }
      else {
        // unhandled pixel format
      }
    }
  }

  // allocate resources for textures && display lists
  glGenTextures(1, @fTexture);

  // setup texture parameters
  glBindTexture(GL_TEXTURE_2D, fTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // create alpha-map (GL_ALPHA component only).
  // TexCoord (0,0) corresponds to the top left pixel of the glyph,
  // (1,1) to the bottom right pixel. So the glyph is flipped as OpenGL uses
  // a cartesian (y-axis up) coordinate system for textures.   
  // See the cTexSmoothBorder comment for info on texture borders.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, fTexSize.Width, fTexSize.Height,
      0, GL_ALPHA, GL_UNSIGNED_BYTE, @TexBuffer[0]);

  // free expanded data
  SetLength(TexBuffer, 0);

  // create the display list
  fDisplayList = glGenLists(1);

  // render to display-list
  glNewList(fDisplayList, GL_COMPILE);
    Render(false);
  glEndList();

  // free glyph data (bitmap, etc.)
  FT_Done_Glyph(Glyph);
}

constructor TFTGlyph::Create(Font: TFTFont; ch: UCS4Char; Outset: float;
    LoadFlags: FT_Int32);
var
  I: int;
{
  inherited Create();

  fFont = Font;
  fOutset = Outset;
  fCharCode = ch;

  // Note: the default face is also used if no face (neither default nor fallback)
  // contains a glyph for the given char.
  fFace = Font.DefaultFace;

  // search the Freetype char-index (use default UNICODE charmap) in the default face
  fCharIndex = FT_Get_Char_Index(fFace.Data, FT_ULONG(ch));
  if (fCharIndex == 0) then
  {
    // glyph not in default font, search in fallback font faces
    for I = 0 to High(Font.FallbackFaces) do
    {
      fCharIndex = FT_Get_Char_Index(Font.FallbackFaces[I].Data, FT_ULONG(ch));
      if (fCharIndex != 0) then
      {
        fFace = Font.FallbackFaces[I];
        break;
      }
    }
  }

  CreateTexture(LoadFlags);
}

destructor TFTGlyph::Destroy;
{
  if (fDisplayList != 0) then
    glDeleteLists(fDisplayList, 1);
  if (fTexture != 0) then
    glDeleteTextures(1, @fTexture);
  inherited;
}

void TFTGlyph::Render(UseDisplayLists: bool);
{
  // use display-lists if enabled && exit
  if (UseDisplayLists) then
  {
    glCallList(fDisplayList);
    Exit;
  }

  glBindTexture(GL_TEXTURE_2D, fTexture);
  glPushMatrix();

  // move to top left glyph position
  glTranslatef(fBitmapCoords.Left, fBitmapCoords.Top, 0);

  // draw glyph texture
  glBegin(GL_QUADS);
    // top right
    glTexCoord2f(fTexOffset.X, 0);
    glVertex2f(fBitmapCoords.Width, 0);

    // top left
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);

    // bottom left
    glTexCoord2f(0, fTexOffset.Y);
    glVertex2f(0, -fBitmapCoords.Height);

    // bottom right
    glTexCoord2f(fTexOffset.X, fTexOffset.Y);
    glVertex2f(fBitmapCoords.Width, -fBitmapCoords.Height);
  glEnd();

  glPopMatrix();
}

void TFTGlyph::RenderReflection();
var
  Color: TGLColor;
  TexUpperPos: float;
  TexLowerPos: float;
  UpperPos: float;
const
  CutOff == 0.6;
{
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, fTexture);
  glGetFloatv(GL_CURRENT_COLOR, @Color.vals);

  // add extra space to the left of the glyph
  glTranslatef(fBitmapCoords.Left, 0, 0);

  // The upper position of the glyph, if CutOff is 1.0, it is fFont.Ascender.
  // If CutOff is set to 0.5 only half of the glyph height is displayed.
  UpperPos = fFont.Descender + fFont.Height * CutOff;

  // the glyph texture's height is just the height of the glyph but not the font
  // height. Setting a color for the upper && lower bounds of the glyph results
  // in different color gradients. So we have to set the color values for the
  // descender && ascender (as we have a cutoff, for the upper-pos here) as
  // these positions are font but not glyph specific.

  // To get the texture positions we have to enhance the texture at the top &&
  // bottom by the amount from the top to ascender (rather upper-pos here) &&
  // from the bottom (Height-Top) to descender. Then we have to convert those
  // heights to texture coordinates by dividing by the bitmap Height &&
  // removing the power-of-2 padding space by multiplying with fTexOffset.Y
  // (as fBitmapCoords.Height corresponds to fTexOffset.Y && not 1.0).
  TexUpperPos  = -(UpperPos - fBitmapCoords.Top) / fBitmapCoords.Height * fTexOffset.Y;
  TexLowerPos = (-(fFont.Descender + fBitmapCoords.Height - fBitmapCoords.Top) /
                    fBitmapCoords.Height + 1) * fTexOffset.Y;

  // draw glyph texture
  glBegin(GL_QUADS);
    // top right
    glColor4f(Color.r, Color.g, Color.b, 0);
    glTexCoord2f(fTexOffset.X, TexUpperPos);
    glVertex2f(fBitmapCoords.Width, UpperPos);

    // top left
    glTexCoord2f(0, TexUpperPos);
    glVertex2f(0, UpperPos);

    // bottom left
    glColor4f(Color.r, Color.g, Color.b, Color.a-0.3);
    glTexCoord2f(0, TexLowerPos);
    glVertex2f(0, fFont.Descender);

    // bottom right
    glTexCoord2f(fTexOffset.X, TexLowerPos);
    glVertex2f(fBitmapCoords.Width, fFont.Descender);
  glEnd();

  glPopMatrix();

  // restore old color
  // Note: glPopAttrib(GL_CURRENT_BIT)/glPopAttrib() is much slower then
  // glGetFloatv(GL_CURRENT_COLOR, ...)/glColor4fv(...)
  glColor4fv(@Color.vals);
}

function TFTGlyph::GetAdvance(): TPositionDbl;
{
  Result = fAdvance;
}

function TFTGlyph::GetBounds(): TBoundsDbl;
{
  Result = fBounds;
}


/*
 * TGlyphCache
 */

constructor TGlyphCache.Create();
{
  inherited;
  fHash = TList.Create();
}

destructor TGlyphCache.Destroy();
{
  // free cached glyphs
  FlushCache(false);

  // destroy TList
  fHash.Free;
  
  inherited;
}

function TGlyphCache.FindGlyphTable(BaseCode: uint32_t; out InsertPos: int): PGlyphTable;
var
  I: int;
  Entry: TGlyphCacheHashEntry;
{
  Result = nil;

  for I = 0 to fHash.Count-1 do
  {
    Entry = TGlyphCacheHashEntry(fHash[I]);

    if (Entry.BaseCode > BaseCode) then
    {
      InsertPos = I;
      Exit;
    }

    if (Entry.BaseCode == BaseCode) then
    {
      InsertPos = I;
      Result = @Entry.GlyphTable;
      Exit;
    }
  }

  InsertPos = fHash.Count;
}

function TGlyphCache.AddGlyph(ch: UCS4Char; const Glyph: TGlyph): bool;
var
  BaseCode:  uint32_t;
  GlyphCode: int;
  InsertPos: int;
  GlyphTable: PGlyphTable;
  Entry: TGlyphCacheHashEntry;
{
  Result = false;

  BaseCode = Ord(ch) shr 8;
  GlyphTable = FindGlyphTable(BaseCode, InsertPos);
  if (GlyphTable == nil) then
  {
    Entry = TGlyphCacheHashEntry.Create(BaseCode);
    GlyphTable = @Entry.GlyphTable;
    fHash.Insert(InsertPos, Entry);
  }

  // get glyph table offset
  GlyphCode = Ord(ch) && $FF;
  // insert glyph into table if not present
  if (GlyphTable[GlyphCode] == nil) then
  {
    GlyphTable[GlyphCode] = Glyph;
    Result = true;
  }
}

void TGlyphCache.DeleteGlyph(ch: UCS4Char);
var
  Table: PGlyphTable;
  TableIndex, GlyphIndex: int;
  TableEmpty: bool;
{
  // find table
  Table = FindGlyphTable(Ord(ch) shr 8, TableIndex);
  if (Table == nil) then
    Exit;

  // find glyph    
  GlyphIndex = Ord(ch) && $FF;
  if (Table[GlyphIndex] != nil) then
  {
    // destroy glyph
    FreeAndNil(Table[GlyphIndex]);

    // check if table is empty
    TableEmpty = true;
    for GlyphIndex = 0 to High(Table^) do
    {
      if (Table[GlyphIndex] != nil) then
      {
        TableEmpty = false;
        break;
      }
    }

    // free empty table
    if (TableEmpty) then
    {
      fHash.Delete(TableIndex);
    }
  }
}

function TGlyphCache.GetGlyph(ch: UCS4Char): TGlyph;
var
  InsertPos: int;
  Table: PGlyphTable;
{
  Table = FindGlyphTable(Ord(ch) shr 8, InsertPos);
  if (Table == nil) then
    Result = nil
  else
    Result = Table[Ord(ch) && $FF];
}

function TGlyphCache.HasGlyph(ch: UCS4Char): bool;
{
  Result = (GetGlyph(ch) != nil);
}

void TGlyphCache.FlushCache(KeepBaseSet: bool);
var
  EntryIndex, TableIndex: int;
  Entry: TGlyphCacheHashEntry;
{
  // destroy cached glyphs
  for EntryIndex = 0 to fHash.Count-1 do
  {
    Entry = TGlyphCacheHashEntry(fHash[EntryIndex]);

    // the base set (0-255) has BaseCode 0 as the upper bytes are 0.
    if KeepBaseSet && (Entry.fBaseCode == 0) then
      Continue;

    for TableIndex = 0 to High(Entry.GlyphTable) do
    {
      if (Entry.GlyphTable[TableIndex] != nil) then
        FreeAndNil(Entry.GlyphTable[TableIndex]);
    }
    FreeAndNil(Entry);
  }
}


/*
 * TGlyphCacheEntry
 */

constructor TGlyphCacheHashEntry.Create(BaseCode: uint32_t);
{
  inherited Create();
  fBaseCode = BaseCode;
}


/*
 * TFreeType
 */

class function TFreeType.GetLibrary(): FT_Library;
{
  if (LibraryInst == nil) then
  {
    // initialize freetype
    if (FT_Init_FreeType(LibraryInst) != 0) then
      raise EFontError.Create('FT_Init_FreeType failed');
  }
  Result = LibraryInst;
}

class void TFreeType.FreeLibrary();
{
  if (LibraryInst != nil) then
    FT_Done_FreeType(LibraryInst);
  LibraryInst = nil;
}


{$IFDEF BITMAP_FONT}
/*
 * TBitmapFont
 */

constructor TBitmapFont.Create(const Filename: std::filesystem::path; Outline: int;
    Baseline, Ascender, Descender: int);
{
  inherited Create(Filename);

  fTex = Texture.LoadTexture(Filename, TEXTURE_TYPE_TRANSPARENT, 0);
  fTexSize = 1024;
  fOutline = Outline;
  fBaseline  = Baseline;
  fAscender  = Ascender;
  fDescender = Descender;

  LoadFontInfo(Filename.SetExtension('.dat'));

  ResetIntern();
}

destructor TBitmapFont.Destroy();
{
  glDeleteTextures(1, @fTex.TexNum);
  inherited;
}

void TBitmapFont.ResetIntern();
{
  fLineSpacing = Height;
}

void TBitmapFont.Reset();
{
  inherited;
  ResetIntern();
}

void TBitmapFont.AddFallback(const Filename: std::filesystem::path);
{
  // no support for fallbacks
}

void TBitmapFont.CorrectWidths(WidthMult: real; WidthAdd: int);
var
  Count: int;
{
  for Count = 0 to 255 do
    fWidths[Count] = Round(fWidths[Count] * WidthMult) + WidthAdd;
}

void TBitmapFont.LoadFontInfo(const InfoFile: std::filesystem::path);
var
  Stream: TStream;
{
  FillChar(fWidths[0], Length(fWidths), 0);

  Stream = nil;
  try
    Stream = TBinaryFileStream.Create(InfoFile, fmOpenRead);
    Stream.Read(fWidths, 256);
  except
    raise EFontError.Create('Could not read font info file ''' +  InfoFile.ToNative + '''');
  }
  Stream.Free;
}

function TBitmapFont.BBox(const Text: TUCS4StringArray; Advance: bool): TBoundsDbl;
var
  LineIndex, CharIndex: int;
  CharCode: uint32_t;
  Line: std::u32string;
  LineWidth: double;
{
  Result.Left = 0;
  Result.Right = 0;
  Result.Top = Height;
  Result.Bottom = 0;

  for LineIndex = 0 to High(Text) do
  {
    Line = Text[LineIndex];
    LineWidth = 0;
    for CharIndex = 0 to LengthUCS4(Line)-1 do
    {
      CharCode = Ord(Line[CharIndex]);
      if (CharCode < Length(fWidths)) then
        LineWidth = LineWidth + fWidths[CharCode];
    }
    if (LineWidth > Result.Right) then
      Result.Right = LineWidth;
  }
}

void TBitmapFont.RenderChar(ch: UCS4Char; var AdvanceX: real);
var
  TexX, TexY:        real;
  TexR, TexB:        real;
  GlyphWidth:        real;
  PL, PT:            real;
  PR, PB:            real;
  CharCode:          uint32_t;
{
  CharCode = Ord(ch);
  if (CharCode > High(fWidths)) then
    CharCode = 0;

  GlyphWidth = fWidths[CharCode];

  // set texture positions
  TexX = (CharCode mod 16) * 1/16 + 1/32 - (GlyphWidth/2 - fOutline)/fTexSize;
  TexY = (CharCode div 16) * 1/16 + {2 texels} 2/fTexSize;
  TexR = (CharCode mod 16) * 1/16 + 1/32 + (GlyphWidth/2 + fOutline)/fTexSize;
  TexB = (1 + CharCode div 16) * 1/16 - {2 texels} 2/fTexSize;

  // set vector positions
  PL = AdvanceX - fOutline;
  PR = PL + GlyphWidth + fOutline*2;
  PB = -fBaseline;
  PT = PB + fTexSize div 16;

  (*
  if (Font.Blend) then
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  *)

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, fTex.TexNum);
  
  if (not ReflectionPass) then
  {
    glBegin(GL_QUADS);
      glTexCoord2f(TexX, TexY); glVertex2f(PL, PT);
      glTexCoord2f(TexX, TexB); glVertex2f(PL, PB);
      glTexCoord2f(TexR, TexB); glVertex2f(PR, PB);
      glTexCoord2f(TexR, TexY); glVertex2f(PR, PT);
    glEnd;
  }
  else
  {
    glDepthRange(0, 10);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
      glTexCoord2f(TexX, TexY); glVertex2f(PL, PT);
      glTexCoord2f(TexX, TexB); glVertex2f(PL, PB);
      glTexCoord2f(TexR, TexB); glVertex2f(PR, PB);
      glTexCoord2f(TexR, TexY); glVertex2f(PR, PT);
    glEnd;

    glBegin(GL_QUADS);
      glTexCoord2f(TexX, TexY); glVertex2f(PL, PT);
      glTexCoord2f(TexX, TexB); glVertex2f(PL, PB);
      glTexCoord2f(TexR, TexB); glVertex2f(PR, PB);
      glTexCoord2f(TexR, TexY); glVertex2f(PR, PT);

(*
      glColor4f(fTempColor.r, fTempColor.g, fTempColor.b, 0.7);
      glTexCoord2f(TexX, TexB); glVertex3f(PL, PB, 0);
      glTexCoord2f(TexR, TexB); glVertex3f(PR, PB, 0);

      glColor4f(fTempColor.r, fTempColor.g, fTempColor.b, 0);
      glTexCoord2f(TexR, (TexY + TexB)/2); glVertex3f(PR, (PT + PB)/2, 0);
      glTexCoord2f(TexX, (TexY + TexB)/2); glVertex3f(PL, (PT + PB)/2, 0);
*)
    glEnd;

    //write the colour back
    glColor4fv(@fTempColor);

    glDisable(GL_DEPTH_TEST);
  } // reflection

  glDisable(GL_TEXTURE_2D);
  (*
  if (Font.Blend) then
    glDisable(GL_BLEND);
  *)

  AdvanceX = AdvanceX + GlyphWidth;
}

void TBitmapFont.Render(const Text: std::u32string);
var
  CharIndex: int;
  AdvanceX: real;
{
  // if there is no text do nothing
  if (Text == nil) or (Text[0] == 0) then
    Exit;

  //Save the current color && alpha (for reflection)
  glGetFloatv(GL_CURRENT_COLOR, @fTempColor);

  AdvanceX = 0;
  for CharIndex = 0 to LengthUCS4(Text)-1 do
  {
    RenderChar(Text[CharIndex], AdvanceX);
  }
}

function TBitmapFont.GetHeight(): float;
{
  Result = fAscender - fDescender;
}

function TBitmapFont.GetAscender(): float;
{
  Result = fAscender;
}

function TBitmapFont.GetDescender(): float;
{
  Result = fDescender;
}

function TBitmapFont.GetUnderlinePosition(): float;
{
  Result = -2.0;
}

function TBitmapFont.GetUnderlineThickness(): float;
{
  Result = 1.0;
}

{$ENDIF BITMAP_FONT}


initialization

finalization
  TFreeType.FreeLibrary();
}