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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/media/UAudioDecoder_FFmpeg.pas $
 * $Id: UAudioDecoder_FFmpeg.pas 3107 2014-11-23 00:02:56Z k-m_schindler $
 */
#include "../switches.h"

#include <string>
#include <vector>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "UMediaCore_FFmpeg.h"

namespace UAudioDecoder_FFmpeg
{
/*******************************************************************************
 *
 * This unit is primarily based upon -
 *   http://www.dranger.com/ffmpeg/ffmpegtutorial_all.html
 *
 *   and tutorial03.c
 *
 *   http://www.inb.uni-luebeck.de/~boehme/using_libavcodec.html
 *
 *******************************************************************************/

// show FFmpeg specific debug output
//#define DebugFFmpegDecode

// FFmpeg is very verbose and shows a bunch of errors.
// Those errors (they can be considered as warnings by us) can be ignored
// as they do not give any useful information.
// There is no solution to fix this except for turning them off.

//#define EnableFFmpegErrorOutput

    /*
uses
  sdl2, // SDL redefines some base types -> include before SysUtils to ignore them
  Classes,
  Math,
  SysUtils,
  avcodec,
  avformat,
  avutil,
  avio,
	ctypes,
  rational,
#ifdef UseSWResample
  swresample,
#endif
  UMusic,
  UIni,
  UMain,
  UMediaCore_FFmpeg,
  ULog,
  UCommon,
  UConfig,
  UPath;
*/

#ifdef UseSWResample
  #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(54, 41, 100}
    #define UseFrameDecoderAPI
    #define ConvertPlanar
  #endif
#else
  #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 0, 0)
    #define UseFrameDecoderAPI
  #endif
#endif

constexpr int MAX_AUDIOQ_SIZE = (5 * 16 * 1024);

const int
  // TODO: The factor 3/2 might not be necessary as we do not need extra
  // space for synchronizing as in the tutorial.
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(20, 0, 0)
  AUDIO_BUFFER_SIZE = (192000 * 3) / 2;
#else
  AUDIO_BUFFER_SIZE = (AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2;
#endif

//type
class TFFmpegDecodeStream : public UMusic::TAudioDecodeStream
{
private:

    std::mutex fStateLock;

    bool fEOFState = false; // end-of-stream flag (locked by StateLock)
    bool fErrorState = false; // error flag (locked by StateLock)

    bool fQuitRequest = false; // (locked by StateLock)
    std::condition_variable fParserIdleCond;

    // parser pause/resume data
    bool fParserLocked = false;
    int fParserPauseRequestCount = 0;
    std::condition_variable fParserUnlockedCond;
    std::condition_variable fParserResumeCond;

    bool fSeekRequest = false; // (locked by StateLock)
    int fSeekFlags = 0; // (locked by StateLock)
    SecDouble fSeekPos = SecDouble::zero();    // stream position to seek for (in secs) (locked by StateLock)
    bool fSeekFlush = false;   // true if the buffers should be flushed after seeking (locked by StateLock)
    std::condition_variable SeekFinishedCond;

    bool fLoop = false; // (locked by StateLock)

    std::unique_ptr<std::thread> fParseThread;
    UMediaCore_FFmpeg::TPacketQueue fPacketQueue;

    UMusic::TAudioFormatInfo fFormatInfo;
#ifdef UseFrameDecoderAPI
    int fBytesPerSample;
#endif
#ifdef ConvertPlanar
    PSwrContext fSwrContext;
#endif

    // FFmpeg specific data
    UMediaCore_FFmpeg::AVFormatContextU fFormatCtx;
    UMediaCore_FFmpeg::AVCodecContextU fCodecCtx;
    AVCodec* fCodec;

    int fAudioStreamIndex;
    UMediaCore_FFmpeg::Stream_Wrapper fAudioStream;
    double fAudioStreamPos; // stream position in seconds (locked by DecoderLock)

    // decoder pause/resume data
    bool fDecoderLocked;
    int fDecoderPauseRequestCount;
    std::condition_variable fDecoderUnlockedCond;
    std::condition_variable fDecoderResumeCond;

    // state-vars for DecodeFrame (locked by DecoderLock)
    UMediaCore_FFmpeg::AVPacketU fAudioPaket;
    std::vector<uint8_t> fAudioPaketData;
    int fAudioPaketSize;
    int fAudioPaketSilence; // number of bytes of silence to return

    // state-vars for AudioCallback (locked by DecoderLock)
    int fAudioBufferPos;
    int fAudioBufferSize;
    std::vector<uint8_t> fAudioBuffer;
#ifdef UseFrameDecoderAPI
    fAudioBufferFrame : PAVFrame;
#endif

    std::filesystem::path fFilename;

    void SetPositionIntern(double Time, bool Flush);
    void SetEOF(bool State);
    void SetError(bool State);
    bool IsSeeking();
    bool IsQuit();
    bool GetLoopInternal();

    void Reset();

    void Parse();
    bool ParseLoop();
    bool PauseParser();
    void ResumeParser();

    int DecodeFrame();
    void FlushCodecBuffers();
    void PauseDecoderUnlocked();
    void ResumeDecoderUnlocked();
    void PauseDecoder();
    void ResumeDecoder();

public:

    TFFmpegDecodeStream();
    ~TFFmpegDecodeStream() override;

    bool Open(const std::filesystem::path& Filename) override;
    void Close() override;

    double GetLength() override;
    UMusic::TAudioFormatInfo GetAudioFormatInfo() override;
    double GetPosition() override;
    void SetPosition(double Time) override;
    bool GetLoop() override;
    void SetLoop(bool Enabled) override;
    bool IsEOF() override;
    bool IsError() override;

    int ReadData(std::vector<uint8_t>& Buffer) override;
};

class TAudioDecoder_FFmpeg : public UMusic::IAudioDecoder
{
public:

    std::string GetName() override;

    bool InitializeDecoder() override;
    bool FinalizeDecoder() override;
    UMusic::TAudioDecodeStream::SPtr Open(const std::filesystem::path& Filename) override;
};

UMediaCore_FFmpeg::TMediaCore_FFmpeg FFmpegCore;

function ParseThreadMain(Data: Pointer);


}