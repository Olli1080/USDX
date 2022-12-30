#pragma once

/** UltraStar Deluxe - Karaoke Game
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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UDraw.pas $
 * $Id: UDraw.pas 2514 2010-06-13 10:57:33Z tobigun $
 **/

#include "../switches.h"

#include <array>
#include <cstdint>

#include <SDL2/SDL.h>

#include "UIni.h"

namespace UDraw
{
/*uses
  UCommon,
  UThemes,
  sdl2,
  UGraphicClasses,
  UIni;*/

void SingDraw();
void SingDrawLines();
void SingDrawBackground();
void SingDrawOscilloscope(double X, double Y, double W, double H, int NrSound);
void SingDrawNoteLines(double Left, double Top, double Right, int LineSpacing = 15);
void SingDrawLyricHelper(int CP, double Left, double LyricsMid);
void SingDrawLine(double Left, double Top, double Right, int Track, int PlayerNumber, int LineSpacing = 15);
void SingDrawPlayerLine(double X, double Y, double W, int Track, int PlayerIndex, int LineSpacing = 15);
void SingDrawPlayerBGLine(double Left, double Top, double Right, int Track, int PlayerIndex, int LineSpacing = 15);

// TimeBar
void SingDrawTimeBar();

//Draw Editor NoteLines
void EditDrawLine(double X, double YBaseNote, double W, double H, int Track, int NumLines = 10);
void EditDrawBorderedBox(int X, int Y, int W, int H, double FillR = 0.9, double FillG = 0.9, double FillB = 0.9, double FillAlpha = 0.5);
void EditDrawBeatDelimiters(double X, double Y, double W, double H, int Track);

// Draw Jukebox
void SingDrawJukebox();
void SingDrawJukeboxBackground();
void SingDrawJukeboxBlackBackground();
void SingDrawJukeboxTimeBar();
void SingDrawLyricHelperJukebox(double Left, double LyricsMid);

// Draw Webcam
void SingDrawWebCamFrame();

struct TRecR
{
    double Top;
    double Left;
    double Right;
    double Bottom;

    double Width;
    double WMid;
    double Height;
    double HMid;
    double Mid;
};


std::array<double, UIni::IMaxPlayerCount> NotesW;
std::array<double, UIni::IMaxPlayerCount> NotesH;
int Starfr;
int StarfrG;

  //SingBar
uint32_t TickOld;
uint32_t TickOld2;

SDL_Thread FrameThread;
SDL_Mutex Mutex;


/*uses
  SysUtils,
  Math,
  dglOpenGL,
  opencv_core,
  TextGL,
  UDrawTexture,
  UGraphic,
  ULog,
  ULyrics,
  UNote,
  UParty,
  UMusic,
  URecord,
  UScreenSingController,
  UScreenJukebox,
  USong,
  UTexture,
  UWebcam;*/


void SingDrawWebCamFrame()
/*var
  status: int;
  TextureCam2: PTexture;*/
{

  Webcam.GetWebcamFrame;

  if (Webcam.TextureCam.TexNum > 0) then
  {
    glColor4f(1, 1, 1, 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, Webcam.TextureCam.TexNum);
    glEnable(GL_BL});
    gl{(GL_QUADS);

      glTexCoord2f(0, 0);
      glVertex2f(800,  0);
      glTexCoord2f(0, Webcam.TextureCam.TexH);
      glVertex2f(800,  600);
      glTexCoord2f( Webcam.TextureCam.TexW, Webcam.TextureCam.TexH);
      glVertex2f(0, 600);
      glTexCoord2f( Webcam.TextureCam.TexW, 0);
      glVertex2f(0, 0);

    gl};
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BL});

    // reset to default
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  };

  /*
  // Save Frame to AVI
  if (ScreenSing.WebcamSave) then
  {
    cvCvtColor(WebcamFrame, WebcamFrame, CV_RGB2BGR);
    //cvInvert(WebcamFrame, WebcamFrame, 0);
    cvWriteFrame(ScreenSing.WebCamVideoWriter, WebcamFrame);
  };
  */

};

void SingDrawBackground;
var
  Rec:    TRecR;
  TexRec: TRecR;
{
  if (ScreenSing.Tex_Background.TexNum > 0) then
  {
    if (Ini.MovieSize <= 1) then  //HalfSize BG
    {
      (* half screen + gradient *)
      Rec.Top = 110; // 80
      Rec.Bottom = Rec.Top + 20;
      Rec.Left  = 0;
      Rec.Right = 800;

      TexRec.Top = (Rec.Top / 600) * ScreenSing.Tex_Background.TexH;
      TexRec.Bottom = (Rec.Bottom / 600) * ScreenSing.Tex_Background.TexH;
      TexRec.Left = 0;
      TexRec.Right = ScreenSing.Tex_Background.TexW;

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, ScreenSing.Tex_Background.TexNum);
      glEnable(GL_BL});
      gl{(GL_QUADS);
        (* gradient draw *)
        (* top *)
        glColor4f(1, 1, 1, 0);
        glTexCoord2f(TexRec.Right, TexRec.Top);    glVertex2f(Rec.Right, Rec.Top);
        glTexCoord2f(TexRec.Left,  TexRec.Top);    glVertex2f(Rec.Left,  Rec.Top);
        glColor4f(1, 1, 1, 1);
        glTexCoord2f(TexRec.Left,  TexRec.Bottom); glVertex2f(Rec.Left,  Rec.Bottom);
        glTexCoord2f(TexRec.Right, TexRec.Bottom); glVertex2f(Rec.Right, Rec.Bottom);
        (* mid *)
        Rec.Top = Rec.Bottom;
        Rec.Bottom = 490 - 20; // 490 - 20
        TexRec.Top = TexRec.Bottom;
        TexRec.Bottom = (Rec.Bottom / 600) * ScreenSing.Tex_Background.TexH;
        glTexCoord2f(TexRec.Left,  TexRec.Top);    glVertex2f(Rec.Left,  Rec.Top);
        glTexCoord2f(TexRec.Left,  TexRec.Bottom); glVertex2f(Rec.Left,  Rec.Bottom);
        glTexCoord2f(TexRec.Right, TexRec.Bottom); glVertex2f(Rec.Right, Rec.Bottom);
        glTexCoord2f(TexRec.Right, TexRec.Top);    glVertex2f(Rec.Right, Rec.Top);
        (* bottom *)
        Rec.Top = Rec.Bottom;
        Rec.Bottom = 490; // 490
        TexRec.Top = TexRec.Bottom;
        TexRec.Bottom = (Rec.Bottom / 600) * ScreenSing.Tex_Background.TexH;
        glTexCoord2f(TexRec.Right, TexRec.Top);    glVertex2f(Rec.Right, Rec.Top);
        glTexCoord2f(TexRec.Left,  TexRec.Top);    glVertex2f(Rec.Left,  Rec.Top);
        glColor4f(1, 1, 1, 0);
        glTexCoord2f(TexRec.Left,  TexRec.Bottom); glVertex2f(Rec.Left,  Rec.Bottom);
        glTexCoord2f(TexRec.Right, TexRec.Bottom); glVertex2f(Rec.Right, Rec.Bottom);

      gl};
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_BL});
    }
    else //Full Size BG
    {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, ScreenSing.Tex_Background.TexNum);
      //glEnable(GL_BL});
      gl{(GL_QUADS);

        glTexCoord2f(0, 0);   glVertex2f(0,  0);
        glTexCoord2f(0,  ScreenSing.Tex_Background.TexH);   glVertex2f(0,  600);
        glTexCoord2f( ScreenSing.Tex_Background.TexW,  ScreenSing.Tex_Background.TexH);   glVertex2f(800, 600);
        glTexCoord2f( ScreenSing.Tex_Background.TexW, 0);   glVertex2f(800, 0);

      gl};
      glDisable(GL_TEXTURE_2D);
      //glDisable(GL_BL});
    };
  };
};

void SingDrawJukeboxBackground;
var
  Rec:    TRecR;
  TexRec: TRecR;
{
  if (ScreenJukebox.Tex_Background.TexNum > 0) then
  {
    if (Ini.MovieSize <= 1) then  //HalfSize BG
    {
      (* half screen + gradient *)
      Rec.Top = 110; // 80
      Rec.Bottom = Rec.Top + 20;
      Rec.Left  = 0;
      Rec.Right = 800;

      TexRec.Top = (Rec.Top / 600) * ScreenJukebox.Tex_Background.TexH;
      TexRec.Bottom = (Rec.Bottom / 600) * ScreenJukebox.Tex_Background.TexH;
      TexRec.Left = 0;
      TexRec.Right = ScreenJukebox.Tex_Background.TexW;

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, ScreenJukebox.Tex_Background.TexNum);
      glEnable(GL_BL});

      gl{(GL_QUADS);
        (* gradient draw *)
        (* top *)
        glColor4f(1, 1, 1, 0);
        glTexCoord2f(TexRec.Right, TexRec.Top);    glVertex2f(Rec.Right, Rec.Top);
        glTexCoord2f(TexRec.Left,  TexRec.Top);    glVertex2f(Rec.Left,  Rec.Top);
        glColor4f(1, 1, 1, 1);
        glTexCoord2f(TexRec.Left,  TexRec.Bottom); glVertex2f(Rec.Left,  Rec.Bottom);
        glTexCoord2f(TexRec.Right, TexRec.Bottom); glVertex2f(Rec.Right, Rec.Bottom);
        (* mid *)
        Rec.Top = Rec.Bottom;
        Rec.Bottom = 490 - 20; // 490 - 20
        TexRec.Top = TexRec.Bottom;
        TexRec.Bottom = (Rec.Bottom / 600) * ScreenJukebox.Tex_Background.TexH;
        glTexCoord2f(TexRec.Left,  TexRec.Top);    glVertex2f(Rec.Left,  Rec.Top);
        glTexCoord2f(TexRec.Left,  TexRec.Bottom); glVertex2f(Rec.Left,  Rec.Bottom);
        glTexCoord2f(TexRec.Right, TexRec.Bottom); glVertex2f(Rec.Right, Rec.Bottom);

        glTexCoord2f(TexRec.Right, TexRec.Top);    glVertex2f(Rec.Right, Rec.Top);
        (* bottom *)
        Rec.Top = Rec.Bottom;
        Rec.Bottom = 490; // 490
        TexRec.Top = TexRec.Bottom;
        TexRec.Bottom = (Rec.Bottom / 600) * ScreenJukebox.Tex_Background.TexH;
        glTexCoord2f(TexRec.Right, TexRec.Top);    glVertex2f(Rec.Right, Rec.Top);
        glTexCoord2f(TexRec.Left,  TexRec.Top);    glVertex2f(Rec.Left,  Rec.Top);
        glColor4f(1, 1, 1, 0);
        glTexCoord2f(TexRec.Left,  TexRec.Bottom); glVertex2f(Rec.Left,  Rec.Bottom);
        glTexCoord2f(TexRec.Right, TexRec.Bottom); glVertex2f(Rec.Right, Rec.Bottom);

      gl};
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_BL});
    }
    else //Full Size BG
    {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, ScreenJukebox.Tex_Background.TexNum);
      glEnable(GL_BL});
      gl{(GL_QUADS);
        glColor4f(1, 1, 1, 1);

        glTexCoord2f(0, 0);   glVertex2f(0,  0);
        glTexCoord2f(0,  ScreenJukebox.Tex_Background.TexH);   glVertex2f(0,  600);
        glTexCoord2f( ScreenJukebox.Tex_Background.TexW,  ScreenJukebox.Tex_Background.TexH);   glVertex2f(800, 600);
        glTexCoord2f( ScreenJukebox.Tex_Background.TexW, 0);   glVertex2f(800, 0);

      gl};
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_BL});
    };
  }
  else
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ScreenJukebox.Tex_Background.TexNum);
    //glEnable(GL_BL});
    gl{(GL_QUADS);
      glColor4f(0, 0, 0, 1);

      glTexCoord2f(0, 0);   glVertex2f(0,  0);
      glTexCoord2f(0,  ScreenJukebox.Tex_Background.TexH);   glVertex2f(0,  600);
      glTexCoord2f( ScreenJukebox.Tex_Background.TexW,  ScreenJukebox.Tex_Background.TexH);   glVertex2f(800, 600);
      glTexCoord2f( ScreenJukebox.Tex_Background.TexW, 0);   glVertex2f(800, 0);

    gl};
    glDisable(GL_TEXTURE_2D);
    //glDisable(GL_BL});
  };
};

