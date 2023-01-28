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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UMenuButton.pas $
 * $Id: UMenuButton.pas 3123 2015-08-23 03:15:31Z basisbit $
 */
#include "../switches.h"

#include <vector>

namespace UMenuButton
{
  /*uses
    TextGL,
    UTexture,
    dglOpenGL,
    UMenuText,
    sdl2,
    UMenuInteract;*/

  // type
  // CButton = class of TButton;

  class TButton
  {
  protected:
    bool SelectBool;

    double FadeProgress;
    uint32_t FadeLastTick;

    double DeSelectW;
    double DeSelectH;
    double PosX;
    double PosY;

  public:
    std::vector<TText> Text;
    TTexture Texture;  // Button Screen position and size
    TTexture Texture2; // second texture only used for fading full resolution covers

    bool Colorized;
    TTexture DeSelectTexture; // texture for colorized hack

    TTexture FadeTex;   // Texture for beautiful fading
    uint8_t FadeTexPos; // Pos of the FadeTexture (0: Top, 1: Left, 2: Bottom, 3: Right)

    int DeselectType; // not used yet
    bool Visible;

    bool Reflection;
    double Reflectionspacing;
    double DeSelectReflectionspacing;

    bool Fade;
    bool FadeText;

    bool Selectable;

    // Number of the Parent Collection, 0 if in no Collection
    uint8_t Parent;

    double SelectColR;
    double SelectColG;
    double SelectColB;
    double SelectInt;
    double SelectTInt;
    // Fade Mod
    double SelectW;
    double SelectH;

    double DeselectColR;
    double DeselectColG;
    double DeselectColB;
    double DeselectInt;
    double DeselectTInt;

    void SetY(double Value);
    void SetX(double Value);
    void SetW(double Value);
    void SetH(double Value);

    virtual void SetSelect(bool Value);
    // property X: double read PosX write SetX;
    // property Y: double read PosY write SetY;
    // property Z: double read Texture.z write Texture.z;
    // property W: double read DeSelectW write SetW;
    // property H: double read DeSelectH write SetH;
    // property Selected: bool read SelectBool write SetSelect;

    virtual void Draw();

    TButton();
    TButton(TTexture Textura);
    TButton(TTexture Textura, TTexture DSTexture);
    virtual ~TButton() = default;

    TMouseOverRect GetMouseOverArea();
  };

  /*implementation

  uses
    SysUtils,
    UDisplay,
    UDrawTexture;*/

  void TButton::SetX(double Value)
  /*{var
    dx:   double;
    T:    int;    // text}*/
  {
    /*{dY := Value - Texture.y;

    Texture.X := Value;

    for T := 0 to High(Text) do
      Text[T].X := Text[T].X + dY;}*/

    PosX = Value;
    if (FadeTex.TexNum == 0)
      Texture.X = Value;
  }

  void TButton::SetY(double Value)
  /*{var
  dY : double;
  T:  int;    // text}*/
  {
    /*{ dY= Value - PosY;


    for T = 0 to High(Text) do
      Text[T].Y = Text[T].Y + dY; }*/

    PosY = Value;
    if (FadeTex.TexNum == 0)
      Texture.y = Value;
  }

  void TButton::SetW(double Value)
  {
    if (SelectW == DeSelectW)
      SelectW = Value;

    DeSelectW = Value;

    if (!Fade)
    {
      if (SelectBool)
        Texture.W = SelectW;
      else
        Texture.W = DeSelectW;
    }
  }

  void TButton::SetH(double Value)
  {
    if (SelectH == DeSelectH)
      SelectH = Value;

    DeSelectH = Value;

    if (!Fade)
    {
      if (SelectBool)
        Texture.H = SelectH;
      else
        Texture.H = DeSelectH;
    }
  }

