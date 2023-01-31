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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UMenuSelectSlide.pas $
 * $Id: UMenuSelectSlide.pas 2293 2010-04-23 22:39:26Z tobigun $
 */
#include <string>
#include <vector>

#include "../base/UCommon.h"

namespace UMenuSelectSlide
{
/*uses
  dglOpenGL,
  TextGL,
  UMenuText,
  UTexture,
  UMenuInteract;*/

//type
  //PSelectSlide = ^TSelectSlide;
  class TSelectSlide
  {
  private:
      bool SelectBool;

      std::string AdjustOptionTextToFit(std::string OptText);

  public:
      // objects
      TText Text; // Main text describing option
      std::vector<TText> TextOpt; // 3 texts in the position of possible options
      std::vector<std::string> TextOptT; // array of names for possible options

      TTexture Texture; // Select Texture
      TTexture TextureSBG; // Background Selections Texture

      bool Colorized;
      TTexture DeSelectTexture; // texture for colorized hack
      bool ColorizedSBG;
      TTexture DeSelectTextureSBG; // texture for colorized hack Select BG

      TTexture Tex_SelectS_ArrowL; // Texture for left arrow
      TTexture Tex_SelectS_ArrowR; // Texture for right arrow

      int SelectOptInt;
      int *PData;

      //For automatically Setting LineCount
      uint8_t Lines;

      //Arrows on/off
      bool showArrows;      //default is false

      //whether to show one item or all that fit into the select
      bool oneItemOnly;      //default is false

      //Visibility
      bool Visible;

      // for selection and deselection
      // main static
      UCommon::TRGB<double> Col;
      double Int;
      UCommon::TRGB<double> DCol;
      double DInt;

      // main text
      UCommon::TRGB<double> TCol;
      double TInt;
      UCommon::TRGB<double> TDCol;
      double TDInt;

      // selection background static
      UCommon::TRGB<double> SBGCol;
      double SBGInt;
      UCommon::TRGB<double> SBGDCol;
      double SBGDInt;

      // selection text
      UCommon::TRGB<double> STCol;
      double STInt;
      UCommon::TRGB<double> STDCol;
      double STDInt;
      
      // position and size
      //property X: double read Texture.x write Texture.x;
      //property Y: double read Texture.y write Texture.y;
      //property W: double read Texture.w write Texture.w;
      //property H: double read Texture.h write Texture.h;

//      property X2: double read Texture2.x write Texture2.x;
//      property Y2: double read Texture2.y write Texture2.y;
//      property W2: double read Texture2.w write Texture2.w;
//      property H2: double read Texture2.h write Texture2.h;

      //property SBGW: double read TextureSBG.w write TextureSBG.w;

      // procedures
      void SetSelect(bool Value);
      //property Selected: bool read SelectBool write SetSelect;
      void SetSelectOpt(int Value);
      //property SelectedOption: int read SelectOptInt write SetSelectOpt;
      void Draw();

      TSelectSlide();

      //Automatically Generate Lines (Texts)
      void GenLines();

