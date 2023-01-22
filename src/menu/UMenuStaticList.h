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
 * $Id:  $
 */


#include <GL/gl.h>

#include "UDrawTexture.h"
#include "../base/UTexture.h"
#include "UMenuInteract.hpp"

namespace UMenuStaticList
{
 //   type
    class TStaticList
    {
    protected:

        bool SelectBool;

    public:

        UTexture::TextureWrapper::SPtr Texture; // Button Screen position and size
        UTexture::TextureWrapper::SPtr DeSelectTexture;

        bool Visible;

        //Reflection Mod
        bool Reflection;
        double Reflectionspacing;

        void Draw();

        TStaticList(UTexture::TTexture::SPtr Texture);
        TMouseOverRect GetMouseOverArea();
    };

    /*
    implementation
        uses
        UDrawTexture,
        UDisplay;
        */
    void TStaticList::Draw()
    {
        if (Visible)
        {
			if (SelectBool)
				UDrawTexture::DrawTexture(*Texture);
            else
            {
                DeSelectTexture.X = Texture.X;
				DeSelectTexture.Y = Texture.Y;
				DeSelectTexture.H = Texture.H;
				DeSelectTexture.W = Texture.W;
                UDrawTexture::DrawTexture(*DeSelectTexture);
			}

		    //Reflection Mod
		    if (Reflection)  // Draw Reflections
		    {
		        with Texture do
				{
			        //Bind Tex and GL Attributes
			        glEnable(GL_TEXTURE_2D);
					glEnable(GL_BLEND);

				    glDepthRange(0, 10);
				    glDepthFunc(GL_LEQUAL);
				    glEnable(GL_DEPTH_TEST);

				    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				    glBindTexture(GL_TEXTURE_2D, TexNum);

				    //Draw
				    glBegin(GL_QUADS);//Top Left
				    glColor4f(ColR* Int, ColG* Int, ColB* Int, Alpha - 0.3);
				    glTexCoord2f(TexX1* TexW, TexY2* TexH);
				    glVertex3f(x, y + h * scaleH + Reflectionspacing, z);

				    //Bottom Left
				    glColor4f(ColR* Int, ColG* Int, ColB* Int, 0);
				    glTexCoord2f(TexX1* TexW, 0.5 * TexH + TexY1);
				    glVertex3f(x, y + h * scaleH + h * scaleH / 2 + Reflectionspacing, z);


				    //Bottom Right
				    glColor4f(ColR* Int, ColG* Int, ColB* Int, 0);
				    glTexCoord2f(TexX2* TexW, 0.5 * TexH + TexY1);
				    glVertex3f(x + w * scaleW, y + h * scaleH + h * scaleH / 2 + Reflectionspacing, z);

				    //Top Right
				    glColor4f(ColR* Int, ColG* Int, ColB* Int, Alpha - 0.3);
				    glTexCoord2f(TexX2* TexW, TexY2* TexH);
				    glVertex3f(x + w * scaleW, y + h * scaleH + Reflectionspacing, z);
				    glEnd();

				    glDisable(GL_TEXTURE_2D);
				    glDisable(GL_DEPTH_TEST);
				    glDisable(GL_BLEND);
				} 
			}
		}
    }

    constructor TStaticList::Create(Textura: TTexture)
    {
        inherited Create;

		Texture = Textura;
		DeSelectTexture = Textura;
    }

    function TStaticList::GetMouseOverArea: TMouseOverRect;
    {
        if (!Display.Cursor_HiddenByScreen) 
        {
            Result.X = Texture.X;
		    Result.Y = Texture.Y;
		    Result.W = Texture.W;
		    Result.H = Texture.H;
		}
    }
}