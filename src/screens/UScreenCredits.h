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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/screens/UScreenCredits.pas $
 * $Id: UScreenCredits.pas 2582 2010-07-18 11:11:57Z whiteshark0 $
 **/

#include <string>
#include <vector>
#include <array>
#include <functional>
#include <filesystem>

#include <GL/gl.h>
#include <SDL2/SDL.h>

#include "../base/ULog.h"
#include "../base/TextGL.h"
#include "../base/UTexture.h"
#include "../menu/UMenu.h"

namespace UScreenCredits
{

//interface

/*$IFDEF FPC*/
  /*$MODE Delphi*/
/*$ENDIF*/

/*$I switches.inc*/

/*
uses
  UDisplay,
  UFiles,
  UGraphicClasses,
  UMenu,
  UMusic,
  UPath,
  UTexture,
  UThemes,
  dglOpenGL,
  SDL2,
  SDL2_image,
  SysUtils;
*/
/* beat detection constants and types */
const int SubChannelCount = 32;
const int HistoryLength = 44;
const int SamplesPerChannel = (FFTSize / 2) / SubChannelCount;
const int BeatEnergyModifier = 80; // modifies detected energy
                           // higher values equal a more sensitive detection

typedef std::array<float, HistoryLength> TEnergyHistory;
typedef std::array<TEnergyHistory, SubChannelCount> TSubchannelHistory;

//type
  enum class TCreditsStages
  {
    InitialDelay, Intro, MainPart, Outro
  };

  class TScreenCredits : public UMenu::TMenu
  {
    private:

      std::filesystem::path CreditsPath;

      double Credits_X;
      //uint32_t Credits_Time;
      uint32_t CTime_hold;

      UTexture::TTexture::SPtr credits_bg_tex;
      UTexture::TTexture::SPtr credits_bg_ovl;
      //credits_bg_logo:   TTexture;
      std::vector<UTexture::TTexture::SPtr> credits_names;
      UTexture::TTexture::SPtr  intro_layer01;
      UTexture::TTexture::SPtr intro_layer02;
      UTexture::TTexture::SPtr intro_layer03;
      UTexture::TTexture::SPtr intro_layer04;
      UTexture::TTexture::SPtr intro_layer05;
      UTexture::TTexture::SPtr intro_layer06;
      UTexture::TTexture::SPtr intro_layer07;
      UTexture::TTexture::SPtr intro_layer08;
      UTexture::TTexture::SPtr intro_layer09;
      UTexture::TTexture::SPtr outro_bg;
      UTexture::TTexture::SPtr outro_esc;
      UTexture::TTexture::SPtr outro_exd;

      int CurrentScrollStart, CurrentScrollEnd;

      TCreditsStages CRDTS_Stage;

      /* beat detection */
      TSubchannelHistory SubChannelHistory;

      /* mouse movement easter eggs: */
      bool MouseMoved;
      double MouseX, MouseY;

      /* saves last x and y angle for easter egg */
      float LogoAngleX, LogoAngleY;

      void LoadNameTextures();

      /* draw different stages */
      void DrawInitialDelay();

      /* Intro */
      void DrawIntro();
      void DrawLayeredLogo(float Separation, float Scale, float AngleX, float AngleY, float AngleZ);

      /* Main */
      void DrawMain();
      void DrawMainBG();
      void DrawFunkyText();

      void DrawMainFG();

      void DrawNames();
      void DoLogoBling();

      /* Outro */
      void DrawOutro();


      /* beat detection */
      void DetectBeat();

    protected:
      /* beat detection stuff
        protected cause we need this information for "on beat
        effect"*/
      uint32_t LastBeatTime;
      bool BeatDetected;
      std::chrono::system_clock::duration CTime;
      std::chrono::system_clock::time_point init_time;

    public:

      bool Fadeout;
      TScreenCredits();
      bool ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown) override;
      bool ParseMouse(int MouseButton, bool BtnDown, int X, int Y) override;
      void OnShow() override;
      void OnHide() override;
      bool Draw() override;
   };

const std::string Funky_Text =
    "Grandma Deluxe v2 has arrived! Thanks to Corvus5, brian-ch, brunzelchen, canni0, k-m_schindler, whiteshark0, BasisBit and the USDX WP team for the massive work on UltraStar and USDX, Wome for the nice tune you are hearing, " \
    "all the people who put massive effort and work in new songs (do not forget UltraStar w/o songs would be nothing), ppl from " \
    "irc helping us, pouet.net, KakiArts, Sourceforge,..";

/* texture names (loaded from gameshared/resources/credits*/
const std::filesystem::path CRDTS_BG_FILE           = "credits_v5_bg.png";
const std::filesystem::path CRDTS_OVL_FILE          = "credits_v5_overlay.png";
const std::filesystem::path INTRO_L01_FILE          = "intro-l-01.png";
const std::filesystem::path INTRO_L02_FILE          = "intro-l-02.png";
const std::filesystem::path INTRO_L03_FILE          = "intro-l-03.png";
const std::filesystem::path INTRO_L04_FILE          = "intro-l-04.png";
const std::filesystem::path INTRO_L05_FILE          = "intro-l-05.png";
const std::filesystem::path INTRO_L06_FILE          = "intro-l-06.png";
const std::filesystem::path INTRO_L07_FILE          = "intro-l-07.png";
const std::filesystem::path INTRO_L08_FILE          = "intro-l-08.png";
const std::filesystem::path INTRO_L09_FILE          = "intro-l-09.png";
const std::filesystem::path OUTRO_BG_FILE           = "outro-bg.png";
const std::filesystem::path OUTRO_ESC_FILE          = "outro-esc.png";
const std::filesystem::path OUTRO_EXD_FILE          = "outro-exit-dark.png";

