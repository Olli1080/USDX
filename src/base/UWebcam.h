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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UPlaylist.pas $
 * $Id: $
 **/

#include "../switches.h"

#include <string>
#include <memory>
#include <numbers>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <gl/GL.h>
#include <opencv2/core/base.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <SDL2/SDL.h>

#include "UIni.h"

namespace UWebcam
{
class TWebcam
{
private:
    bool IsEnabled;
    int Interval;
    Uint32 LastTickFrame;
    cv::Mat LastFrame;
    cv::Mat CurrentFrame;
    std::mutex Mutex;
    std::condition_variable StopCond;
    std::unique_ptr<std::thread> CaptureThread;
    cv::Mat BGR2YUV;
    cv::Mat YUV2BGR;
    cv::Mat Mat1;
    cv::Mat Mat2;

    int CaptureLoop();

public:
    std::unique_ptr<cv::VideoCapture> Capture;
    TTexture TextureCam;

    TWebcam();
    virtual ~TWebcam();

    void Start();
    void Release();
    void Restart();
    void GetWebcamFrame();
    cv::Mat FrameEffect(int Nr_Effect, cv::Mat Frame);
    cv::Mat FrameAdjust(cv::Mat Frame);
};

inline TWebcam Webcam;
//----------
}