#include "UFont.h"

#include <freetype/ftstroke.h>
#include <gl/glext.h>

namespace UFont
{
//TODO:: replace conversions with boost locale
/*
 * TFont
 */
TFont::TFont(const std::filesystem::path& Filename) : fFilename(Filename)
{
    ResetIntern();
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

void TFont::SplitLines(const std::u32string& Text, TUCS4StringArray& Lines)
{
    // split lines on newline
    Lines.resize(0);

    auto ststr = std::stringstream<char32_t>(Text);
    for (std::u32string line; std::getline(ststr, line, '\n');)
        Lines.push_back(line);
}

template<>
TBoundsDbl TFont::BBox(const std::u32string& Text, bool Advance)
{
    TUCS4StringArray LineArray;
    SplitLines(Text, LineArray);
    return BBox(LineArray, Advance);
}

template<typename TextType>
TBoundsDbl TFont::BBox(const std::basic_string<TextType>& Text, bool Advance)
{
    return BBox(std::codecvt<char32_t, TextType, std::mbstate_t>(Text), Advance);
}

std::filesystem::path TFont::Filename() const
{
    return fFilename;
}

bool TFont::ReflectionPass() const
{
    return fReflectionPass;
}

void TFont::Print(const TUCS4StringArray& Text)
    /*var
      LineIndex: int;*/
{
    // recursively call this function to draw reflected text
    if (Style().contains(Reflect) && !fReflectionPass)
    {
        ReflectionPass(true);
        Print(Text);
        ReflectionPass(false);
    }

    // store current color, enable-flags, matrix-mode
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);

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

#ifdef FLIP_Y_AXIS
    glPushMatrix();
    glScalef(1, -1, 1);
#endif

    // display text
    for (int LineIndex = 0; LineIndex < Text.size(); ++LineIndex)
    {
        glPushMatrix();

        // move to baseline
        glTranslatef(0, -LineSpacing() * LineIndex, 0);

        if (Style().contains(Underline) && !fReflectionPass)
        {
            glDisable(GL_TEXTURE_2D);
            DrawUnderline(Text[LineIndex]);
            glEnable(GL_TEXTURE_2D);
        }

        // draw reflection
        if (fReflectionPass)
        {
            // set reflection spacing
            glTranslatef(0, -ReflectionSpacing(), 0);
            // flip y-axis
            glScalef(1, -1, 1);
        }

        // shear for italic effect
        if (Style().contains(Italic))
            glMultMatrixf(cShearMatrix.data());

        // render text line
        Render(Text[LineIndex]);

        glPopMatrix();
    }

    // restore settings
#ifdef FLIP_Y_AXIS
    glPopMatrix();
#endif
    glPopAttrib();
}

template<>
void TFont::Print(const std::u32string& Text)
{
    TUCS4StringArray LineArray;
    SplitLines(Text, LineArray);
    Print(LineArray);
}

template<typename CharType>
void TFont::Print(const std::basic_string<CharType>& Text)
{
    Print(std::codecvt<char32_t, CharType, std::mbstate_t>(Text));
}

void TFont::DrawUnderline(const std::u32string& Text)
{
    const float UnderlineY1 = UnderlinePosition();
    const float UnderlineY2 = UnderlineY1 + UnderlineThickness();
    const auto Bounds = BBox(Text, false);
    glRectf(Bounds.Left, UnderlineY1, Bounds.Right, UnderlineY2);
}

void TFont::Style(TFontStyle Style)
{
    fStyle = Style;
}

void TFont::AddStyle(FontStyles style)
{
    fStyle.emplace(style);
}

void TFont::RemoveStyle(FontStyles style)
{
    fStyle.erase(style);
}

TFontStyle TFont::Style() const
{
    return fStyle;
}

void TFont::LineSpacing(float Spacing)
{
    fLineSpacing = Spacing;
}

float TFont::LineSpacing() const
{
    return fLineSpacing;
}

void TFont::GlyphSpacing(float Spacing)
{
    fGlyphSpacing = Spacing;
}

float TFont::GlyphSpacing() const
{
    return fGlyphSpacing;
}

void TFont::ReflectionSpacing(float Spacing)
{
    fReflectionSpacing = Spacing;
}

float TFont::ReflectionSpacing() const
{
    return fReflectionSpacing;
}

void TFont::UseKerning(bool Enable)
{
    fUseKerning = Enable;
}

bool TFont::UseKerning() const
{
    return fUseKerning;
}

void TFont::ReflectionPass(bool Enable)
{
    fReflectionPass = Enable;
}


/*
 * TScalableFont
 */
TScalableFont::TScalableFont(std::shared_ptr<TFont> Font, bool UseMipmaps)
	:
	TFont(Font->Filename()),
    fBaseFont(Font),
    fUseMipmaps(UseMipmaps)
{
    fMipmapFonts[0] = Font;
    ResetIntern();
}

void TScalableFont::initMipMaps()
{
    if (!fUseMipmaps || mipMapInit)
        return;

    // create mipmap fonts if requested
    for (int MipmapLevel = 1; MipmapLevel < cMaxMipmapLevel; ++MipmapLevel)
    {
        fMipmapFonts[MipmapLevel] = CreateMipmap(MipmapLevel, 1.f / static_cast<float>(1 << MipmapLevel));
        // stop if no smaller mipmap font is returned
        if (!fMipmapFonts[MipmapLevel])
            break;
    }
}


void TScalableFont::ResetIntern()
{
    fScale = 1.0;
    fStretch = 1.0;
}

void TScalableFont::Reset()
{
    //TFont::Reset(); //inherited;
    ResetIntern();
    for (auto& font : fMipmapFonts)
        font.reset();
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
    double PM15x2 = 2 * abs(ModelMatrix[3][0] * ProjMatrix[0][3]
        + ModelMatrix[3][1] * ProjMatrix[1][3]
        + ModelMatrix[3][2] * ProjMatrix[2][3]
        + ModelMatrix[3][3] * ProjMatrix[3][3]);

    double Dist = ViewPortArray[2] * (ModelMatrix[0][0] * ProjMatrix[0][0]
        + ModelMatrix[0][1] * ProjMatrix[1][0]
        + ModelMatrix[0][2] * ProjMatrix[2][0]
        + ModelMatrix[0][3] * ProjMatrix[3][0]);
    double Dist2 = ViewPortArray[3] * (ModelMatrix[0][0] * ProjMatrix[0][1]
        + ModelMatrix[0][1] * ProjMatrix[1][1]
        + ModelMatrix[0][2] * ProjMatrix[2][1]
        + ModelMatrix[0][3] * ProjMatrix[3][1]);
    double WidthScale = 1;
    double DistSum = Dist * Dist + Dist2 * Dist2;

    if (DistSum > 0)
        WidthScale = PM15x2 / std::sqrt(DistSum);

    Dist = ViewPortArray[2] * (ModelMatrix[1][0] * ProjMatrix[0][0]
        + ModelMatrix[1][1] * ProjMatrix[1][0]
        + ModelMatrix[1][2] * ProjMatrix[2][0]
        + ModelMatrix[1][3] * ProjMatrix[3][0]);
    Dist2 = ViewPortArray[3] * (ModelMatrix[1][0] * ProjMatrix[0][1]
        + ModelMatrix[1][1] * ProjMatrix[1][1]
        + ModelMatrix[1][2] * ProjMatrix[2][1]
        + ModelMatrix[1][3] * ProjMatrix[3][1]);

    double HeightScale = 1;
    DistSum = Dist * Dist + Dist2 * Dist2;

    if (DistSum > 0)
        HeightScale = PM15x2 / std::sqrt(DistSum);

    //writeln(Format('Scale %f, %f', [WidthScale, HeightScale]));

    // 4. Now that we have got the scale, take the bigger minification scale
    // && get it to a logarithmic scale as each mipmap is 1/2 the size of its
    // predecessor (Mipmap_size[i] == Mipmap_size[i-1]/2).
    // The result is our mipmap-level == the index of the mipmap to use.

    // Level > 0: Minification; < 0: Magnification
    const auto res = static_cast<size_t>(std::log2l(std::max(WidthScale, HeightScale)) + cBias);

    // clamp to valid range
    return std::min(std::max<size_t>(0, res), fMipmapFonts.size() - 1);
}

std::optional<float> TScalableFont::GetMipmapScale(int Level)
{
    initMipMaps();

    if (!fMipmapFonts[Level])
        return std::nullopt;

    return fScale * fMipmapFonts[0]->Height() / fMipmapFonts[Level]->Height();
}

/**
 * Returns the correct mipmap font for the current scale && projection
 * matrix. The modelview scale is adjusted to the mipmap level, so
 * Result.Print() will display the font in the correct size.
 */
std::shared_ptr<TFont> TScalableFont::ChooseMipmapFont()
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
    const float MipmapScale = fMipmapFonts[0]->Height() / Result->Height();
    glScalef(MipmapScale, MipmapScale, 0);