void SingDrawJukeboxBlackBackground;
var
  Rec:    TRecR;
  TexRec: TRecR;
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, ScreenJukebox.Tex_Background.TexNum);
  //glEnable(GL_BL});
  gl{(GL_QUADS);
    glColor4f(0, 0, 0, 1);

    glTexCoord2f(0, 0);   glVertex2f(0,  0);
    glTexCoord2f(0,  ScreenJukebox.Tex_Background.TexH);   glVertex2f(0,  600);
    glTexCoord2f( ScreenJukebox.Tex_Background.TexW,  ScreenJukebox.Tex_Background.TexH);   glVertex2f(800, 600);
    glTexCoord2f( ScreenJukebox.Tex_Background.TexW, 0);   glVertex2f(800, 0);

  gl};
  glDisable(GL_TEXTURE_2D);
  //glDisable(GL_BL});
};

void SingDrawOscilloscope(X, Y, W, H: double; NrSound: int);
var
  SampleIndex: int;
  Sound:       TCaptureBuffer;
  MaxX, MaxY:  double;
  Col: TRGB;
{;
  Sound = AudioInputProcessor.Sound[NrSound];

  //  Log.LogStatus('Oscilloscope', 'SingDraw');
  //glColor3f(Skin_OscR, Skin_OscG, Skin_OscB);

  if (Party.bPartyGame) then
    Col = GetPlayerColor(Ini.TeamColor[NrSound])
  else
    Col = GetPlayerColor(Ini.PlayerColor[NrSound]);

  glColor3f(Col.R, Col.G, Col.B);
/*
  if (ParamStr(1)=='-black') or (ParamStr(1)=='-fsblack') then
    glColor3f(1, 1, 1);
*/
  MaxX = W-1;
  MaxY = (H-1) / 2;

  Sound.LockAnalysisBuffer();

  gl{(GL_LINE_STRIP);
    for SampleIndex = 0 to High(Sound.AnalysisBuffer) do
    {
      glVertex2f(X + MaxX * SampleIndex/High(Sound.AnalysisBuffer),
                 Y + MaxY * (1 - Sound.AnalysisBuffer[SampleIndex]/-Low(Smallint)));
    };
  gl};

  Sound.UnlockAnalysisBuffer();
};

void SingDrawNoteLines(Left, Top, Right: double; LineSpacing: int);
var
  Count: int;
{
  glEnable(GL_BL});
  glColor4f(Skin_P1_LinesR, Skin_P1_LinesG, Skin_P1_LinesB, 0.4);
  gl{(GL_LINES);
  for Count = 0 to 9 do
  {
    glVertex2f(Left,  Top + Count * LineSpacing);
    glVertex2f(Right, Top + Count * LineSpacing);
  };
  gl};
  glDisable(GL_BL});
};

// draw blank Notebars
void SingDrawLine(Left, Top, Right: double; Track, PlayerNumber: int; LineSpacing: int);
var
  Rec:   TRecR;
  Count: int;
  TempR: double;

  GoldenStarPos: double;
{
// We actually don't have a playernumber in this void, it should reside in Track - but it is always set to zero
// So we exploit this behavior a bit - we give Track the playernumber, keep it in playernumber - and then we set Track to zero
// This could also come quite in handy when we do the duet mode, cause just the notes for the player that has to sing should be drawn then
// BUT this is not implemented yet, all notes are drawn! :D
  if (ScreenSing.settings.NotesVisible and (1 shl Track) != 0) then
  {
    //PlayerNumber = Track + 1; // Player 1 is 0

    // exploit done

    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BL});
    glBl}Func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if not Tracks[Track].Lines[Tracks[Track].CurrentLine].HasLength(TempR) then TempR = 0
    else TempR = (Right-Left) / TempR;


    with Tracks[Track].Lines[Tracks[Track].CurrentLine] do
    {
      for Count = 0 to HighNote do
      {
        with Notes[Count] do
        {
          if NoteType != ntFreestyle then
          {
            if Ini.EffectSing==0 then
              // If Golden note Effect of then Change not Color
            {
              case NoteType of
                ntNormal: glColor4f(1, 1, 1, 1);   // We set alpha to 1, cause we can control the transparency through the png itself
                ntGolden: glColor4f(1, 1, 0.3, 1); // no stars, paint yellow -> glColor4f(1, 1, 0.3, 0.85); - we could
                ntRap:    glColor4f(1, 1, 1, 1);
                ntRapGolden: glColor4f(1, 1, 0.3, 1);
            }; // case
            } //Else all Notes same Color
            else
              glColor4f(1, 1, 1, 1);        // We set alpha to 1, cause we can control the transparency through the png itself

            // left part
            Rec.Left  = (StartBeat - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat) * TempR + Left + 0.5 + 10*ScreenX;
            Rec.Right = Rec.Left + NotesW[PlayerNumber - 1];
            Rec.Top = Top - (Tone-BaseNote)*LineSpacing/2 - NotesH[PlayerNumber - 1];
            Rec.Bottom = Rec.Top + 2 * NotesH[PlayerNumber - 1];
            If (NoteType==ntRap) or (NoteType==ntRapGolden) then
            {
              glBindTexture(GL_TEXTURE_2D, Tex_plain_Left_Rap[PlayerNumber].TexNum);
            }
            else
            {
              glBindTexture(GL_TEXTURE_2D, Tex_plain_Left[PlayerNumber].TexNum);
            };
            gl{(GL_QUADS);
              glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
              glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
              glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
              glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
            gl};

            //We keep the postion of the top left corner b4 it's overwritten
            GoldenStarPos = Rec.Left;
            //done

            // middle part
            Rec.Left = Rec.Right;
            Rec.Right = (StartBeat + Duration - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat) * TempR + Left - NotesW[PlayerNumber - 1] - 0.5 + 10*ScreenX;

            // the left note is more right than the right note itself, sounds weird - so we fix that xD
            if Rec.Right <= Rec.Left then
              Rec.Right = Rec.Left;

            If (NoteType==ntRap) or (NoteType==ntRapGolden) then
            {
              glBindTexture(GL_TEXTURE_2D, Tex_plain_Mid_Rap[PlayerNumber].TexNum);
            }
            else
            {
              glBindTexture(GL_TEXTURE_2D, Tex_plain_Mid[PlayerNumber].TexNum);
            };
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
            gl{(GL_QUADS);
              glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
              glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
              glTexCoord2f(round((Rec.Right-Rec.Left)/32), 1); glVertex2f(Rec.Right, Rec.Bottom);
              glTexCoord2f(round((Rec.Right-Rec.Left)/32), 0); glVertex2f(Rec.Right, Rec.Top);
            gl};

            // right part
            Rec.Left  = Rec.Right;
            Rec.Right = Rec.Right + NotesW[PlayerNumber - 1];


            if (NoteType==ntRap) or (NoteType==ntRapGolden) then
            {
              glBindTexture(GL_TEXTURE_2D, Tex_plain_Right_Rap[PlayerNumber].TexNum);
            }
            else
            {
              glBindTexture(GL_TEXTURE_2D, Tex_plain_Right[PlayerNumber].TexNum);
            };
            gl{(GL_QUADS);
              glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
              glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
              glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
              glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
            gl};

            // Golden Star Patch
            if ((NoteType==ntGolden) or (NoteType==ntRapGolden)) and (Ini.EffectSing=1) then
            {
              GoldenRec.SaveGoldenStarsRec(GoldenStarPos, Rec.Top, Rec.Right, Rec.Bottom);
            };
          }; // if not FreeStyle
        }; // with
      }; // for
    }; // with
    glDisable(GL_BL});
    glDisable(GL_TEXTURE_2D);
  };
};

// draw sung notes
void SingDrawPlayerLine(X, Y, W: double; Track, PlayerIndex: int; LineSpacing: int);
var
  TempR:      double;
  Rec:        TRecR;
  N: int;
