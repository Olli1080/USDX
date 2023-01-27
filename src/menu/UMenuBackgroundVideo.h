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
#include <filesystem>

#include "UMenuBackground.hpp"

namespace UMenuBackgroundVideo
{
/*
uses
  UThemes,
  UMenuBackground,
  UMusic,
  UVideo,
  UPath;
*/
//TMenuBackgroundColor - Background Color
//--------

//type
  //DefaultBGVideoPlayback = TVideoPlayback_FFmpeg;
/*
{type
  TBGVideoPool = class;

  PBGVideoPoolItem = ^TBGVideoPoolItem;
  TBGVideoPoolItem = record
    Parent: TBGVideoPool;
    VideoPlayback = IVideoPlayback;
    ReferenceCounter: cardinal; //Number of Creations
  }

  TBGVideo = class
    private
      myItem: PBGVideoPoolItem;
    public
      constructor Create(Item: PBGVideoPoolItem); override;

      function    GetVideoPlayback: IVideoPlayback;
      void   Draw;

      destructor  Destroy;
  }

  TBGVideoPool = class
    private
      Items: PBGVideoPoolItem;
    public
      constructor Create;

      function    GetBGVideo(filename: IPath): TBGVideo;
      void   RemoveItem(
      void   FreeAllItems;

      destructor  Destroy;
  }

type }*/
  class TMenuBackgroundVideo : public UMenuBackground::TMenuBackground
  {
    private:

      std::filesystem::path fFilename;
      IVideo fBgVideo;

    public:

      TMenuBackgroundVideo(const TThemeBackground ThemedSettings);
      ~TMenuBackgroundVideo() override = default;

      void   OnShow() override;
      void   Draw() override;
      void   OnFinish() override;
  };

// var BGVideoPool: TBGVideoPool;
const std::array<std::string, 7>
  SUPPORTED_EXTS_BACKGROUNDVIDEO = {".avi", ".mov", ".divx", ".mpg", ".mp4", ".mpeg", ".m2v"};

/*
implementation

uses
  dglOpenGL,
  SysUtils,
  UTime,
  USkins,
  UCommon,
  UGraphic;
*/
TMenuBackgroundVideo::TMenuBackgroundVideo(const TThemeBackground ThemedSettings)
{
  inherited;
  if (ThemedSettings.Tex.empty())
    throw EMenuBackgroundError("TMenuBackgroundVideo: No video filename present");

  fFilename = Skin.GetTextureFileName(ThemedSettings.Tex);
  if (std::filesystem::is_regular_file(fFilename))
    throw EMenuBackgroundError("TMenuBackgroundVideo: Can't load background video: " + fFilename.string());
}

void TMenuBackgroundVideo::OnShow()
{
  fBgVideo = VideoPlayback.Open(fFileName);
  if (fBgVideo)
  {
    VideoBGTimer.SetTime(0);
    VideoBGTimer.Start();
    fBgVideo.Loop = true;
    fBgVideo.Play;
  }
}

void   TMenuBackgroundVideo::OnFinish()
{
  // unload video
  fBgVideo.reset();
}

void TMenuBackgroundVideo::Draw()
{
  // clear just once when in dual screen mode
  if (ScreenAct == 1)
  {
    glClear(GL_DEPTH_BUFFER_BIT);
    // video failure -> draw blank background
    if (!fBgVideo)
      glClear(GL_COLOR_BUFFER_BIT);
  }

  if (fBgVideo)
  {
    fBgVideo.GetFrame(VideoBGTimer.GetTime());
    fBgVideo.SetScreen(ScreenAct);
    fBgVideo.Draw();
  }
}

// Implementation of TBGVideo
//--------
/*constructor TBGVideo.Create(Item: PBGVideoPoolItem);
{
  myItem = PBGVideoPoolItem;
  Inc(myItem.ReferenceCounter);
}

destructor  TBGVideo.Destroy;
{
  Dec(myItem.ReferenceCounter);
}

function    TBGVideo.GetVideoPlayback: IVideoPlayback;
{

}

void   TBGVideo.Draw;
{

}

// Implementation of TBGVideoPool
//--------

constructor TBGVideoPool.Create;
{

}

destructor  TBGVideoPool.Destroy;
{

}

function    TBGVideoPool.GetBGVideo(filename: IPath): TBGVideo;
{

}

void   TBGVideoPool.FreeAllItems;
{

}  */
}