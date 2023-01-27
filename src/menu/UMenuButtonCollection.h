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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/menu/UMenuButtonCollection.pas $
 * $Id: UMenuButtonCollection.pas 1692 2009-04-24 18:43:12Z k-m_schindler $
 */
#include <vector>

#include "UMenuButton.h"

namespace UMenuButtonCollection
{
/*uses
  UMenuButton;*/

//type
  //----------------
  //TButtonCollection
  //No Extra Attributes or Functions ATM
  //----------------
  typedef std::vector<UMenuButton::TButton> AButton;
  typedef std::shared_ptr<AButton> PAButton;
  class TButtonCollection : public UMenuButton::TButton
  {
    public:
    //num of the First Button, that can be Selected
    uint8_t FirstChild;
    uint8_t CountChilds;
    
    PAButton ScreenButton;

    void SetSelect(bool Value) override;
    void Draw() override;
  };

void TButtonCollection::SetSelect(bool Value)
{
  inherited;

  //Set Visible for Every Button that is a Child of this ButtonCollection
  if (!Fade)
    for (Index = 0 to High(ScreenButton^))
      if (ScreenButton^[Index].Parent == Parent)
        ScreenButton^[Index].Visible = Value;
}

void TButtonCollection::Draw()
//var
  //I, J: integer;
{
  inherited;
  //If fading is activated, Fade Child Buttons
  if (!Fade)
    return;
  
  for (I = 0 to High(ScreenButton^))
    if (ScreenButton^[I].Parent == Parent)
    {
      if (FadeProgress < 0.5)
      {
        ScreenButton^[I].Visible = SelectBool;

        for (J = 0 to High(ScreenButton^[I].Text))
          ScreenButton^[I].Text[J].Visible = SelectBool;
      }
      else
      {
        ScreenButton^[I].Texture.Alpha = (FadeProgress-0.666)*3;
        
        for (J = 0 to High(ScreenButton^[I].Text))
          ScreenButton^[I].Text[J].Alpha = (FadeProgress-0.666)*3;
      }
    }
}
}