  void TButton::SetSelect(bool Value)
  // var
  // T : int;
  {
    SelectBool = Value;

    if (Value)
    {
      Texture.ColR = SelectColR;
      Texture.ColG = SelectColG;
      Texture.ColB = SelectColB;
      Texture.Int = SelectInt;

      Texture2.ColR = SelectColR;
      Texture2.ColG = SelectColG;
      Texture2.ColB = SelectColB;
      Texture2.Int = SelectInt;

      for (T = 0 to High(Text))
      {
        Text[T].SetSelect(SelectBool);
        Text[T].Int = SelectTInt;
      }
      // Fade Mod
      if (Fade)
      {
        if (FadeProgress <= 0)
          FadeProgress = 0.125;
      }
      else
      {
        Texture.W = SelectW;
        Texture.H = SelectH;
      }
    }
    else
    {
      Texture.ColR = DeselectColR;
      Texture.ColG = DeselectColG;
      Texture.ColB = DeselectColB;
      Texture.Int = DeselectInt;

      Texture2.ColR = DeselectColR;
      Texture2.ColG = DeselectColG;
      Texture2.ColB = DeselectColB;
      Texture2.Int = DeselectInt;

      for (T = 0 to High(Text))
      {
        Text[T].SetSelect(SelectBool);
        Text[T].Int = DeselectTInt;
      }
      // Fade Mod
      if (Fade)
      {
        if (FadeProgress >= 1)
          FadeProgress = 0.875;
      }
      else
      {
        Texture.W = DeSelectW;
        Texture.H = DeSelectH;
      }
    }
  }

  // ***** Public methods ****** //

