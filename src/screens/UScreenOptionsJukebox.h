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
 * $URL: $
 * $Id: $
 */
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>

#include "../menu/UMenu.h"

namespace UScreenOptionsJukebox
{
/*uses
  UCommon,
  UDisplay,
  UFiles,
  UIni,
  ULyrics,
  UMenu,
  UMusic,
  UThemes,
  sdl2,
  TextGL;*/

  class TScreenOptionsJukebox : public UMenu::TMenu
  {
    private:

      TLyricEngine Lyrics;
      TLine Line;

      int FontSelect;
      int StyleSelect;
      int LineSelect;
      int PropertySelect;
      int LineColorSelect;
      int RedSelect;
      int GreenSelect;
      int BlueSelect;

      int PointerR;
      int PointerG;
      int PointerB;

      int TexR;
      int TexG;
      int TexB;
      int TexColor;

      int Red;
      int Green;
      int Blue;

      double RSalt;
      double GSalt;
      double BSalt;

      double StartR;
      double StartG;
      double StartB;

      SingColor: array [0..2] of TRGB;
      SingOutlineColor: array [0..2] of TRGB;

      NextColor: array [0..2] of TRGB;
      NextOutlineColor: array [0..2] of TRGB;

      ActualColor: array [0..2] of TRGB;
      ActualOutlineColor: array [0..2] of TRGB;

    public:

      TScreenOptionsJukebox();

      bool ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown) override;
      void OnShow() override;
      bool Draw() override;
      void InteractInc() override;
      void InteractDec() override;

      void RefreshSelectsColors();
      void ChangeOtherColor();
      void ChangeOtherOColor();
      void ChangeColor();

      void UpdatePropertyList();
      void LyricSample();
  };

const std::string ID = "ID_081";   //for help system
/*
implementation

uses
  UGraphic,
  UHelp,
  ULog,
  UUnicodeUtils,
  SysUtils;*/