//  R, G, B, A: double;
  NotesH2:    double;
{
  if (ScreenSing.Settings.InputVisible) then
  {
    //Log.LogStatus('Player notes', 'SingDraw');

    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BL});
    glBl}Func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //if Player[NrGracza].LengthNote > 0 then
    {
      if not Tracks[Track].Lines[Tracks[Track].CurrentLine].HasLength(TempR) then TempR = 0
      else TempR = W / TempR;

      for N = 0 to Player[PlayerIndex].HighNote do
      {
        with Player[PlayerIndex].Note[N] do
        {
          // Left part of note
          Rec.Left  = X + (Start - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat) * TempR + 0.5 + 10*ScreenX;
          Rec.Right = Rec.Left + NotesW[PlayerIndex];

          // Draw it in half size, if not hit
          if Hit then
          {
            NotesH2 = NotesH[PlayerIndex]
          }
          else
          {
            NotesH2 = int(NotesH[PlayerIndex] * 0.65);
          };

          Rec.Top    = Y - (Tone-Tracks[Track].Lines[Tracks[Track].CurrentLine].BaseNote)*LineSpacing/2 - NotesH2;
          Rec.Bottom = Rec.Top + 2 * NotesH2;

          // draw the left part
          glColor3f(1, 1, 1);
          If (NoteType==ntRap) or (NoteType==ntRapGolden) then
          {
            glBindTexture(GL_TEXTURE_2D, Tex_Left_Rap[PlayerIndex+1].TexNum);
          }
          else
          {
            glBindTexture(GL_TEXTURE_2D, Tex_Left[PlayerIndex+1].TexNum);
          };
          gl{(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
            glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
            glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
            glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
          gl};

          // Middle part of the note
          Rec.Left  = Rec.Right;
          Rec.Right = X + (Start + Duration - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat) * TempR - NotesW[PlayerIndex] - 0.5  + 10*ScreenX;

          // new
          if (Start + Duration - 1==LyricsState.CurrentBeatD) then
            Rec.Right = Rec.Right - (1-Frac(LyricsState.MidBeatD)) * TempR;

          // the left note is more right than the right note itself, sounds weird - so we fix that xD
          if Rec.Right <= Rec.Left then
            Rec.Right = Rec.Left;

          // draw the middle part
          If (NoteType==ntRap) or (NoteType==ntRapGolden) then
          {
            glBindTexture(GL_TEXTURE_2D, Tex_Mid_Rap[PlayerIndex+1].TexNum);
          }
          else
          {
            glBindTexture(GL_TEXTURE_2D, Tex_Mid[PlayerIndex+1].TexNum);
          };
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
          gl{(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
            glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
            glTexCoord2f(round((Rec.Right-Rec.Left)/32), 1); glVertex2f(Rec.Right, Rec.Bottom);
            glTexCoord2f(round((Rec.Right-Rec.Left)/32), 0); glVertex2f(Rec.Right, Rec.Top);
          gl};
          glColor3f(1, 1, 1);

          // the right part of the note
          Rec.Left  = Rec.Right;
          Rec.Right = Rec.Right + NotesW[PlayerIndex];

          If (NoteType==ntRap) or (NoteType==ntRapGolden) then
          {
            glBindTexture(GL_TEXTURE_2D, Tex_Right_Rap[PlayerIndex+1].TexNum);
          }
          else
          {
          glBindTexture(GL_TEXTURE_2D, Tex_Right[PlayerIndex+1].TexNum);
          };
          gl{(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
            glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
            glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
            glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
          gl};

          // Perfect note is stored
          if Perfect and (Ini.EffectSing=1) then
          {
            //A = 1 - 2*(LyricsState.GetCurrentTime() - GetTimeFromBeat(Start + Duration));
            if not (Start + Duration - 1==LyricsState.CurrentBeatD) then
            {
              //Star animation counter
              //inc(Starfr);
              //Starfr = Starfr mod 128;
             // if not(CurrentSong.isDuet) or (PlayerIndex mod 2==Track) then
                GoldenRec.SavePerfectNotePos(Rec.Left, Rec.Top);
            };
          };
        }; // with
      }; // for

      // actually we need a comparison here, to determine if the singing process
      // is ahead Rec.Right even if there is no singing

      if (Ini.EffectSing==1) then
        GoldenRec.GoldenNoteTwinkle(Rec.Top,Rec.Bottom,Rec.Right, PlayerIndex);
    }; // if
  }; // if
};

//draw Note glow
void SingDrawPlayerBGLine(Left, Top, Right: double; Track, PlayerIndex: int; LineSpacing: int);
var
  Rec:            TRecR;
  Count:          int;
  TempR:          double;
  W, H:           double;
{
  if (ScreenSing.settings.NotesVisible and (1 shl PlayerIndex) != 0) then
  {
    //glColor4f(1, 1, 1, sqrt((1+sin( AudioPlayback.Position * 3))/4)/ 2 + 0.5 );
    glColor4f(1, 1, 1, sqrt((1 + sin(AudioPlayback.Position * 3)))/2 + 0.05);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BL});
    glBl}Func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if not Tracks[Track].Lines[Tracks[Track].CurrentLine].HasLength(TempR) then TempR = 0
    else TempR = (Right-Left) / TempR;

    with Tracks[Track].Lines[Tracks[Track].CurrentLine] do
    {
      for Count = 0 to HighNote do
      {
        with Notes[Count] do
        {
          if NoteType != ntFreestyle then
          {
            // {: 14, 20
            // easy: 6, 11
            W = NotesW[PlayerIndex] * 2 + 2;
            H = NotesH[PlayerIndex] * 1.5 + 3.5;

            /*
            X2 = (Start-Tracks[Track].Lines[Tracks[Track].Current].Notes[0].Start) * TempR + Left + 0.5 + 10*ScreenX + 4;
            X1 = X2-W;

            X3 = (Start+Length-Tracks[Track].Lines[Tracks[Track].Current].Notes[0].Start) * TempR + Left - 0.5 + 10*ScreenX - 4;
            X4 = X3+W;
            */

            // left
            Rec.Right = (StartBeat - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat) * TempR + Left + 0.5 + 10*ScreenX + 4;
            Rec.Left  = Rec.Right - W;
            Rec.Top = Top - (Tone-BaseNote)*LineSpacing/2 - H;
            Rec.Bottom = Rec.Top + 2 * H;

            If (NoteType==ntRap) or (NoteType==ntRapGolden) then
            {
              glBindTexture(GL_TEXTURE_2D, Tex_BG_Left_Rap[PlayerIndex+1].TexNum);
            }
            else
            {
              glBindTexture(GL_TEXTURE_2D, Tex_BG_Left[PlayerIndex+1].TexNum);
            };
            gl{(GL_QUADS);
              glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
              glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
              glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
              glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
            gl};

            // middle part
            Rec.Left  = Rec.Right;
            Rec.Right = (StartBeat + Duration - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat) * TempR + Left - 0.5 + 10*ScreenX - 4;

            // the left note is more right than the right note itself, sounds weird - so we fix that xD
            if Rec.Right <= Rec.Left then
              Rec.Right = Rec.Left;

            If (NoteType==ntRap) or (NoteType==ntRapGolden) then
            {
              glBindTexture(GL_TEXTURE_2D, Tex_BG_Mid_Rap[PlayerIndex+1].TexNum);
            }
            else
            {
              glBindTexture(GL_TEXTURE_2D, Tex_BG_Mid[PlayerIndex+1].TexNum);
            };
            gl{(GL_QUADS);
              glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
              glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
              glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
              glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
            gl};

            // right part
            Rec.Left  = Rec.Right;
            Rec.Right = Rec.Left + W;

            If (NoteType==ntRap) or (NoteType==ntRapGolden) then
            {
              glBindTexture(GL_TEXTURE_2D, Tex_BG_Right_Rap[PlayerIndex+1].TexNum);
            }
            else
            {
              glBindTexture(GL_TEXTURE_2D, Tex_BG_Right[PlayerIndex+1].TexNum);
            };
            gl{(GL_QUADS);
              glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
              glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
              glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
              glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
            gl};

          }; // if not FreeStyle
        }; // with
      }; // for
    }; // with

    glDisable(GL_BL});
    glDisable(GL_TEXTURE_2D);
  };
};

(**
 * Draws the lyrics helper bar.
 * Left: position the bar starts at
 * LyricsMid: the middle of the lyrics relative to the position Left 
 *)
void SingDrawLyricHelper(CP: int; Left, LyricsMid: double);
var
  Bounds: TRecR;           // bounds of the lyric help bar
  BarProgress: double;       // progress of the lyrics helper
  BarMoveDelta: double;      // current beat relative to the beat the bar starts to move at
  BarAlpha: double;          // transparency
  CurLine:  PLine;         // current lyric line (beat specific)
  LineWidth: double;         // lyric line width
  FirstNoteBeat: int;  // beat of the first note in the current line
  FirstNoteDelta: int; // time in beats between the start of the current line and its first note
  MoveStartX: double;        // x-pos. the bar starts to move from
  MoveDist: double;          // number of pixels the bar will move
  LyricEngine: TLyricEngine;
  Col: TRGB;
const
  BarWidth ==50; // width  of the lyric helper bar
  BarHeight==30; // height of the lyric helper bar
  BarMoveLimit==40; // max. number of beats remaining before the bar starts to move
{
  // get current lyrics line and the time in beats of its first note
  CurLine = @Tracks[CP].Lines[Tracks[CP].CurrentLine];

  // FIXME: accessing ScreenSing is not that generic
  if (CurrentSong.isDuet) and (PlayersPlay != 1) then
  {
    if (CP==1) then
      LyricEngine = ScreenSing.LyricsDuetP2
    else
      LyricEngine = ScreenSing.LyricsDuetP1;
  }
  else
    LyricEngine = ScreenSing.Lyrics;

  LyricEngine.FontFamily = Ini.LyricsFont;
  LyricEngine.FontStyle  = Ini.LyricsStyle;

  // do not draw the lyrics helper if the current line does not contain any note
  if (Length(CurLine.Notes) > 0) then
  {
    // start beat of the first note of this line
    FirstNoteBeat = CurLine.Notes[0].StartBeat;
    // time in beats between the start of the current line and its first note
    FirstNoteDelta = FirstNoteBeat - CurLine.StartBeat;

    // beats from current beat to the first note of the line
    BarMoveDelta = FirstNoteBeat - LyricsState.MidBeat;

    if (FirstNoteDelta > 8) and  // if the wait-time is large enough
       (BarMoveDelta > 0) then   // and the first note of the line is not reached
    {
      // let the bar blink to the beat
      BarAlpha = 0.75 + cos(BarMoveDelta/2) * 0.25;

      // if the number of beats to the first note is too big,
      // the bar stays on the left side.
      if (BarMoveDelta > BarMoveLimit) then
        BarMoveDelta = BarMoveLimit;

      // limit number of beats the bar moves
      if (FirstNoteDelta > BarMoveLimit) then
        FirstNoteDelta = BarMoveLimit;

      // calc bar progress
      BarProgress = 1 - BarMoveDelta / FirstNoteDelta;

      // retrieve the width of the upper lyrics line on the display
      if (LyricEngine.GetUpperLine() != nil) then
        LineWidth = LyricEngine.GetUpperLine().Width
      else
        LineWidth = 0;

      // distance the bar will move (LyricRec.Left to {ning of text)
      MoveDist = LyricsMid - LineWidth / 2 - BarWidth;
      // if the line is too long the helper might move from right to left
      // so we have to assure the start position is left of the text.
      if (MoveDist >= 0) then
        MoveStartX = Left
      else
        MoveStartX = Left + MoveDist;

      // determine lyric help bar position and size
      Bounds.Left = MoveStartX + BarProgress * MoveDist;
      Bounds.Right = Bounds.Left + BarWidth;
      if (CurrentSong.isDuet) and (PlayersPlay != 1) then
      {
        if (CP==0) then
          Bounds.Top = Theme.LyricBarDuetP1.IndicatorYOffset + Theme.LyricBarDuetP1.UpperY
        else
          Bounds.Top = Theme.LyricBarDuetP2.IndicatorYOffset + Theme.LyricBarDuetP2.UpperY ;
      }
      else
        Bounds.Top = Theme.LyricBar.IndicatorYOffset + Theme.LyricBar.UpperY ;

      Bounds.Bottom = Bounds.Top + BarHeight + 3;

      // draw lyric help bar
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BL});

      if (CurrentSong.isDuet) then
      {
        if (PlayersPlay==1) or (PlayersPlay==2) then
          Col = GetLyricBarColor(Ini.SingColor[CP])
        else
        {
          if (PlayersPlay==3) or (PlayersPlay==6) then
          {
            //if (PlayersPlay==3) then
              Col = GetLyricBarColor(Ini.SingColor[CP]);

            //if (PlayersPlay==6) then
            //  Col = GetLyricBarColor(CP + 1);
          }
          else
          {
            if ScreenAct==1 then
              Col = GetLyricBarColor(Ini.SingColor[CP])
            else
              Col = GetLyricBarColor(Ini.SingColor[CP + 2]);
          };
        };
      }
      else
        Col = GetLyricBarColor(1);

      glColor4f(Col.R, Col.G, Col.B, BarAlpha);

      glBl}Func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D, Tex_Lyric_Help_Bar.TexNum);
      gl{(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(Bounds.Left,  Bounds.Top);
        glTexCoord2f(0, 1); glVertex2f(Bounds.Left,  Bounds.Bottom);
        glTexCoord2f(1, 1); glVertex2f(Bounds.Right, Bounds.Bottom);
        glTexCoord2f(1, 0); glVertex2f(Bounds.Right, Bounds.Top);
      gl};
      glDisable(GL_BL});
    };
  };
};