    return Result;
}

void TScalableFont::Print(const TUCS4StringArray& Text)
{
    glPushMatrix();

    // set scale && stretching
    glScalef(fScale * fStretch, fScale, 0);

    // print text
    if (fUseMipmaps)
        ChooseMipmapFont()->Print(Text);
    else
        fBaseFont->Print(Text);

    glPopMatrix();
}

void TScalableFont::Render(const std::u32string& Text)
{
    throw std::exception("Unused TScalableFont::Render() was called");
}

TBoundsDbl TScalableFont::BBox(const TUCS4StringArray& Text, bool Advance)
{
    auto res = fBaseFont->BBox(Text, Advance);
    res.Left *= fScale * fStretch;
    res.Right *= fScale * fStretch;
    res.Top *= fScale;
    res.Bottom *= fScale;

    return res;
}

void TScalableFont::Height(float Height)
{
    fScale = Height / fBaseFont->Height();
}

float TScalableFont::Height() const
{
    return fBaseFont->Height() * fScale;
}

void TScalableFont::Stretch(float Stretch)
{
    fStretch = Stretch;
}

float TScalableFont::Stretch() const
{
    return fStretch;
}

float TScalableFont::Ascender() const
{
    return fBaseFont->Ascender() * fScale;
}

float TScalableFont::Descender() const
{
    return fBaseFont->Descender() * fScale;
}

void TScalableFont::LineSpacing(float Spacing)
{
    for (int Level = 0; Level < fMipmapFonts.size(); ++Level)
        if (fMipmapFonts[Level])
            fMipmapFonts[Level]->LineSpacing(Spacing / GetMipmapScale(Level).value());
}

float TScalableFont::LineSpacing() const
{
    return fBaseFont->LineSpacing() * fScale;
}

void TScalableFont::GlyphSpacing(float Spacing)
{
    for (size_t Level = 0; Level < fMipmapFonts.size(); ++Level)
        if (fMipmapFonts[Level])
            fMipmapFonts[Level]->GlyphSpacing(Spacing / GetMipmapScale(Level).value());
}

float TScalableFont::GlyphSpacing() const
{
    return fBaseFont->GlyphSpacing() * fScale;
}

void TScalableFont::ReflectionSpacing(float Spacing)
{
    for (size_t Level = 0; Level < fMipmapFonts.size(); ++Level)
    {
        if (!fMipmapFonts[Level])
            continue;

        const float scale = GetMipmapScale(Level).value();
        if (scale <= 0)
            continue;

        fMipmapFonts[Level]->ReflectionSpacing(Spacing / scale);
    }
}

float TScalableFont::ReflectionSpacing() const
{
    return fBaseFont->LineSpacing() * fScale;
}

void TScalableFont::Style(TFontStyle Style)
{
    for (auto& font : fMipmapFonts)
        if (font)
            font->Style(Style);
}

void TScalableFont::AddStyle(FontStyles style)
{
    for (auto& font : fMipmapFonts)
        if (font)
            font->AddStyle(style);
}

void TScalableFont::RemoveStyle(FontStyles style)
{
    for (auto& font : fMipmapFonts)
        if (font)
            font->RemoveStyle(style);
}

TFontStyle TScalableFont::Style() const
{
    return fBaseFont->Style();
}

float TScalableFont::UnderlinePosition() const
{
    return fBaseFont->UnderlinePosition();
}

float TScalableFont::UnderlineThickness() const
{
    return fBaseFont->UnderlinePosition();
}

void TScalableFont::UseKerning(bool Enable)
{
    for (auto& font : fMipmapFonts)
        if (font)
            font->UseKerning(Enable);
}


/*
 * TCachedFont
 */

TCachedFont::TCachedFont(const std::filesystem::path Filename) : TFont(Filename)
{}

TCachedFont::~TCachedFont()
{}

std::shared_ptr<TGlyph> TCachedFont::GetGlyph(char32_t ch)
{
    auto res = fCache.GetGlyph(ch);
    if (!res)
    {
        res = LoadGlyph(ch);
        if (!fCache.AddGlyph(ch, res))
            return nullptr;
    }
    return res;
}