bool TScreenOptionsJukebox::ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown)
//var
  //SDL_ModState:  word;
  //Salt_Mod:      int;
{
  if (!PressedDown)
    return true;
  // Key Down
  // check normal keys
  switch (UCS4UpperCase(CharCode))
  {
    case Ord("Q"):
    {
      return false;
    }
  }

  uint64_t SDL_ModState = SDL_GetModState() & (KMOD_LSHIFT | KMOD_RSHIFT
            | KMOD_LCTRL | KMOD_RCTRL | KMOD_LALT | KMOD_RALT);

  int Salt_Mod = 0;
  // check special keys
  switch (PressedKey)
  {
    case SDLK_ESCAPE: [[fallthrough]]
    case SDLK_BACKSPACE :
      {
        UIni::Ini.Save();
        AudioPlayback.PlaySound(SoundLib.Back);
        FadeTo(@ScreenOptions);
        break;
      }
    case SDLK_TAB:
      {
        ScreenPopupHelp.ShowPopup();
        break;
      }
    case SDLK_RETURN:
      {
        if (SelInteraction == 10)
        {
          Ini.Save;
          AudioPlayback.PlaySound(SoundLib.Back);
          FadeTo(@ScreenOptions);
        }
        break;
      }
    case SDLK_DOWN:
      InteractNext();
      break;
    case SDLK_UP:
      InteractPrev();
      break;
    case SDLK_RIGHT:
      {
        if (SelInteraction >= 0 && SelInteraction <= 9)
        {
          AudioPlayback.PlaySound(SoundLib.Option);

          if (SelInteraction >= 7 && SelInteraction <= 9) 
          {
            if (SDL_ModState & (KMOD_LSHIFT | KMOD_RSHIFT) != 0 && SelectsS[SelInteraction].SelectOptInt <= 245) 
              Salt_Mod = 9;
            else
              Salt_Mod = 0;

            if (SelectsS[PropertySelect].SelectedOption == 0) 
            {
              switch (SelectsS[LineSelect].SelectedOption)
              {
                case 0: 
                SelectsS[LineColorSelect].SetSelectOpt(High(ISingLineColor));
                break;
                case 1: 
                SelectsS[LineColorSelect].SetSelectOpt(High(IActualLineColor));
                break;
                case 2: 
                SelectsS[LineColorSelect].SetSelectOpt(High(INextLineColor));
                break;
              }
            }

            if (SelectsS[PropertySelect].SelectedOption = 1) 
              SelectsS[LineColorSelect].SetSelectOpt(2);

          }

          if (SelInteraction == RedSelect && Red < 255) 
          {
            Red += Salt_Mod;
            SelectsS[SelInteraction].SelectOptInt = Red;
            Statics[PointerR].Texture.X = Statics[PointerR].Texture.X + RSalt * (Salt_Mod + 1);
          }

          if (SelInteraction == GreenSelect && Green < 255) 
          {
            Green += Salt_Mod;
            SelectsS[SelInteraction].SelectOptInt = Green;
            Statics[PointerG].Texture.X = Statics[PointerG].Texture.X + GSalt * (Salt_Mod + 1);
          }

          if (SelInteraction == BlueSelect && Blue < 255) 
          {
            Blue += Salt_Mod;
            SelectsS[SelInteraction].SelectOptInt = Blue;
            Statics[PointerB].Texture.X = Statics[PointerB].Texture.X + BSalt * (Salt_Mod + 1);
          }

          Statics[TexColor].Texture.ColR = Red/255;
          Statics[TexColor].Texture.ColG = Green/255;
          Statics[TexColor].Texture.ColB = Blue/255;

          InteractInc();
        }
        break;
      }
    case SDLK_LEFT:
      {
        if (SelInteraction >= 0 && SelInteraction <= 9) 
        {
          AudioPlayback.PlaySound(SoundLib.Option);

          if (SelInteraction >= 7 && SelInteraction <= 9) 
          {
            if (SDL_ModState and (KMOD_LSHIFT or KMOD_RSHIFT) != 0 && SelectsS[SelInteraction].SelectOptInt >= 10) 
              Salt_Mod = 9;
            else
              Salt_Mod = 0;

            if (SelectsS[PropertySelect].SelectedOption == 0) 
            {
              switch (SelectsS[LineSelect].SelectedOption)
              {
                case 0: 
                  SelectsS[LineColorSelect].SetSelectOpt(High(ISingLineColor));
                  break;
                case 1: 
                  SelectsS[LineColorSelect].SetSelectOpt(High(IActualLineColor));
                  break;
                case 2: 
                  SelectsS[LineColorSelect].SetSelectOpt(High(INextLineColor));
                  break;
              }
            }

            if (SelectsS[PropertySelect].SelectedOption = 1) 
              SelectsS[LineColorSelect].SetSelectOpt(2);
          }

          if (SelInteraction = RedSelect && Red > 0) 
          {
            Red -= Salt_Mod;
            SelectsS[SelInteraction].SelectOptInt = Red;
            Statics[PointerR].Texture.X = Statics[PointerR].Texture.X - RSalt * (Salt_Mod + 1);
          }

          if (SelInteraction = GreenSelect && Green > 0) 
          {
            Green -= Salt_Mod;
            SelectsS[SelInteraction].SelectOptInt = Green;
            Statics[PointerG].Texture.X = Statics[PointerG].Texture.X - GSalt * (Salt_Mod + 1);
          }

          if (SelInteraction = BlueSelect && Blue > 0) 
          {
            Blue -= Salt_Mod;
            SelectsS[SelInteraction].SelectOptInt = Blue;
            Statics[PointerB].Texture.X = Statics[PointerB].Texture.X - BSalt * (Salt_Mod + 1);
          }

          Statics[TexColor].Texture.ColR = Red/255;
          Statics[TexColor].Texture.ColG = Green/255;
          Statics[TexColor].Texture.ColB = Blue/255;

          InteractDec();
        }
        break;
      }
  }
  return true;
}