(**
 * Draws the lyrics helper bar jukebox.
 * Left: position the bar starts at
 * LyricsMid: the middle of the lyrics relative to the position Left
 *)
void SingDrawLyricHelperJukebox(Left, LyricsMid: double);
var
  Bounds: TRecR;           // bounds of the lyric help bar
  BarProgress: double;       // progress of the lyrics helper
  BarMoveDelta: double;      // current beat relative to the beat the bar starts to move at
  BarAlpha: double;          // transparency
  CurLine:  PLine;         // current lyric line (beat specific)
  LineWidth: double;         // lyric line width
  FirstNoteBeat: int;  // beat of the first note in the current line
  FirstNoteDelta: int; // time in beats between the start of the current line and its first note
  MoveStartX: double;        // x-pos. the bar starts to move from
  MoveDist: double;          // number of pixels the bar will move
  LyricEngine: TLyricEngine;
const
  BarWidth ==50; // width  of the lyric helper bar
  BarHeight==30; // height of the lyric helper bar
  BarMoveLimit==40; // max. number of beats remaining before the bar starts to move
{

  // get current lyrics line and the time in beats of its first note
  CurLine = @Tracks[0].Lines[Tracks[0].CurrentLine];

  // FIXME: accessing ScreenSing is not that generic
  LyricEngine = ScreenJukebox.Lyrics;

  // do not draw the lyrics helper if the current line does not contain any note
  if (Length(CurLine.Notes) > 0) then
  {
    // start beat of the first note of this line
    FirstNoteBeat = CurLine.Notes[0].StartBeat;
    // time in beats between the start of the current line and its first note
    FirstNoteDelta = FirstNoteBeat - CurLine.StartBeat;

    // beats from current beat to the first note of the line
    BarMoveDelta = FirstNoteBeat - LyricsState.MidBeat;

    if (FirstNoteDelta > 8) and  // if the wait-time is large enough
       (BarMoveDelta > 0) then   // and the first note of the line is not reached
    {
      // let the bar blink to the beat
      BarAlpha = 0.75 + cos(BarMoveDelta/2) * 0.25;

      // if the number of beats to the first note is too big,
      // the bar stays on the left side.
      if (BarMoveDelta > BarMoveLimit) then
        BarMoveDelta = BarMoveLimit;

      // limit number of beats the bar moves
      if (FirstNoteDelta > BarMoveLimit) then
        FirstNoteDelta = BarMoveLimit;

      // calc bar progress
      BarProgress = 1 - BarMoveDelta / FirstNoteDelta;

      // retrieve the width of the upper lyrics line on the display
      if (LyricEngine.GetUpperLine() != nil) then
        LineWidth = LyricEngine.GetUpperLine().Width
      else
        LineWidth = 0;

      // distance the bar will move (LyricRec.Left to {ning of text)
      MoveDist = LyricsMid - LineWidth/2 - BarWidth;

      if (LineWidth/2 + (BarWidth - Left) * 2 >= 400) then
        Bounds.Left = - BarProgress * MoveDist + Left - BarWidth
      else
      {
        // if the line is too long the helper might move from right to left
        // so we have to assure the start position is left of the text.
        if (MoveDist >= 0) then
          MoveStartX = Left
        else
          MoveStartX = Left + MoveDist;

        // determine lyric help bar position and size
        Bounds.Left = MoveStartX + BarProgress * MoveDist;
      };

      Bounds.Right = Bounds.Left + BarWidth;
      Bounds.Top = Theme.LyricBarJukebox.IndicatorYOffset + ScreenJukeBox.Lyrics.UpperLineY;
      Bounds.Bottom = Bounds.Top + BarHeight + 3;

      // draw lyric help bar
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BL});

      //glColor4f(1, 0.75, 0, BarAlpha);
      glColor4f(ScreenJukebox.LyricHelper.R, ScreenJukebox.LyricHelper.G, ScreenJukebox.LyricHelper.B, BarAlpha);

      glBl}Func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D, Tex_Lyric_Help_Bar.TexNum);
      gl{(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(Bounds.Left,  Bounds.Top);
        glTexCoord2f(0, 1); glVertex2f(Bounds.Left,  Bounds.Bottom);
        glTexCoord2f(1, 1); glVertex2f(Bounds.Right, Bounds.Bottom);
        glTexCoord2f(1, 0); glVertex2f(Bounds.Right, Bounds.Top);
      gl};
      glDisable(GL_BL});
    };
  };
};

void SingDrawLines;
var
  NR: TRecR;         // lyrics area bounds (NR==NoteRec?)
  LyricEngine: TLyricEngine;
  LyricEngineDuet: TLyricEngine;
{
  // positions
  if Ini.SingWindow==0 then
    NR.Left = 120
  else
    NR.Left = 20;

  NR.Right = 780;

  NR.Width = NR.Right - NR.Left;
  NR.WMid  = NR.Width / 2;
  NR.Mid   = NR.Left + NR.WMid;

  // draw note-lines

  // to-do : needs fix when party mode works w/ 2 screens
  if (PlayersPlay==1) and (Ini.NoteLines==1) and (ScreenSing.settings.NotesVisible and (1) != 0) then
    SingDrawNoteLines(NR.Left + 10*ScreenX, Skin_P2_NotesB - 105, NR.Right + 10*ScreenX, 15);

  if (PlayersPlay==2) and (Ini.NoteLines==1) then
  {
    if (ScreenSing.settings.NotesVisible and (1 shl 0) != 0) then
      SingDrawNoteLines(Nr.Left + 10*ScreenX, Skin_P1_NotesB - 105, Nr.Right + 10*ScreenX, 15);
    if (ScreenSing.settings.NotesVisible and (1 shl 1) != 0) then
      SingDrawNoteLines(Nr.Left + 10*ScreenX, Skin_P2_NotesB - 105, Nr.Right + 10*ScreenX, 15);
  };

  if (PlayersPlay==4) and (Ini.NoteLines==1) then
  {
    if (ScreenSing.settings.NotesVisible and (1 shl 0) != 0) then
    {
      if (Ini.Screens==1) then
        SingDrawNoteLines(Nr.Left + 10*ScreenX, Skin_P1_NotesB - 105, Nr.Right + 10*ScreenX, 15)
      else
      {
        SingDrawNoteLines(Nr.Left + 10*ScreenX, Skin_P1_NotesB - 105, Nr.Right/2 - 5 + 10*ScreenX, 15);
        SingDrawNoteLines(Nr.Right/2 - 20 + 10*ScreenX + Nr.Left + 10*ScreenX, Skin_P1_NotesB - 105, Nr.Right + 10*ScreenX, 15)
      };
    };

    if (ScreenSing.settings.NotesVisible and (1 shl 1) != 0) then
    {
      if (Ini.Screens==1) then
        SingDrawNoteLines(Nr.Left + 10*ScreenX, Skin_P2_NotesB - 105, Nr.Right + 10*ScreenX, 15)
      else
      {
        SingDrawNoteLines(Nr.Left + 10*ScreenX, Skin_P2_NotesB - 105, Nr.Right/2 - 5 + 10*ScreenX, 15);
        SingDrawNoteLines(Nr.Right/2 - 20 + 10*ScreenX + Nr.Left + 10*ScreenX, Skin_P2_NotesB - 105, Nr.Right + 10*ScreenX, 15)
      };
    };
  };

  if (PlayersPlay==3) and (Ini.NoteLines==1) then {
    if (ScreenSing.settings.NotesVisible and (1 shl 0) != 0) then
      SingDrawNoteLines(Nr.Left + 10*ScreenX, 120, Nr.Right + 10*ScreenX, 12);
    if (ScreenSing.settings.NotesVisible and (1 shl 1) != 0) then
      SingDrawNoteLines(Nr.Left + 10*ScreenX, 245, Nr.Right + 10*ScreenX, 12);
    if (ScreenSing.settings.NotesVisible and (1 shl 2) != 0) then
      SingDrawNoteLines(Nr.Left + 10*ScreenX, 370, Nr.Right + 10*ScreenX, 12);
  };

  if (PlayersPlay==6) and (Ini.NoteLines==1) then {
    if (ScreenSing.settings.NotesVisible and (1 shl 0) != 0) then
    {
      if (Ini.Screens==1) then
        SingDrawNoteLines(Nr.Left + 10*ScreenX, 120, Nr.Right + 10*ScreenX, 12)
      else
      {
        SingDrawNoteLines(Nr.Left + 10*ScreenX, 120, Nr.Right/2 - 5 + 10*ScreenX, 12);
        SingDrawNoteLines(Nr.Right/2 - 20 + 10*ScreenX + Nr.Left + 10*ScreenX, 120, Nr.Right + 10*ScreenX, 12);
      };
    };

    if (ScreenSing.settings.NotesVisible and (1 shl 1) != 0) then
    {
      if (Ini.Screens==1) then
        SingDrawNoteLines(Nr.Left + 10*ScreenX, 245, Nr.Right + 10*ScreenX, 12)
      else
      {
        SingDrawNoteLines(Nr.Left + 10*ScreenX, 245, Nr.Right/2 - 5 + 10*ScreenX, 12);
        SingDrawNoteLines(Nr.Right/2 - 20 + 10*ScreenX + Nr.Left + 10*ScreenX, 245, Nr.Right + 10*ScreenX, 12);
      };
    };

    if (ScreenSing.settings.NotesVisible and (1 shl 2) != 0) then
    {
      if (Ini.Screens==1) then
        SingDrawNoteLines(Nr.Left + 10*ScreenX, 370, Nr.Right + 10*ScreenX, 12)
      else
      {
        SingDrawNoteLines(Nr.Left + 10*ScreenX, 370, Nr.Right/2 - 5 + 10*ScreenX, 12);
        SingDrawNoteLines(Nr.Right/2 - 20 + 10*ScreenX + Nr.Left + 10*ScreenX, 370, Nr.Right + 10*ScreenX, 12);
      };
    };
  };
};

