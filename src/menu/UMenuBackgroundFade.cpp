#include "UMenuBackgroundFade.h"

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

    constructor TMenuBackgroundFade.Create(const ThemedSettings : TThemeBackground);
    var
        texFilename : IPath;
    begin
        inherited;
FadeTime: = 0;

Color: = ThemedSettings.Color;
Alpha: = ThemedSettings.Alpha;
    if (Length(ThemedSettings.Tex) > 0) then
        begin
        texFilename : = Skin.GetTextureFileName(ThemedSettings.Tex);
Tex: = Texture.GetTexture(texFilename, TEXTURE_TYPE_PLAIN);

UseTexture: = (Tex.TexNum < > 0);
    end
    else
        UseTexture : = false;

    /*{if (not UseTexture) then
        FreeInstance(Tex); //BasisBit ToDo}*/
    end;

    destructor TMenuBackgroundFade.Destroy;
    begin
        //Why isn't there any Tex.free method?
    /*{ if UseTexture then
      FreeandNil(Tex); }*/
        inherited;
    end;

    procedure   TMenuBackgroundFade.OnShow;
    begin
        FadeTime : = SDL_GetTicks;
    end;

    procedure   TMenuBackgroundFade.Draw;
    var
        Progress : real;
    begin
        if FadeTime = 0 then
            Progress : = Alpha
        else
            Progress : = Alpha * (SDL_GetTicks - FadeTime) / FADEINTIME;

    if Progress > Alpha then
        begin
        FadeTime : = 0;
Progress: = Alpha;
    end;

    if (UseTexture) then
        begin //Draw Texture to Screen
        if (ScreenAct = 1) then //Clear just once when in dual screen mode
            glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColorRGB(Color, Progress);
    glBindTexture(GL_TEXTURE_2D, Tex.TexNum);

    glBegin(GL_QUADS);
    glTexCoord2f(Tex.TexX1 * Tex.TexW, Tex.TexY1 * Tex.TexH);
    glVertex2f(0, 0);

    glTexCoord2f(Tex.TexX1 * Tex.TexW, Tex.TexY2 * Tex.TexH);
    glVertex2f(0, 600);

    glTexCoord2f(Tex.TexX2 * Tex.TexW, Tex.TexY2 * Tex.TexH);
    glVertex2f(800, 600);

    glTexCoord2f(Tex.TexX2 * Tex.TexW, Tex.TexY1 * Tex.TexH);
    glVertex2f(800, 0);
    glEnd;

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    end
        else
            begin //Clear Screen w/ progress Alpha + Color
            if (ScreenAct = 1) then //Clear just once when in dual screen mode
                glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColorRGB(Color, Progress);

    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 600);
    glVertex2f(800, 600);
    glVertex2f(800, 0);
    glEnd;

    glDisable(GL_BLEND);
    end;
    end;
}