void TScreenOptionsJukebox::UpdatePropertyList()
//var
  //IProperty: array of UTF8String;
  //I: int;
{

  if (SelectsS[StyleSelect].SelectedOption == ftOutline) 
  {
    SetLength(IProperty, Length(IPropertyTranslated));
    for (I = 0 to High(IPropertyTranslated))
      IProperty[I] = IPropertyTranslated[I];
  }
  else
  {
    SetLength(IProperty, 1);
    IProperty[0] = "---";

    Ini.JukeboxProperty = 0;
  }

  UpdateSelectSlideOptions(Theme.OptionsJukebox.SelectProperty, PropertySelect, IProperty, Ini.JukeboxProperty);
}

void TScreenOptionsJukebox::InteractInc()
{
  inherited InteractInc;

  if (SelInteraction = 1) 
    UpdatePropertyList();

  RefreshSelectsColors();
}

void TScreenOptionsJukebox::InteractDec()
{
  inherited InteractDec;

  if (SelInteraction = 1) 
    UpdatePropertyList();

  RefreshSelectsColors();
}

void TScreenOptionsJukebox::RefreshSelectsColors()
{
  if (SelectsS[PropertySelect].SelectedOption == 1) 
  {
    if (SelectsS[LineSelect].SelectedOption == 0) 
      UpdateSelectSlideOptions(Theme.OptionsJukebox.SelectColor, LineColorSelect, ISingLineOColorTranslated, Ini.JukeboxSingLineOutlineColor);

    if (SelectsS[LineSelect].SelectedOption == 1) 
      UpdateSelectSlideOptions(Theme.OptionsJukebox.SelectColor, LineColorSelect, IActualLineOColorTranslated, Ini.JukeboxActualLineOutlineColor);

    if (SelectsS[LineSelect].SelectedOption == 2) 
      UpdateSelectSlideOptions(Theme.OptionsJukebox.SelectColor, LineColorSelect, INextLineOColorTranslated, Ini.JukeboxNextLineOutlineColor);
  }
  else
  {
    if (SelectsS[LineSelect].SelectedOption == 0) 
      UpdateSelectSlideOptions(Theme.OptionsJukebox.SelectColor, LineColorSelect, ISingLineColorTranslated, Ini.JukeboxSingLineColor);

    if (SelectsS[LineSelect].SelectedOption == 1) 
      UpdateSelectSlideOptions(Theme.OptionsJukebox.SelectColor, LineColorSelect, IActualLineColorTranslated, Ini.JukeboxActualLineColor);

    if (SelectsS[LineSelect].SelectedOption == 2) 
      UpdateSelectSlideOptions(Theme.OptionsJukebox.SelectColor, LineColorSelect, INextLineColorTranslated, Ini.JukeboxNextLineColor);
  }

  if (SelInteraction >= 7 && SelInteraction <= 9) 
  {
    if (SelectsS[PropertySelect].SelectedOption = 1) 
      ChangeOtherOColor();
    else
      ChangeOtherColor();
  }
  ChangeColor();
}

void TScreenOptionsJukebox::ChangeOtherColor()
{
  switch (SelectsS[LineSelect].SelectedOption)
  {
    case 0: 
    {
      Ini.JukeboxSingLineOtherColorR = Red;
      Ini.JukeboxSingLineOtherColorG = Green;
      Ini.JukeboxSingLineOtherColorB = Blue;
      break;
    }
    case 1: 
    {
      Ini.JukeboxActualLineOtherColorR = Red;
      Ini.JukeboxActualLineOtherColorG = Green;
      Ini.JukeboxActualLineOtherColorB = Blue;
      break;
    }
    case 2: 
    {
      Ini.JukeboxNextLineOtherColorR = Red;
      Ini.JukeboxNextLineOtherColorG = Green;
      Ini.JukeboxNextLineOtherColorB = Blue;
      break;
    }
  }
}

