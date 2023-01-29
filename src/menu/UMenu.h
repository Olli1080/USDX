#pragma once
/* UltraStar Deluxe - Karaoke Game
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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UMenu.pas $
 * $Id: UMenu.pas 3103 2014-11-22 23:21:19Z k-m_schindler $
 */
#include "../switches.h"

#include "../base/UCommon.h"
#include "../base/types.hpp"
#include <filesystem>

namespace UMenu
{
    /*
    uses
      SysUtils,
      Math,
      dglOpenGL,
      sdl2,
      UPath,
      UMenuBackground,
      UMenuButton,
      UMenuButtonCollection,
      UMenuInteract,
      UMenuSelectSlide,
      UMenuStatic,
      UMenuText,
      UMusic,
      UTexture,
      UThemes;
    */

    // type
    //{  Int16 = SmallInt;}

    //  PMenu = ^TMenu;
    class TMenu
    {
    protected:
        std::vector<TInteract> Interactions;
        int SelInteraction;

        int ButtonPos;
        std::vector<TButton> Button;

        std::vector<TSelectSlide> SelectsS;
        std::vector<TButtonCollection> ButtonCollection;

    public:
        TMenuBackground Background;
        std::vector<TText> Text;
        std::vector<TStatic> Statics;
        std::vector<TStatic> StaticsList;
        int mX; // mouse X
        int mY; // mouse Y

        int Fade;        // fade type
        bool ShowFinish; // true if there is no fade
        bool RightMbESC; // true to simulate ESC keypress when RMB is pressed

        TMenu();
        virtual ~TMenu();
        // constructor Create(Back: string);  virtual; // Back is a JPG resource name for background
        // constructor Create(Back: string; W, H: int);  virtual; // W and H are the number of overlaps

        // interaction
        void AddInteraction(int Typ, int Num);
        virtual void SetInteraction(int Num);

        // property Interaction: int read SelInteraction write SetInteraction;

        // void load bg, texts, statics and button collections from themebasic
        void LoadFromTheme(const TThemeBasic ThemeBasic);

        void PrepareButtonCollections(const AThemeButtonCollection Collections);
        void AddButtonCollection(const TThemeButtonCollection ThemeCollection, const uint8_t Num);

        // background
        void AddBackground(TThemeBackground ThemedSettings);

        // static
        int AddStatic(TThemeStatic ThemeStatic);
        int AddStatic(Rect<double> rect, const std::filesystem::path TexName);
        int AddStatic(Rect<double> rect, const std::filesystem::path TexName, TTextureType Typ);
        int AddStatic(Rect<double> rect, UCommon::TRGB<double> Col, const std::filesystem::path TexName, TTextureType Typ);
        int AddStatic(Rect<double> rect, double Z, UCommon::TRGB<double> Col, const std::filesystem::path TexName, TTextureType Typ);
        int AddStatic(Rect<double> rect, UCommon::TRGB<double> Col, const std::filesystem::path TexName, TTextureType Typ, int Color);
        int AddStatic(Rect<double> rect, double Z, UCommon::TRGB<double> Col, const std::filesystem::path TexName, TTextureType Typ, int Color);
        int AddStatic(Rect<double> rect, double Z, UCommon::TRGB<double> Col, UCommon::TTexCoords Tex, double Alpha, const std::filesystem::path TexName, TTextureType Typ, int Color, bool Reflection, double ReflectionSpacing);

        // list
        int AddListItem(Rect<double> rect, double Z, UCommon::TRGB<double> Col, UCommon::TRGB<double> DCol, const std::filesystem::path TexName, const std::filesystem::path DTexName, TTextureType Typ, bool Reflection, double ReflectionSpacing);

        // text
        int AddText(TThemeText ThemeText);
        int AddText(Position2D<double> Pos, const std::string Text_);
        int AddText(Position2D<double> Pos, int Font, int Style, double Size, UCommon::TRGB<double> Col, const std::string Text);
        int AddText(Rect<double> rect, int Font, int Style, double Size, UCommon::TRGB<double> Col, int Align, const std::string Text_, bool Reflection_, double ReflectionSpacing_, double Z, bool Writable);