/* some timings */
const auto Delay_Before_Start = std::chrono::milliseconds(200);
const auto Intro_Flare_Start = std::chrono::milliseconds(600);
const auto Intro_Zoom_End = std::chrono::milliseconds(1490);
const auto Intro_Stand_End = std::chrono::milliseconds(1550);
const auto Intro_Separation_End = std::chrono::milliseconds(1700);
const auto Intro_FadeToWhite_Start = std::chrono::milliseconds(2610);
const auto Intro_Zoomout_Start = std::chrono::milliseconds(2710);
const auto Main_Start = std::chrono::milliseconds(2710);
const auto Main_OnBeatTwinkle_Start = std::chrono::milliseconds(2800);
const auto Main_Names_Start = std::chrono::milliseconds(3590);
const auto Main_Names_End = std::chrono::milliseconds(28330);
const auto Main_FadeOut_Start = std::chrono::milliseconds(30960);
const auto Tune_End = std::chrono::milliseconds(33660);

/* constants for developer names */
typedef std::function<void(const UTexture::TTexture, double Progress)> TFadeEffect;
struct TCRTZ_Developer
{
  std::string Name;           // developer name for texture loading (names_"devel".png)
  bool Twinkle;       // should there be twinkles on show
  TFadeEffect FadeIn;  // fade in effect
  TFadeEffect Draw;  // effect during draw
  TFadeEffect FadeOut;  // fade out effect
};

/* effects are called with blending, texture and matrix prepared */
void Effect_Draw             (const UTexture::TTexture, double Progress);
void Effect_OnBeatJitter     (const UTexture::TTexture, double Progress);

void Effect_Rotate_Left_Top  (const UTexture::TTexture, double Progress);
void Effect_Rotate_Right_Bot (const UTexture::TTexture, double Progress);
void Effect_ZoomIn_Rotate    (const UTexture::TTexture, double Progress);
void Effect_ZoomOut_Shift    (const UTexture::TTexture, double Progress);
void Effect_Shift_Left       (const UTexture::TTexture, double Progress);
void Effect_Shift_Right_Top  (const UTexture::TTexture, double Progress);
void Effect_Flip_Bot         (const UTexture::TTexture, double Progress);
void Effect_Flip_Right_Top   (const UTexture::TTexture, double Progress);
void Effect_Flip_Right       (const UTexture::TTexture, double Progress);
void Effect_Flip_Right_Bot   (const UTexture::TTexture, double Progress);
void Effect_Rotate_Right_Top (const UTexture::TTexture, double Progress);
void Effect_Shift_Weird      (const UTexture::TTexture, double Progress);
void Effect_Shift_Right_Bot  (const UTexture::TTexture, double Progress);
void Effect_Rotate_Right_Top2(const UTexture::TTexture, double Progress);
void Effect_Flip_Left_Bot    (const UTexture::TTexture, double Progress);
void Effect_Flip_Right_Top2  (const UTexture::TTexture, double Progress);
void Effect_Twinkle_Down     (const UTexture::TTexture, double Progress);

const std::array<TCRTZ_Developer, 11> Developers {
    TCRTZ_Developer{ "alexanders",   true,   &Effect_Rotate_Left_Top,    &Effect_OnBeatJitter, &Effect_Rotate_Right_Bot },
    TCRTZ_Developer{ "blindy",       true,   &Effect_ZoomIn_Rotate,      &Effect_OnBeatJitter, &Effect_ZoomOut_Shift },
    TCRTZ_Developer{ "brunzel",      true,   &Effect_Shift_Left,         &Effect_Draw,         &Effect_Shift_Right_Top },
    TCRTZ_Developer{ "canni",        true,   &Effect_Flip_Bot,           &Effect_Draw,         &Effect_Flip_Right_Top },
    TCRTZ_Developer{ "hennymcc",     true,   &Effect_Flip_Right,         &Effect_OnBeatJitter, &Effect_Flip_Right_Bot },
    TCRTZ_Developer{ "jaybinks",     true,   &Effect_Rotate_Right_Top,   &Effect_OnBeatJitter, &Effect_Shift_Weird },
    TCRTZ_Developer{ "krueger",      true,   &Effect_Shift_Right_Bot,    &Effect_OnBeatJitter, &Effect_Rotate_Right_Top2 },
    TCRTZ_Developer{ "mezzox",       true,   &Effect_Flip_Left_Bot,      &Effect_OnBeatJitter, &Effect_Flip_Right_Top },
    TCRTZ_Developer{ "mischi",       true,   &Effect_Shift_Weird,        &Effect_OnBeatJitter, &Effect_Flip_Bot },
    TCRTZ_Developer{ "mog",          false,  &Effect_Twinkle_Down,       &Effect_OnBeatJitter, &Effect_ZoomIn_Rotate },
    TCRTZ_Developer{ "whiteshark",   true,   &Effect_Rotate_Right_Top2,  &Effect_OnBeatJitter, &Effect_Shift_Left }
};

  /* name specific times */
const auto TimePerName = (std::chrono::duration_cast<SecDouble>(Main_Names_End - Main_Names_Start).count() / Developers.size());
const auto NameFadeTime = std::chrono::milliseconds(120);   // duration of fade in/out in 1/100 secs
const auto NameWaitTime = std::chrono::milliseconds(50);    // delay between fade out and fade in of the next devel in 1/100 secs
const auto NameTwinkleTime = std::chrono::milliseconds(20); // duration of star effects in 1/100 secs
const auto BeatJitterTime = std::chrono::milliseconds(30);  // duration of on beat jitter effect
  /* position at which the names show up
    note: due to use of translate this is the center
    of the names not the upper left corner as usual */
const int NameX = 223;
const int NameY = 329;
const int NameW = 326;
const int NameH = 258;

const std::string ID = "ID_003";   //for help system

