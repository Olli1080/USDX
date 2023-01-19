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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/media/UMediaCore_FFmpeg.pas $
 * $Id: UMediaCore_FFmpeg.pas 3103 2014-11-22 23:21:19Z k-m_schindler $
 */
#include "../switches.h"

#include <string>
#include <optional>
#include <list>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <filesystem>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include "../base/UMusic.h"

namespace UMediaCore_FFmpeg
{
    /*uses
      Classes,
      ctypes,
      sdl2,
      avcodec,
      avformat,
      avutil,
      avio,
      swscale,
      UMusic,
      ULog,
      UPath;*/

      //type
        //PPacketQueue = ^TPacketQueue;
    struct AVPacketDeleter
    {
        void operator()(AVPacket* packet)
        {
            av_packet_free(&packet);
            //av_packet_unref()
        }
    };
    typedef std::unique_ptr<AVPacket, AVPacketDeleter> AVPacketU;
    template<typename T, typename K>
    T makePacket(K packet)
    {
        return T(packet, AVPacketDeleter());
    }


    struct AVFormatContextDeleter
    {
        void operator()(AVFormatContext* ctx)
        {
            avformat_close_input(&ctx);
            //avformat_free_context(ctx);
        }
    };
    typedef std::unique_ptr<AVFormatContext, AVFormatContextDeleter> AVFormatContextU;
    template<typename T, typename K>
    T makeFormatContext(K ctx)
    {
        return T(ctx, AVFormatContextDeleter());
    }
    /*
    struct AVIOContextDeleter
    {
        void operator()(AVIOContext* ctx)
        {
            avio_context_free(ctx);
        }
    };*/
    //typedef std::unique_ptr<AVFormatContext, AVFormatContextDeleter> AVFormatContextU;

    struct AVMemDeleter
    {
        void operator()(void* buffer)
        {
            av_free(buffer);
        }
    };
    template<typename T>
    using AVMemU = std::unique_ptr<T, AVMemDeleter>;

    template<typename T, typename K>
    T makeMem(K* mem)
    {
        return T(mem, AVMemDeleter());
    }

    inline std::mutex codecMtx;
    struct AVCodecContextDeleter
    {
        void operator()(AVCodecContext* ctx)
        {
            std::unique_lock lock(codecMtx);
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 0, 0)
            avcodec_close(fCodecCtx);
#else
            avcodec_free_context(&ctx);
#endif
        }
    };
    typedef std::unique_ptr<AVCodecContext, AVCodecContextDeleter> AVCodecContextU;

    template<typename T>
    T GetCodecContext(AVStream* stream, AVCodec* codec)
    {
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 76, 100)
        return T(stream->codec, AVCodecContextDeleter())

#else

        auto res = T(avcodec_alloc_context3(codec), AVCodecContextDeleter());
        if (res && avcodec_parameters_to_context(res.get(), stream->codecpar) < 0)
            res.reset();
        if (!res)
            ULog::Log.LogErrorLocation("Failed to allocate context");
        return res;
#endif
    }

    int open_avcodec(const AVCodecContextU& ctx, AVCodec* codec)
    {
        std::unique_lock lock(codecMtx);
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 5, 0)
        return avcodec_open2(ctx.get(), codec, nullptr);
#else
        return avcodec_open(ctx.get(), codec);
