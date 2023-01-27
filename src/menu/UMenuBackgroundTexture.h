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
 * $URL$
 * $Id$
 */
#include <string>
#include <array>

#include "UMenuBackground.hpp"

namespace UMenuBackgroundTexture
{
/*uses
  UCommon,
  UThemes,
  UTexture,
  UMenuBackground,
  UPath;*/

//TMenuBackgroundColor - Background Color
//--------

class TMenuBackgroundTexture : public UMenuBackground::TMenuBackground
{
    private:

      Tex: TTexture;
      Color: TRGB;

    public:

      TMenuBackgroundTexture(const TThemeBackground ThemedSettings);
      void Draw() override;
      ~TMenuBackgroundTexture() override;
};

const std::array<std::string, 14> SUPPORTED_EXTS_BACKGROUNDTEXTURE = {".png", ".bmp", ".jpg", ".jpeg", ".gif", ".pnm", ".ppm", ".pgm", ".pbm", ".xpm", ".lbm", ".pcx", ".tga", ".tiff"};
/*
implementation
uses
  USkins,
  SysUtils,
  dglOpenGL,
  UGraphic;
*/
TMenuBackgroundTexture::TMenuBackgroundTexture(const TThemeBackground ThemedSettings)
//var
  //texFilename: IPath;
{
  inherited;

  if (ThemedSettings.Tex.empty()) 
    throw EMenuBackgroundError("TMenuBackgroundTexture: No texture filename present");

  Color       = ThemedSettings.Color;

  texFilename = Skin.GetTextureFileName(ThemedSettings.Tex);
  Tex         = Texture.GetTexture(texFilename, TEXTURE_TYPE_PLAIN);

  if (Tex.TexNum == 0) 
  {
    freeandnil(Tex);
    throw EMenuBackgroundError("TMenuBackgroundTexture: Can't load texture");
  }
}

void   TMenuBackgroundTexture::Draw()
{
  If (ScreenAct == 1)  //Clear just once when in dual screen mode
    glClear(GL_DEPTH_BUFFER_BIT);
    
  glColorRGB(Color);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindTexture(GL_TEXTURE_2D, Tex.TexNum);

  glBegin(GL_QUADS);
    glTexCoord2f(Tex.TexX1*Tex.TexW, Tex.TexY1*Tex.TexH);
    glVertex2f(0, 0);

    glTexCoord2f(Tex.TexX1*Tex.TexW, Tex.TexY2*Tex.TexH);
    glVertex2f(0, 600);

    glTexCoord2f(Tex.TexX2*Tex.TexW, Tex.TexY2*Tex.TexH);
    glVertex2f(800, 600);

    glTexCoord2f(Tex.TexX2*Tex.TexW, Tex.TexY1*Tex.TexH);
    glVertex2f(800, 0);
  glEnd();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}
}