      TMouseOverRect GetMouseOverArea();
      TMouseClickAction OnClick(double X, double Y);
  };

const int ArrowAlphaOptionsLeft = 1;
const int ArrowAlphaNoOptionsLeft = 0;
const int MinItemSpacing = 5;
const int MinSideSpacing = 24;
/*
implementation

uses
  math,
  SysUtils,
  UDrawTexture,
  ULog;*/

// ------------ Select
TSelectSlide::TSelectSlide()
{
  inherited Create;
  Text = TText.Create;
  SetLength(TextOpt, 1);
  TextOpt[0] = TText.Create;
  Visible = true;

  Colorized = false;
  ColorizedSBG = false;
  Col = { 1, 1, 1 };
  Int = 1;
  DCol = { 1, 1, 1 };
  DInt = 1;

  SBGCol = { 1, 1, 1 };
  SBGInt = 1;
  SBGDCol = { 1, 1, 1 };
  SBGDInt = 1;
}

void TSelectSlide::SetSelect(bool Value)
{
  SelectBool = Value;
  if (Value)
  {
    Texture.Col = Col;
    Texture.Int = Int;

    Text.Col = TCol;
    Text.Int = TInt;

    TextureSBG.Col = SBGCol;
    TextureSBG.Int = SBGInt;
  }
  else
  {
    if (Colorized)
      DeSelectTexture.Int = DInt;
    else
    {
      Texture.Col = DCol;
      Texture.Int = DInt;
    }

    Text.Col = TDCol;
    Text.Int = TDInt;

    if (ColorizedSBG)
      DeselectTextureSBG.Int = SBGDInt;
    else
    {
      TextureSBG.Col = SBGDCol;
      TextureSBG.Int = SBGDInt;
    }
  }
}

void TSelectSlide::SetSelectOpt(int Value)
//var
  //int SO;
  //int HalfL;
  //int HalfR; 
{
  auto DoSelection = [this](uint32_t Sel) -> void
  //var
    //int I;
  {
    for (auto& opt : TextOpt)
    {
      opt.Col = STDCol;
      opt.Int = STDInt;
    }

    if (Sel < TextOpt.size())
    {
      TextOpt[Sel].Col = STCol;
      TextOpt[Sel].Int = STInt;
     }
   };


  SelectOptInt = Value;
  PData = Value;

  if (!TextOpt.empty() && !TextOptT.empty())
  {

    //First option selected
    if (Value <= 0)
    {
      Value = 0;

      Tex_SelectS_ArrowL.alpha = ArrowAlphaNoOptionsLeft;
      if (TextOptT.size() > 1)
        Tex_SelectS_ArrowR.alpha = ArrowAlphaOptionsLeft;
      else
        Tex_SelectS_ArrowR.alpha = ArrowAlphaNoOptionsLeft;

      for (size_t SO = 0; SO < TextOpt.size(); ++SO)
        TextOpt[SO].Text = AdjustOptionTextToFit(TextOptT[SO]);
      
      DoSelection(0);
    }

    //Last option selected
    else if (Value >= TextOptT.size() - 1)
    {
      Value = TextOptT.size() - 1;

      Tex_SelectS_ArrowL.alpha = ArrowAlphaOptionsLeft;
      Tex_SelectS_ArrowR.alpha = ArrowAlphaNoOptionsLeft;

      for (size_t SO = TextOpt.size() - 1; SO >= 0; --SO)
        TextOpt[SO].Text = AdjustOptionTextToFit(TextOptT[TextOptT.size() - 1 - (Lines - SO - 1)]);

      DoSelection(Lines - 1);
    }

    //in between first and last
    else
    {
      Tex_SelectS_ArrowL.alpha = ArrowAlphaOptionsLeft;
      Tex_SelectS_ArrowR.alpha = ArrowAlphaOptionsLeft;

      int HalfL = std::ceil((Lines - 1) / 2);
      int HalfR = Lines - 1 - HalfL;

      //Selected option is near to the left side
      if (Value <= HalfL)
      {
        //Change texts
      	for (size_t SO = 0; SO < TextOpt.size(); ++SO)
      	  TextOpt[SO].Text = AdjustOptionTextToFit(TextOptT[SO]);

      	DoSelection(Value);
      }
  
      //Selected option is near to the right side
      else if (Value > TextOptT.size() - 1 - HalfR)
      {
      	HalfR = TextOptT.size() - 1 - Value;
      	HalfL = Lines - 1 - HalfR;
      	//Change texts
       	for (size_t SO = TextOpt.size() - 1; SO >= 0; --SO)
	        TextOpt[SO].Text = AdjustOptionTextToFit(TextOptT[TextOptT.size() - 1 - (Lines - SO - 1)]);

      	DoSelection (HalfL);
      }

      else
      {
      	//Change Texts
       	for (size_t SO = 0; SO < TextOpt.size(); ++SO)
	        TextOpt[SO].Text = AdjustOptionTextToFit(TextOptT[Value - HalfL + SO]);

        DoSelection(HalfL);
      }
    }
  }
}

// cuts the text if it is too long to fit on the selectbg
std::string TSelectSlide::AdjustOptionTextToFit(std::string OptText)
  //var
    //double MaxLen;
    //int Len;
{
  std::string Result = OptText;
  
  if (TextureSBG.W > 0)
  {
    double MaxLen = TextureSBG.W;
    if (showArrows)
      MaxLen -= MinSideSpacing * 2;

    SetFontFamily(0);
    SetFontStyle(ftRegular);
    SetFontSize(Text.Size);

    // we will remove min. 2 letters by default and replace them w/ points
    // if the whole text don't fit
    size_t Len = OptText.size() - 1;

    while (glTextWidth(Result) > MaxLen && Len > 0)
    {
      // ensure that we only cut at full letters
      /* this code may be a problem if there is a text that
        consists of many multi byte characters and only few
        one byte characters TODO::*/
      repeat
        Dec(Len);
      until (byte(OptText[Len]) and 128) == 0;
      
      Result = copy(OptText, 1, Len) + '..';
    }
  }
  return Result;
}

void TSelectSlide::Draw()
//var
  //int SO;
{
  if (Visible)
  {
    if (SelectBool || !Colorized)
    {
      DrawTexture(Texture);
    }
    else
    {
      DeselectTexture.X = Texture.X;
      DeselectTexture.Y = Texture.Y;
      DeselectTexture.W = Texture.W;
      DeselectTexture.H = Texture.H;
      DrawTexture(DeselectTexture);
    }

    if (SelectBool || !ColorizedSBG)
    {
      DrawTexture(TextureSBG);
    }
    else
    {
      DeselectTextureSBG.X = TextureSBG.X;
      DeselectTextureSBG.Y = TextureSBG.Y;
      DeselectTextureSBG.W = TextureSBG.W;
      DeselectTextureSBG.H = TextureSBG.H;
      DrawTexture(DeselectTextureSBG);
    }

    if (showArrows)
    {
      DrawTexture(Tex_SelectS_ArrowL);
      DrawTexture(Tex_SelectS_ArrowR);
    }

    Text.Draw();

    for (auto& opt : TextOpt)
      opt.Draw();

  }
}

void TSelectSlide::GenLines()
//var
  //double maxlength;
  //int I;
{
  SetFontFamily(0);
  SetFontStyle(ftRegular{Text.Style});
  SetFontSize(Text.Size);
  double maxlength = 0;

  for (const auto& optT : TextOptT)
    maxlength = std::max(glTextWidth(optT), maxlength);


  if (!oneItemOnly)
  {
    //show all items
    Lines = std::floor((TextureSBG.W - MinSideSpacing * 2) / (maxlength + MinItemSpacing));
    Lines = std::max<uint8_t>(1, std::max<uint8_t>(TextOptT.size(), Lines));
  }
  else
  {
    //show one item only
    Lines = 1;
  }

  //Free old Space used by Texts
  for (size_t I = 0; I < TextOpt.size(); ++I)
    TextOpt[I].Free;
    
  TextOpt.resize(Lines);

  for (size_t I = 0; I < TextOpt.size(); ++I)
  {
    TextOpt[I] = TText.Create;
    TextOpt[I].Size = Text.Size;
    TextOpt[I].Visible = true;
    TextOpt[I].Style = 0;

    TextOpt[I].Col = STDCol;
    TextOpt[I].Int = STDInt;

    // generate positions
    TextOpt[I].Y = TextureSBG.Y + (TextureSBG.H - Text.Size) / 2;

    // better look with 2 options and a single option
    if (Lines == 2)
    {
      TextOpt[I].X = TextureSBG.X + 20 + (TextureSBG.W -40 - glTextWidth(TextOptT[1])) * I;
      TextOpt[I].Align = 0;
    }
    else if (Lines == 1)
    {
      TextOpt[I].X = TextureSBG.X + (TextureSBG.W / 2);
      TextOpt[I].Align = 1; //center text
    }
    else
    {
      TextOpt[I].X = TextureSBG.X + TextureSBG.W / 2 + (TextureSBG.W - MinSideSpacing*2) * (I / Lines - 0.5);
      TextOpt[I].Align = 0;
    }
  }
}

TMouseOverRect TSelectSlide::GetMouseOverArea()
{
  TMouseOverRect Result;
  Result.X = Texture.X;
  Result.Y = Texture.Y;
  Result.W = (TextureSBG.X + TextureSBG.W) - Result.X;
  Result.H = std::max(Texture.H, TextureSBG.H);

  return Result;
}

TMouseClickAction TSelectSlide::OnClick(double X, double Y)
  //var
    //Real AreaW;
{
  // default: press return on click

  // use left sides to inc or dec selection by click
  double AreaW = Tex_SelectS_ArrowL.W;

  if (Y >= TextureSBG.Y && Y <= TextureSBG.Y + TextureSBG.H)
  {
    if (X >= TextureSBG.X && X <= TextureSBG.X + AreaW)
      return maLeft;   // hit left area
    else if (X >= TextureSBG.X + TextureSBG.W - AreaW && X <= TextureSBG.X + TextureSBG.W)
      return maRight; // hit right area
  }
  return maReturn;
}
}