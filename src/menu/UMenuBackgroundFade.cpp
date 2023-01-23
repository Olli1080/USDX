#include "UMenuBackgroundFade.h"

#include "../base/UThemes.h"
#include "../base/USkins.h"
#include "../base/UGraphic.h"

namespace UMenuBackgroundFade
{

    /*
implementation
uses
  sdl2,
  dglOpenGL,
  USkins,
  UGraphic;
*/

    TMenuBackgroundFade::TMenuBackgroundFade(const UThemes::TThemeBackground ThemedSettings)
	    : FadeTimeStart({})
    //var
        //texFilename : IPath;
    {
		Color = ThemedSettings.Color;
        if (!ThemedSettings.Tex.empty() > 0)
        {
            const auto texFilename = USkins::Skin.GetTextureFileName(ThemedSettings.Tex);
            Tex = UTexture::Texture.GetTexture(texFilename, UTexture::TTextureType::TEXTURE_TYPE_PLAIN);

            useTexture = Tex->TexNum != 0;
        }
		else
            useTexture = false;

		/*{if (not UseTexture) 
			FreeInstance(Tex); //BasisBit ToDo}*/
    }

    void TMenuBackgroundFade::OnShow()
    {
        FadeTimeStart = std::chrono::system_clock::now();
    }

    void TMenuBackgroundFade::Draw()
    //var
      //  Progress : real;
    {
        double Progress = Color.A;
        auto dTime = std::chrono::system_clock::now() - FadeTimeStart;

        if (dTime < FADEINTIME)
            Progress *= std::chrono::duration_cast<std::chrono::milliseconds>(dTime) / FADEINTIME;

        if (UGraphic::screen.Act == 1)  //Clear just once when in dual screen mode
            glClear(GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        UThemes::glColorRGB(Color, Progress);

        if (useTexture)
        { //Draw Texture to Screen
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, Tex->TexNum);

            const auto X1 = Tex->coords.X1 * Tex->ratio.W;
            const auto X2 = Tex->coords.X2 * Tex->ratio.W;

            const auto Y1 = Tex->coords.Y1 * Tex->ratio.H;
            const auto Y2 = Tex->coords.Y2 * Tex->ratio.H;

            glBegin(GL_QUADS);
            glTexCoord2f(X1, Y1);
            glVertex2f(0, 0);

            glTexCoord2f(X1, Y2);
            glVertex2f(0, 600);

            glTexCoord2f(X2, Y2);
            glVertex2f(800, 600);

            glTexCoord2f(X2, Y1);
            glVertex2f(800, 0);
            glEnd();

            glDisable(GL_TEXTURE_2D);
        }
        else
        { //Clear Screen w/ progress Alpha + Color
            glDisable(GL_TEXTURE_2D);

		    glBegin(GL_QUADS);
		    glVertex2f(0, 0);
		    glVertex2f(0, 600);
		    glVertex2f(800, 600);
		    glVertex2f(800, 0);
		    glEnd();
	    }
        glDisable(GL_BLEND);
    }
}