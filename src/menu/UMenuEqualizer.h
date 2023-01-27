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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UMenuEqualizer.pas $
 * $Id: UMenuEqualizer.pas 1692 2009-04-24 18:43:12Z k-m_schindler $
 */
#include <vector>
#include <chrono>

namespace UMenuEqualizer
{
/*uses
  UMusic,
  UCommon,
  UThemes;*/

//type
  //----------------
  //Tms_Equalizer
  //Class displaying an equalizer (Songscreen)
  //----------------
  class Tms_Equalizer //(TObject)
  {
    private:

      TFFTData FFTData; // moved here to avoid stack overflows
      std::vector<uint8_t> BandData;
      std::chrono::system_clock::time_point RefreshTime;

      IAudioPlayback Source;

      void Analyse();

    public:

      int X;
      int Y;
      double Z;

      int W;
      int H;
      int Space;

      bool Visible;
      double Alpha;
      TRGB Color;

      bool Direction;
      int BandLength;

      bool Reflection;
      double Reflectionspacing;


      Tms_Equalizer(IAudioPlayback Source, TThemeEqualizer mySkin);

      void Draw();
      void SetBands(uint8_t Value);
      uint8_t GetBands();
      //property  Bands: uint8_t read GetBands write SetBands;
      void SetSource(IAudioPlayback newSource);
  };

/*implementation
uses
  math,
  sdl2,
  dglOpenGL;*/

Tms_Equalizer::Tms_Equalizer(IAudioPlayback Source, TThemeEqualizer mySkin)
//var
  //I: int;
{
  if (Source)
  {
    X          = mySkin.X;
    Y          = mySkin.Y;
    W          = mySkin.W;
    H          = mySkin.H;
    Z          = mySkin.Z;

    Space      = mySkin.Space;

    Visible    = mySkin.Visible;
    Alpha      = mySkin.Alpha;
    Color.R    = mySkin.ColR;
    Color.G    = mySkin.ColG;
    Color.B    = mySkin.ColB;

    Direction  = mySkin.Direction;
    Bands      = mySkin.Bands;
    BandLength = mySkin.Length;

    Reflection = mySkin.Reflection;
    Reflectionspacing = mySkin.Reflectionspacing;

    this->Source = Source;


    //Check if Visible
    if (Bands  <= 0)  ||
       (BandLength <= 0)  ||
       (W      <= 0)  ||
       (H      <= 0)  ||
       (Alpha  <= 0)
      Visible = false;

    //ClearArray
    for (I = low(BandData) to high(BandData))
      BandData[I] = 3;
  }
  else
    Visible = false;
}

//--------
// evaluate FFT-Data
//--------
void Tms_Equalizer::Analyse()
//var
  //I:            int;
  //ChansPerBand: uint8_t;        // channels per band
  //MaxChannel:   int;
  //Pos:          double;
  //CurBand:      int;
{
  Source.GetFFTData(FFTData);

  double Pos = 0;
  // use only the first approx. 92 of 256 FFT-channels (approx. up to 8kHz
  uint8_t ChansPerBand = ceil(92 / Bands); // How much channels are used for one Band
  int MaxChannel = ChansPerBand * Bands - 1;

  // Change Lengths
  for (i = 0 to MaxChannel)
  {
    // Gain higher freq. data so that the bars are visible
    if (i > 35)
      FFTData[i] *= 8;
    else if (i > 11)
      FFTData[i] *= 4.5;
    else
      FFTData[i] *= 1.1;

    // clamp data
    if (FFTData[i] > 1)
      FFTData[i] = 1;

    // Get max. pos
    Pos = std::max(FFTData[i] * BandLength, Pos);

    // Check if this is the last channel in the band
    if ((i+1) % ChansPerBand == 0)
    {
      int CurBand = i / ChansPerBand;

      // Smooth delay if new equalizer is lower than the old one
      if ((BandData[CurBand] > Pos && BandData[CurBand] > 1))
        --BandData[CurBand];
      else
        BandData[CurBand] = std::round(Pos);

      Pos = 0;
    }
  }
}

//--------
// Draw SpectrumAnalyser, Call Analyse
//--------
void Tms_Equalizer::Draw()
//var
  //CurTime:    uint32_t;
  //PosX, PosY: double;
  //I, J:       int;
  //Diff:       double;
{
  auto GetAlpha = [this](float Diff) -> float
  {
    if (Direction)
      return (Alpha * 0.6) * (0.5 - Diff/(BandLength * (H + Space)));
    else
      return (Alpha * 0.6) * (0.5 - Diff/(Bands      * (H + Space)));
  };

  if (Visible && !AudioPlayback.Finished)
  {
    //Call Analyse if necessary
    auto CurTime = std::chrono::system_clock::now();
    if (CurTime > RefreshTime)
    {
      Analyse();

      RefreshTime = CurTime + std::chrono::milliseconds(44);
    }

    //Draw Equalizer Bands
    // Setup OpenGL
    glColorRGB(Color, Alpha);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    // Set position of the first equalizer bar
    double PosY = Y;
    double PosX = X;

    // Draw bars for each band
    for (I = 0 to High(BandData))
    {
      // Reset to lower or left position depending on the drawing-direction
      if (Direction) // Vertical bars
        // FIXME: Is Y the upper or lower coordinate?
        PosY = Y; //+ (H + Space) * BandLength
      else                                   // Horizontal bars
        PosX = X;

      // Draw the bar as a stack of blocks
      for (J = 1 to BandData[I])
      {
        // Draw block
        glBegin(GL_QUADS);
          glVertex3f(PosX, PosY, Z);
          glVertex3f(PosX, PosY+H, Z);
          glVertex3f(PosX+W, PosY+H, Z);
          glVertex3f(PosX+W, PosY, Z);
        glEnd();

        if (Reflection && J <= BandLength / 2)
        {
          double Diff = (Y-PosY) + H;

          //Draw Reflection
          if (Direction)
          {
            glBegin(GL_QUADS);
              glColorRGB(Color, GetAlpha(Diff));
              glVertex3f(PosX, Diff + Y + Reflectionspacing, Z);

              //bottom v
              glColorRGB(Color, GetAlpha(Diff + H));
              glVertex3f(PosX, Diff + Y+H + Reflectionspacing, Z);
              glVertex3f(PosX+W, Diff + Y+H + Reflectionspacing, Z);

              glColorRGB(Color, GetAlpha(Diff));
              glVertex3f(PosX+W, Diff + Y + Reflectionspacing, Z);
            glEnd();
          }
          else
          {
            glBegin(GL_QUADS);
              glColorRGB(Color, GetAlpha(Diff));
              glVertex3f(PosX, Diff + Y + (H + Space)*Bands + Reflectionspacing, Z);
              glVertex3f(PosX, Diff + Y+H  + (H + Space)*Bands + Reflectionspacing, Z);
              glVertex3f(PosX+W, Diff + Y+H  + (H + Space)*Bands + Reflectionspacing, Z);
              glVertex3f(PosX+W, Diff + Y + (H + Space)*Bands + Reflectionspacing, Z);
              glColorRGB(Color, GetAlpha(Diff + H));
            glEnd();
          }

          glColorRGB(Color, Alpha);
        }


        // Calc position of the bar's next block
        if (Direction) // Vertical bars
          PosY -= H - Space;
        else                                   // Horizontal bars
          PosX += W + Space;
      }

      // Calc position of the next bar
      if (Direction) // Vertical bars
        PosX += W + Space;
      else                                   // Horizontal bars
        PosY += H + Space;
    }


  }
}

void Tms_Equalizer::SetBands(uint8_t Value)
{
  BandData.resize(Value);
}

uint8_t Tms_Equalizer::GetBands()
{
  return BandData.size();
}

void Tms_Equalizer::SetSource(IAudioPlayback newSource);
{
  if (newSource)
    Source = newSource;
}
}