void TCachedFont::FlushCache(bool KeepBaseSet)
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
{
    //int b1 = 3;

    fFilename = Filename;
    fSize = Size;

#ifdef _WIN32
    auto SourceFile = std::basic_ifstream<FT_Byte>(Filename, std::ios::binary);

    byarr1 = std::vector<FT_Byte>(
        std::istreambuf_iterator(SourceFile),
        std::istreambuf_iterator()
        );
    fFace = makeFace<TFaceS>(byarr1);
#else
	fFace = makeFace<TFaceS>(Filename);
#endif
    // load font information
    /*if (b1 != 0)
    {
        const auto msg = "FT_New_Face: Could not load font \"" + Filename.string() + "\"";
        throw EFontError(msg.c_str());
    }*/

    // support scalable fonts only
    if (!FT_IS_SCALABLE(fFace))
        throw EFontError("Font is not scalable");

    auto raw = fFace.get();
    if (FT_Set_Pixel_Sizes(raw, 0, Size) != 0)
        throw EFontError("FT_Set_Pixel_Sizes fails");
    // get scale factor for font-unit to pixel-size transformation
    fFontUnitScale.X = static_cast<double>(fFace->size->metrics.x_ppem) / fFace->units_per_EM;
    fFontUnitScale.Y = static_cast<double>(fFace->size->metrics.y_ppem) / fFace->units_per_EM;
}

/*
 * TFTFontFaceCache
 */
std::shared_ptr<TFTFontFace> TFTFontFaceCache::LoadFace(const std::filesystem::path Filename, int Size)
{
#ifdef ENABLE_FT_FACE_CACHE
    for (auto& face : fFaces)
    {
        auto strong = face.lock();
        if (strong->Filename() == Filename && strong->Size() == Size)
            return strong;
    }
#endif

    // face not in cache -> load it
    auto Face = std::make_shared<TFTFontFace>(Filename, Size);

#ifdef ENABLE_FT_FACE_CACHE
    // add face to cache
    fFaces.emplace_back(Face);
    //SetLength(fFacesRefCnt, Length(fFaces)+1);
    //fFacesRefCnt[High(fFaces)] = 1;
#endif

    return Face;
}

void TFTFontFaceCache::UnloadFace(std::weak_ptr<TFTFontFace> Face)
{
#ifdef ENABLE_FT_FACE_CACHE
    if (auto it = std::find(fFaces.begin(), fFaces.end(), Face); it != fFaces.end())
    {
        if (it->expired())
            fFaces.erase(it);
    }
#endif
}

/*
 * TFTFont
 */

TFTFont::TFTFont(
    const std::filesystem::path& Filename,
    int Size, float Outset,
    bool PreCache,
    FT_Int32 LoadFlags)
    :
    TCachedFont(Filename),
    fFace(GetFaceCache()->LoadFace(Filename, Size)),
    fSize(Size),
    fOutset(Outset),
    fPreCache(PreCache),
    fLoadFlags(LoadFlags),
    fUseDisplayLists(true),
    fPart(fpNone)
{
    ResetIntern();
}

    
TFTFont::SPtr TFTFont::make(const std::filesystem::path& Filename,
    int Size, float Outset,
    bool PreCache,
    FT_Int32 LoadFlags)
{
    auto temp = std::make_shared<TFTFont>(Filename, Size, Outset, PreCache, LoadFlags);
    // pre-cache some commonly used glyphs (' ' - '~')
    if (PreCache)
    {
        for (char32_t ch = 32; ch <= 126; ++ch) //TODO:: test for shared_ptr cycle
            fCache.AddGlyph(ch, std::make_shared<TFTGlyph>(temp, ch, Outset, LoadFlags));
    }
    return temp;
}

TFTFont::~TFTFont()
{
    // free faces
    auto cache = GetFaceCache();
    cache->UnloadFace(fFace);
    for (auto& fbFace : fFallbackFaces)
        cache->UnloadFace(fbFace);
}

inline std::shared_ptr<TFTFontFaceCache> FontFaceCache;

std::shared_ptr<TFTFontFaceCache> TFTFont::GetFaceCache()
{
    if (!FontFaceCache)
        FontFaceCache = std::make_shared<TFTFontFaceCache>();
    return FontFaceCache;
}

void TFTFont::ResetIntern()
{
    // Note: outset && non outset fonts use same spacing
    fLineSpacing = fFace->Data()->height * fFace->FontUnitScale().Y;
    fReflectionSpacing = -2 * fFace->Data()->descender * fFace->FontUnitScale().Y;
}

void TFTFont::Reset()
{
    TCachedFont::Reset();
    ResetIntern();
}

void TFTFont::AddFallback(const std::filesystem::path& Filename)
{
    auto FontFace = GetFaceCache()->LoadFace(Filename, Size());
    fFallbackFaces.emplace_back(FontFace);
}

std::shared_ptr<TGlyph> TFTFont::LoadGlyph(char32_t ch)
{
    return std::make_shared<TFTGlyph>(this->shared_from_this(), ch, Outset(), fLoadFlags);
}

TBoundsDbl TFTFont::BBox(const TUCS4StringArray& Text, bool Advance)
{
    // Reset global bounds
    TBoundsDbl res = {
        std::numeric_limits<double>::max(),
        0,
        std::numeric_limits<double>::max(),
        0
    };

    // reset last glyph
    std::shared_ptr<TFTGlyph> PrevGlyph;

    // display text
    for (int LineIndex = 0; LineIndex < Text.size(); ++LineIndex)
    {
        // get next text line
        auto TextLine = Text[LineIndex];
        float LineYOffset = -LineSpacing() * LineIndex;

        // reset line bounds
        TBoundsDbl LineBounds = {
            std::numeric_limits<double>::max(),
            0,
            std::numeric_limits<double>::max(),
            0
        };

        // for each glyph image, compute its bounding box
        for (size_t CharIndex = 0; CharIndex < TextLine.size(); ++CharIndex)
        {
            auto Glyph = std::dynamic_pointer_cast<TFTGlyph>(GetGlyph(TextLine[CharIndex]));
            if (Glyph)
            {
                auto GlyBounds = Glyph->Bounds();

                // get kerning
                if (fUseKerning && FT_HAS_KERNING(fFace->Data()) && PrevGlyph)
                {
                    FT_Vector KernDelta;
                    FT_Get_Kerning(fFace->Data().get(), PrevGlyph->CharIndex(), Glyph->CharIndex(),
                        FT_KERNING_UNSCALED, &KernDelta);
                    LineBounds.Right += KernDelta.x * fFace->FontUnitScale().X;
                }

                // update left bound (must be done before right bound is updated)
                LineBounds.Left = std::min(LineBounds.Right + GlyBounds.Left, LineBounds.Left);

                // update right bound
                if (CharIndex < TextLine.size() - 1 ||  // not the last character
                    TextLine[CharIndex] == U' ' ||      // on space char (Bounds.Right == 0)
                    Advance)                             // or in advance mode
                {
                    // add advance && glyph spacing
                    LineBounds.Right += Glyph->Advance().X + GlyphSpacing();
                }
                else
                {
                    // add glyph's right bound
                    LineBounds.Right += GlyBounds.Right;
                }

                // update bottom && top bounds
                LineBounds.Bottom = std::min(GlyBounds.Bottom, LineBounds.Bottom);
                LineBounds.Top = std::max(GlyBounds.Top, LineBounds.Top);
            }

            PrevGlyph = Glyph;
        }

        // handle italic font style
        if (Style().contains(Italic))
        {
            LineBounds.Left += LineBounds.Bottom * cShearFactor;
            LineBounds.Right += LineBounds.Top * cShearFactor;
        }

        // handle underlined font style
        if (Style().contains(Underline))
        {
            double UnderlinePos = UnderlinePosition();
            LineBounds.Bottom = std::min(UnderlinePos, LineBounds.Bottom);
        }

        // add line offset
        LineBounds.Bottom += LineYOffset;
        LineBounds.Top += LineYOffset;

        // adjust global bounds

        res.Left = std::max(LineBounds.Left, res.Left);
        res.Right = std::min(LineBounds.Right, res.Right);
        res.Bottom = std::max(LineBounds.Bottom, res.Bottom);
        res.Top = std::min(LineBounds.Top, res.Top);
    }

    // if left or bottom bound was not set, set them to 0
    if (res.Left == std::numeric_limits<double>::max())
        res.Left = 0.0;
    if (res.Bottom == std::numeric_limits<double>::max())
        res.Bottom = 0.0;

    return res;
}