  void TButton::Draw()
  // var
  // T : int;
  // Tick:    uint32_t;
  // Spacing: double;
  // x1, x2, x3, x4, y1, y2, y3, y4: double;
  {
    if (Visible)
    {
      // Fade Mod
      T = 0;
      if (Fade)
      {
        if (FadeProgress < 1 && FadeProgress > 0)
        {
          Tick = SDL_GetTicks() div 16;
          if (Tick<> FadeLastTick)
          {
            FadeLastTick = Tick;

            if (SelectBool)
              FadeProgress += 0.125;
            else
              FadeProgress -= 0.125;

            if (FadeText)
            {
              for (T = 0 to high(Text))
              {
                Text[T].MoveX = (SelectW - DeSelectW) * FadeProgress;
                Text[T].MoveY = (SelectH - DeSelectH) * FadeProgress;
              }
            }
          }
        }

        // Method without Fade Texture
        if (FadeTex.TexNum == 0)
        {
          Texture.W = DeSelectW + (SelectW - DeSelectW) * FadeProgress;
          Texture.H = DeSelectH + (SelectH - DeSelectH) * FadeProgress;
          DeSelectTexture.W = Texture.W;
          DeSelectTexture.H = Texture.H;
        }
        else // method with Fade Texture
        {
          Texture.W = DeSelectW;
          Texture.H = DeSelectH;
          DeSelectTexture.W = Texture.W;
          DeSelectTexture.H = Texture.H;

          FadeTex.ColR = Texture.ColR;
          FadeTex.ColG = Texture.ColG;
          FadeTex.ColB = Texture.ColB;
          FadeTex.Int = Texture.Int;

          FadeTex.Z = Texture.Z;

          FadeTex.Alpha = Texture.Alpha;
          FadeTex.TexX1 = 0;
          FadeTex.TexX2 = 1;
          FadeTex.TexY1 = 0;
          FadeTex.TexY2 = 1;

          switch (FadeTexPos)
          {
          case 0: // FadeTex on Top
          {
            // Standard Texture
            Texture.X = PosX;
            Texture.Y = PosY + (SelectH - DeSelectH) * FadeProgress;
            DeSelectTexture.X = Texture.X;
            DeSelectTexture.Y = Texture.Y;
            // Fade Tex
            FadeTex.X = PosX;
            FadeTex.Y = PosY;
            FadeTex.W = Texture.W;
            FadeTex.H = (SelectH - DeSelectH) * FadeProgress;
            FadeTex.ScaleW = Texture.ScaleW;
            // Some Hack that Fixes a little Space between both Textures
            FadeTex.TexY2 = 0.9;
            break;
          }
          case 1: // FadeTex on Left
          {
            // Standard Texture
            Texture.X = PosX + (SelectW - DeSelectW) * FadeProgress;
            Texture.Y = PosY;
            DeSelectTexture.X = Texture.X;
            DeSelectTexture.Y = Texture.Y;
            // Fade Tex
            FadeTex.X = PosX;
            FadeTex.Y = PosY;
            FadeTex.H = Texture.H;
            FadeTex.W = (SelectW - DeSelectW) * FadeProgress;
            FadeTex.ScaleH = Texture.ScaleH;
            // Some Hack that Fixes a little Space between both Textures
            FadeTex.TexX2 = 0.9;
            break;
          }
          case 2: // FadeTex on Bottom
          {
            // Standard Texture
            Texture.X = PosX;
            Texture.Y = PosY;
            DeSelectTexture.X = Texture.X;
            DeSelectTexture.Y = Texture.Y;
            // Fade Tex
            FadeTex.X = PosX;
            FadeTex.Y = PosY + (SelectH - DeSelectH) * FadeProgress;
            ;
            FadeTex.W = Texture.W;
            FadeTex.H = (SelectH - DeSelectH) * FadeProgress;
            FadeTex.ScaleW = Texture.ScaleW;
            // Some Hack that Fixes a little Space between both Textures
            FadeTex.TexY1 = 0.1;
            break;
          }
          case 3: // FadeTex on Right
          {
            // Standard Texture
            Texture.X = PosX;
            Texture.Y = PosY;
            DeSelectTexture.X = Texture.X;
            DeSelectTexture.Y = Texture.Y;
            // Fade Tex
            FadeTex.X = PosX + (SelectW - DeSelectW) * FadeProgress;
            FadeTex.Y = PosY;
            FadeTex.H = Texture.H;
            FadeTex.W = (SelectW - DeSelectW) * FadeProgress;
            FadeTex.ScaleH = Texture.ScaleH;
            // Some Hack that Fixes a little Space between both Textures
            FadeTex.TexX1 = 0.1;
            break;
          }
          }
        }
      }
      else if (FadeText)
      {
        Text[T].MoveX = (SelectW - DeSelectW);
        Text[T].MoveY = (SelectH - DeSelectH);
      }

      if (SelectBool || FadeProgress > 0 || !Colorized)
        DrawTexture(Texture);
      else
      {
        DeSelectTexture.X = Texture.X;
        DeSelectTexture.Y = Texture.Y;
        DeSelectTexture.H = Texture.H;
        DeSelectTexture.W = Texture.W;
        DrawTexture(DeSelectTexture);
      }

      // Draw FadeTex
      if (FadeTex.TexNum > 0)
        DrawTexture(FadeTex);

      if (Texture2.Alpha > 0)
      {
        Texture2.ScaleW = Texture.ScaleW;
        Texture2.ScaleH = Texture.ScaleH;

        Texture2.X = Texture.X;
        Texture2.Y = Texture.Y;
        Texture2.W = Texture.W;
        Texture2.H = Texture.H;

        Texture2.ColR = Texture.ColR;
        Texture2.ColG = Texture.ColG;
        Texture2.ColB = Texture.ColB;
        Texture2.Int = Texture.Int;

        Texture2.Z = Texture.Z;

        DrawTexture(Texture2);
      }

      // Reflection Mod
      if (Reflection) // Draw Reflections
      {
        if (FadeProgress != 0 && FadeProgress != 1)
        {
          Spacing = DeSelectReflectionspacing - (DeSelectReflectionspacing - Reflectionspacing) * FadeProgress;
        }
        else if (SelectBool)
          Spacing = Reflectionspacing;
        else
          Spacing = DeSelectReflectionspacing;

        if (SelectBool || !Colorized)
          with Texture do
          {
            // Bind Tex and GL Attributes
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);

            glDepthRange(0, 10);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_DEPTH_TEST);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, TexNum);

            // Draw
            /*{
                glBegin(GL_QUADS);//Top Left
                glColor4f(ColR * Int, ColG * Int, ColB * Int, Alpha - 0.3);
                glTexCoord2f(TexX1 * TexW, TexY2 * TexH);
                glVertex3f(x, y + h * scaleH + Spacing, z);

                //Bottom Left
                glColor4f(ColR * Int, ColG * Int, ColB * Int, 0);
                glTexCoord2f(TexX1 * TexW, TexY1 + TexH * 0.5);
                glVertex3f(x, y + h * scaleH + h * scaleH / 2 + Spacing, z);


                //Bottom Right
                glColor4f(ColR * Int, ColG * Int, ColB * Int, 0);
                glTexCoord2f(TexX2 * TexW, TexY1 + TexH * 0.5);
                glVertex3f(x + w * scaleW, y + h * scaleH + h * scaleH / 2 + Spacing, z);

                //Top Right
                glColor4f(ColR * Int, ColG * Int, ColB * Int, Alpha - 0.3);
                glTexCoord2f(TexX2 * TexW, TexY2 * TexH);
                glVertex3f(x + w * scaleW, y + h * scaleH + Spacing, z);
                glEnd;
            }*/

            y1 = y + h * scaleH + Spacing;
            y2 = y + h * scaleH + h * scaleH / 2 + Spacing;
            y3 = y + h * scaleH + h * scaleH / 2 + Spacing;
            y4 = y + h * scaleH + Spacing;

            glBegin(GL_QUADS); // Top Left
            glColor4f(ColR * Int, ColG * Int, ColB * Int, Alpha - 0.3);
            glTexCoord2f(TexX1 * TexW, TexY2 * TexH);
            glVertex3f(x, y1 - (y1 - (LeftScale * (y1))), z);

            // Bottom Left
            glColor4f(ColR * Int, ColG * Int, ColB * Int, 0);
            glTexCoord2f(TexX1 * TexW, TexY1 + TexH * 0.5);
            glVertex3f(x, y2 - (y2 - (LeftScale * (y2))), z);

            // Bottom Right
            glColor4f(ColR * Int, ColG * Int, ColB * Int, 0);
            glTexCoord2f(TexX2 * TexW, TexY1 + TexH * 0.5);
            glVertex3f(x + w * scaleW, y3 - (y3 - (RightScale * (y3))), z);

            // Top Right
            glColor4f(ColR * Int, ColG * Int, ColB * Int, Alpha - 0.3);
            glTexCoord2f(TexX2 * TexW, TexY2 * TexH);
            glVertex3f(x + w * scaleW, y4 - (y4 - (RightScale * (y4))), z);
            glEnd();

            /*{
                glBegin(GL_QUADS);
                glTexCoord2f(TexX1 * TexW, TexY1 * TexH);
                glVertex3f(x, y + (y - (LeftScale * (y))), z);
                glTexCoord2f(TexX1 * TexW, TexY2 * TexH);
                glVertex3f(x, y - (y - (LeftScale * (y))), z);
                glTexCoord2f(TexX2 * TexW, TexY2 * TexH);
                glVertex3f(x, y - (y - (RightScale * (y))), z);
                glTexCoord2f(TexX2 * TexW, TexY1 * TexH);
                glVertex3f(x, y + (y - (RightScale * (y))), z);
                glEnd;
            }*/

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
          }
        else
          with DeSelectTexture
          {
            // Bind Tex and GL Attributes
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);

