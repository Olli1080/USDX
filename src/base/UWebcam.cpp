#include "UWebcam.h"

#include "UIni.h"

namespace UWebcam
{
    //----------
//Create - Construct Class
//----------
TWebcam::TWebcam() : IsEnabled(false)
{
    Mat1 = cv::Mat(3, 3, CV_32FC1);
    Mat2 = cv::Mat(3, 3, CV_32FC1);

    BGR2YUV = (cv::Mat_<float>(3, 3) << 
        0.114,                  0.587,                  0.299, 
        1,                      -0.587 / (1 - 0.114),   -0.299 / (1 - 0.114), 
        -0.114 / (1 - 0.299),   -0.587 / (1 - 0.299),   1);
    YUV2BGR = BGR2YUV.inv();
}

TWebcam::~TWebcam()
{
    Release();
}

void TWebcam::Start()
{
    if (UIni::Ini.webcam.ID == 0)
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
        CaptureThread = std::make_unique<std::thread>([this]()
            {
                CaptureLoop();
            });
    }
}

void TWebcam::Release()
{
    if (IsEnabled)
    {
        {
            std::unique_lock lock(Mutex);
            IsEnabled = false;
            StopCond.notify_all();
        }
        CaptureThread->join();
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
    {
    }
}

int TWebcam::CaptureLoop()
{
    std::unique_lock lock(Mutex);
    while (IsEnabled)
    {
        lock.unlock();

        LastTickFrame = SDL_GetTicks();

        cv::Mat WebcamFrame;
        Capture->read(WebcamFrame);

        lock.lock();
        if (!WebcamFrame.empty())
            CurrentFrame = WebcamFrame.clone();

        const Uint32 Now = SDL_GetTicks();
        if (IsEnabled && Now - LastTickFrame < Interval)
        {
            StopCond.wait_for(lock, std::chrono::milliseconds(Interval - (Now - LastTickFrame)));
        }
    }
    lock.unlock();
    return 0;
}

void TWebcam::GetWebcamFrame()
{
    cv::Mat WebcamFrame;

    if (IsEnabled)
    {
        std::unique_lock lock(Mutex);
        WebcamFrame = CurrentFrame;
        CurrentFrame = cv::Mat();
        Interval = 1000 / std::stoi(UIni::IWebcamFPS[UIni::Ini.WebcamFPS]);
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

        TextureCam = UTexture::Texture.CreateTexture(WebcamFrame.data, nullptr, WebcamFrame.cols, WebcamFrame.rows);

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
cv::Mat TWebcam::FrameEffect(Effect effect, cv::Mat Frame)
{
    int CamEffectParam;
    // params values
    switch (effect)
    {
    case Effect::BINARY_IMAGE:
        CamEffectParam = 20; // binary image
        break;
    case Effect::DILATE:
        CamEffectParam = 2;  // dilate
        break;
    case Effect::THRESHOLD:
        CamEffectParam = 60; // threshold
        break;
    case Effect::EDGES:
        CamEffectParam = 70; // edges
        break;
    case Effect::GAUSSIAN_BLUR:
        CamEffectParam = 11; // gaussian blur
        break;
    case Effect::ERODE:
        CamEffectParam = 2; // erode
        break;
    default:
        CamEffectParam = 0;
    }

    //auto Size = Frame.size cv::Size(Frame.cols width, Frame.height);

    //ImageFrame = cvCreateImage(Frame.size, Frame.depth, 1);
    //EffectFrame = cvCreateImage(Frame.size, Frame.depth, 1);
    //DiffFrame = cvCreateImage(Frame.size, Frame.depth, 1);
    //RGBFrame = cvCreateImage(Frame.size, Frame.depth, 3);

    cv::Mat ImageFrame, EffectFrame, RGBFrame;

    switch (effect)
    {
    case Effect::GRAYSCALE:
    { // Grayscale
        cv::cvtColor(Frame, EffectFrame, cv::COLOR_BGR2GRAY);
        cv::cvtColor(EffectFrame, RGBFrame, cv::COLOR_GRAY2RGB);
        break;
    }
    case Effect::BLACK_AND_WHITE:
    { // Black & White
        cv::cvtColor(Frame, ImageFrame, cv::COLOR_BGR2GRAY);
        cv::threshold(ImageFrame, EffectFrame, 128, 255, cv::THRESH_OTSU);
        cv::cvtColor(EffectFrame, RGBFrame, cv::COLOR_GRAY2RGB);
        break;
    }
    case Effect::NEGATIVE:
    { // Negative
        cv::cvtColor(Frame, RGBFrame, cv::COLOR_BGR2RGB);
        cv::bitwise_not(RGBFrame, RGBFrame);
        break;
    }
    case Effect::BINARY_IMAGE:
    {
        cv::Mat DiffFrame;
        // Binary Image
        //Convert frame to gray and store in image
        cv::cvtColor(Frame, ImageFrame, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(ImageFrame, ImageFrame);

        //Copy Image
        if (LastFrame.empty())
            LastFrame = ImageFrame.clone();

        //Differences with actual and last image
        cv::absdiff(ImageFrame, LastFrame, DiffFrame);

        //threshold image
        cv::threshold(DiffFrame, EffectFrame, CamEffectParam, 255, 0);

        //Change data;
        LastFrame = ImageFrame.clone();

        cv::cvtColor(EffectFrame, RGBFrame, cv::COLOR_GRAY2RGB);
        break;
    }
    case Effect::DILATE:
    { // Dilate
        cv::dilate(Frame, Frame, cv::Mat(), cv::Point(-1, -1), CamEffectParam);
        cv::cvtColor(Frame, RGBFrame, cv::COLOR_BGR2RGB);
        break;
    }
    case Effect::THRESHOLD:
    { //threshold
        cv::cvtColor(Frame, ImageFrame, cv::COLOR_BGR2GRAY);
        cv::threshold(ImageFrame, EffectFrame, CamEffectParam, 100, 3);
        cv::cvtColor(EffectFrame, RGBFrame, cv::COLOR_GRAY2RGB);
        break;
    }
    case Effect::EDGES:
    { // Edges
        cv::cvtColor(Frame, ImageFrame, cv::COLOR_BGR2GRAY);
        cv::Canny(ImageFrame, EffectFrame, CamEffectParam, CamEffectParam, 3);
        cv::cvtColor(EffectFrame, RGBFrame, cv::COLOR_GRAY2RGB);
        break;
    }
    case Effect::GAUSSIAN_BLUR:
    { // Gaussian Blur
        cv::blur(Frame, Frame, cv::Size(CamEffectParam, CamEffectParam));
        cv::cvtColor(Frame, RGBFrame, cv::COLOR_BGR2RGB);
        break;
    }
    case Effect::EQUALIZED:
    { // Equalized
        cv::cvtColor(Frame, ImageFrame, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(ImageFrame, EffectFrame);
        cv::cvtColor(EffectFrame, RGBFrame, cv::COLOR_GRAY2RGB);
        break;
    }
    case Effect::ERODE:
    { // Erode
        cv::erode(Frame, Frame, cv::Mat(), cv::Point(-1, -1), CamEffectParam);
        cv::cvtColor(Frame, RGBFrame, cv::COLOR_BGR2RGB);
        break;
    }
    /*
    11:{ // Color
         RGBFrame = cvCreateImage(Size, Frame.depth, 3);

         cvAddS(Frame, CV_RGB(255, 0, 0), Frame);
         cv::cvtColor(Frame, RGBFrame, cv::COLOR_BGR2RGB);
       }

    12:{ // Value
         ImageFrame = cvCreateImage(Size, Frame.depth, 1);
         RGBFrame = cvCreateImage(Size, Frame.depth, 3);

         // Convert from Red-Green-Blue to Hue-Saturation-Value
         cv::cvtColor(Frame, RGBFrame, CV_BGR2HSV );

         // Split hue, saturation and value of hsv on them
         cvSplit(RGBFrame, nil, nil, ImageFrame, 0);
         cv::cvtColor(ImageFrame, RGBFrame, cv::COLOR_GRAY2RGB);
       }
       */
    default:
    {
        //normal
        cv::cvtColor(Frame, RGBFrame, cv::COLOR_BGR2RGB);
    }
    }
    return RGBFrame;
}

cv::Mat TWebcam::FrameAdjust(cv::Mat Frame)
{
    const int BrightValue = UIni::Ini.WebcamBrightness;
    const int SaturationValue = UIni::Ini.WebcamSaturation;
    const int HueValue = UIni::Ini.WebcamHue;

    if (BrightValue != 100 || SaturationValue != 100 || HueValue != 180)
    {
        float C = (HueValue - 180.f) * std::numbers::pi / 180.f;
        float S = std::sin(C) * SaturationValue / 100.f;
        C = std::cos(C) * SaturationValue / 100;

        Mat1 = cv::Mat::zeros(3, 3, CV_32FC1);
        Mat1.at<float>(0, 0) = BrightValue / 100;
        Mat1.at<float>(1, 1) = C;
        Mat1.at<float>(1, 2) = S; //maybe have to switch positions
        Mat1.at<float>(2, 1) = -S;
        Mat1.at<float>(2, 2) = C;
        cv::gemm(Mat1, BGR2YUV, 1, cv::Mat(), 0, Mat2);
        cv::gemm(YUV2BGR, Mat2, 1, cv::Mat(), 0, Mat1);
        cv::transform(Frame, Frame, Mat1);
    }

    return Frame;
}
}