void TFTFont::Render(const std::u32string& Text)
{
    // reset last glyph
    std::shared_ptr<TFTGlyph> PrevGlyph;

    // draw current line
    for (size_t CharIndex = 0; CharIndex < Text.size(); ++CharIndex)
    {
        auto Glyph = std::dynamic_pointer_cast<TFTGlyph>(GetGlyph(Text[CharIndex]));
        if (Glyph)
        {
            // get kerning
            if (fUseKerning && FT_HAS_KERNING(fFace->Data()) && PrevGlyph)
            {
                FT_Vector KernDelta;
                FT_Get_Kerning(fFace->Data().get(), PrevGlyph->CharIndex(), Glyph->CharIndex(),
                    FT_KERNING_UNSCALED, &KernDelta);
                glTranslatef(KernDelta.x * fFace->FontUnitScale().X, 0, 0);
            }

            if (fReflectionPass)
                Glyph->RenderReflection();
            else
                Glyph->Render(fUseDisplayLists);

            glTranslatef(Glyph->Advance().X + fGlyphSpacing, 0, 0);
        }

        PrevGlyph = Glyph;
    }
}

float TFTFont::Height() const
{
    return Ascender() - Descender();
}

float TFTFont::Ascender() const
{
    return fFace->Data()->ascender * fFace->FontUnitScale().Y + Outset() * 2;
}

float TFTFont::Descender() const
{
    // Note: outset is not part of the descender as the baseline is lifted
    return fFace->Data()->descender * fFace->FontUnitScale().Y;
}

float TFTFont::UnderlinePosition() const
{
    return fFace->Data()->underline_position * fFace->FontUnitScale().Y - Outset();
}

float TFTFont::UnderlineThickness() const
{
    return fFace->Data()->underline_thickness * fFace->FontUnitScale().Y + Outset() * 2;
}


/*
 * TFTScalableFont
 */

TFTScalableFont::TFTScalableFont(const std::filesystem::path& Filename,
    int Size, float OutsetAmount,
    bool UseMipmaps,
    bool PreCache)
    :
    TScalableFont
    (
        std::make_shared<TFTFont>(Filename, Size, Size * OutsetAmount, PreCache, FT_LOAD_DEFAULT | (UseMipmaps ? FT_LOAD_NO_HINTING : 0)),
        UseMipmaps
    )
{
    // Disable hinting && grid-fitting to preserve font outlines at each font
    // size, otherwise the font widths/heights do not match resulting in ugly
    // text size changes during zooming.
    // A drawback is a reduced quality with smaller font sizes but it is not that
    // bad with gray-scaled rendering (at least it looks better than OpenGL's
    // linear downscaling on minification).
}

std::shared_ptr<TFont> TFTScalableFont::CreateMipmap(int Level, float Scale)
{
    const auto BaseFont = std::dynamic_pointer_cast<TFTFont>(fBaseFont);
    float ScaledSize = std::round(BaseFont->Size() * Scale);
    // do not create mipmap fonts < 8 pixels
    if (ScaledSize < 8)
        return nullptr;
    return std::make_shared<TFTFont>(BaseFont->Filename(),
        ScaledSize, BaseFont->Outset() * Scale,
        BaseFont->PreCache(),
        FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);
}

float TFTScalableFont::GetOutset() const
{
    return std::static_pointer_cast<TFTFont>(fBaseFont)->Outset() * fScale;
}

void TFTScalableFont::AddFallback(const std::filesystem::path& Filename)
{
    for (auto& font : fMipmapFonts)
        if (font)
            std::static_pointer_cast<TFTFont>(font)->AddFallback(Filename);
}

void TFTScalableFont::FlushCache(bool KeepBaseSet)
{
    for (auto& font : fMipmapFonts)
        if (font)
            std::static_pointer_cast<TFTFont>(font)->FlushCache(KeepBaseSet);
}


/*
 * TOutlineFont
 */

TFTOutlineFont::TFTOutlineFont(
    const std::filesystem::path Filename,
    int Size, float Outset,
    bool PreCache,
    FT_Int32 LoadFlags)
    :
    TFont(Filename),
    fSize(Size), fOutset(Outset),
	fInnerFont(std::make_shared<TFTFont>(Filename, Size, 0.0, PreCache, LoadFlags)),
    fOutlineFont(std::make_shared<TFTFont>(Filename, Size, Outset, PreCache, LoadFlags)),
    fPreCache(PreCache)
{
    fInnerFont->Part(fpInner);
    fOutlineFont->Part(fpOutline);

    ResetIntern();
}

TFTOutlineFont::~TFTOutlineFont()
{}

void TFTOutlineFont::ResetIntern()
{
    // TODO: maybe swap fInnerFont/fOutlineFont.GlyphSpacing to use the spacing
    // of the outline font?
    //fInnerFont.GlyphSpacing = fOutset*2;
    fOutlineFont->GlyphSpacing(-fOutset * 2);

    fLineSpacing = fOutlineFont->LineSpacing();
    fReflectionSpacing = fOutlineFont->ReflectionSpacing();
    fOutlineColor = TGLColor(0, 0, 0, -1);
}

void TFTOutlineFont::Reset()
{
    ResetIntern();
}