#endif
    }

    class TPacketQueue
    {
    private:

        std::queue<AVPacketU> entries;
        //int PacketCount; //packet count is entries.size
        std::mutex Mutex;
        std::condition_variable Condition;

        int Size = 0;
        bool AbortRequest;

    public:

        TPacketQueue();
        ~TPacketQueue();

        int Put(AVPacketU&& Packet);
        int PutStatus(int StatusFlag, Pointer StatusInfo);
        void FreeStatusInfo(TAVPacket& Packet);
        Pointer GetStatusInfo(TAVPacket& Packet);
        int Get(TAVPacket& Packet, bool Blocking);
        int GetSize();
        void Flush();
        void Abort();
        bool IsAborted();
    };

    const std::string STATUS_PACKET = "STATUS_PACKET";
    constexpr int PKT_STATUS_FLAG_EOF = 1; // signal end-of-file
    constexpr int PKT_STATUS_FLAG_FLUSH = 2; // request the decoder to flush its avcodec decode buffers
    constexpr int PKT_STATUS_FLAG_ERROR = 3; // signal an error state
    constexpr int PKT_STATUS_FLAG_EMPTY = 4; // request the decoder to output empty data (silence or black frames)

    class Stream_Wrapper
    {
    public:

        typedef std::ratio<1, AV_TIME_BASE> AVTimeRatio;
        typedef std::chrono::duration<long long, AVTimeRatio> AVDuration;

        Stream_Wrapper() = default;
        Stream_Wrapper(const std::filesystem::path& filename)
        {
            auto res = AVFormatOpenInput(filename);
            if (res < 0)
                throw std::exception(std::to_string(res).c_str());
        }
        ~Stream_Wrapper()
        {
            AVFormatCloseInput();
        }

        int AVFormatOpenInput(const std::filesystem::path& filename);

        void AVFormatCloseInput();

        bool is_open() { return stream.is_open(); }
        int& flags() { return formatCtx->flags; }

        int findStreamInfo()
        {
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(53, 2, 0)
            return avformat_find_stream_info(formatCtx.get(), nullptr);
#else
            return av_find_stream_info(formatCtx.get());
#endif
        }

        std::optional<int> findAudioStreamIndex()
        {
            // find the first audio stream
            for (size_t i = 0; i < formatCtx->nb_streams; ++i)
            {
                auto& Stream = formatCtx->streams[i];

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(52, 64, 0) // < 52.64.0
                if (Stream->codec->codec_type == CODEC_TYPE_AUDIO)
#elif defined(FF_API_LAVF_AVCTX) && FF_API_LAVF_AVCTX
                if (Stream->codec->codec_type == AVMEDIA_TYPE_AUDIO)
#else
                if (Stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
#endif
                {
                    return i;
                }
            }
            return std::nullopt;
        }

        [[nodiscard]] AVStream* get_stream(int idx) const
        {
            return formatCtx->streams[idx];
        }

        [[nodiscard]] AVDuration get_start_time() const
        {
            return AVDuration(formatCtx->start_time);
        }

        [[nodiscard]] AVDuration get_duration() const
        {
            return AVDuration(formatCtx->duration);
        }

        [[nodiscard]] AVDuration get_end_time() const
        {
            return get_start_time() + get_end_time();
        }

        static AVDuration invalid_time()
        {
            return AVDuration(AV_NOPTS_VALUE);
        }

    private:

        static constexpr int BLOCKSIZE = 4 * 1024;

        static int readStreamWrapper(void* opaque, uint8_t* buf, int buf_size);
        int readStream(uint8_t* buf, int buf_size);

        static int64_t seekWrapper(void* opaque, int64_t offset, int whence);
        int64_t seek(int64_t offset, int whence);

        std::filesystem::path filename;
        std::basic_fstream<uint8_t> stream;
        AVFormatContextU formatCtx;
        AVMemU<uint8_t> buffer;
    };

    static std::optional<UMusic::TAudioSampleFormat> ConvertFFmpegToAudioFormat(AVSampleFormat FFmpegFormat);
    static std::optional<AVSampleFormat> ConvertAudioFormatToFFmpeg(UMusic::TAudioSampleFormat Format);
    static std::string GetErrorString(int ErrorNum);

    //type
    class TMediaCore_FFmpeg
    {
    private:

        std::mutex AVCodecLock;

    public:

        TMediaCore_FFmpeg();
        ~TMediaCore_FFmpeg();
        static TMediaCore_FFmpeg GetInstance();

        bool FindStreamIDs(const AVFormatContext& FormatCtx, int& FirstVideoStream, int& FirstAudioStream);
        int FindAudioStreamIndex(const AVFormatContext& FormatCtx);
        std::unique_lock<std::mutex>&& LockAVCodec();
        //int AVFormatOpenInput(AVFormatContextU& ps, const std::filesystem::path& filename);
        //void AVFormatCloseInput(ps: PPAVFormatContext);
        //function GetCodecContext(stream: PAVStream; codec: PAVCodec) : PAVCodecContext;
    };

    /*
    implementation

    uses
      SysUtils,
      UConfig;
    */
    /*
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
// redeclaration of constants with the same names as deprecated
  //constants in order to reuse old callback definitions
const
  URL_RDONLY = 0; //**< read-only *)
  URL_WRONLY = 1; //**< write-only *)
  URL_RDWR   = 2; //**< read-write *)
  BLOCKSIZE  = 4 * 1024;
#else
function FFmpegStreamOpen(h: PURLContext; filename: PAnsiChar; flags: cint): cint; cdecl; forward;
function FFmpegStreamRead(h: PURLContext; buf: PByteArray; size: cint): cint; cdecl; forward;
function FFmpegStreamWrite(h: PURLContext; buf: PByteArray; size: cint): cint; cdecl; forward;
function FFmpegStreamSeek(h: PURLContext; pos: cint64; whence: cint): cint64; cdecl; forward;
function FFmpegStreamClose(h: PURLContext): cint; cdecl; forward;

const
  UTF8FileProtocol: TURLProtocol = (
      name:      "ufile";
      url_open:  FFmpegStreamOpen;
      url_read:  FFmpegStreamRead;
      url_write: FFmpegStreamWrite;
      url_seek:  FFmpegStreamSeek;
      url_close: FFmpegStreamClose;
  );
#endif
*/
    inline TMediaCore_FFmpeg Instance;
    /*
    void CheckVersions()
    {
      libVersion = avcodec_version();
      headerVersion = LIBAVCODEC_VERSION_INT;
      if (libVersion <> headerVersion)
      {
        Log.LogError(Format("%s header (%s) and DLL (%s) versions do not match.",
            ["libavcodec", hexVerToStr(headerVersion), hexVerToStr(libVersion)]));
      }

      #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(52, 20, 0) // 52.20.0
        libVersion = avformat_version();
        headerVersion = LIBAVFORMAT_VERSION_INT;
      if (libVersion <> headerVersion)
      {
        Log.LogError(Format("%s header (%s) and DLL (%s) versions do not match.",
            ["libavformat", hexVerToStr(headerVersion), hexVerToStr(libVersion)]));
      }
      #endif

      #if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(49, 8, 0) // 49.8.0
      libVersion = avutil_version();
      headerVersion = AV_VERSION_INT(
          LIBAVUTIL_VERSION_MAJOR,
          LIBAVUTIL_VERSION_MINOR,
          LIBAVUTIL_VERSION_RELEASE);
      if (libVersion <> headerVersion)
      {
        Log.LogError(Format("%s header (%s) and DLL (%s) versions do not match.",
            ["libavutil", hexVerToStr(headerVersion), hexVerToStr(libVersion)]));
      }
      #endif

      #if LIBSWSCALE_VERSION_INT >= AV_VERSION_INT(0, 6, 1) // 0.6.1
      libVersion = swscale_version();
      headerVersion = AV_VERSION_INT(
          LIBSWSCALE_VERSION_MAJOR,
          LIBSWSCALE_VERSION_MINOR,
          LIBSWSCALE_VERSION_RELEASE);
      if (libVersion <> headerVersion)
      {
        Log.LogError(Format("%s header (%s) and DLL (%s) versions do not match.",
            ["libswscale", hexVerToStr(headerVersion), hexVerToStr(libVersion)]));
      }
      #endif
    }*/
}