/*
implementation

uses
  UCommon,
  UGraphic,
  UHelp,
  UIni,
  ULanguage,
  ULog,
  UMain,
  UPathUtils,
  USongs,
  UUnicodeUtils,
  Math,
  Textgl;
*/
bool TScreenCredits::ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown)
{
  if (!PressedDown)
    return true;
  // Key Down
  // check normal keys
  switch (UCS4UpperCase(CharCode))
  {
    case "Q":
    {
      return false;
    }
  }

  // check special keys
  switch (PressedKey)
  {
    case SDLK_ESCAPE: [[fallthrough]]
    case SDLK_BACKSPACE: [[fallthrough]]
    case SDLK_RETURN:
    {
      FadeTo(@ScreenMain);
      AudioPlayback.PlaySound(SoundLib.Back);
    }
    case SDLK_TAB:
    {
      ScreenPopupHelp.ShowPopup();
    }

/*
    SDLK_SPACE:
        {
          setlength(CTime_hold,length(CTime_hold)+1);
          CTime_hold[high(CTime_hold)]=CTime;
        }
*/
  } // esac
}

bool TScreenCredits::ParseMouse(int MouseButton, bool BtnDown, int X, int Y)
{
  Result = inherited ParseMouse(MouseButton, BtnDown, X, Y);
  
  /* calculate mouse coordinates from -1 to 1
    relative to screen center */
  MouseX = (X - (ScreenW / Screens) / 2) / ((ScreenW / Screens) / 2);
  MouseY = (Y - ScreenH / 2) / (ScreenH / 2);

  MouseMoved = true;
}