        // button
        void SetButtonLength(uint32_t Length); // Function that Set Length of Button Array in one Step instead of register new Memory for every Button
        int AddButton(TThemeButton ThemeButton);
        int AddButton(Rect<double> rect, const std::filesystem::path TexName);
        int AddButton(Rect<double> rect, const std::filesystem::path TexName, TTextureType Typ, bool Reflection);
        int AddButton(Rect<double> rect, UCommon::TRGB<double> Col, double Int, UCommon::TRGB<double> DCol, double DInt, const std::filesystem::path TexName, TTextureType Typ, bool Reflection, double ReflectionSpacing, double DeSelectReflectionSpacing);
        void ClearButtons();
        void AddButtonText(Position2D<double> AddPos, const std::string AddText);
        void AddButtonText(Position2D<double> AddPos, UCommon::TRGB<double> Col, const std::string AddText);
        void AddButtonText(Position2D<double> AddPos, UCommon::TRGB<double> Col, int Font, int Style, int Size, int Align, const std::string AddText);
        void AddButtonText(TButton CustomButton, Position2D<double> AddPos, UCommon::TRGB<double> Col, int Font, int Style, int Size, int Align, const std::string AddText);

        // select slide
        int AddSelectSlide(TThemeSelectSlide ThemeSelectS, int &Data, const std::vector<std::string> Values);
        int AddSelectSlide(Rect<double> rect, double SkipX, double SBGW,
                           UCommon::TRGB<double> Col, double Int,
                           UCommon::TRGB<double> DCol, double DInt,
                           UCommon::TRGB<double> TCol, double TInt,
                           UCommon::TRGB<double> TDCol, double TDInt,
                           UCommon::TRGB<double> SBGCol, double SBGInt,
                           UCommon::TRGB<double> SBGDCol, double SBGDInt,
                           UCommon::TRGB<double> STCol, double STInt,
                           UCommon::TRGB<double> STDCol, double STDInt,
                           const std::filesystem::path TexName, TTextureType Typ,
                           const std::filesystem::path SBGName, TTextureType SBGTyp,
                           const std::string Caption, int &Data);

        void AddSelectSlideOption(const std::string AddText);
        void AddSelectSlideOption(uint32_t SelectNo, const std::string AddText);
        void UpdateSelectSlideOptions(TThemeSelectSlide ThemeSelectSlide, int SelectNum, const std::vector<std::string> Values, int &Data);

        //      function AddWidget(X, Y : UInt16; WidgetSrc : PSDL_Surface): Int16;
        //      void ClearWidgets(MinNumber : Int16);
        void FadeTo(PMenu Screen);
        void FadeTo(PMenu Screen, TAudioPlaybackStream aSound);
        // popup hack
        void CheckFadeTo(PMenu Screen, std::string Msg);

        virtual bool DrawBG();
        virtual bool DrawFG();
        virtual bool Draw();
        virtual bool ShouldHandleInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown, bool &SuppressKey);
        virtual bool ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown);
        virtual bool ParseMouse(int MouseButton, bool BtnDown, int X, int Y);
        bool InRegion(double X, double Y, TMouseOverRect A);
        bool InRegionX(double X, TMouseOverRect A);
        bool InRegionY(double Y, TMouseOverRect A);
        int InteractAt(double X, double Y);
        int CollectionAt(double X, double Y);
        virtual void OnShow();
        virtual void OnShowFinish();
        virtual void OnHide();

        virtual void OnWindowResized();

        virtual void SetAnimationProgress(double Progress);

        bool IsSelectable(uint32_t Int);

        virtual void InteractNext();
        virtual void InteractCustom(int CustomSwitch);
        virtual void InteractPrev();
        virtual void InteractInc();
        virtual void InteractDec();
        virtual void InteractNextRow(); // this is for the options screen, so button down makes sense
        virtual void InteractPrevRow(); // this is for the options screen, so button up makes sense
        void AddBox(Rect<double> rect);
    };

    uint32_t RGBFloatToInt(UCommon::TRGB<double> Col);

    constexpr int MENU_MDOWN = 8;
    constexpr int MENU_MUP = 0;

    constexpr int pmMove = 1;
    constexpr int pmClick = 2;
    constexpr int pmUnClick = 3;

    constexpr int iButton = 0; // interaction type
    constexpr int iText = 2;
    constexpr int iSelectS = 3;
    constexpr int iBCollectionChild = 5;

    //  fBlack = 0; // fade type
    //  fWhite = 1;
}