void TFTOutlineFont::DrawUnderline(const std::u32string& Text)
{
    TGLColor CurrentColor;
    // save current color
    glGetFloatv(GL_CURRENT_COLOR, CurrentColor.vals);

    // if the outline's alpha component is < 0 use the current alpha
    TGLColor OutlineColor = fOutlineColor;
    if (OutlineColor.a() < 0)
        OutlineColor.a() = CurrentColor.a();

    // draw underline outline (in outline color)
    glColor4fv(OutlineColor.vals);
    fOutlineFont->DrawUnderline(Text);
    glColor4fv(CurrentColor.vals);

    // draw underline inner part (in current color)
    glPushMatrix();
    glTranslatef(fOutset, 0, 0);
    fInnerFont->DrawUnderline(Text);
    glPopMatrix();
}

void TFTOutlineFont::Render(const std::u32string& Text)
{
    TGLColor CurrentColor;
    // save current color
    glGetFloatv(GL_CURRENT_COLOR, CurrentColor.vals);

    // if the outline's alpha component is < 0 use the current alpha
    TGLColor OutlineColor = fOutlineColor;
    if (OutlineColor.a() < 0)
        OutlineColor.a() = CurrentColor.a();

    //{ setup && render outline font }

    glColor4fv(OutlineColor.vals);
    glPushMatrix();
    fOutlineFont->Render(Text);
    glPopMatrix();
    glColor4fv(CurrentColor.vals);

    //{ setup && render inner font }

    glPushMatrix();
    glTranslatef(fOutset, fOutset, 0);
    fInnerFont->Render(Text);
    glPopMatrix();
}

void TFTOutlineFont::SetOutlineColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    fOutlineColor = TGLColor(r, g, b, a);
}

void TFTOutlineFont::FlushCache(bool KeepBaseSet)
{
    fOutlineFont->FlushCache(KeepBaseSet);
    fInnerFont->FlushCache(KeepBaseSet);
}

void TFTOutlineFont::AddFallback(const std::filesystem::path& Filename)
{
    fOutlineFont->AddFallback(Filename);
    fInnerFont->AddFallback(Filename);
}

TBoundsDbl TFTOutlineFont::BBox(const TUCS4StringArray& Text, bool Advance)
{
    return fOutlineFont->BBox(Text, Advance);
}

float TFTOutlineFont::Height() const
{
    return fOutlineFont->Height();
}

float TFTOutlineFont::Ascender() const
{
    return fOutlineFont->Ascender();
}

float TFTOutlineFont::Descender() const
{
    return fOutlineFont->Descender();
}

void TFTOutlineFont::LineSpacing(float Spacing)
{
    TFont::LineSpacing(Spacing);
    fInnerFont->LineSpacing(Spacing);
    fOutlineFont->LineSpacing(Spacing);
}

void TFTOutlineFont::GlyphSpacing(float Spacing)
{
    TFont::GlyphSpacing(Spacing);
    fInnerFont->GlyphSpacing(Spacing);
    fOutlineFont->GlyphSpacing(Spacing - Outset() * 2);
}

void TFTOutlineFont::ReflectionSpacing(float Spacing)
{
    TFont::ReflectionSpacing(Spacing);
    fInnerFont->ReflectionSpacing(Spacing);
    fOutlineFont->ReflectionSpacing(Spacing);
}

void TFTOutlineFont::Style(TFontStyle Style)
{
    TFont::Style(Style);
    fInnerFont->Style(Style);
    fOutlineFont->Style(Style);
}

TFontStyle TFTOutlineFont::Style() const
{
    return TFont::Style();
}

float TFTOutlineFont::UnderlinePosition() const
{
    return fOutlineFont->UnderlinePosition();
}

float TFTOutlineFont::UnderlineThickness() const
{
    return fOutlineFont->UnderlinePosition();
}

void TFTOutlineFont::UseKerning(bool Enable)
{
    TFont::UseKerning(Enable);
    fInnerFont->UseKerning(Enable);
    fOutlineFont->UseKerning(Enable);
}

void TFTOutlineFont::ReflectionPass(bool Enable)
{
    TFont::ReflectionPass(Enable);
    fInnerFont->ReflectionPass(Enable);
    fOutlineFont->ReflectionPass(Enable);
}

/**
 * TScalableOutlineFont
 */
TFTScalableOutlineFont::TFTScalableOutlineFont(
    const std::filesystem::path Filename,
    int Size, float OutsetAmount,
	bool UseMipmaps,
	bool PreCache)
	:
    TScalableFont(
        std::make_shared<TFTOutlineFont>(Filename, Size, Size * OutsetAmount, PreCache, FT_LOAD_DEFAULT | (UseMipmaps ? FT_LOAD_NO_HINTING : 0)),
        UseMipmaps)
{
    // Disable hinting && grid-fitting (see TFTScalableFont::Create)
}

std::shared_ptr<TFont> TFTScalableOutlineFont::CreateMipmap(int Level, float Scale)
{
    auto BaseFont = std::static_pointer_cast<TFTOutlineFont>(fBaseFont);
    float ScaledSize = std::round(BaseFont->Size() * Scale);
    // do not create mipmap fonts < 8 pixels
    if (ScaledSize < 8)
        return nullptr;
    return std::make_shared<TFTOutlineFont>(BaseFont->Filename(),
        ScaledSize, BaseFont->Outset()* Scale,
        BaseFont->PreCache(),
        FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);
}

float TFTScalableOutlineFont::Outset() const
{
    return std::static_pointer_cast<TFTOutlineFont>(fBaseFont)->Outset() * fScale;
}

void TFTScalableOutlineFont::OutlineColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    for (auto& font : fMipmapFonts)
        if (font)
            std::static_pointer_cast<TFTOutlineFont>(font)->SetOutlineColor(r, g, b, a);
}

void TFTScalableOutlineFont::FlushCache(bool KeepBaseSet)
{
    for (auto& font : fMipmapFonts)
        if (font)
            std::static_pointer_cast<TFTOutlineFont>(font)->FlushCache(KeepBaseSet);
}

void TFTScalableOutlineFont::AddFallback(const std::filesystem::path& Filename)
{
    for (auto& font : fMipmapFonts)
        if (font)
            std::static_pointer_cast<TFTOutlineFont>(font)->AddFallback(Filename);
}


/*
 * TFTGlyph
 */

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
inline const int cTexSmoothBorder = 1;