void TScreenCredits::LoadNameTextures()
{
  credits_names.resize(Developers.size());
  for (size_t i = 0; i < Developers.size(); ++i)
    credits_names[i] = UTexture::Texture.LoadTexture(CreditsPath / ("names_" + Developers[i].Name + ".png"), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
}

TScreenCredits::TScreenCredits()
{
  inherited Create;

  CreditsPath = ResourcesPath / "credits";

  credits_bg_tex = UTexture::Texture.LoadTexture(CreditsPath / CRDTS_BG_FILE, UTexture::TTextureType::TEXTURE_TYPE_PLAIN, 0);
  credits_bg_ovl = UTexture::Texture.LoadTexture(CreditsPath / CRDTS_OVL_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);

  LoadNameTextures();

  intro_layer01 = UTexture::Texture.LoadTexture(CreditsPath / INTRO_L01_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
  intro_layer02 = UTexture::Texture.LoadTexture(CreditsPath / INTRO_L02_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
  intro_layer03 = UTexture::Texture.LoadTexture(CreditsPath / INTRO_L03_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
  intro_layer04 = UTexture::Texture.LoadTexture(CreditsPath / INTRO_L04_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
  intro_layer05 = UTexture::Texture.LoadTexture(CreditsPath / INTRO_L05_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
  intro_layer06 = UTexture::Texture.LoadTexture(CreditsPath / INTRO_L06_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
  intro_layer07 = UTexture::Texture.LoadTexture(CreditsPath / INTRO_L07_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
  intro_layer08 = UTexture::Texture.LoadTexture(CreditsPath / INTRO_L08_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
  intro_layer09 = UTexture::Texture.LoadTexture(CreditsPath / INTRO_L09_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);

  outro_bg  = UTexture::Texture.LoadTexture(CreditsPath / OUTRO_BG_FILE,  UTexture::TTextureType::TEXTURE_TYPE_PLAIN, 0);
  outro_esc = UTexture::Texture.LoadTexture(CreditsPath / OUTRO_ESC_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
  outro_exd = UTexture::Texture.LoadTexture(CreditsPath / OUTRO_EXD_FILE, UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);

  CRDTS_Stage = TCreditsStages::InitialDelay;
}

void TScreenCredits::OnShow()
{
  inherited;

  if (!Help.SetHelpID(ID))
    ULog::Log.LogWarn("No Entry for Help-ID " + ID, "ScreenCredits");

  /* pause background music */
  SoundLib.PauseBgMusic();

  CRDTS_Stage    = TCreditsStages::InitialDelay;
  CTime = 0;
  init_time = std::chrono::system_clock::now();
  Credits_X      = 580;

  /* open credits tune, we play it after initial delay */
  AudioPlayback.Open(soundpath / "wome-credits-tune.mp3"); // thank you wetue

  /* reset twinkling stars */
  GoldenRec.KillAll;
  
  /* reset mouse coords */
  MouseMoved = false;
  MouseX = 0;
  MouseY = 0;

  /* hide cursor */
  Display.SetCursor();
}

void TScreenCredits::OnHide()
{
  AudioPlayback.Stop();

  /* show cursor */
  Display.SetCursor();

  SoundLib.StartBgMusic();
}

bool TScreenCredits::Draw()
/*  var
    T: uint32_t;*/
{
  // reset beat detection
  BeatDetected = false;

  CTime = std::chrono::system_clock::now() - init_time;

  T = SDL_GetTicks()  /  33;
  if (T <> Credits_Time)
  {
    Credits_Time = T;
    inc(CTime);
    inc(CTime_hold);
    Credits_X -= 2;

    if (CRDTS_Stage == TCreditsStages::InitialDelay && CTime >= Delay_Before_Start)
    {
      CRDTS_Stage = TCreditsStages::Intro;
      //CTime = std::chrono; 0
      AudioPlayback.Play;
    }
    else if (CRDTS_Stage == TCreditsStages::Intro && CTime >= Main_Start)
    {
      CRDTS_Stage = TCreditsStages::MainPart;
    }
    else if (CRDTS_Stage == TCreditsStages::MainPart && CTime >= Tune_End)
    {
      CRDTS_Stage = TCreditsStages::Outro;
    }
    
    // dis does teh muiwk y0r   to be translated :-)
    DetectBeat();
  }

  switch (CRDTS_Stage)
  {
    case TCreditsStages::InitialDelay: 
      DrawInitialDelay(); 
      break;
    case TCreditsStages::Intro: 
      DrawIntro();
      break;
    case TCreditsStages::MainPart:
      DrawMain();
      break;
    case TCreditsStages::Outro:
      DrawOutro();
      break;  
  }

  // make the stars shine
  GoldenRec.Draw();

  return true;
}

void TScreenCredits::DrawInitialDelay()
{
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TScreenCredits::DrawIntro()
  /*var
    Separation, Scale,
    AngleX, AngleY, AngleZ: single;
    FlareX, FlareY: single;
    I: int;*/
{
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float Scale = 1.f;
  float AngleX = 0, AngleY = 0, AngleZ = 0;
  float Seperation;

  /* rotate logo anti clockwise and make it grow */
  if (CTime >= Intro_Separation_End)
  {
    auto phaseTime = CTime - Intro_Separation_End;
    auto phaseDuration = Main_Start - Intro_Separation_End;

    Separation = 1;
    Scale += std::pow(phaseTime, 2) / (32 * phaseDuration);
    AngleZ = 20 * std::pow(phaseTime, 2) / std::pow(phaseDuration / 2, 2);
  }
  /* separate layers */
  else if (CTime >= Intro_Stand_End)
  {
    auto phaseTime = CTime - Intro_Stand_End;
    auto phaseDuration = Intro_Separation_End - Intro_Stand_End;
    Separation = 0.5 * (1.0 + phaseTime / phaseDuration);
  }
  /* stand still */
  else if (CTime >= Intro_Zoom_End)
  {
    Separation = 0.5;
  }
  /* rotate left */
  else 
  {
    auto leftOver = Intro_Zoom_End - CTime;
    auto t0 = leftOver / Intro_Zoom_End;

    Separation = 0.5 * (1.0 + t0);
    AngleX = 10 * t0;
    AngleY = 20 * t0;
  }
  /* the user moved the mouse, overwrite X and Y angle with
    according to mouse position */
  if (MouseMoved)
  {
    // calculate destination angle
    AngleX = 30 * MouseY;
    AngleY = 30 * MouseX;

    /* move angle towards destination */
    if (std::abs(LogoAngleX - AngleX) > 0.001)
      AngleX = LogoAngleX + 0.05 * (AngleX - LogoAngleX);

    if (std::abs(LogoAngleY - AngleY) > 0.001)
      AngleY = LogoAngleY + 0.05 * (AngleY - LogoAngleY);
  }

  // save last angle
  LogoAngleX = AngleX;
  LogoAngleY = AngleY;

  DrawLayeredLogo(Separation, Scale, AngleX, AngleY, AngleZ);

  /* do some sparkling effects */
  if (CTime < Intro_Zoom_End && CTime > Intro_Flare_Start)
  {
    for (size_t i = 0; i < 3; ++i)
    {
       FlareX = 410 + Floor((CTime - Intro_Flare_Start) / (Intro_Zoom_End - Intro_Flare_Start) * (536 - 410)) + RandomRange(-5, 5);
       FlareY = Floor((Intro_Zoom_End - CTime) / 22) + RandomRange(285, 301);
       GoldenRec.Spawn(FlareX, FlareY, 1, 16, 0, -1, Flare, 0);
    }
  }

  /* fade to white at end */
  if (Ctime > Intro_FadeToWhite_Start)
  {
    glColor4f(1, 1, 1, sqr(CTime - Intro_FadeToWhite_Start) * (CTime - Intro_FadeToWhite_Start) / sqr(Main_Start - Intro_FadeToWhite_Start));
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
      glVertex2f(  0,   0);
      glVertex2f(  0, 600);
      glVertex2f(800, 600);
      glVertex2f(800,   0);
    glEnd();
    glDisable(GL_BLEND);
  }
}

void Start3D()
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glFrustum(-0.3 * 4 / 3, 0.3 * 4 / 3, -0.3, 0.3, 1, 1000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void End3D()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void TScreenCredits::DrawLayeredLogo(Separation, Scale, AngleX, AngleY, AngleZ: single)
  //var
    //TotalAngle: single;
{
  Start3D;
  glPushMatrix();

  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glTranslatef(0, 0, -5 + 0.5 * Separation);

  TotalAngle = Abs(AngleX) + Abs(AngleY) + Abs(AngleZ);
  if not isZero(TotalAngle) then
    glRotatef(TotalAngle, AngleX / TotalAngle, AngleY / TotalAngle, AngleZ / TotalAngle);

  glScalef(Scale, Scale, 1);

  glScalef(4/3, -1, 1);
  glColor4f(1, 1, 1, 1);

  glBindTexture(GL_TEXTURE_2D, intro_layer01->TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, -0.4 * Separation);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, -0.4 * Separation);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, -0.4 * Separation);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, -0.4 * Separation);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, intro_layer02.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, -0.3 * Separation);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, -0.3 * Separation);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, -0.3 * Separation);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, -0.3 * Separation);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, intro_layer03.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, -0.2 * Separation);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, -0.2 * Separation);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, -0.2 * Separation);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, -0.2 * Separation);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, intro_layer04.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, -0.1 * Separation);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, -0.1 * Separation);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, -0.1 * Separation);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, -0.1 * Separation);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, intro_layer05.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0 * Separation);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, 0 * Separation);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, 0 * Separation);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, 0 * Separation);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, intro_layer06.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0.1 * Separation);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, 0.1 * Separation);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, 0.1 * Separation);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, 0.1 * Separation);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, intro_layer07.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0.2 * Separation);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, 0.2 * Separation);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, 0.2 * Separation);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, 0.2 * Separation);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, intro_layer08.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0.3 * Separation);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, 0.3 * Separation);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, 0.3 * Separation);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, 0.3 * Separation);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, intro_layer09.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0.22 * Separation);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, 0.22 * Separation);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, 0.22 * Separation);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, 0.22 * Separation);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  glPopMatrix();
  End3D;
}