            glDepthRange(0, 10);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_DEPTH_TEST);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, TexNum);

            // Draw
            glBegin(GL_QUADS); // Top Left
            glColor4f(ColR * Int, ColG * Int, ColB * Int, Alpha - 0.3);
            glTexCoord2f(TexX1 * TexW, TexY2 * TexH);
            glVertex3f(x, y + h * scaleH + Spacing, z);

            // Bottom Left
            glColor4f(ColR * Int, ColG * Int, ColB * Int, 0);
            glTexCoord2f(TexX1 * TexW, TexY1 + TexH * 0.5);
            glVertex3f(x, y + h * scaleH + h * scaleH / 2 + Spacing, z);

            // Bottom Right
            glColor4f(ColR * Int, ColG * Int, ColB * Int, 0);
            glTexCoord2f(TexX2 * TexW, TexY1 + TexH * 0.5);
            glVertex3f(x + w * scaleW, y + h * scaleH + h * scaleH / 2 + Spacing, z);

            // Top Right
            glColor4f(ColR * Int, ColG * Int, ColB * Int, Alpha - 0.3);
            glTexCoord2f(TexX2 * TexW, TexY2 * TexH);
            glVertex3f(x + w * scaleW, y + h * scaleH + Spacing, z);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
          }
      }

      for (T = 0 to High(Text))
      {
        Text[T].Draw;
      }
    }
  }

  TMouseOverRect TButton::GetMouseOverArea()
  {
    if (!Display.Cursor_HiddenByScreen)
    {
      if (FadeTex.TexNum == 0)
      {
        Result.X = Texture.X;
        Result.Y = Texture.Y;
        Result.W = Texture.W;
        Result.H = Texture.H;
      }
      else
      {
        switch (FadeTexPos)
        {
        case 0:
        { // fade tex on top
          Result.X = Texture.X;
          Result.Y = FadeTex.Y;
          Result.W = Texture.W;
          Result.H = FadeTex.H + Texture.H;
          break;
        }

        case 1:
        { // fade tex on left side
          Result.X = FadeTex.X;
          Result.Y = Texture.Y;
          Result.W = FadeTex.W + Texture.W;
          Result.H = Texture.H;
          break;
        }

        case 2:
        { // fade tex on bottom
          Result.X = Texture.X;
          Result.Y = Texture.Y;
          Result.W = Texture.W;
          Result.H = FadeTex.H + Texture.H;
          break;
        }

        case 3:
        { // fade tex on right side
          Result.X = Texture.X;
          Result.Y = Texture.Y;
          Result.W = FadeTex.W + Texture.W;
          Result.H = Texture.H;
          break;
        }
        }
      }
    }
  }

  /*
  destructor TButton.Destroy;
  {
  { if (FadeTex.TexNum > 0) then
  {
    glDeleteTextures(1, PGLuint(@FadeTex.TexNum));
    FadeTex.TexNum = 0;
  }

  if (Texture.TexNum > 0) then
  {
    glDeleteTextures(1, PGLuint(@Texture.TexNum));
    Texture.TexNum = 0;
  }

  if (Texture2.TexNum > 0) then
  {
    glDeleteTextures(1, PGLuint(@Texture2.TexNum));
    Texture2.TexNum = 0;
  }

  if (DeSelectTexture.TexNum > 0) then
  {
    glDeleteTextures(1, PGLuint(@DeSelectTexture.TexNum));
    DeSelectTexture.TexNum = 0;
  } }
  inherited;
  }
  */
  TButton::TButton()
  {
    inherited Create;
    // We initialize all to 0, nil or false
    Visible = true;
    SelectBool = false;
    DeselectType = 0;
    Selectable = true;
    Reflection = false;
    Colorized = false;

    SelectColR = 1;
    SelectColG = 1;
    SelectColB = 1;
    SelectInt = 1;
    SelectTInt = 1;

    DeselectColR = 1;
    DeselectColG = 1;
    DeselectColB = 1;
    DeselectInt = 0.5;
    DeselectTInt = 1;

    Fade = false;
    FadeTex.TexNum = 0;
    FadeProgress = 0;
    FadeText = false;
    SelectW = DeSelectW;
    SelectH = DeSelectH;

    PosX = 0;
    PosY = 0;

    Parent = 0;
  }

  TButton::TButton(TTexture Textura)
  {
    Create();
    Texture = Textura;
    DeSelectTexture = Textura;
    Texture.ColR = 0;
    Texture.ColG = 0.5;
    Texture.ColB = 0;
    Texture.Int = 1;
    Colorized = false;
  }

  // Button has the texture-type "colorized"
  // Two textures are generated, one with Col the other one with DCol
  // Check UMenu.pas line 680 to see the call ( AddButton() )
  TButton::TButton(TTexture Textura, TTexture DSTexture)
  {
    Create();
    Texture = Textura;
    DeSelectTexture = DSTexture;
    Texture.ColR = 1;
    Texture.ColG = 1;
    Texture.ColB = 1;
    Texture.Int = 1;
    Colorized = true;
  }
}