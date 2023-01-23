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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/screens/UScreenLoading.pas $
 * $Id: UScreenLoading.pas 1939 2009-11-09 00:27:55Z s_alexander $
 */

#include "../menu/UMenu.h"

namespace UScreenLoading
{
  /*
uses
  UMenu,
  sdl2,
  SysUtils,
  UThemes,
  dglOpenGL;
*/
  class TScreenLoading : public UMenu::TMenu
  {
  public:
      bool Fadeout;
      int TextDescription;

      TScreenLoading();
      void OnShow() override;
      bool ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown) override;
  };
/*
implementation

uses
  UGraphic,
  UTime;
*/
bool TScreenLoading::ParseInput(uint32_t PressedKey, UCS4Char CharCode, bool PressedDown)
{
  return true;
}

TScreenLoading::TScreenLoading()
{
  inherited Create;

  LoadFromTheme(Theme.Loading);

  Fadeout = false;
}

void TScreenLoading::OnShow()
{
  inherited;
}
}