void SingDraw;
var
  NR: TRecR;         // lyrics area bounds (NR==NoteRec?)
  LyricEngine: TLyricEngine;
  LyricEngineDuetP1: TLyricEngine;
  LyricEngineDuetP2: TLyricEngine;
  I: int;
  Difficulty: int;
{
  // positions
  if Ini.SingWindow==0 then
    NR.Left = 120
  else
    NR.Left = 20;

  NR.Right = 780;

  NR.Width = NR.Right - NR.Left;
  NR.WMid  = NR.Width / 2;
  NR.Mid   = NR.Left + NR.WMid;

  // FIXME: accessing ScreenSing is not that generic
  if (CurrentSong.isDuet) and (PlayersPlay != 1) then
  {
    LyricEngineDuetP1 = ScreenSing.LyricsDuetP1;
    LyricEngineDuetP2 = ScreenSing.LyricsDuetP2;
  }
  else
    LyricEngine = ScreenSing.Lyrics;

  // draw time-bar
  if (ScreenSing.Settings.TimeBarVisible) then
    SingDrawTimeBar();

  // draw lyrics
  if (ScreenSing.Settings.LyricsVisible) then
  {
    if (CurrentSong.isDuet) and (PlayersPlay != 1) then
    {
      LyricEngineDuetP1.Draw(LyricsState.MidBeat);
      SingDrawLyricHelper(0, NR.Left, NR.WMid);

      LyricEngineDuetP2.Draw(LyricsState.MidBeat);
      SingDrawLyricHelper(1, NR.Left, NR.WMid);
    }
    else
    {
      LyricEngine.Draw(LyricsState.MidBeat);
      SingDrawLyricHelper(0, NR.Left, NR.WMid);
    };
  };

  // oscilloscope
  if (ScreenSing.Settings.OscilloscopeVisible) then
  {
    if PlayersPlay==1 then
      SingDrawOscilloscope(Theme.Sing.SingP1Oscilloscope.X, Theme.Sing.SingP1Oscilloscope.Y, Theme.Sing.SingP1Oscilloscope.W, Theme.Sing.SingP1Oscilloscope.H, 0);

    if PlayersPlay==2 then
    {
      SingDrawOscilloscope(Theme.Sing.SingP1TwoPOscilloscope.X, Theme.Sing.SingP1TwoPOscilloscope.Y, Theme.Sing.SingP1TwoPOscilloscope.W, Theme.Sing.SingP1TwoPOscilloscope.H, 0);
      SingDrawOscilloscope(Theme.Sing.SingP2ROscilloscope.X, Theme.Sing.SingP2ROscilloscope.Y, Theme.Sing.SingP2ROscilloscope.W, Theme.Sing.SingP2ROscilloscope.H, 1);
    };

    if PlayersPlay==3 then
    {
      if (CurrentSong.isDuet) then
      {
        SingDrawOscilloscope(Theme.Sing.SingDuetP1ThreePOscilloscope.X, Theme.Sing.SingDuetP1ThreePOscilloscope.Y, Theme.Sing.SingDuetP1ThreePOscilloscope.W, Theme.Sing.SingDuetP1ThreePOscilloscope.H, 0);
        SingDrawOscilloscope(Theme.Sing.SingDuetP2MOscilloscope.X, Theme.Sing.SingDuetP2MOscilloscope.Y, Theme.Sing.SingDuetP2MOscilloscope.W, Theme.Sing.SingDuetP2MOscilloscope.H, 1);
        SingDrawOscilloscope(Theme.Sing.SingDuetP3ROscilloscope.X, Theme.Sing.SingDuetP3ROscilloscope.Y, Theme.Sing.SingDuetP3ROscilloscope.W, Theme.Sing.SingDuetP3ROscilloscope.H, 2);
      }
      else
      {
        SingDrawOscilloscope(Theme.Sing.SingP1ThreePOscilloscope.X, Theme.Sing.SingP1ThreePOscilloscope.Y, Theme.Sing.SingP1ThreePOscilloscope.W, Theme.Sing.SingP1ThreePOscilloscope.H, 0);
        SingDrawOscilloscope(Theme.Sing.SingP2MOscilloscope.X, Theme.Sing.SingP2MOscilloscope.Y, Theme.Sing.SingP2MOscilloscope.W, Theme.Sing.SingP2MOscilloscope.H, 1);
        SingDrawOscilloscope(Theme.Sing.SingP3ROscilloscope.X, Theme.Sing.SingP3ROscilloscope.Y, Theme.Sing.SingP3ROscilloscope.W, Theme.Sing.SingP3ROscilloscope.H, 2);
      };
    };

    if PlayersPlay==4 then
    {
      if (Ini.Screens==1) then
      {
        if ScreenAct==1 then
        {
          SingDrawOscilloscope(Theme.Sing.SingP1TwoPOscilloscope.X, Theme.Sing.SingP1TwoPOscilloscope.Y, Theme.Sing.SingP1TwoPOscilloscope.W, Theme.Sing.SingP1TwoPOscilloscope.H, 0);
          SingDrawOscilloscope(Theme.Sing.SingP2ROscilloscope.X, Theme.Sing.SingP2ROscilloscope.Y, Theme.Sing.SingP2ROscilloscope.W, Theme.Sing.SingP2ROscilloscope.H, 1);
        };
        if ScreenAct==2 then
        {
          SingDrawOscilloscope(Theme.Sing.SingP1TwoPOscilloscope.X, Theme.Sing.SingP1TwoPOscilloscope.Y, Theme.Sing.SingP1TwoPOscilloscope.W, Theme.Sing.SingP1TwoPOscilloscope.H, 2);
          SingDrawOscilloscope(Theme.Sing.SingP2ROscilloscope.X, Theme.Sing.SingP2ROscilloscope.Y, Theme.Sing.SingP2ROscilloscope.W, Theme.Sing.SingP2ROscilloscope.H, 3);
        };
      }
      else
      {
        if (CurrentSong.isDuet) then
        {
          SingDrawOscilloscope(Theme.Sing.SingP1DuetFourPOscilloscope.X, Theme.Sing.SingP1DuetFourPOscilloscope.Y, Theme.Sing.SingP1DuetFourPOscilloscope.W, Theme.Sing.SingP1DuetFourPOscilloscope.H, 0);
          SingDrawOscilloscope(Theme.Sing.SingP2DuetFourPOscilloscope.X, Theme.Sing.SingP2DuetFourPOscilloscope.Y, Theme.Sing.SingP2DuetFourPOscilloscope.W, Theme.Sing.SingP2DuetFourPOscilloscope.H, 1);
          SingDrawOscilloscope(Theme.Sing.SingP3DuetFourPOscilloscope.X, Theme.Sing.SingP3DuetFourPOscilloscope.Y, Theme.Sing.SingP3DuetFourPOscilloscope.W, Theme.Sing.SingP3DuetFourPOscilloscope.H, 2);
          SingDrawOscilloscope(Theme.Sing.SingP4DuetFourPOscilloscope.X, Theme.Sing.SingP4DuetFourPOscilloscope.Y, Theme.Sing.SingP4DuetFourPOscilloscope.W, Theme.Sing.SingP4DuetFourPOscilloscope.H, 3);
        }
        else
        {
          SingDrawOscilloscope(Theme.Sing.SingP1FourPOscilloscope.X, Theme.Sing.SingP1FourPOscilloscope.Y, Theme.Sing.SingP1FourPOscilloscope.W, Theme.Sing.SingP1FourPOscilloscope.H, 0);
          SingDrawOscilloscope(Theme.Sing.SingP2FourPOscilloscope.X, Theme.Sing.SingP2FourPOscilloscope.Y, Theme.Sing.SingP2FourPOscilloscope.W, Theme.Sing.SingP2FourPOscilloscope.H, 1);
          SingDrawOscilloscope(Theme.Sing.SingP3FourPOscilloscope.X, Theme.Sing.SingP3FourPOscilloscope.Y, Theme.Sing.SingP3FourPOscilloscope.W, Theme.Sing.SingP3FourPOscilloscope.H, 2);
          SingDrawOscilloscope(Theme.Sing.SingP4FourPOscilloscope.X, Theme.Sing.SingP4FourPOscilloscope.Y, Theme.Sing.SingP4FourPOscilloscope.W, Theme.Sing.SingP4FourPOscilloscope.H, 3);
        };
      };
    };

    if PlayersPlay==6 then
    {
      if (Ini.Screens==1) then
      {
        if (CurrentSong.isDuet) then
        {
          if ScreenAct==1 then
          {
            SingDrawOscilloscope(Theme.Sing.SingDuetP1ThreePOscilloscope.X, Theme.Sing.SingDuetP1ThreePOscilloscope.Y, Theme.Sing.SingDuetP1ThreePOscilloscope.W, Theme.Sing.SingDuetP1ThreePOscilloscope.H, 0);
            SingDrawOscilloscope(Theme.Sing.SingDuetP2MOscilloscope.X, Theme.Sing.SingDuetP2MOscilloscope.Y, Theme.Sing.SingDuetP2MOscilloscope.W, Theme.Sing.SingDuetP2MOscilloscope.H, 1);
            SingDrawOscilloscope(Theme.Sing.SingDuetP3ROscilloscope.X, Theme.Sing.SingDuetP3ROscilloscope.Y, Theme.Sing.SingDuetP3ROscilloscope.W, Theme.Sing.SingDuetP3ROscilloscope.H, 2);
          };
          if ScreenAct==2 then
          {
            SingDrawOscilloscope(Theme.Sing.SingDuetP1ThreePOscilloscope.X, Theme.Sing.SingDuetP1ThreePOscilloscope.Y, Theme.Sing.SingDuetP1ThreePOscilloscope.W, Theme.Sing.SingDuetP1ThreePOscilloscope.H, 3);
            SingDrawOscilloscope(Theme.Sing.SingDuetP2MOscilloscope.X, Theme.Sing.SingDuetP2MOscilloscope.Y, Theme.Sing.SingDuetP2MOscilloscope.W, Theme.Sing.SingDuetP2MOscilloscope.H, 4);
            SingDrawOscilloscope(Theme.Sing.SingDuetP3ROscilloscope.X, Theme.Sing.SingDuetP3ROscilloscope.Y, Theme.Sing.SingDuetP3ROscilloscope.W, Theme.Sing.SingDuetP3ROscilloscope.H, 5);
          };
        }
        else
        {
          if ScreenAct==1 then
          {
            SingDrawOscilloscope(Theme.Sing.SingP1ThreePOscilloscope.X, Theme.Sing.SingP1ThreePOscilloscope.Y, Theme.Sing.SingP1ThreePOscilloscope.W, Theme.Sing.SingP1ThreePOscilloscope.H, 0);
            SingDrawOscilloscope(Theme.Sing.SingP2MOscilloscope.X, Theme.Sing.SingP2MOscilloscope.Y, Theme.Sing.SingP2MOscilloscope.W, Theme.Sing.SingP2MOscilloscope.H, 1);
            SingDrawOscilloscope(Theme.Sing.SingP3ROscilloscope.X, Theme.Sing.SingP3ROscilloscope.Y, Theme.Sing.SingP3ROscilloscope.W, Theme.Sing.SingP3ROscilloscope.H, 2);
          };

          if ScreenAct==2 then
          {
            SingDrawOscilloscope(Theme.Sing.SingP1ThreePOscilloscope.X, Theme.Sing.SingP1ThreePOscilloscope.Y, Theme.Sing.SingP1ThreePOscilloscope.W, Theme.Sing.SingP1ThreePOscilloscope.H, 3);
            SingDrawOscilloscope(Theme.Sing.SingP2MOscilloscope.X, Theme.Sing.SingP2MOscilloscope.Y, Theme.Sing.SingP2MOscilloscope.W, Theme.Sing.SingP2MOscilloscope.H, 4);
            SingDrawOscilloscope(Theme.Sing.SingP3ROscilloscope.X, Theme.Sing.SingP3ROscilloscope.Y, Theme.Sing.SingP3ROscilloscope.W, Theme.Sing.SingP3ROscilloscope.H, 5);
          };
        };
      }
      else
      {
        if (CurrentSong.isDuet) then
        {
          SingDrawOscilloscope(Theme.Sing.SingP1DuetSixPOscilloscope.X, Theme.Sing.SingP1DuetSixPOscilloscope.Y, Theme.Sing.SingP1DuetSixPOscilloscope.W, Theme.Sing.SingP1DuetSixPOscilloscope.H, 0);
          SingDrawOscilloscope(Theme.Sing.SingP2DuetSixPOscilloscope.X, Theme.Sing.SingP2DuetSixPOscilloscope.Y, Theme.Sing.SingP2DuetSixPOscilloscope.W, Theme.Sing.SingP2DuetSixPOscilloscope.H, 1);
          SingDrawOscilloscope(Theme.Sing.SingP3DuetSixPOscilloscope.X, Theme.Sing.SingP3DuetSixPOscilloscope.Y, Theme.Sing.SingP3DuetSixPOscilloscope.W, Theme.Sing.SingP3DuetSixPOscilloscope.H, 2);
          SingDrawOscilloscope(Theme.Sing.SingP4DuetSixPOscilloscope.X, Theme.Sing.SingP4DuetSixPOscilloscope.Y, Theme.Sing.SingP4DuetSixPOscilloscope.W, Theme.Sing.SingP4DuetSixPOscilloscope.H, 3);
          SingDrawOscilloscope(Theme.Sing.SingP5DuetSixPOscilloscope.X, Theme.Sing.SingP5DuetSixPOscilloscope.Y, Theme.Sing.SingP5DuetSixPOscilloscope.W, Theme.Sing.SingP5DuetSixPOscilloscope.H, 4);
          SingDrawOscilloscope(Theme.Sing.SingP6DuetSixPOscilloscope.X, Theme.Sing.SingP6DuetSixPOscilloscope.Y, Theme.Sing.SingP6DuetSixPOscilloscope.W, Theme.Sing.SingP6DuetSixPOscilloscope.H, 5);
        }
        else
        {
          SingDrawOscilloscope(Theme.Sing.SingP1SixPOscilloscope.X, Theme.Sing.SingP1SixPOscilloscope.Y, Theme.Sing.SingP1SixPOscilloscope.W, Theme.Sing.SingP1SixPOscilloscope.H, 0);
          SingDrawOscilloscope(Theme.Sing.SingP2SixPOscilloscope.X, Theme.Sing.SingP2SixPOscilloscope.Y, Theme.Sing.SingP2SixPOscilloscope.W, Theme.Sing.SingP2SixPOscilloscope.H, 1);
          SingDrawOscilloscope(Theme.Sing.SingP3SixPOscilloscope.X, Theme.Sing.SingP3SixPOscilloscope.Y, Theme.Sing.SingP3SixPOscilloscope.W, Theme.Sing.SingP3SixPOscilloscope.H, 2);
          SingDrawOscilloscope(Theme.Sing.SingP4SixPOscilloscope.X, Theme.Sing.SingP4SixPOscilloscope.Y, Theme.Sing.SingP4SixPOscilloscope.W, Theme.Sing.SingP4SixPOscilloscope.H, 3);
          SingDrawOscilloscope(Theme.Sing.SingP5SixPOscilloscope.X, Theme.Sing.SingP5SixPOscilloscope.Y, Theme.Sing.SingP5SixPOscilloscope.W, Theme.Sing.SingP5SixPOscilloscope.H, 4);
          SingDrawOscilloscope(Theme.Sing.SingP6SixPOscilloscope.X, Theme.Sing.SingP6SixPOscilloscope.Y, Theme.Sing.SingP6SixPOscilloscope.W, Theme.Sing.SingP6SixPOscilloscope.H, 5);
        };
      };
    };
  };

  for I = 1 to PlayersPlay do
  {

    if (ScreenSong.Mode==smNormal) then
      Difficulty = Ini.PlayerLevel[I - 1]
    else
      Difficulty = Ini.Difficulty;

    case Difficulty of
      0:
        {
          NotesH[I - 1] = 11; // 9
          NotesW[I - 1] = 6; // 5
        };
      1:
        {
          NotesH[I - 1] = 8; // 7
          NotesW[I - 1] = 4; // 4
        };
      2:
        {
          NotesH[I - 1] = 5;
          NotesW[I - 1] = 3;
        };
    };

    if PlayersPlay==3 then
    {
      NotesW[I - 1] = NotesW[I - 1] * 0.8;
      NotesH[I - 1] = NotesH[I - 1] * 0.8;
    };

    if PlayersPlay==4 then
    {
      if (Ini.Screens==0) then
      {
        NotesW[I - 1] = NotesW[I - 1] * 0.9;
      };
    };

    if PlayersPlay==6 then
    {
      NotesW[I - 1] = NotesW[I - 1] * 0.8;
      NotesH[I - 1] = NotesH[I - 1] * 0.8;
    };

  };

  // Draw the Notes
  if PlayersPlay==1 then
  {
    SingDrawPlayerBGLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 0, 15);  // Background glow    - colorized in playercolor
    SingDrawLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 1, 15);             // Plain unsung notes - colorized in playercolor
    SingDrawPlayerLine(NR.Left + 20, Skin_P2_NotesB, NR.Width - 40, 0, 0, 15);       // imho the sung notes
  };

  if PlayersPlay==2 then
  {
    if (CurrentSong.isDuet) then
    {
      SingDrawPlayerBGLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 0, 15);
      SingDrawPlayerBGLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 1, 1, 15);

      SingDrawLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 1, 15);
      SingDrawLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 1, 2, 15);

      SingDrawPlayerLine(NR.Left + 20, Skin_P1_NotesB, NR.Width - 40, 0, 0, 15);
      SingDrawPlayerLine(NR.Left + 20, Skin_P2_NotesB, NR.Width - 40, 1, 1, 15);
    }
    else
    {
      SingDrawPlayerBGLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 0, 15);
      SingDrawPlayerBGLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 1, 15);

      SingDrawLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 1, 15);
      SingDrawLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 2, 15);

      SingDrawPlayerLine(NR.Left + 20, Skin_P1_NotesB, NR.Width - 40, 0, 0, 15);
      SingDrawPlayerLine(NR.Left + 20, Skin_P2_NotesB, NR.Width - 40, 0, 1, 15);
    };
  };

  if PlayersPlay==3 then
  {
    if (CurrentSong.isDuet) then
    {
      SingDrawPlayerBGLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 0, 12);
      SingDrawPlayerBGLine(NR.Left + 20, 245+95, NR.Right - 20, 1, 1, 12);
      SingDrawPlayerBGLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 2, 12);

      SingDrawLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 1, 12);
      SingDrawLine(NR.Left + 20, 245+95, NR.Right - 20, 1, 2, 12);
      SingDrawLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 3, 12);

      SingDrawPlayerLine(NR.Left + 20, 120+95, NR.Width - 40, 0, 0, 12);
      SingDrawPlayerLine(NR.Left + 20, 245+95, NR.Width - 40, 1, 1, 12);
      SingDrawPlayerLine(NR.Left + 20, 370+95, NR.Width - 40, 0, 2, 12);
    }
    else
    {
      SingDrawPlayerBGLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 0, 12);
      SingDrawPlayerBGLine(NR.Left + 20, 245+95, NR.Right - 20, 0, 1, 12);
      SingDrawPlayerBGLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 2, 12);

      SingDrawLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 1, 12);
      SingDrawLine(NR.Left + 20, 245+95, NR.Right - 20, 0, 2, 12);
      SingDrawLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 3, 12);

      SingDrawPlayerLine(NR.Left + 20, 120+95, NR.Width - 40, 0, 0, 12);
      SingDrawPlayerLine(NR.Left + 20, 245+95, NR.Width - 40, 0, 1, 12);
      SingDrawPlayerLine(NR.Left + 20, 370+95, NR.Width - 40, 0, 2, 12);
    };
  };

  if PlayersPlay==4 then
  {

    if ScreenAct==1 then
    {
      if not(CurrentSong.isDuet) then
      {
        if (Ini.Screens==1) then
        {
          SingDrawPlayerBGLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 0, 15);
          SingDrawPlayerBGLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 1, 15);
        }
        else
        {
          SingDrawPlayerBGLine(NR.Left + 20, Skin_P1_NotesB, NR.Right/2 - 20, 0, 0, 15);
          SingDrawPlayerBGLine(NR.Left + 20, Skin_P2_NotesB, NR.Right/2 - 20, 0, 1, 15);
          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 2, 15);
          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 3, 15);
        };
      }
      else
      {
        if (Ini.Screens==1) then
        {
          SingDrawPlayerBGLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 0, 15);
          SingDrawPlayerBGLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 1, 1, 15);
        }
        else
        {
          SingDrawPlayerBGLine(NR.Left + 20, Skin_P1_NotesB, NR.Right/2 - 20, 0, 0, 15);
          SingDrawPlayerBGLine(NR.Left + 20, Skin_P2_NotesB, NR.Right/2 - 20, 1, 1, 15);
          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 2, 15);
          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 1, 3, 15);
        };
      };
    };

    if ScreenAct==2 then
    {
      if not(CurrentSong.isDuet) then
      {
        SingDrawPlayerBGLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 2, 15);
        SingDrawPlayerBGLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 3, 15);
      }
      else
      {
        SingDrawPlayerBGLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 2, 15);
        SingDrawPlayerBGLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 1, 3, 15);
      };
    };

    if ScreenAct==1 then
    {
      if not(CurrentSong.isDuet) then
      {
        if (Ini.Screens==1) then
        {
          SingDrawLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 1, 15);
          SingDrawLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 2, 15);
        }
        else
        {
          SingDrawLine(NR.Left + 20, Skin_P1_NotesB, NR.Right/2 - 20, 0, 1, 15);
          SingDrawLine(NR.Left + 20, Skin_P2_NotesB, NR.Right/2 - 20, 0, 2, 15);
          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 3, 15);
          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 4, 15);
        };
      }
      else
      {
        if (Ini.Screens==1) then
        {
          SingDrawLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 1, 15);
          SingDrawLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 1, 2, 15);
        }
        else
        {
          SingDrawLine(NR.Left + 20, Skin_P1_NotesB, NR.Right/2 - 20, 0, 1, 15);
          SingDrawLine(NR.Left + 20, Skin_P2_NotesB, NR.Right/2 - 20, 1, 2, 15);
          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 3, 15);
          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 1, 4, 15);
        };
      };
    };

    if ScreenAct==2 then
    {
      if not(CurrentSong.isDuet) then
      {
        SingDrawLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 3, 15);
        SingDrawLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 0, 4, 15);
      }
      else
      {
        SingDrawLine(NR.Left + 20, Skin_P1_NotesB, NR.Right - 20, 0, 3, 15);
        SingDrawLine(NR.Left + 20, Skin_P2_NotesB, NR.Right - 20, 1, 4, 15);
      };
    };

    if ScreenAct==1 then
    {
      if not(CurrentSong.isDuet) then
      {
        if (Ini.Screens==1) then
        {
          SingDrawPlayerLine(NR.Left + 20, Skin_P1_NotesB, NR.Width - 40, 0, 0, 15);
          SingDrawPlayerLine(NR.Left + 20, Skin_P2_NotesB, NR.Width - 40, 0, 1, 15);
        }
        else
        {
          SingDrawPlayerLine(NR.Left + 20, Skin_P1_NotesB, NR.Width/2 - 50, 0, 0, 15);
          SingDrawPlayerLine(NR.Left + 20, Skin_P2_NotesB, NR.Width/2 - 50, 0, 1, 15);
          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, Skin_P1_NotesB, NR.Width/2 - 30, 0, 2, 15);
          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, Skin_P2_NotesB, NR.Width/2 - 30, 0, 3, 15);
        };
      }
      else
      {
        if (Ini.Screens==1) then
        {
          SingDrawPlayerLine(NR.Left + 20, Skin_P1_NotesB, NR.Width - 40, 0, 0, 15);
          SingDrawPlayerLine(NR.Left + 20, Skin_P2_NotesB, NR.Width - 40, 1, 1, 15);
        }
        else
        {
          SingDrawPlayerLine(NR.Left + 20, Skin_P1_NotesB, NR.Width/2 - 50, 0, 0, 15);
          SingDrawPlayerLine(NR.Left + 20, Skin_P2_NotesB, NR.Width/2 - 50, 1, 1, 15);
          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, Skin_P1_NotesB, NR.Width/2 - 30, 0, 2, 15);
          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, Skin_P2_NotesB, NR.Width/2 - 30, 1, 3, 15);
        };
      };
    };

    if ScreenAct==2 then
    {
      if not(CurrentSong.isDuet) then
      {
        SingDrawPlayerLine(NR.Left + 20, Skin_P1_NotesB, NR.Width - 40, 0, 2, 15);
        SingDrawPlayerLine(NR.Left + 20, Skin_P2_NotesB, NR.Width - 40, 0, 3, 15);
      }
      else
      {
        SingDrawPlayerLine(NR.Left + 20, Skin_P1_NotesB, NR.Width - 40, 0, 2, 15);
        SingDrawPlayerLine(NR.Left + 20, Skin_P2_NotesB, NR.Width - 40, 1, 3, 15);
      };
    };
  };

  if PlayersPlay==6 then
  {
    if ScreenAct==1 then
    {
      if not(CurrentSong.isDuet) then
      {
        if (Ini.Screens==1) then
        {
          SingDrawPlayerBGLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 0, 12);
          SingDrawPlayerBGLine(NR.Left + 20, 245+95, NR.Right - 20, 0, 1, 12);
          SingDrawPlayerBGLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 2, 12);
        }
        else
        {
          SingDrawPlayerBGLine(NR.Left + 20, 120+95, NR.Right/2 - 20, 0, 0, 12);
          SingDrawPlayerBGLine(NR.Left + 20, 245+95, NR.Right/2 - 20, 0, 1, 12);
          SingDrawPlayerBGLine(NR.Left + 20, 370+95, NR.Right/2 - 20, 0, 2, 12);

          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, 120+95, NR.Right - 20, 0, 3, 12);
          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, 245+95, NR.Right - 20, 0, 4, 12);
          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, 370+95, NR.Right - 20, 0, 5, 12);
        };
      }
      else
      {
        if (Ini.Screens==1) then
        {
          SingDrawPlayerBGLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 0, 12);
          SingDrawPlayerBGLine(NR.Left + 20, 245+95, NR.Right - 20, 1, 1, 12);
          SingDrawPlayerBGLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 2, 12);
        }
        else
        {
          SingDrawPlayerBGLine(NR.Left + 20, 120+95, NR.Right/2 - 20, 0, 0, 12);
          SingDrawPlayerBGLine(NR.Left + 20, 245+95, NR.Right/2 - 20, 1, 1, 12);
          SingDrawPlayerBGLine(NR.Left + 20, 370+95, NR.Right/2 - 20, 0, 2, 12);

          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, 120+95, NR.Right - 20, 1, 3, 12);
          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, 245+95, NR.Right - 20, 0, 4, 12);
          SingDrawPlayerBGLine(NR.Right/2 - 20 + NR.Left + 20, 370+95, NR.Right - 20, 1, 5, 12);
        };
      };
    };

    if ScreenAct==2 then
    {
      if not(CurrentSong.isDuet) then
      {
        SingDrawPlayerBGLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 3, 12);
        SingDrawPlayerBGLine(NR.Left + 20, 245+95, NR.Right - 20, 0, 4, 12);
        SingDrawPlayerBGLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 5, 12);
      }
      else
      {
        SingDrawPlayerBGLine(NR.Left + 20, 120+95, NR.Right - 20, 1, 3, 12);
        SingDrawPlayerBGLine(NR.Left + 20, 245+95, NR.Right - 20, 0, 4, 12);
        SingDrawPlayerBGLine(NR.Left + 20, 370+95, NR.Right - 20, 1, 5, 12);
      };
    };

    if ScreenAct==1 then
    {
      if not(CurrentSong.isDuet) then
      {
        if (Ini.Screens==1) then
        {
          SingDrawLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 1, 12);
          SingDrawLine(NR.Left + 20, 245+95, NR.Right - 20, 0, 2, 12);
          SingDrawLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 3, 12);
        }
        else
        {
          SingDrawLine(NR.Left + 20, 120+95, NR.Right/2 - 20, 0, 1, 12);
          SingDrawLine(NR.Left + 20, 245+95, NR.Right/2 - 20, 0, 2, 12);
          SingDrawLine(NR.Left + 20, 370+95, NR.Right/2 - 20, 0, 3, 12);

          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, 120+95, NR.Right - 20, 0, 4, 12);
          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, 245+95, NR.Right - 20, 0, 5, 12);
          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, 370+95, NR.Right - 20, 0, 6, 12);
        };
      }
      else
      {
        if (Ini.Screens==1) then
        {
          SingDrawLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 1, 12);
          SingDrawLine(NR.Left + 20, 245+95, NR.Right - 20, 1, 2, 12);
          SingDrawLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 3, 12);
        }
        else
        {
          SingDrawLine(NR.Left + 20, 120+95, NR.Right/2 - 20, 0, 1, 12);
          SingDrawLine(NR.Left + 20, 245+95, NR.Right/2 - 20, 1, 2, 12);
          SingDrawLine(NR.Left + 20, 370+95, NR.Right/2 - 20, 0, 3, 12);

          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, 120+95, NR.Right - 20, 1, 4, 12);
          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, 245+95, NR.Right - 20, 0, 5, 12);
          SingDrawLine(NR.Right/2 - 20 + NR.Left + 20, 370+95, NR.Right - 20, 1, 6, 12);
        };
      };
    };

    if ScreenAct==2 then
    {
      if not(CurrentSong.isDuet) then
      {
        SingDrawLine(NR.Left + 20, 120+95, NR.Right - 20, 0, 4, 12);
        SingDrawLine(NR.Left + 20, 245+95, NR.Right - 20, 0, 5, 12);
        SingDrawLine(NR.Left + 20, 370+95, NR.Right - 20, 0, 6, 12);
      }
      else
      {
        SingDrawLine(NR.Left + 20, 120+95, NR.Right - 20, 1, 4, 12);
        SingDrawLine(NR.Left + 20, 245+95, NR.Right - 20, 0, 5, 12);
        SingDrawLine(NR.Left + 20, 370+95, NR.Right - 20, 1, 6, 12);
      };
    };

    if ScreenAct==1 then
    {
      if not(CurrentSong.isDuet) then
      {
        if (Ini.Screens==1) then
        {
          SingDrawPlayerLine(NR.Left + 20, 120+95, NR.Width - 40, 0, 0, 12);
          SingDrawPlayerLine(NR.Left + 20, 245+95, NR.Width - 40, 0, 1, 12);
          SingDrawPlayerLine(NR.Left + 20, 370+95, NR.Width - 40, 0, 2, 12);
        }
        else
        {
          SingDrawPlayerLine(NR.Left + 20, 120+95, NR.Width/2 - 50, 0, 0, 12);
          SingDrawPlayerLine(NR.Left + 20, 245+95, NR.Width/2 - 50, 0, 1, 12);
          SingDrawPlayerLine(NR.Left + 20, 370+95, NR.Width/2 - 50, 0, 2, 12);

          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, 120+95, NR.Width/2 - 30, 0, 3, 12);
          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, 245+95, NR.Width/2 - 30, 0, 4, 12);
          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, 370+95, NR.Width/2 - 30, 0, 5, 12);
        };
      }
      else
      {
        if (Ini.Screens==1) then
        {
          SingDrawPlayerLine(NR.Left + 20, 120+95, NR.Width - 40, 0, 0, 12);
          SingDrawPlayerLine(NR.Left + 20, 245+95, NR.Width - 40, 1, 1, 12);
          SingDrawPlayerLine(NR.Left + 20, 370+95, NR.Width - 40, 0, 2, 12);
        }
        else
        {
          SingDrawPlayerLine(NR.Left + 20, 120+95, NR.Width/2 - 50, 0, 0, 12);
          SingDrawPlayerLine(NR.Left + 20, 245+95, NR.Width/2 - 50, 1, 1, 12);
          SingDrawPlayerLine(NR.Left + 20, 370+95, NR.Width/2 - 50, 0, 2, 12);

          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, 120+95, NR.Width/2 - 30, 1, 3, 12);
          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, 245+95, NR.Width/2 - 30, 0, 4, 12);
          SingDrawPlayerLine(NR.Width/2 - 10 + NR.Left + 20, 370+95, NR.Width/2 - 30, 1, 5, 12);
        };
      };
    };

    if ScreenAct==2 then
    {
      if not(CurrentSong.isDuet) then
      {
        SingDrawPlayerLine(NR.Left + 20, 120+95, NR.Width - 40, 0, 3, 12);
        SingDrawPlayerLine(NR.Left + 20, 245+95, NR.Width - 40, 0, 4, 12);
        SingDrawPlayerLine(NR.Left + 20, 370+95, NR.Width - 40, 0, 5, 12);
      }
      else
      {
        SingDrawPlayerLine(NR.Left + 20, 120+95, NR.Width - 40, 1, 3, 12);
        SingDrawPlayerLine(NR.Left + 20, 245+95, NR.Width - 40, 0, 4, 12);
        SingDrawPlayerLine(NR.Left + 20, 370+95, NR.Width - 40, 1, 5, 12);
      };
    };
  };
  glDisable(GL_BL});
  glDisable(GL_TEXTURE_2D);
};