void TScreenCredits::DrawMain()
{
  DrawMainBG();
  DrawFunkyText();
  DrawNames();
  DrawMainFG();
  DoLogoBling();

  // fade out at end of main part
  if (Ctime > Main_FadeOut_Start)
  {
    glColor4f(0, 0, 0, (CTime - Main_FadeOut_Start) / (Tune_End - Main_FadeOut_Start));
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
      glVertex2f(  0,   0);
      glVertex2f(  0, 600);
      glVertex2f(800, 600);
      glVertex2f(800,   0);
    glEnd();
    glDisable(GL_BLEND);
  }
}

void TScreenCredits::DrawMainBG()
{
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glColor4f(1, 1, 1, 1);
  glBindTexture(GL_TEXTURE_2D, credits_bg_tex.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(       0,        0); glVertex2f(      0,       0);
    glTexCoord2f(       0, 600/1024); glVertex2f(      0, RenderH);
    glTexCoord2f(800/1024, 600/1024); glVertex2f(RenderW, RenderH);
    glTexCoord2f(800/1024,        0); glVertex2f(RenderW,       0);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

void TScreenCredits::DrawFunkyText()
/*var
  S:           int;
  X, Y, A:     double;
  visibleText: string;*/
{
  SetFontSize(30);

  // init ScrollingText
  if (CTime = Main_Start) then
  {
    // set position of text
    Credits_X          = 600;
    CurrentScrollStart = 1;
    CurrentScrollEnd   = 1;
  }

  if (CTime > Main_Start) and
     (CurrentScrollStart < length(Funky_Text)) then
  {
    X = 0;
    visibleText = Copy(Funky_Text, CurrentScrollStart, CurrentScrollEnd);

    for S = 1 to length(visibleText) do
    {
      Y = abs(sin((Credits_X + X) * 0.93 /* * (((Credits_X + X)) / 1200) */ / 100 * pi));
      SetFontPos(Credits_X + X, 538 - Y * (Credits_X + X) * (Credits_X + X) * (Credits_X + X) / 1000000);

      if (Credits_X + X > 32) then
        A = 17
      else if (Credits_X + X >= 15) then
        A = Credits_X + X - 15
      else
        A = 0;

      glColor4f(230 / 255 - 40 / 255 + Y * (Credits_X + X)/  900,
                200 / 255 - 30 / 255 + Y * (Credits_X + X)/ 1000,
                155 / 255 - 20 / 255 + Y * (Credits_X + X)/ 1100,
                  A / 17);
      glPrint(visibleText[S]);
      X = X + glTextWidth(visibleText[S]);
    }

    if (Credits_X < 0) and (CurrentScrollStart < length(Funky_Text)) then
    {
      Credits_X = Credits_X + glTextWidth(Funky_Text[CurrentScrollStart]);
      inc(CurrentScrollStart);
    }

    visibleText = Copy(Funky_Text, CurrentScrollStart, CurrentScrollEnd);

    if (Credits_X + glTextWidth(visibleText) < 600) and
       (CurrentScrollEnd < length(Funky_Text)) then
    {
      inc(CurrentScrollEnd);
    }
  }
/*
// timing hack
  X=5;
  SetFontStyle(2);
  SetFontItalic(false);
  SetFontSize(27);
  glColor4f(1, 1, 1, 1);
  for S = 0 to high(CTime_hold) do
  {
    visibleText = inttostr(CTime_hold[S]);
    SetFontPos (500, X);
    glPrint(visibleText[0]);
    X = X + 20;
  }
*/
}

void TScreenCredits::DrawNames()
  var
    Dev: int;
    Ticks: int;
    DevTicks: int;
    TwinkleW, TwinkleH: int;
{
  Ticks = (CTime - Main_Names_Start);
  Dev = Ticks  /  TimePerName;
  DevTicks = Ticks % TimePerName;

  /*// debug stuff
  SetFontPos(20, 20);
  glPrint("Ticks: " + IntToStr(Ticks));
  SetFontPos(20, 45);
  glPrint("Dev: " + IntToStr(Dev));
  SetFontPos(20, 70);
  glPrint("DevTicks: " + IntToStr(DevTicks)); //*/

  if (Ticks >= 0) and (Dev <= High(Developers)) then
  {
    /* spawn twinkling stars */
    if (Developers[Dev].Twinkle) and (DevTicks >= NameFadeTime) and (DevTicks <= NameFadeTime + NameTwinkleTime) then
    {
      TwinkleW = Round(NameW * 0.6);
      TwinkleH = Round(NameH * 0.6);

      GoldenRec.Spawn(NameX + RandomRange(-TwinkleW, TwinkleW), NameY + RandomRange(-TwinkleH, TwinkleH), 1, 16, 0, -1, PerfectLineTwinkle, 0);
      GoldenRec.Spawn(NameX + RandomRange(-TwinkleW, TwinkleW), NameY + RandomRange(-TwinkleH, TwinkleH), 1, 16, 0, -1, PerfectLineTwinkle, 1);
      GoldenRec.Spawn(NameX + RandomRange(-TwinkleW, TwinkleW), NameY + RandomRange(-TwinkleH, TwinkleH), 1, 16, 0, -1, PerfectLineTwinkle, 5);
      GoldenRec.Spawn(NameX + RandomRange(-TwinkleW, TwinkleW), NameY + RandomRange(-TwinkleH, TwinkleH), 1, 16, 0, -1, PerfectLineTwinkle, 0);
      GoldenRec.Spawn(NameX + RandomRange(-TwinkleW, TwinkleW), NameY + RandomRange(-TwinkleH, TwinkleH), 1, 16, 0, -1, PerfectLineTwinkle, 1);
      GoldenRec.Spawn(NameX + RandomRange(-TwinkleW, TwinkleW), NameY + RandomRange(-TwinkleH, TwinkleH), 1, 16, 0, -1, PerfectLineTwinkle, 5);
      GoldenRec.Spawn(NameX + RandomRange(-TwinkleW, TwinkleW), NameY + RandomRange(-TwinkleH, TwinkleH), 1, 16, 0, -1, PerfectLineTwinkle, 0);
      GoldenRec.Spawn(NameX + RandomRange(-TwinkleW, TwinkleW), NameY + RandomRange(-TwinkleH, TwinkleH), 1, 16, 0, -1, PerfectLineTwinkle, 1);
      GoldenRec.Spawn(NameX + RandomRange(-TwinkleW, TwinkleW), NameY + RandomRange(-TwinkleH, TwinkleH), 1, 16, 0, -1, PerfectLineTwinkle, 5);
    }

    /* prepare drawing */
    glPushMatrix();
    glTranslatef(NameX, NameY, 0);
    glBindTexture(GL_TEXTURE_2D, credits_names[Dev].TexNum);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    // calculate progress and call effect
    if (DevTicks <= NameFadeTime) then
      Developers[Dev].FadeIn(credits_names[Dev], DevTicks / NameFadeTime)
    else if (DevTicks >= TimePerName - NameFadeTime - NameWaitTime) then
    {
      if (DevTicks < TimePerName - NameWaitTime) then
        Developers[Dev].FadeOut(credits_names[Dev], ((TimePerName - NameWaitTime) - DevTicks) / NameFadeTime);
    }
    else
      Developers[Dev].Draw(credits_names[Dev], (DevTicks - NameFadeTime) / (TimePerName - NameFadeTime * 2 - NameWaitTime));

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glPopMatrix();
  }
}

void TScreenCredits::DrawMainFG;
{
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(1, 1, 1, 1);
  glBindTexture(GL_TEXTURE_2D, credits_bg_ovl.TexNum);
  glBegin(GL_QUADS);
    glTexCoord2f(      0,        0); glVertex2f(800-393,   0);
    glTexCoord2f(      0, 600/1024); glVertex2f(800-393, 600);
    glTexCoord2f(393/512, 600/1024); glVertex2f(800,     600);
    glTexCoord2f(393/512,        0); glVertex2f(800,       0);
  glEnd();


  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

void TScreenCredits::DoLogoBling;
  const
    myLogoCoords: array[0..27,0..1] of uint32_t = (
      ( 39,32),( 84,32),(100,16),(125,24),
      (154,31),(156,58),(168,32),(203,36),
      (258,34),(251,50),(274,93),(294,84),
      (232,54),(278,62),(319,34),(336,92),
      (347,23),(374,32),(377,58),(361,83),
      (385,91),(405,91),(429,35),(423,51),
      (450,32),(485,34),(444,91),(486,93)
    );
  var
    Coords: int;
    StartFrame: int;
{
  if (CTime > Main_OnBeatTwinkle_Start ) and
     (CTime < Main_FadeOut_Start) then
    {
      /* spawn stars only in frames where a beat was detected */
      if BeatDetected then
      {
         StartFrame = RandomRange(6, 16);
         Coords = RandomRange(0, 27);

         GoldenRec.Spawn(myLogoCoords[Coords,0], myLogoCoords[Coords,1], 16-StartFrame, StartFrame, 0, -1, PerfectNote, 0);
      }
    }
}

void TScreenCredits::DrawOutro()
{
  if (CTime >= Tune_End)
  {
    CTime_hold = 0;
    AudioPlayback.Stop;
    AudioPlayback.Open(SoundPath / "credits-outro-tune.mp3"));
    AudioPlayback.SetVolume(0.2);
    AudioPlayback.SetLoop(true);
    AudioPlayback.Play;
  }

  //TODO:: implement infinite replay instead of restarting play
  if CTime_hold > 231 then
  {
    AudioPlayback.Play;
    Ctime_hold = 0;
  }

  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT or GL_DEPTH_BUFFER_BIT);

  // do something useful
  // outro background
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glColor4f(1, 1, 1, 1);
  glBindTexture(GL_TEXTURE_2D, outro_bg.TexNum);
  glBegin(gl_quads);
    glTexCoord2f(       0,        0); glVertex2f(  0,   0);
    glTexCoord2f(       0, 600/1024); glVertex2f(  0, 600);
    glTexCoord2f(800/1024, 600/1024); glVertex2f(800, 600);
    glTexCoord2f(800/1024,        0); glVertex2f(800,   0);
  glEnd();

  // outro overlays
  glColor4f(1, 1, 1, (2 + sin(CTime / 15)) / 3);
  glBindTexture(GL_TEXTURE_2D, outro_esc.TexNum);
  glBegin(Gl_Quads);
    glTexCoord2f(      0,       0); glVertex2f(  0,   0);
    glTexCoord2f(      0, 223/256); glVertex2f(  0, 223);
    glTexCoord2f(487/512, 223/256); glVertex2f(487, 223);
    glTexCoord2f(487/512,       0); glVertex2f(487,   0);
  glEnd();

  if (RandomRange(0,20) <= 18) then
  {
    glColor4f(1, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, outro_exd.TexNum);
    glBegin(Gl_Quads);
      glTexCoord2f(      0,       0); glVertex2f(800-310, 600-247);
      glTexCoord2f(      0, 247/256); glVertex2f(800-310, 600    );
      glTexCoord2f(310/512, 247/256); glVertex2f(800,     600    );
      glTexCoord2f(310/512,       0); glVertex2f(800,     600-247);
    glEnd();
  }

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  // outro scrollers?
  // ...
}

/* name effects */
/* effects are called with blending texture and matrix prepared */
void Effect_Draw (const UTexture::TTexture, double Progress);
{
  glColor4f(1, 1, 1, 1);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_OnBeatJitter (const UTexture::TTexture, double Progress);
  var
    Diff: uint32_t;
    Alpha: double;
{
  Diff = ScreenCredits.CTime - ScreenCredits.LastBeatTime;
  if (Diff < BeatJitterTime) then  
    Alpha = 0.5 + 0.5 * Diff / BeatJitterTime
  else
    Alpha = 1;

  glColor4f(1, 1, 1, Alpha);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_Rotate_Left_Top (const UTexture::TTexture, double Progress);
{
  glColor4f(1, 1, 1, Progress);

  gltranslatef(-NameX, 0, 0);
  glrotatef(Progress * 90 + 270, 0, 0, 1);
  gltranslatef(NameX, 0, 0);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_Rotate_Right_Bot (const UTexture::TTexture, double Progress);
{
  glColor4f(1, 1, 1, Progress);

  gltranslatef(NameX, 0, 0);
  glrotatef((Progress - 1) * 90, 0, 0, 1);
  gltranslatef(-NameX, 0, 0);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_ZoomIn_Rotate (const UTexture::TTexture, double Progress);
{
  glColor4f(1, 1, 1, Progress);

  glscalef(sqr(Progress), sqr(Progress), sqr(Progress));
  glrotatef(Progress * 360, 0, 0, 1);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_ZoomOut_Shift (const UTexture::TTexture, double Progress);
  var
    X: double;
{
  glColor4f(1, 1, 1, Progress);

  X = (1 - Progress);
  gltranslatef(X * 300, -X * 100, 0);
  glscalef(1 + X, 1 + X, 1 + X);
  glrotatef(X * 90, 0, 0, 1);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_Shift_Left (const UTexture::TTexture, double Progress);
{
  glColor4f(1, 1, 1, Progress);

  glTranslatef((Progress - 1) * 210, 0, 0);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_Shift_Right_Top (const UTexture::TTexture, double Progress);
{
  glColor4f(1, 1, 1, Progress);

  glTranslatef((1 - Progress) * 210, (Progress - 1) * 105, 0);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_Flip_Bot (const UTexture::TTexture, double Progress);
  var
    X: double;
{
  glColor4f(1, 1, 1, Progress);

  X = NameH * (1 - Progress);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2 - 1.5 * X, -NameH/2 + 1.5 * X);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2 + 1.5 * X, -NameH/2 + 1.5 * X);
  glEnd();
}

void Effect_Flip_Right_Top (const UTexture::TTexture, double Progress);
  var
    X: double;
{
  glColor4f(1, 1, 1, Progress);

  X = NameW * (1 - Progress);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2 + X, -NameH/2 - X/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2 + X,  NameH/2 - (X * 1.5 * NameH / NameW));
    glTexCoord2f(1, 1); glVertex2f( NameW/2 + X,  NameH/2 + X / 4);
    glTexCoord2f(1, 0); glVertex2f( NameW/2 + X, -NameH/2 - X / 4);
  glEnd();
}

void Effect_Flip_Right (const UTexture::TTexture, double Progress);
  var
    X: double;
{
  glColor4f(1, 1, 1, Progress);

  X = NameW * (1 - Progress);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2,     -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,      NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2 - X,  NameH/2 + X * 1.5);
    glTexCoord2f(1, 0); glVertex2f( NameW/2 - X, -NameH/2 - X * 1.5);
  glEnd();
}

void Effect_Flip_Right_Bot (const UTexture::TTexture, double Progress);
  var
    X: double;
{
  glColor4f(1, 1, 1, Progress);

  X = NameW * (1 - Progress);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2 + X * 1.5, -NameH/2 + X * 1.5);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2 + X * 1.2,  NameH/2 + X);
    glTexCoord2f(1, 1); glVertex2f( NameW/2 + X / 2,    NameH/2 + X / 4);
    glTexCoord2f(1, 0); glVertex2f( NameW/2 + X * 1.5, -NameH/2);
  glEnd();
}

void Effect_Rotate_Right_Top (const UTexture::TTexture, double Progress);
{
  glColor4f(1, 1, 1, Progress);

  glTranslatef(NameX, 0, 0);
  glrotatef((1 - Progress) * 90, 0, 0, 1);
  glTranslatef(-NameX, 0, 0);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_Shift_Weird (const UTexture::TTexture, double Progress);
  var
    X: double;
{
  glColor4f(1, 1, 1, Progress);

  X = (Progress - 1);

  glTranslatef(X * 200, X * 100, 0);
  glScalef(Progress, Progress, Progress);
  glRotatef(X * 90, 0, 0, 1);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_Shift_Right_Bot (const UTexture::TTexture, double Progress);
{
  glColor4f(1, 1, 1, Progress);

  glTranslatef((1 - Progress) * 200, (1 - Progress) * 100, 0);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_Rotate_Right_Top2 (const UTexture::TTexture, double Progress);
{
  glColor4f(1, 1, 1, Progress);

  glTranslatef(0, -NameX, 0);
  glRotatef((Progress - 1) * 90, 0, 0, 1);
  glTranslatef(0, NameX, 0);
  glRotatef((1 - Progress) * 90, 0, 0, 1);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH/2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2,  NameH/2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2,  NameH/2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH/2);
  glEnd();
}

void Effect_Flip_Left_Bot (const UTexture::TTexture, double Progress);
  var
    X: double;
{
  glColor4f(1, 1, 1, Progress);

  X = (1 - Progress) * NameW;

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2 - X,       -NameH/2 + X / 4);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2 - X / 4,    NameH/2 + X / 4);
    glTexCoord2f(1, 1); glVertex2f( NameW/2 - X * 1.2,  NameH/2 + X / 2);
    glTexCoord2f(1, 0); glVertex2f( NameW/2 - X * 1.5, -NameH/2 + X * 1.5);
  glEnd();
}

void Effect_Flip_Right_Top2 (const UTexture::TTexture, double Progress);
  var
    X: double;
{
  glColor4f(1, 1, 1, Progress);

  X = (1 - Progress) * NameW;

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2 + X,     -NameH/2 - X / 2);
    glTexCoord2f(0, 1); glVertex2f(-NameW/2 + X,      NameH/2 + X / 2);
    glTexCoord2f(1, 1); glVertex2f( NameW/2 + X / 4,  NameH/2 - X / 4);
    glTexCoord2f(1, 0); glVertex2f( NameW/2 + X / 4, -NameH/2 + X / 4);
  glEnd();
}

void Effect_Twinkle_Down     (const UTexture::TTexture, double Progress);
{
  // draw name
  glColor4f(1, 1, 1, 1);

  glTranslatef(0, NameH/2, 0);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-NameW/2, -NameH * Progress);
    glTexCoord2f(0, Progress); glVertex2f(-NameW/2, 0);
    glTexCoord2f(1, Progress); glVertex2f( NameW/2, 0);
    glTexCoord2f(1, 0); glVertex2f( NameW/2, -NameH * Progress);
  glEnd();

  //spawn some stars on the edge
  GoldenRec.Spawn(NameX + RandomRange(-NameW  /  2, NameW  /  2), NameY - NameH/2 + (1 - Progress) * NameH, 1, 16, 0, -1, PerfectLineTwinkle, 0);
  GoldenRec.Spawn(NameX + RandomRange(-NameW  /  2, NameW  /  2), NameY - NameH/2 + (1 - Progress) * NameH, 1, 16, 0, -1, PerfectLineTwinkle, 1);
  GoldenRec.Spawn(NameX + RandomRange(-NameW  /  2, NameW  /  2), NameY - NameH/2 + (1 - Progress) * NameH, 1, 16, 0, -1, PerfectLineTwinkle, 5);
  GoldenRec.Spawn(NameX + RandomRange(-NameW  /  2, NameW  /  2), NameY - NameH/2 + (1 - Progress) * NameH, 1, 16, 0, -1, PerfectLineTwinkle, 0);
  GoldenRec.Spawn(NameX + RandomRange(-NameW  /  2, NameW  /  2), NameY - NameH/2 + (1 - Progress) * NameH, 1, 16, 0, -1, PerfectLineTwinkle, 1);
  GoldenRec.Spawn(NameX + RandomRange(-NameW  /  2, NameW  /  2), NameY - NameH/2 + (1 - Progress) * NameH, 1, 16, 0, -1, PerfectLineTwinkle, 5);
  GoldenRec.Spawn(NameX + RandomRange(-NameW  /  2, NameW  /  2), NameY - NameH/2 + (1 - Progress) * NameH, 1, 16, 0, -1, PerfectLineTwinkle, 0);
  GoldenRec.Spawn(NameX + RandomRange(-NameW  /  2, NameW  /  2), NameY - NameH/2 + (1 - Progress) * NameH, 1, 16, 0, -1, PerfectLineTwinkle, 1);
  GoldenRec.Spawn(NameX + RandomRange(-NameW  /  2, NameW  /  2), NameY - NameH/2 + (1 - Progress) * NameH, 1, 16, 0, -1, PerfectLineTwinkle, 5);
}

/* beat detection algorithm
  based on a tutorial from Fr�d�ric Patin on gamedev.net
  http://www.gamedev.net/reference/programming/features/beatdetection/default.asp */

/* calculates average value of a history buffer */
function Average(History: TEnergyHistory): single;
  var I: int; 
{
  Result = 0;

  for I = 0 to HistoryLength - 1 do
    Result = Result + History[I];

  Result = Result / HistoryLength;
}

/* calculates variance value of a history buffer */
function Variance(History: TEnergyHistory; Average: single): single;
  var I: int;
{
  Result = 0;

  for I = 0 to HistoryLength - 1 do
    Result = Result + sqr(History[I] - Average);

  Result = Result / HistoryLength;
}

/* shifts all values of the history to the right and
  adds the new value at the front */
void AddHistory(Value: single; var History: TEnergyHistory);
  var I: int;
{
  for I = HistoryLength - 1 downto 1 do
    History[I] = History[I-1];

  History[0] = Value;
}

/* calculates instant energy from FFT data for a specific
  subchannel (0..SubChannelCount - 1) */
function CalculateInstantEnergy(SubChannel: int; Data: TFFTData): single;
  var I: int;
{
  Result = 0;
  for I = SubChannel * SamplesPerChannel to (SubChannel + 1) * SamplesPerChannel - 1 do
    Result = Result + Data[I] * BeatEnergyModifier;

  Result = Result / SamplesPerChannel;
}

void TScreenCredits::DetectBeat()
  var
    Data: TFFTData;
    I: int;
    Instant: single;
    C, E, V: single;
{
  AudioPlayback.GetFFTData(Data);

  // do beatdetection for every subchannel
  for I = 0 to SubChannelCount - 1 do
  {
    Instant = CalculateInstantEnergy(I, Data);
    E = Average(SubchannelHistory[I]);
    V = Variance(SubchannelHistory[I], E);

    C = (-0.0025714 * V) + 1.5142857;

    AddHistory(Instant, SubChannelHistory[I]);

    if (Instant > 2) and (Instant > C * E) then
    {
      // beat detected
      BeatDetected = true;
      LastBeatTime = CTime;
    }
  }
}
}