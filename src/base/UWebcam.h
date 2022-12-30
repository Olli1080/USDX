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

#include <gl/GL.h>
#include <opencv2/core/base.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <SDL2/SDL.h>

#include "UIni.h"

namespace UWebcam
{


/*$IFDEF FPC*/
  /*$MODE Delphi*/
/*$}IF*/

/*$I switches.inc*/

/*uses
  Classes,
  UTexture,
  sdl2,
  opencv_highgui,
  opencv_core,
  opencv_imgproc,
  opencv_types;
  */
    class TWebcam
    {
    private:
        bool IsEnabled;
        int Interval;
        Uint32 LastTickFrame;
        cv::Mat LastFrame;
        cv::Mat CurrentFrame;
        SDL_mutex* Mutex;
        SDL_cond* StopCond;
        SDL_Thread* CaptureThread;
	    cv::Mat BGR2YUV;
	    cv::Mat YUV2BGR;
	    cv::Mat Mat1;
	    cv::Mat Mat2;

        int CaptureLoop();
        static int CaptureThreadMain(Pointer Data);

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

/*uses
  dglOpenGL,
  SysUtils,
  ULog,
  UIni;*/

//----------
//Create - Construct Class
//----------
TWebcam::TWebcam()
{
    Mutex = SDL_CreateMutex();
    StopCond = SDL_CreateCond();
    IsEnabled = false;
    /*
    BGR2YUV = cv::Mat(3, 3, CV_32FC1);
    YUV2BGR = cv::Mat(3, 3, CV_32FC1);
    */
    Mat1 = cv::Mat(3, 3, CV_32FC1);
    Mat2 = cv::Mat(3, 3, CV_32FC1);

    BGR2YUV = (cv::Mat_<float>(3, 3)
        << 0.114                << 0.587                << 0.299
        << 1                    << -0.587 / (1 - 0.114) << -0.299 / (1 - 0.114)
        << -0.114 / (1 - 0.299) << -0.587 / (1 - 0.299) << 1);
    YUV2BGR = BGR2YUV.inv();
}

TWebcam::~TWebcam()
{
  Release();
  SDL_DestroyCond(StopCond);
  SDL_DestroyMutex(Mutex);
}

void TWebcam::Start()
/*var
  H, W, I: int;
  s: string;*/
{

    if (UIni::Ini.WebcamID == 0)
        return;
    try
    {
        Capture = std::make_unique<cv::VideoCapture>(UIni::Ini.WebcamID - 1);
        IsEnabled = Capture->isOpened();
    }
    catch (...)
    {
        IsEnabled = false;
    }

    if (IsEnabled)
    {
        std::string S = UIni::IWebcamResolution[UIni::Ini.WebcamResolution];

        int I = S.find('x');
        int W = std::stoi(S.substr(0, I - 1));
        int H = std::stoi(S.substr(I, 1000));

        Capture->set(cv::CAP_PROP_FRAME_WIDTH, W);
        Capture->set(cv::CAP_PROP_FRAME_HEIGHT, H);

        Interval = 1000 / std::stoi(UIni::IWebcamFPS[UIni::Ini.WebcamFPS]);
        CaptureThread = SDL_CreateThread(@TWebcam::CaptureThreadMain, nullptr, Self);
    }
}

void TWebcam::Release()
{
    if (IsEnabled)
    {
        SDL_LockMutex(Mutex);
        IsEnabled = false;
        SDL_CondSignal(StopCond);
        SDL_UnlockMutex(Mutex);
        SDL_WaitThread(CaptureThread, nullptr);
    }
}

void TWebcam::Restart()
{
  Release();
  try
  {
      Start();
  }
  catch (...)
  {}
}

int TWebcam::CaptureLoop()
{
  SDL_LockMutex(Mutex);
  while (IsEnabled)
  {
    SDL_UnlockMutex(Mutex);

    LastTickFrame = SDL_GetTicks();
    
    cv::Mat WebcamFrame;
  	Capture->read(WebcamFrame);

    SDL_LockMutex(Mutex);
    if (!WebcamFrame.empty())
      CurrentFrame = WebcamFrame.clone();

    const Uint32 Now = SDL_GetTicks();
    if (IsEnabled && Now - LastTickFrame < Interval)
    {
      SDL_CondWaitTimeout(StopCond, Mutex, Interval - (Now - LastTickFrame));
    }
  }
  SDL_UnlockMutex(Mutex);
  return 0;
}

int TWebcam::CaptureThreadMain(Pointer Data)
{
  return TWebcam(Data).CaptureLoop();
}

void TWebcam::GetWebcamFrame()
{
    cv::Mat WebcamFrame;

    if (IsEnabled)
    {
        SDL_LockMutex(Mutex);
        WebcamFrame = CurrentFrame;
        CurrentFrame = cv::Mat();
        Interval = 1000 / std::stoi(UIni::IWebcamFPS[UIni::Ini.WebcamFPS]);
        SDL_UnlockMutex(Mutex);
    }

    if (!WebcamFrame.empty())
    {
        if (TextureCam.TexNum > 0)
        {
            glDeleteTextures(1, PGLuint(@TextureCam.TexNum));
            TextureCam.TexNum = 0;
        }

        if (UIni::Ini.WebcamFlip == 0)
            cv::flip(WebcamFrame, WebcamFrame, 1);

        WebcamFrame = FrameAdjust(WebcamFrame);
        WebcamFrame = FrameEffect(UIni::Ini.WebcamEffect, WebcamFrame);

        TextureCam = Texture.CreateTexture(WebcamFrame.data, nullptr, WebcamFrame.cols, WebcamFrame.rows);

        WebcamFrame.deallocate();
    }
}

// 0  -> NORMAL
// 1  -> GRAYSCALE
// 2  -> BLACK & WHITE
// 3  -> NEGATIVE
// 4  -> BINARY IMAGE
// 5  -> DILATE
// 6  -> THRESHOLD
// 7  -> EDGES
// 8  -> GAUSSIAN BLUR
// 9  -> EQUALIZED
// 10 -> ERODE
cv::Mat TWebcam::FrameEffect(int Nr_Effect, cv::Mat Frame)
/*var
  Size: CvSize;
  CamEffectParam: int;
  ImageFrame, EffectFrame, DiffFrame, RGBFrame: cv::Mat;*/
{
    int CamEffectParam;
    // params values
    switch (Nr_Effect)
    {
    case 4: 
        CamEffectParam = 20; // binary image
        break;
    case 5: 
        CamEffectParam = 2;  // dilate
        break;
    case 6: 
        CamEffectParam = 60; // threshold
        break;
    case 7: 
        CamEffectParam = 70; // edges
        break;
    case 8: 
        CamEffectParam = 11; // gaussian blur
        break;
    case 10: 
        CamEffectParam = 2; // erode
        break;
    default: 
        CamEffectParam = 0;
    }

    Size = cvSizeV(Frame.width, Frame.height);

    ImageFrame = cvCreateImage(Size, Frame.depth, 1);
    EffectFrame = cvCreateImage(Size, Frame.depth, 1);
    DiffFrame = cvCreateImage(Size, Frame.depth, 1);
    RGBFrame = cvCreateImage(Size, Frame.depth, 3);

    switch (Nr_Effect)
    {
    case 1: 
    { // Grayscale
        cvCvtColor(Frame, EffectFrame, CV_BGR2GRAY);
        cvCvtColor(EffectFrame, RGBFrame, CV_GRAY2RGB);
        break;
    }
    case 2: 
    { // Black & White
        cvCvtColor(Frame, ImageFrame, CV_BGR2GRAY);
        cvThreshold(ImageFrame, EffectFrame, 128, 255, CV_THRESH_OTSU);
        cvCvtColor(EffectFrame, RGBFrame, CV_GRAY2RGB);
        break;
    }
    case 3: 
    { // Negative
        cvCvtColor(Frame, RGBFrame, CV_BGR2RGB);
        cvNot(RGBFrame, RGBFrame);
        break;
    }
    case 4: 
    { // Binary Image

        //Convert frame to gray and store in image
        cvCvtColor(Frame, ImageFrame, CV_BGR2GRAY);
        cvEqualizeHist(ImageFrame, ImageFrame);

        //Copy Image
        if (LastFrame == nil) then
            LastFrame = cvCloneImage(ImageFrame);

        //Differences with actual and last image
        cvAbsDiff(ImageFrame, LastFrame, DiffFrame);

        //threshold image
        cvThreshold(DiffFrame, EffectFrame, CamEffectParam, 255, 0);

        cvReleaseImage(@LastFrame);

        //Change datas;
        LastFrame = cvCloneImage(ImageFrame);

        cvCvtColor(EffectFrame, RGBFrame, CV_GRAY2RGB);
        break;
    }
    case 5: 
    { // Dilate
        cvDilate(Frame, Frame, nil, CamEffectParam);
        cvCvtColor(Frame, RGBFrame, CV_BGR2RGB);
        break;
    }
    case 6: 
    { //threshold
        cvCvtColor(Frame, ImageFrame, CV_BGR2GRAY);
        cvThreshold(ImageFrame, EffectFrame, CamEffectParam, 100, 3);
        cvCvtColor(EffectFrame, RGBFrame, CV_GRAY2RGB);
        break;
    }
    case 7: 
    { // Edges
        cvCvtColor(Frame, ImageFrame, CV_BGR2GRAY);
        cvCanny(ImageFrame, EffectFrame, CamEffectParam, CamEffectParam, 3);
        cvCvtColor(EffectFrame, RGBFrame, CV_GRAY2RGB);
        break;
    }
    case 8: 
    { // Gaussian Blur
        cvSmooth(Frame, Frame, CV_BLUR, CamEffectParam, CamEffectParam);
        cvCvtColor(Frame, RGBFrame, CV_BGR2RGB);
        break;
    }
    case 9: 
    { // Equalized
        cvCvtColor(Frame, ImageFrame, CV_BGR2GRAY);
        cvEqualizeHist(ImageFrame, EffectFrame);
        cvCvtColor(EffectFrame, RGBFrame, CV_GRAY2RGB);
        break;
    }
    case 10:
    { // Erode
        cvErode(Frame, Frame, nil, CamEffectParam);
        cvCvtColor(Frame, RGBFrame, CV_BGR2RGB);
        break;
    }
           /*
           11:{ // Color
                RGBFrame = cvCreateImage(Size, Frame.depth, 3);

                cvAddS(Frame, CV_RGB(255, 0, 0), Frame);
                cvCvtColor(Frame, RGBFrame, CV_BGR2RGB);
              }

           12:{ // Value
                ImageFrame = cvCreateImage(Size, Frame.depth, 1);
                RGBFrame = cvCreateImage(Size, Frame.depth, 3);

                // Convert from Red-Green-Blue to Hue-Saturation-Value
                cvCvtColor(Frame, RGBFrame, CV_BGR2HSV );

                // Split hue, saturation and value of hsv on them
                cvSplit(RGBFrame, nil, nil, ImageFrame, 0);
                cvCvtColor(ImageFrame, RGBFrame, CV_GRAY2RGB);
              }
              */
    default:
    {
        //normal
        cvCvtColor(Frame, RGBFrame, CV_BGR2RGB);
    }
    }

    cvReleaseImage(@ImageFrame);
    cvReleaseImage(@DiffFrame);
    cvReleaseImage(@EffectFrame);
    cvReleaseImage(@Frame);
    return RGBFrame;
}

function TWebcam::FrameAdjust(Frame: cv::Mat): cv::Mat;
var
  BrightValue, SaturationValue, HueValue: int;
  C, S: real;
{

  BrightValue = Ini.WebcamBrightness;
  SaturationValue = Ini.WebCamSaturation;
  HueValue = Ini.WebCamHue;

  if (BrightValue != 100) or (SaturationValue != 100) or (HueValue != 180) then
  {
    C = (HueValue - 180) * Pi / 180;
    S = sin(C) * SaturationValue / 100;
    C = cos(C) * SaturationValue / 100;
    cvSetZero(PCvArr(Mat1));
    cvSetReal2D(PCvArr(Mat1), 0, 0, BrightValue / 100);
    cvSetReal2D(PCvArr(Mat1), 1, 1, C);
    cvSetReal2D(PCvArr(Mat1), 1, 2, S);
    cvSetReal2D(PCvArr(Mat1), 2, 1, -S);
    cvSetReal2D(PCvArr(Mat1), 2, 2, C);
    cvGEMM(PCvArr(Mat1), PCvArr(BGR2YUV), 1, nil, 0, PCvArr(Mat2));
    cvGEMM(PCvArr(YUV2BGR), PCvArr(Mat2), 1, nil, 0, PCvArr(Mat1));
    cvTransform(PCvArr(Frame), PCvArr(Frame), Mat1);
  }

  Result = Frame;
}

//----------

};