void SingDrawJukebox;
var
  NR: TRecR;         // lyrics area bounds (NR==NoteRec?)
  LyricEngine: TLyricEngine;
{
  // positions
  if Ini.SingWindow==0 then
    NR.Left = 120
  else
    NR.Left = 20;

  NR.Right = 780;

  NR.Width = NR.Right - NR.Left;
  NR.WMid  = NR.Width / 2;
  NR.Mid   = NR.Left + NR.WMid;

  // FIXME: accessing ScreenJukebox is not that generic
  LyricEngine = ScreenJukebox.Lyrics;

  // draw Lyrics
  if (ScreenJukebox.ShowLyrics) then
  {
    if (ScreenJukebox.LyricsStart) or ((not(ScreenJukebox.LyricsStart) and (LyricsState.GetCurrentTime() * 1000 >= LyricsState.StartTime - 3000))) then
    {
        LyricEngine.Draw(LyricsState.MidBeat);
        SingDrawLyricHelperJukebox(NR.Left, NR.WMid);
        ScreenJukebox.LyricsStart = true;
    };
  };

  glDisable(GL_BL});
  glDisable(GL_TEXTURE_2D);
};

// Draw Note Bars for Editor
// There are 11 reasons for a new void:   (nice binary :D )
// 1. It does not look good when you draw the golden note star effect in the editor
// 2. You can see the freestyle notes in the editor semitransparent
// 3. It is easier and faster then changing the old void
void EditDrawLine(X, YBaseNote, W, H: double; Track: int; NumLines: int);
var
  Rec:   TRecR;
  Count: int;
  TempR: double;
  GoldenStarPos: double;
  Space: double;
{
  Space = H / (NumLines - 1);
  glColor3f(1, 1, 1);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BL});
  glBl}Func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if not Tracks[Track].Lines[Tracks[Track].CurrentLine].HasLength(TempR) then TempR = 0
  else TempR = W / TempR;

  with Tracks[Track].Lines[Tracks[Track].CurrentLine] do
  {
    for Count = 0 to HighNote do
    {
      with Notes[Count] do
      {

        // Golden Note Patch
        case NoteType of
          ntFreestyle: glColor4f(1, 1, 1, 0.35);
          ntNormal: glColor4f(1, 1, 1, 0.85);
          ntGolden: Glcolor4f(1, 1, 0.3, 0.85);
          ntRap: glColor4f(1, 1, 1, 0.85);
          ntRapGolden: Glcolor4f(1, 1, 0.3, 0.85);
        }; // case

        // left part
        Rec.Left  = (StartBeat - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat) * TempR + X + 0.5 + 10*ScreenX;
        Rec.Right = Rec.Left + NotesW[0];
        Rec.Top = YBaseNote - (Tone-BaseNote)*Space/2 - NotesH[0];
        Rec.Bottom = Rec.Top + 2 * NotesH[0];
        If (NoteType==ntRap) or (NoteType==ntRapGolden) then
        {
          glBindTexture(GL_TEXTURE_2D, Tex_Left_Rap[Color].TexNum);
        }
        else
        {
          glBindTexture(GL_TEXTURE_2D, Tex_Left[Color].TexNum);
        };
        gl{(GL_QUADS);
          glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
          glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
          glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
          glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
        gl};
        GoldenStarPos = Rec.Left;

        // middle part
        Rec.Left  = Rec.Right;
        Rec.Right = (StartBeat + Duration - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat) * TempR + X - NotesW[0] - 0.5 + 10*ScreenX;

        If (NoteType==ntRap) or (NoteType==ntRapGolden) then
        {
          glBindTexture(GL_TEXTURE_2D, Tex_Mid_Rap[Color].TexNum);
        }
        else
        {
          glBindTexture(GL_TEXTURE_2D, Tex_Mid[Color].TexNum);
        };
        gl{(GL_QUADS);
          glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
          glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
          glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
          glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
        gl};

        // right part
        Rec.Left  = Rec.Right;
        Rec.Right = Rec.Right + NotesW[0];

        If (NoteType==ntRap) or (NoteType==ntRapGolden) then
        {
          glBindTexture(GL_TEXTURE_2D, Tex_Right_Rap[Color].TexNum);
        }
        else
        {
          glBindTexture(GL_TEXTURE_2D, Tex_Right[Color].TexNum);
        };
        gl{(GL_QUADS);
          glTexCoord2f(0, 0); glVertex2f(Rec.Left,  Rec.Top);
          glTexCoord2f(0, 1); glVertex2f(Rec.Left,  Rec.Bottom);
          glTexCoord2f(1, 1); glVertex2f(Rec.Right, Rec.Bottom);
          glTexCoord2f(1, 0); glVertex2f(Rec.Right, Rec.Top);
        gl};

        if ((NoteType==ntGolden) or (NoteType==ntRapGolden)) and (Ini.EffectSing==1) then
        {
          GoldenRec.SaveGoldenStarsRec(GoldenStarPos, Rec.Top, Rec.Right, Rec.Bottom);
        };
        
      }; // with
    }; // for
  }; // with

  glDisable(GL_BL});
  glDisable(GL_TEXTURE_2D);
};

