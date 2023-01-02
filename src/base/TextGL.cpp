#include "TextGL.h"

#include "UPathUtils.h"

namespace TextGL
{

	/**
	 * Returns either Filename if it is absolute or a path relative to FontPath.
	 */
	std::filesystem::path FindFontFile(const std::string Filename)
	{
		auto tmp = UPathUtils::FontPath / Filename;
		// if path does not exist, try as an absolute path
		if (!std::filesystem::is_regular_file(tmp))
			return Filename;
	}

	/*
	 * This part of the code is in his own void as we need the font family list
	 * early on to read/write from the user"s configuration file in UIni.
	 */
	void LoadFontFamilyNames()
	{
		CurrentFont.FontFamily = 0;
		CurrentFont.FontStyle = 0;
		const std::string prefix = "Font_";

		auto FontIni = TMemIniFile(UPathUtils::FontPath / "fonts.ini");

		// each section describes one font family
		FontIni.ReadSections(FontSections);

		// read font family names into FontFamilyNamesArray (to be used in ScreenOptionsLyrics)
		FontFamilyNames.resize(0);
		for (const auto& fontSection : FontSections)
		{
			if (fontSection.starts_with(prefix))
			{
				FontFamilyNames.emplace_back(
					FontIni.ReadString(fontSection, "Name", fontSection.substr(prefix.size()))
				);
			}
		}
		// set font array size: Fonts[available font families (defined in fonts.ini)][possible font styles (fixed, see FONT_STYLES)]
		Fonts = std::vector<std::vector<PGLFont>>(FontFamilyNames.size(), std::vector<PGLFont>(FONT_STYLES.size()));
		// close ini-file
	}

	void BuildFonts()
	{
		auto FontIni = TMemIniFile(UPathUtils::FontPath / "fonts.ini");

		try
		{
			std::list<std::string>::iterator SectionName = FontSections.begin();
			for (int FontNameIndex = 0; FontNameIndex < FontSections.size(); ++FontNameIndex, ++SectionName)
			{
				for (int FontStyleIndex = 0; FontStyleIndex < FONT_STYLES.size(); ++FontStyleIndex)
				{
					//std::string SectionName =  FontSections[FontNameIndex];
					const auto& fontStyle = FONT_STYLES[FontStyleIndex];
					auto FontFile = FindFontFile(FontIni.ReadString(*SectionName, fontStyle + "File", ""));
					if (FontFile.empty())
						continue;;

					int FontMaxResolution = FontIni.ReadInteger(*SectionName, fontStyle + "MaxResolution", 64);
					float Embolden = FontIni.ReadFloat(*SectionName, fontStyle + "Embolden", 0.0);
					float Outline = FontIni.ReadFloat(*SectionName, fontStyle + "Outline", 0.0);
					int FontPreCache = FontIni.ReadInteger(*SectionName, fontStyle + "PreCache", 1);

					const auto& font = Fonts[FontNameIndex][FontStyleIndex];


					if (fontStyle != "Outline")
					{
						// normal (non-outlined) font
						font->Font = std::make_shared<UFont::TFTScalableFont>(
							FontFile,
							FontMaxResolution,
							Embolden,
							true,
							(FontPreCache != 0)
							);
						font->Outlined = false;
					}
					else
					{
						// outlined font
						auto OutlineFont = std::make_shared<UFont::TFTScalableOutlineFont>(
							FontFile,
							FontMaxResolution,
							Outline,
							true,
							(FontPreCache != 0)
							);
						OutlineFont->OutlineColor(
							FontIni.ReadFloat(*SectionName, "OutlineColorR", 0.0),
							FontIni.ReadFloat(*SectionName, "OutlineColorG", 0.0),
							FontIni.ReadFloat(*SectionName, "OutlineColorB", 0.0),
							FontIni.ReadFloat(*SectionName, "OutlineColorA", -1.0)
						);
						font->Font = OutlineFont;
						font->Outlined = true;
					}

					font->Font->GlyphSpacing(FontIni.ReadFloat(*SectionName, fontStyle + "GlyphSpacing", 0.0));
					font->Font->Stretch(FontIni.ReadFloat(*SectionName, fontStyle + "Stretch", 1.0));

					for (int FallbackIndex = 1; FallbackIndex <= 25; ++FallbackIndex)
					{
						FontFile = FindFontFile(FontIni.ReadString(*SectionName, fontStyle + "FallbackFile" + std::to_string(FallbackIndex), ""));
						if (FontFile.empty())
							continue;
						try
						{
							font->Font->AddFallback(FontFile);
						}
						catch (const std::exception& e)
						{
							ULog::Log.LogError("Setting font fallback \"" + FontFile.string() + "\" failed: " + e.what());
						}
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			ULog::Log.LogCritical(e.what(), "BuildFont");
		}

		FontSections.resize(0);
	}


	// Deletes the font
	void KillFonts()
		/*var
		  I, J: int;*/
	{
		//TODO:: do we need this?
		/*
	  for I = 0 to High(Fonts) do
		for J = 0 to High(Fonts[I]) do
		  Fonts[I][J].Font.Free;
		  */
	}

	double glTextWidth(const std::string text)
		/*var
		  Bounds: TBoundsDbl;*/
	{
		auto Bounds = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle]->Font->BBox(text, true);
		return Bounds.Right;
	}

	// Custom GL "Print" Routine
	void glPrint(const std::string Text)
		/*var
		  GLFont: PGLFont;*/
	{
		// if there is no text do nothing
		if (Text.empty())
			return;

		PGLFont GLFont = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle];

		glPushMatrix();
		// set font position
		glTranslatef(GLFont->X, GLFont->Y + GLFont->Font->Ascender(), GLFont->Z);
		// draw string
		GLFont->Font->Print(Text);
		glPopMatrix();
	}

	void ResetFont()
	{
		SetFontPos(0, 0);
		SetFontZ(0);
		SetFontItalic(false);
		SetFontReflection(false, 0);
		SetOutlineColor(0, 0, 0, -1);
	}

	void SetFontPos(double X, double Y)
	{
		Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle]->X = X;
		Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle]->Y = Y;
	}

	void SetFontZ(double Z)
	{
		Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle]->Z = Z;
	}

	void SetFontSize(double Size)
	{
		Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle]->Font->Height(Size);
	}

	void SetFontFamily(int FontFamily)
	{
		CurrentFont.FontFamily = FontFamily;
	}

	void SetFontStyle(int FontStyle)
	{
		CurrentFont.FontStyle = FontStyle;
	}

	void SetFont(int Family, int Style)
	{
		SetFontFamily(Family);
		SetFontStyle(Style);
	}

	void SetFont(const TFont& Font)
	{
		SetFontFamily(Font.FontFamily);
		SetFontStyle(Font.FontStyle);
	}

	void SetFontItalic(bool Enable)
	{
		auto& font = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle]->Font;

		if (Enable)
			font->AddStyle(UFont::Italic);
		else
			font->RemoveStyle(UFont::Italic);
	}

	void SetFontReflection(bool Enable, double Spacing)
	{
		auto& font = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle]->Font;

		if (Enable)
			font->AddStyle(UFont::Reflect);
		else
			font->RemoveStyle(UFont::Reflect);
		font->ReflectionSpacing(Spacing - font->Descender());
	}

	void SetOutlineColor(GLfloat R, GLfloat G, GLfloat B, GLfloat A)
	{
		if (Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle]->Outlined())
			std::static_pointer_cast<UFont::TFTScalableOutlineFont>(Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle]->Font)->OutlineColor(R, G, B, A);
	}
}