void TFTGlyph::StrokeBorder(FT_Glyph& Glyph)
/*var
Outline : PFT_Outline;
OuterStroker, InnerStroker:  FT_Stroker;
OuterNumPoints, InnerNumPoints, GlyphNumPoints: FT_UInt;
OuterNumContours, InnerNumContours, GlyphNumContours: FT_UInt;
OuterBorder, InnerBorder: FT_StrokerBorder;
OutlineFlags: FT_Int;
UseStencil: bool;*/
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
    
    // If we are to create the interior of an outlined font (fInner == true)
    // we have to create two borders:
    // - one extruded to the outside by fOutset pixels &&
    // - one extruded to the inside by almost 0 zero pixels.
    // The second one is used as a stencil for the first one, clearing the
    // interiour of the glyph.
    // The stencil is not needed to create bold fonts.
    bool UseStencil = fFont->Part() == fpInner;

    // we cannot extrude bitmaps, only vector based glyphs.
    // Check for FT_GLYPH_FORMAT_OUTLINE otherwise a cast to FT_OutlineGlyph is
    // invalid && FT_Stroker_ParseOutline() will crash
    if (Glyph->format != FT_GLYPH_FORMAT_OUTLINE)
        return;

    auto Outline = FT_OutlineGlyph(Glyph)->outline;

    FT_StrokerBorder InnerBorder;
    auto OuterBorder = FT_Outline_GetOutsideBorder(&Outline);
    if (OuterBorder == FT_STROKER_BORDER_LEFT)
        InnerBorder = FT_STROKER_BORDER_RIGHT;
    else
        InnerBorder = FT_STROKER_BORDER_LEFT;

    //{ extrude outer border }

    FT_Stroker OuterStroker;
    if (FT_Stroker_New(FreeTypeLib.get(), &OuterStroker) != 0)
        throw EFontError("FT_Stroker_New failed!");
    FT_Stroker_Set(
        OuterStroker,
        std::roundl(fOutset * 64),
        FT_STROKER_LINECAP_ROUND,
        FT_STROKER_LINEJOIN_BEVEL,
        0);

    // similar to FT_Glyph_StrokeBorder(inner == FT_FALSE) but it is possible to
    // use FT_Stroker_ExportBorder() afterwards to combine inner && outer borders
    if (FT_Stroker_ParseOutline(OuterStroker, &Outline, false) != 0)
        throw EFontError("FT_Stroker_ParseOutline failed!");

    FT_UInt OuterNumPoints, OuterNumContours;
    FT_Stroker_GetBorderCounts(OuterStroker, OuterBorder, &OuterNumPoints, &OuterNumContours);

    //{ extrude inner border(== stencil) }

    FT_UInt InnerNumPoints, InnerNumContours;
    FT_Stroker InnerStroker;
    if (UseStencil)
    {
      if (FT_Stroker_New(FreeTypeLib.get(), &InnerStroker) != 0)
        throw EFontError("FT_Stroker_New failed!");
      FT_Stroker_Set(
          InnerStroker,
          63, // extrude at most one pixel to avoid a black border
          FT_STROKER_LINECAP_ROUND,
          FT_STROKER_LINEJOIN_BEVEL,
          0);

      if (FT_Stroker_ParseOutline(InnerStroker, &Outline, false) != 0)
        throw EFontError("FT_Stroker_ParseOutline failed!");
      
      FT_Stroker_GetBorderCounts(InnerStroker, InnerBorder, &InnerNumPoints, &InnerNumContours);
    }
    else {
        InnerNumPoints = 0;
        InnerNumContours = 0;
    }

    //{ combine borders(subtract: OuterBorder - InnerBorder) }

    FT_UInt GlyphNumPoints = InnerNumPoints + OuterNumPoints;
    FT_UInt GlyphNumContours = InnerNumContours + OuterNumContours;

    // save flags before deletion (TODO: set them on the resulting outline)
    FT_Int OutlineFlags = Outline.flags;

    // resize glyph outline to hold inner && outer border
    FT_Outline_Done(FreeTypeLib.get(), &Outline);
    if (FT_Outline_New(FreeTypeLib.get(), GlyphNumPoints, GlyphNumContours, &Outline) != 0)
        throw EFontError("FT_Outline_New failed!");

    Outline.n_points = 0;
    Outline.n_contours = 0;

    // add points to outline. The inner-border is used as a stencil.
    FT_Stroker_ExportBorder(OuterStroker, OuterBorder, &Outline);
    if (UseStencil)
        FT_Stroker_ExportBorder(InnerStroker, InnerBorder, &Outline);
    if (FT_Outline_Check(&Outline) != 0)
        throw EFontError("FT_Stroker_ExportBorder failed!");

    if (InnerStroker)
        FT_Stroker_Done(InnerStroker);
    if (OuterStroker)
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
    if (fOutset > 0)
        LoadFlags |= FT_LOAD_NO_BITMAP;

    // load the Glyph for our character
    if (FT_Load_Glyph(fFace->Data().get(), fCharIndex, LoadFlags) != 0)
        throw EFontError("FT_Load_Glyph failed");

    // move the face's glyph into a Glyph object
    auto uniqueGlyph = makeGlyph<TGlyphU>(fFace->Data()->glyph);
    auto Glyph = Glyph.get();

    if (fOutset > 0)
        StrokeBorder(Glyph);

    // store scaled advance width/height in glyph-object
    double common = 64.0 + fOutset * 2.0;
    fAdvance.X = fFace->Data()->glyph->advance.x / common;
    fAdvance.Y = fFace->Data()->glyph->advance.y / common;

    // get the contour's bounding box (in 1/64th pixels, not font-units)
    FT_BBox CBox;
    FT_Glyph_Get_CBox(Glyph, FT_GLYPH_BBOX_UNSCALED, &CBox);
    // convert 1/64th values to double values
    fBounds.Left = CBox.xMin / 64;
    fBounds.Right = CBox.xMax / 64 + fOutset * 2;
    fBounds.Bottom = CBox.yMin / 64;
    fBounds.Top = CBox.yMax / 64 + fOutset * 2;

    // convert the glyph to a bitmap (&& destroy original glyph image).
    // Request 8 bit gray level pixel mode. 
    FT_Glyph_To_Bitmap(&Glyph, FT_RENDER_MODE_NORMAL, nullptr, 1);
    const auto BitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(Glyph);

    // get bitmap offsets
    fBitmapCoords.Left = BitmapGlyph->left - cTexSmoothBorder;
    // Note: add 1*fOutset for lifting the baseline so outset fonts to not intersect
    // with the baseline; std::ceil(fOutset) for the outset pixels added to the bitmap.
    fBitmapCoords.Top = BitmapGlyph->top + fOutset + std::ceill(fOutset) + cTexSmoothBorder;

    // make accessing the bitmap easier
    auto Bitmap = BitmapGlyph->bitmap;
    // get bitmap dimensions
    int post = (std::ceil(fOutset) + cTexSmoothBorder) * 2;
    fBitmapCoords.Width = Bitmap.width + post;
    fBitmapCoords.Height = Bitmap.rows + post;

    // get power-of-2 bitmap widths
    fTexSize.Width =
        NextPowerOf2(fBitmapCoords.Width);
    fTexSize.Height =
        NextPowerOf2(fBitmapCoords.Height);

    // texture-widths ignoring empty (power-of-2) padding space
    fTexOffset.X = static_cast<double>(fBitmapCoords.Width) / fTexSize.Width;
    fTexOffset.Y = static_cast<double>(fBitmapCoords.Height) / fTexSize.Height;

    // allocate memory for texture data
    TGLubyteDynArray TexBuffer = TGLubyteDynArray(fTexSize.Width * fTexSize.Height);

    // Freetype stores the bitmap with either upper (pitch is > 0) or lower
    // (pitch < 0) glyphs line first. Set the buffer to the upper line.
    // See http://freetype.sourceforge.net/freetype2/docs/glyphs/glyphs-7.html

    unsigned char* BitmapBuffer;
    if (Bitmap.pitch > 0)
        BitmapBuffer = &Bitmap.buffer[0];
    else
        BitmapBuffer = &Bitmap.buffer[(Bitmap.rows - 1) * std::abs(Bitmap.pitch)];

    // copy data to texture bitmap (upper line first).
    for (size_t Y = 0; Y < Bitmap.rows; ++Y)
    {
        // set pointer to first pixel in line that holds bitmap data.
        // Each line starts with a cTexSmoothBorder pixel && multiple outset pixels
        // that are added by Extrude() later.
        GLubyte* TexLine = &TexBuffer[(Y + cTexSmoothBorder + std::ceill(fOutset)) * fTexSize.Width +
            cTexSmoothBorder + std::ceill(fOutset)];
        // get next lower line offset, use pitch instead of width as it tells
        // us the storage direction of the lines. In addition a line might be padded.
        auto BitmapLine = &BitmapBuffer[Y * Bitmap.pitch];

        // check for pixel mode && copy pixels
        // Should be 8 bit gray, but even with FT_RENDER_MODE_NORMAL, freetype
        // sometimes (e.g. 16px sized japanese fonts) fallbacks to 1 bit pixels.
        switch (Bitmap.pixel_mode)
        {
        case FT_PIXEL_MODE_GRAY:
        {  // 8 bit gray
            for (int X = 0; X < Bitmap.width; ++X)
                TexLine[X] = BitmapLine[X];
            break;
        }
        case FT_PIXEL_MODE_MONO:
        {  // 1 bit mono
            for (int X = 0; X < Bitmap.width; ++X)
                TexLine[X] = std::numeric_limits<GLubyte>::max() * ((BitmapLine[X / 8] >> (7 - (X % 8))) & 0x1);
            break;
        }
        default:
        {
            // unhandled pixel format
            break;
        }
        }
    }


    // allocate resources for textures && display lists
    glGenTextures(1, &fTexture);

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
        0, GL_ALPHA, GL_UNSIGNED_BYTE, TexBuffer.data());
    
    // create the display list
    fDisplayList = glGenLists(1);

    // render to display-list
    glNewList(fDisplayList, GL_COMPILE);
    Render(false);
    glEndList();

    // free glyph data (bitmap, etc.)
}