void TScreenOptionsJukebox::ChangeOtherOColor()
{
  switch (SelectsS[LineSelect].SelectedOption)
  {
    case 0: 
    {
      Ini.JukeboxSingLineOtherOColorR = Red;
      Ini.JukeboxSingLineOtherOColorG = Green;
      Ini.JukeboxSingLineOtherOColorB = Blue;
      break;
    }
    case 1: 
    {
      Ini.JukeboxActualLineOtherOColorR = Red;
      Ini.JukeboxActualLineOtherOColorG = Green;
      Ini.JukeboxActualLineOtherOColorB = Blue;
      break;
    }
    case 2: 
    {
      Ini.JukeboxNextLineOtherOColorR = Red;
      Ini.JukeboxNextLineOtherOColorG = Green;
      Ini.JukeboxNextLineOtherOColorB = Blue;
      break;
    }
  }
}

void TScreenOptionsJukebox::ChangeColor()
//var
  //Col: TRGB;
{
  if (SelectsS[PropertySelect].SelectedOption == 0) 
  {
    if (SelectsS[LineSelect].SelectedOption == 0) 
    {
      if (SelectsS[LineColorSelect].SelectedOption != High(ISingLineColor)) 
        Col = GetLyricColor(SelectsS[LineColorSelect].SelectedOption);
      else
        Col = GetJukeboxLyricOtherColor(SelectsS[LineSelect].SelectedOption);
    }

    if (SelectsS[LineSelect].SelectedOption == 1) 
    {
      if (SelectsS[LineColorSelect].SelectedOption != High(IActualLineColor)) 
        Col = GetLyricGrayColor(SelectsS[LineColorSelect].SelectedOption);
      else
        Col = GetJukeboxLyricOtherColor(SelectsS[LineSelect].SelectedOption);
    }

    if (SelectsS[LineSelect].SelectedOption == 2) 
    {
      if (SelectsS[LineColorSelect].SelectedOption != High(INextLineColor)) 
        Col = GetLyricGrayColor(SelectsS[LineColorSelect].SelectedOption);
      else
        Col = GetJukeboxLyricOtherColor(SelectsS[LineSelect].SelectedOption);
    }
  }
  else
  {
    if (SelectsS[LineColorSelect].SelectedOption != 2) 
      Col = GetLyricOutlineColor(SelectsS[LineColorSelect].SelectedOption)
    else
      Col = GetJukeboxLyricOtherOutlineColor(SelectsS[LineSelect].SelectedOption)
  }

  Red = std::round(Col.R * 255);
  Green = std::round(Col.G * 255);
  Blue = std::round(Col.B * 255);

  Statics[PointerR].Texture.X = StartR + RSalt * Red;
  Statics[PointerG].Texture.X = StartG + GSalt * Green;
  Statics[PointerB].Texture.X = StartB + BSalt * Blue;

  SelectsS[RedSelect].SetSelectOpt(Red);
  SelectsS[GreenSelect].SetSelectOpt(Green);
  SelectsS[BlueSelect].SetSelectOpt(Blue);

  Statics[TexColor].Texture.ColR = Red/255;
  Statics[TexColor].Texture.ColG = Green/255;
  Statics[TexColor].Texture.ColB = Blue/255;

  Ini.CurrentJukeboxSingLineOutlineColor = Ini.JukeboxSingLineOutlineColor;
  Ini.CurrentJukeboxActualLineOutlineColor = Ini.JukeboxActualLineOutlineColor;
  Ini.CurrentJukeboxNextLineOutlineColor = Ini.JukeboxNextLineOutlineColor;
}