void EditDrawBorderedBox(X, Y, W, H: int; FillR, FillG, FillB, FillAlpha: double);
{
  glDisable(GL_BL});

  // box
  glColor4f(FillR,
            FillG,
            FillB,
            FillAlpha);
  gl{(gl_quads);
    glVertex2f(X, Y);
    glVertex2f(X, Y+H);
    glVertex2f(X+W, Y+H);
    glVertex2f(X+W, Y);
  gl};

  // black border
  glColor4f(0, 0, 0, 1);
  glLineWidth(2);
  gl{(GL_LINE_LOOP);
    glVertex2f(X-1, Y-1);
    glVertex2f(X+W+1, Y-1);
    glVertex2f(X+W+1, Y+H+1);
    glVertex2f(X-1, Y+H+1);
  gl};
};

void EditDrawBeatDelimiters(X, Y, W, H: double; Track: int);
var
  Count: int;
  TempR: double;
{

  if not Tracks[Track].Lines[Tracks[Track].CurrentLine].HasLength(TempR) then TempR = 0
  else TempR = W / TempR;

  if (Tracks[Track].Lines[Tracks[Track].CurrentLine].ScoreValue > 0) and ( W > 0 ) and ( (Tracks[Track].Lines[Tracks[Track].CurrentLine].}Beat - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat) > 0 ) then
      TempR = W / (Tracks[Track].Lines[Tracks[Track].CurrentLine].}Beat - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat)
    else
      TempR = 0;
  glEnable(GL_BL});
  gl{(GL_LINES);
  for Count = Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat to Tracks[Track].Lines[Tracks[Track].CurrentLine].}Beat do
  {
    if (Count mod Tracks[Track].Resolution)==Tracks[Track].NotesGAP then
      glColor4f(0, 0, 0, 1)
    else
      glColor4f(0, 0, 0, 0.3);
    glVertex2f(X + TempR * (Count - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat), Y);
    glVertex2f(X + TempR * (Count - Tracks[Track].Lines[Tracks[Track].CurrentLine].Notes[0].StartBeat), Y + H);
  };
  gl};
  glDisable(GL_BL});
};

