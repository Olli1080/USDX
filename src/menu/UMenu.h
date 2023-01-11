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

    //type
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

        int Fade; // fade type
		bool ShowFinish; // true if there is no fade
		bool RightMbESC; // true to simulate ESC keypress when RMB is pressed

        TMenu();
        virtual ~TMenu();
        //constructor Create(Back: string);  virtual; // Back is a JPG resource name for background
        //constructor Create(Back: string; W, H: int);  virtual; // W and H are the number of overlaps

        // interaction
        void AddInteraction(int Typ, int Num);
        virtual void SetInteraction(int Num);

        //property Interaction: int read SelInteraction write SetInteraction;

        // void load bg, texts, statics and button collections from themebasic
        void LoadFromTheme(const TThemeBasic ThemeBasic);

        void PrepareButtonCollections(const Collections : AThemeButtonCollection);
        void AddButtonCollection(const ThemeCollection : TThemeButtonCollection; const Num : byte);

        // background
        void AddBackground(TThemeBackground ThemedSettings);

        // static
        int AddStatic(ThemeStatic: TThemeStatic) 
        int AddStatic(X, Y, W, H: real; const TexName : IPath) : int; 
        int AddStatic(X, Y, W, H: real; const TexName : IPath; Typ: TTextureType) : int; 
        int AddStatic(X, Y, W, H: real; ColR, ColG, ColB: real; const TexName : IPath; Typ: TTextureType) : int; 
        int AddStatic(X, Y, W, H, Z: real; ColR, ColG, ColB: real; const TexName : IPath; Typ: TTextureType) : int; 
        int AddStatic(X, Y, W, H: real; ColR, ColG, ColB: real; const TexName : IPath; Typ: TTextureType; Color: int) : int; 
        int AddStatic(X, Y, W, H, Z: real; ColR, ColG, ColB: real; const TexName : IPath; Typ: TTextureType; Color: int) : int; 
        int AddStatic(X, Y, W, H, Z: real; ColR, ColG, ColB: real; TexX1, TexY1, TexX2, TexY2: real; Alpha: real; const TexName : IPath; Typ: TTextureType; Color: int; Reflection: bool; ReflectionSpacing: real) : int; 

        // list
        function AddListItem(X, Y, W, H, Z: real; ColR, ColG, ColB: real; DColR, DColG, DColB: real; const TexName : IPath; const DTexName : IPath; Typ: TTextureType; Reflection: bool; ReflectionSpacing: real) : int;

        // text
        function AddText(ThemeText: TThemeText) : int; 
        function AddText(X, Y: real; const Text_ : UTF8String) : int; 
        function AddText(X, Y: real; Font, Style: int; Size, ColR, ColG, ColB: real; const Text : UTF8String) : int; 
        function AddText(X, Y, W, H: real; Font, Style: int; Size, ColR, ColG, ColB: real; Align: int; const Text_ : UTF8String; Reflection_: bool; ReflectionSpacing_: real; Z: real; Writable: bool) : int; 

        // button
        void SetButtonLength(Length: cardinal); //Function that Set Length of Button Array in one Step instead of register new Memory for every Button
        function AddButton(ThemeButton: TThemeButton) : int; 
        function AddButton(X, Y, W, H: real; const TexName : IPath) : int; 
        function AddButton(X, Y, W, H: real; const TexName : IPath; Typ: TTextureType; Reflection: bool) : int; 
        function AddButton(X, Y, W, H, ColR, ColG, ColB, Int, DColR, DColG, DColB, DInt: real; const TexName : IPath; Typ: TTextureType; Reflection: bool; ReflectionSpacing, DeSelectReflectionSpacing: real) : int; 
        void ClearButtons();
        void AddButtonText(AddX, AddY: real; const AddText : UTF8String); 
        void AddButtonText(AddX, AddY: real; ColR, ColG, ColB: real; const AddText : UTF8String); 
        void AddButtonText(AddX, AddY: real; ColR, ColG, ColB: real; Font, Style: int; Size: int; Align: int; const AddText : UTF8String); 
        void AddButtonText(CustomButton: TButton; AddX, AddY: real; ColR, ColG, ColB: real; Font, Style: int; Size: int; Align: int; const AddText : UTF8String); 

        // select slide
        function AddSelectSlide(ThemeSelectS: TThemeSelectSlide; var Data : int; const Values : array of UTF8String) : int; 
        function AddSelectSlide(X, Y, W, H, SkipX, SBGW, ColR, ColG, ColB, Int, DColR, DColG, DColB, DInt,
            TColR, TColG, TColB, TInt, TDColR, TDColG, TDColB, TDInt,
            SBGColR, SBGColG, SBGColB, SBGInt, SBGDColR, SBGDColG, SBGDColB, SBGDInt,
            STColR, STColG, STColB, STInt, STDColR, STDColG, STDColB, STDInt: real;
        const TexName : IPath; Typ: TTextureType; const SBGName : IPath; SBGTyp: TTextureType;
        const Caption : UTF8String; var Data : int) : int; 
        void AddSelectSlideOption(const AddText : UTF8String); 
        void AddSelectSlideOption(SelectNo: cardinal; const AddText : UTF8String); 
        void UpdateSelectSlideOptions(ThemeSelectSlide: TThemeSelectSlide; SelectNum: int; const Values : array of UTF8String; var Data : int);

        //      function AddWidget(X, Y : UInt16; WidgetSrc : PSDL_Surface): Int16;
        //      void ClearWidgets(MinNumber : Int16);
        void FadeTo(Screen: PMenu); 
        void FadeTo(Screen: PMenu; aSound: TAudioPlaybackStream); 
        //popup hack
        void CheckFadeTo(Screen: PMenu; Msg: UTF8String);

        function DrawBG : bool; virtual;
        function DrawFG : bool; virtual;
        function Draw : bool; virtual;
        function ShouldHandleInput(PressedKey: cardinal; CharCode: UCS4Char; PressedDown: bool; out SuppressKey : bool) : bool; virtual;
        function ParseInput(PressedKey: cardinal; CharCode: UCS4Char; PressedDown: bool) : bool; virtual;
        function ParseMouse(MouseButton: int; BtnDown: bool; X, Y: int) : bool; virtual;
        function InRegion(X, Y: real; A: TMouseOverRect) : bool;
        function InRegionX(X: real; A: TMouseOverRect) : bool;
        function InRegionY(Y: real; A: TMouseOverRect) : bool;
        function InteractAt(X, Y: real) : int;
        function CollectionAt(X, Y: real) : int;
        virtual void OnShow();
        virtual void OnShowFinish();
        virtual void OnHide();

        void OnWindowResized(); virtual;

        void SetAnimationProgress(Progress: real); virtual;

        function IsSelectable(Int: cardinal) : bool;

        virtual void InteractNext();
        void InteractCustom(CustomSwitch: int); virtual;
        virtual void InteractPrev();
        virtual void InteractInc();
        virtual void InteractDec();
        virtual void InteractNextRow(); // this is for the options screen, so button down makes sense
        virtual void InteractPrevRow(); // this is for the options screen, so button up makes sense
        void AddBox(X, Y, W, H: real);
    };

    function RGBFloatToInt(R, G, B: double) : cardinal;

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