TScreenOptionsJukebox::TScreenOptionsJukebox()
{
  inherited Create;

  LoadFromTheme(Theme.OptionsJukebox);

  Theme.OptionsJukebox.SelectLyricsFont.showArrows = true;
  Theme.OptionsJukebox.SelectLyricsFont.oneItemOnly = true;
  FontSelect = AddSelectSlide(Theme.OptionsJukebox.SelectLyricsFont, Ini.JukeboxFont, FontFamilyNames);

  Theme.OptionsJukebox.SelectLyricsStyle.showArrows = true;
  Theme.OptionsJukebox.SelectLyricsStyle.oneItemOnly = true;
  StyleSelect = AddSelectSlide(Theme.OptionsJukebox.SelectLyricsStyle, Ini.JukeboxStyle, ILyricsStyleTranslated);

  Theme.OptionsJukebox.SelectLyricsEffect.showArrows = true;
  Theme.OptionsJukebox.SelectLyricsEffect.oneItemOnly = true;
  AddSelectSlide(Theme.OptionsJukebox.SelectLyricsEffect, Ini.JukeboxEffect, ILyricsEffectTranslated);

  Theme.OptionsJukebox.SelectLyricsAlpha.showArrows = true;
  Theme.OptionsJukebox.SelectLyricsAlpha.oneItemOnly = true;
  AddSelectSlide(Theme.OptionsJukebox.SelectLyricsAlpha, Ini.JukeboxAlpha, ILyricsAlpha);

  Theme.OptionsJukebox.SelectLine.showArrows = true;
  Theme.OptionsJukebox.SelectLine.oneItemOnly = true;
  LineSelect = AddSelectSlide(Theme.OptionsJukebox.SelectLine, Ini.JukeboxLine, ILineTranslated);

  Theme.OptionsJukebox.SelectProperty.showArrows = true;
  Theme.OptionsJukebox.SelectProperty.oneItemOnly = true;
  PropertySelect = AddSelectSlide(Theme.OptionsJukebox.SelectProperty, Ini.JukeboxProperty, IPropertyTranslated);

  Theme.OptionsJukebox.SelectColor.showArrows = true;
  Theme.OptionsJukebox.SelectColor.oneItemOnly = true;
  LineColorSelect = AddSelectSlide(Theme.OptionsJukebox.SelectColor, Ini.JukeboxSingLineColor, ISingLineColorTranslated);

  Theme.OptionsJukebox.SelectR.showArrows = false;
  Theme.OptionsJukebox.SelectR.oneItemOnly = true;
  RedSelect = AddSelectSlide(Theme.OptionsJukebox.SelectR, Red, IRed);

  Theme.OptionsJukebox.SelectG.showArrows = false;
  Theme.OptionsJukebox.SelectG.oneItemOnly = true;
  GreenSelect = AddSelectSlide(Theme.OptionsJukebox.SelectG, Green, IGreen);

  Theme.OptionsJukebox.SelectB.showArrows = false;
  Theme.OptionsJukebox.SelectB.oneItemOnly = true;
  BlueSelect = AddSelectSlide(Theme.OptionsJukebox.SelectB, Blue, IBlue);

  TexR = AddStatic(Theme.OptionsJukebox.TexR);
  TexG = AddStatic(Theme.OptionsJukebox.TexG);
  TexB = AddStatic(Theme.OptionsJukebox.TexB);
  TexColor = AddStatic(Theme.OptionsJukebox.TexColor);

  PointerR = AddStatic(Theme.OptionsJukebox.PointerR);
  PointerG = AddStatic(Theme.OptionsJukebox.PointerG);
  PointerB = AddStatic(Theme.OptionsJukebox.PointerB);

  RSalt = Statics[TexR].Texture.W/255;
  GSalt = Statics[TexR].Texture.W/255;
  BSalt = Statics[TexR].Texture.W/255;

  StartR = Statics[PointerR].Texture.X;
  StartG = Statics[PointerG].Texture.X;
  StartB = Statics[PointerB].Texture.X;

  Statics[TexR].Texture.ColR = 1;
  Statics[TexR].Texture.ColG = 0;
  Statics[TexR].Texture.ColB = 0;

  Statics[TexG].Texture.ColR = 0;
  Statics[TexG].Texture.ColG = 1;
  Statics[TexG].Texture.ColB = 0;

  Statics[TexB].Texture.ColR = 0;
  Statics[TexB].Texture.ColG = 0;
  Statics[TexB].Texture.ColB = 1;

  AddButton(Theme.OptionsJukebox.ButtonExit);
  if (Length(Button[0].Text)=0) 
    AddButtonText(20, 5, Theme.Options.Description[OPTIONS_DESC_INDEX_BACK]);

  // lyric sample
  Lyrics = TLyricEngine(
      Theme.OptionsJukebox.UpperX, Theme.OptionsJukebox.UpperY, Theme.OptionsJukebox.UpperW, Theme.OptionsJukebox.UpperH,
      Theme.OptionsJukebox.LowerX, Theme.OptionsJukebox.LowerY, Theme.OptionsJukebox.LowerW, Theme.OptionsJukebox.LowerH);

  //Line.Lyric = "Lorem ipsum dolor sit amet";
  // 1st line
  SetLength(Line.Notes, 6);
  Line.Notes[0].Text = "Lor";
  Line.Notes[1].Text = "em";
  Line.Notes[2].Text = " ipsum";
  Line.Notes[3].Text = " dolor";
  Line.Notes[4].Text = " sit";
  Line.Notes[5].Text = " amet";

  Line.Notes[0].StartBeat = 0;
  Line.Notes[1].StartBeat = 10;
  Line.Notes[2].StartBeat = 20;
  Line.Notes[3].StartBeat = 30;
  Line.Notes[4].StartBeat = 40;
  Line.Notes[5].StartBeat = 50;

  Line.Notes[0].Duration = 10;
  Line.Notes[1].Duration = 10;
  Line.Notes[2].Duration = 10;
  Line.Notes[3].Duration = 10;
  Line.Notes[4].Duration = 10;
  Line.Notes[5].Duration = 10;

  Line.ScoreValue = 6;
  Line.EndBeat = 60;
  Line.StartBeat = 0;
  Line.LastLine = true;
  Lyrics.AddLine(@Line);

  // 2nd line
  //consectetur adipiscing elit
  SetLength(Line.Notes, 3);

  Line.Notes[0].Text = "consectetur";
  Line.Notes[1].Text = " adipiscing";
  Line.Notes[2].Text = " elit";

  Line.Notes[0].StartBeat = 60;
  Line.Notes[1].StartBeat = 70;
  Line.Notes[2].StartBeat = 80;

  Line.Notes[0].Duration = 10;
  Line.Notes[1].Duration = 10;
  Line.Notes[2].Duration = 10;

  Line.LastLine = true;

  Lyrics.AddLine(@Line);
  Lyrics.AddLine(@Line);
}