void SingDrawTimeBar();
var
  x, y:           double;
  width, height:  double;
  LyricsProgress: double;
  CurLyricsTime:  double;
  TotalTime:      double;

{
  x = Theme.Sing.StaticTimeProgress.x;
  y = Theme.Sing.StaticTimeProgress.y;

  width  = Theme.Sing.StaticTimeProgress.w;
  height = Theme.Sing.StaticTimeProgress.h;

  glColor4f(Theme.Sing.StaticTimeProgress.ColR,
            Theme.Sing.StaticTimeProgress.ColG,
            Theme.Sing.StaticTimeProgress.ColB, 1); //Set Color

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BL});

  glBindTexture(GL_TEXTURE_2D, Tex_TimeProgress.TexNum);

  gl{(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(x, y);

    if ScreenSong.Mode==smMedley then
    {
      CurLyricsTime = LyricsState.GetCurrentTime() - ScreenSing.MedleyStart;
      TotalTime = ScreenSing.Medley} - ScreenSing.MedleyStart;
    }
    else
    {
      CurLyricsTime = LyricsState.GetCurrentTime();
      TotalTime = LyricsState.TotalTime;
    };

    if (CurLyricsTime > 0) and
       (TotalTime > 0) then
    {
      LyricsProgress = CurLyricsTime / TotalTime;
      // avoid that the bar "overflows" for inaccurate song lengths
      if LyricsProgress > 1.0 then
        LyricsProgress = 1.0;
      glTexCoord2f((width * LyricsProgress) / 8, 0);
      glVertex2f(x + width * LyricsProgress, y);

      glTexCoord2f((width * LyricsProgress) / 8, 1);
      glVertex2f(x + width * LyricsProgress, y + height);
    };

    glTexCoord2f(0, 1);
    glVertex2f(x, y + height);
  gl};

 glDisable(GL_TEXTURE_2D);
 glDisable(GL_BL});
 glcolor4f(1, 1, 1, 1);
};

void SingDrawJukeboxTimeBar();
var
  x, y:           double;
  width, height:  double;
  LyricsProgress: double;
  CurLyricsTime:  double;
{

  if (ScreenJukebox.SongListVisible) then
  {
    x = Theme.Jukebox.StaticTimeProgress.x;
    y = Theme.Jukebox.StaticTimeProgress.y;

    width  = Theme.Jukebox.StaticTimeProgress.w;
    height = Theme.Jukebox.StaticTimeProgress.h;

    glColor4f(Theme.Jukebox.StaticTimeProgress.ColR,
              Theme.Jukebox.StaticTimeProgress.ColG,
              Theme.Jukebox.StaticTimeProgress.ColB, 1); //Set Color
  };

  if (ScreenJukebox.SongMenuVisible) then
  {
    x = Theme.Jukebox.StaticSongMenuTimeProgress.x;
    y = Theme.Jukebox.StaticSongMenuTimeProgress.y;

    width  = Theme.Jukebox.StaticSongMenuTimeProgress.w;
    height = Theme.Jukebox.StaticSongMenuTimeProgress.h;

    glColor4f(Theme.Jukebox.StaticSongMenuTimeProgress.ColR,
              Theme.Jukebox.StaticSongMenuTimeProgress.ColG,
              Theme.Jukebox.StaticSongMenuTimeProgress.ColB, 1); //Set Color
  };

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BL});

  glBindTexture(GL_TEXTURE_2D, Tex_JukeboxTimeProgress.TexNum);

  gl{(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(x, y);

    CurLyricsTime = LyricsState.GetCurrentTime();
    if (CurLyricsTime > 0) and
       (LyricsState.TotalTime > 0) then
    {
      LyricsProgress = CurLyricsTime / LyricsState.TotalTime;
      // avoid that the bar "overflows" for inaccurate song lengths
      if (LyricsProgress > 1.0) then
        LyricsProgress = 1.0;
      glTexCoord2f((width * LyricsProgress) / 8, 0);
      glVertex2f(x + width * LyricsProgress, y);

      glTexCoord2f((width * LyricsProgress) / 8, 1);
      glVertex2f(x + width * LyricsProgress, y + height);
    };

    glTexCoord2f(0, 1);
    glVertex2f(x, y + height);
  gl};

 glDisable(GL_TEXTURE_2D);
 glDisable(GL_BL});
 glcolor4f(1, 1, 1, 1);

};

};