TFTGlyph::TFTGlyph(std::shared_ptr<TFTFont> Font, char32_t ch, float Outset,
    FT_Int32 LoadFlags) : fFont(Font), fOutset(Outset), fCharCode(ch)
{
    // Note: the default face is also used if no face (neither default nor fallback)
    // contains a glyph for the given char.
    fFace = Font->DefaultFace();

    // search the Freetype char-index (use default UNICODE charmap) in the default face
    fCharIndex = FT_Get_Char_Index(fFace->Data().get(), ch);
    if (fCharIndex == 0)
    {
        // glyph not in default font, search in fallback font faces
        for (auto& fbFace : Font->FallbackFaces())
        {
            auto strong = fbFace.lock();
            fCharIndex = FT_Get_Char_Index(strong->Data().get(), ch);
            if (fCharIndex != 0)
            {
                fFace = strong;
                break;
            }
        }
    }
    CreateTexture(LoadFlags);
}

TFTGlyph::~TFTGlyph()
{
    if (fDisplayList != 0)
        glDeleteLists(fDisplayList, 1);
    if (fTexture != 0)
        glDeleteTextures(1, &fTexture);
}

void TFTGlyph::Render(bool UseDisplayLists)
{
    // use display-lists if enabled && exit
    if (UseDisplayLists)
    {
        glCallList(fDisplayList);
        return;
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

void TFTGlyph::RenderReflection()
{
    constexpr double CutOff = 0.6;
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, fTexture);

    TGLColor Color;
    glGetFloatv(GL_CURRENT_COLOR, Color.vals);

    // add extra space to the left of the glyph
    glTranslatef(fBitmapCoords.Left, 0, 0);

    // The upper position of the glyph, if CutOff is 1.0, it is fFont.Ascender.
    // If CutOff is set to 0.5 only half of the glyph height is displayed.
    float UpperPos = fFont->Descender() + fFont->Height() * CutOff;

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
    const float TexUpperPos = -(UpperPos - fBitmapCoords.Top) / fBitmapCoords.Height * fTexOffset.Y;
    const float TexLowerPos = (-(fFont->Descender() + fBitmapCoords.Height - fBitmapCoords.Top) /
        fBitmapCoords.Height + 1) * fTexOffset.Y;

    // draw glyph texture
    glBegin(GL_QUADS);
    // top right
    glColor4f(Color.r(), Color.g(), Color.b(), 0);
    glTexCoord2f(fTexOffset.X, TexUpperPos);
    glVertex2f(fBitmapCoords.Width, UpperPos);

    // top left
    glTexCoord2f(0, TexUpperPos);
    glVertex2f(0, UpperPos);

    // bottom left
    glColor4f(Color.r(), Color.g(), Color.b(), Color.a() - 0.3);
    glTexCoord2f(0, TexLowerPos);
    glVertex2f(0, fFont->Descender());

    // bottom right
    glTexCoord2f(fTexOffset.X, TexLowerPos);
    glVertex2f(fBitmapCoords.Width, fFont->Descender());
    glEnd();

    glPopMatrix();

    // restore old color
    // Note: glPopAttrib(GL_CURRENT_BIT)/glPopAttrib() is much slower then
    // glGetFloatv(GL_CURRENT_COLOR, ...)/glColor4fv(...)
    glColor4fv(Color.vals);
}

TPositionDbl TFTGlyph::Advance() const
{
    return fAdvance;
}

TBoundsDbl TFTGlyph::Bounds() const
{
    return fBounds;
}


/*
 * TGlyphCache
 */
bool TGlyphCache::AddGlyph(char32_t ch, const std::shared_ptr<TGlyph> Glyph)
{
    const auto it = fGlyphs.try_emplace(ch, Glyph);
    return it.second;
}

void TGlyphCache::DeleteGlyph(char32_t ch)
{
    fGlyphs.erase(ch);
}

std::shared_ptr<TGlyph> TGlyphCache::GetGlyph(char32_t ch)
{
    auto it = fGlyphs.find(ch);
    return (it != fGlyphs.end()) ? it->second : nullptr;
}

bool TGlyphCache::HasGlyph(char32_t ch) const
{
    return fGlyphs.contains(ch);
}

void TGlyphCache::FlushCache(bool KeepBaseSet)
{
    if (!KeepBaseSet)
    {
        fGlyphs = {};
        return;
    }
    std::erase_if(fGlyphs, [](const auto& item)
        {
    		auto const& [key, value] = item;
			return key >= 256;
        });
}

#ifdef BITMAP_FONT
/*
 * TBitmapFont
 */

TBitmapFont::TBitmapFont(const std::filesystem::path Filename, int Outline,
 int Baseline, int Ascender, int Descender)
	:
	TFont(Filename),
    fTex(UTexture::Texture.LoadTexture(Filename, UTexture::TEXTURE_TYPE_TRANSPARENT, 0)),
    fTexSize(1024),
	fBaseline(Baseline),
	fAscender(Ascender),
	fDescender(Descender),
	fOutline(Outline)
{
    auto FilenameCopy = Filename;
    LoadFontInfo(FilenameCopy.replace_extension(".dat"));

    ResetIntern();
}

TBitmapFont::~TBitmapFont()
{
    glDeleteTextures(1, &fTex->TexNum);
}

void TBitmapFont::ResetIntern()
{
    fLineSpacing = Height();
}

void TBitmapFont::Reset()
{
    TFont::Reset();
    ResetIntern();
}

void TBitmapFont::AddFallback(const std::filesystem::path& Filename)
{
    // no support for fallbacks
}

void TBitmapFont::CorrectWidths(double WidthMult, int WidthAdd)
{
    for (auto& width : fWidths)
        width = std::round(width * WidthMult) + WidthAdd;
}

void TBitmapFont::LoadFontInfo(const std::filesystem::path InfoFile)
{
    try
    {
        auto Stream = std::basic_ifstream<uint8_t>(InfoFile, std::fstream::binary);
        if (!Stream.is_open())
            throw std::exception();

        Stream.read(fWidths.data(), 256);
    }
    catch (...)
    {
        const std::string msg = "Could not read font info file \"" + InfoFile.string() + "\"";
    	throw EFontError(msg.c_str());
    }
}

TBoundsDbl TBitmapFont::BBox(const TUCS4StringArray& Text, bool Advance)
{
    TBoundsDbl res = {
			0, 0, 0, Height()
    };

    for (const auto& Line : Text)
    {
        double LineWidth = 0;
        for (const auto& c : Line)
        {
            const auto CharCode = static_cast<uint32_t>(c);
            if (CharCode < fWidths.size())
                LineWidth += fWidths[CharCode];
        }
        res.Right = std::max(LineWidth, res.Right);
    }
    return res;
}

void TBitmapFont::RenderChar(char32_t ch, double& AdvanceX)
{
    auto CharCode = static_cast<uint32_t>(ch);
    if (CharCode >= fWidths.size())
        CharCode = 0;

    const double GlyphWidth = fWidths[CharCode];

    // set texture positions
    const double TexX = (CharCode % 16) * 1.0 / 16.0 + 1.0 / 32.0 - (GlyphWidth / 2.0 - fOutline) / fTexSize;
    const double TexY = (CharCode / 16) * 1.0 / 16.0 + /*{2 texels}*/ 2.0 / fTexSize;
    const double TexR = (CharCode % 16) * 1.0 / 16.0 + 1.0 / 32.0 + (GlyphWidth / 2.0 + fOutline) / fTexSize;
    const double TexB = (1 + CharCode / 16) * 1.0 / 16.0 - /*{2 texels}*/ 2.0 / fTexSize;

    // set vector positions
    const double PL = AdvanceX - fOutline;
    const double PR = PL + GlyphWidth + fOutline * 2;
    const double PB = -fBaseline;
    const double PT = PB + fTexSize / 16;

    /*
        if (Font.Blend) then
        {
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    */

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fTex->TexNum);

    if (!ReflectionPass)
    {
        glBegin(GL_QUADS);
        glTexCoord2f(TexX, TexY); glVertex2f(PL, PT);
        glTexCoord2f(TexX, TexB); glVertex2f(PL, PB);
        glTexCoord2f(TexR, TexB); glVertex2f(PR, PB);
        glTexCoord2f(TexR, TexY); glVertex2f(PR, PT);
        glEnd();
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
        glEnd();

        glBegin(GL_QUADS);
        glTexCoord2f(TexX, TexY); glVertex2f(PL, PT);
        glTexCoord2f(TexX, TexB); glVertex2f(PL, PB);
        glTexCoord2f(TexR, TexB); glVertex2f(PR, PB);
        glTexCoord2f(TexR, TexY); glVertex2f(PR, PT);

        /*
            glColor4f(fTempColor.r(), fTempColor.g(), fTempColor.b(), 0.7);
        glTexCoord2f(TexX, TexB); glVertex3f(PL, PB, 0);
        glTexCoord2f(TexR, TexB); glVertex3f(PR, PB, 0);

        glColor4f(fTempColor.r, fTempColor.g, fTempColor.b, 0);
        glTexCoord2f(TexR, (TexY + TexB) / 2); glVertex3f(PR, (PT + PB) / 2, 0);
        glTexCoord2f(TexX, (TexY + TexB) / 2); glVertex3f(PL, (PT + PB) / 2, 0);
        */
        glEnd();

        //write the colour back
        glColor4fv(fTempColor.vals);

        glDisable(GL_DEPTH_TEST);
    } // reflection

    glDisable(GL_TEXTURE_2D);
    /*
        if (Font.Blend) then
            glDisable(GL_BLEND);
    */

    AdvanceX += GlyphWidth;
}

void TBitmapFont::Render(const std::u32string& Text)
{
    // if there is no text do nothing
    //if (Text == nil) or (Text[0] == 0)
    //  Exit;

    if (Text.empty())
        return;

    //Save the current color && alpha (for reflection)
    glGetFloatv(GL_CURRENT_COLOR, fTempColor.vals);

    double AdvanceX = 0;
    for (const auto& c : Text)
        RenderChar(c, AdvanceX);
}

float TBitmapFont::Height() const
	{
    return static_cast<float>(fAscender - fDescender);
}

float TBitmapFont::Ascender() const
{
    return static_cast<float>(fAscender);
}

float TBitmapFont::Descender() const
{
    return static_cast<float>(fDescender);
}

float TBitmapFont::UnderlinePosition() const
{
    return -2.0;
}

float TBitmapFont::UnderlineThickness() const
{
    return 1.0;
}

#endif BITMAP_FONT
};