void TScreenOptionsJukebox::LyricSample()
//var
  //TRGB Col;
{
  Lyrics.FontFamily = Ini.JukeboxFont;
  Lyrics.FontStyle  = Ini.JukeboxStyle;

  if (Ini.JukeboxSingLineColor = High(ISingLineColor)) 
    Col = GetJukeboxLyricOtherColor(0);
  else
    Col = GetLyricColor(Ini.JukeboxSingLineColor);

  Lyrics.LineColor_act.R = Col.R;
  Lyrics.LineColor_act.G = Col.G;
  Lyrics.LineColor_act.B = Col.B;

  if (Ini.JukeboxActualLineColor = High(IActualLineColor)) 
    Col = GetJukeboxLyricOtherColor(1);
  else
    Col = GetLyricGrayColor(Ini.JukeboxActualLineColor);

  Lyrics.LineColor_en.R = Col.R;
  Lyrics.LineColor_en.G = Col.G;
  Lyrics.LineColor_en.B = Col.B;

  if (Ini.JukeboxNextLineColor = High(INextLineColor)) 
    Col = GetJukeboxLyricOtherColor(2);
  else
    Col = GetLyricGrayColor(Ini.JukeboxNextLineColor);

  Lyrics.LineColor_dis.R = Col.R;
  Lyrics.LineColor_dis.G = Col.G;
  Lyrics.LineColor_dis.B = Col.B;

  Lyrics.Draw(LyricsState.MidBeat);
}

void TScreenOptionsJukebox::OnShow()
{
  inherited;

  if (!Help.SetHelpID(ID)) 
    ULog::Log.LogError("No Entry for Help-ID " + ID + " (ScreenOptionsJukebox)");

  Interaction = 0;

  UpdatePropertyList();
  ChangeColor();

  LyricsState.StartTime = 0;
  LyricsState.UpdateBeats();
}

bool TScreenOptionsJukebox::Draw()
{
  Result = inherited Draw;

  LyricSample();
}
}