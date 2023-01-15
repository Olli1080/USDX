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
    }
  };
  typedef std::unique_ptr<AVPacket, AVPacketDeleter> AVPacketU;
  template<typename T>
  T make(AVPacket* packet)
  {
    return T(packet, AVPacketDeleter());
  }


  struct AVFormatContextDeleter
  {
    void operator()(AVFormatContext* ctx)
    {
      avformat_free_context(ctx);
    }
  };
  typedef std::unique_ptr<AVFormatContext, AVFormatContextDeleter> AVFormatContextU;
  template<typename T>
  T make(AVFormatContext* ctx)
  {
    return T(ctx, AVFormatContextDeleter());
  }


  class TPacketQueue
  {
    private:

      std::queue<AVPacketU> entries;
      int PacketCount;
      std::mutex Mutex;
      std::condition_variable Condition;
      //int Size;
      bool AbortRequest;

    public:

      TPacketQueue();
      ~TPacketQueue();

      int Put(AVPacketU Packet);
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
constexpr int PKT_STATUS_FLAG_EOF     = 1; // signal end-of-file
constexpr int PKT_STATUS_FLAG_FLUSH   = 2; // request the decoder to flush its avcodec decode buffers
constexpr int PKT_STATUS_FLAG_ERROR   = 3; // signal an error state
constexpr int PKT_STATUS_FLAG_EMPTY   = 4; // request the decoder to output empty data (silence or black frames)

class Stream_Wrapper
{
public:

  Stream_Wrapper() = default;
  Stream_Wrapper(const std::filesystem::path& filename)
  {
    AVFormatOpenInput();
  }

  int AVFormatOpenInput(AVFormatContextU& ps, const std::filesystem::path& filename)
  {
    auto Stream = std::basic_ifstream<uint8_t>(filename, std::ios::binary);
    if (!Stream.is_open())
      return AVERROR_STREAM_NOT_FOUND;

    ps = make<AVFormatContextU>(avformat_alloc_context());
    auto buffer = static_cast<uint8_t*>(av_malloc(BLOCKSIZE));
    //FFmpegStreamOpen(h, filename, URL_RDONLY);

    ps->pb = avio_alloc_context(buffer, BLOCKSIZE, 0, this, 
      &Stream_Wrapper::readStreamWrapper, 
      nullptr, 
      &Stream_Wrapper::seekWrapper
      );
    return avformat_open_input(ps.release(), filename.c_str(), nullptr, nullptr);
  }
  void AVFormatCloseInput(PPAVFormatContext ps);

private:

  static constexpr int BLOCKSIZE = 4 * 1024;

  static int readStreamWrapper(void* opaque, uint8_t* buf, int buf_size)
  {
    auto instance = static_cast<Stream_Wrapper*>(opaque);
    return instance->readStream(buf, buf_size);
  }

  int readStream(uint8_t* buf, int buf_size)
  {
    //Read
    //auto& Stream = *reinterpret_cast<TypeStream*>(opaque);
    stream.read(buf, buf_size);

    if (stream.fail())
      return -1;
    if (stream.eof())
      return AVERROR_EOF;
    return 0;
  }

  static int64_t seekWrapper(void* opaque, int64_t offset, int whence)
  {
    auto instance = static_cast<Stream_Wrapper*>(opaque);
    return instance->seek(offset, whence);
  }

  int64_t seek(int64_t offset, int whence)
  {
    //Seek
    switch (whence)
    {
      case SEEK_SET:
        stream.seekg(offset, std::ios::beg);
        break;
      case SEEK_CUR:
        stream.seekg(offset, std::ios::cur);
        break;
      case SEEK_END:
        stream.seekg(offset, std::ios::end);
        break;
      case AVSEEK_SIZE:
        return std::filesystem::file_size(filename);        
        break;
    }
    return stream.tellg();
  }

  std::filesystem::path filename;
  std::basic_fstream<uint8_t> stream;
};

//type
  class TMediaCore_FFmpeg
  {
    private:

      std::mutex AVCodecLock;

    public:

      TMediaCore_FFmpeg();
      ~TMediaCore_FFmpeg();
      static TMediaCore_FFmpeg GetInstance();

      std::string GetErrorString(int ErrorNum);
      bool FindStreamIDs(const AVFormatContext& FormatCtx, out FirstVideoStream, FirstAudioStream: int );
      int FindAudioStreamIndex(const AVFormatContext& FormatCtx);
      static std::optional<UMusic::TAudioSampleFormat> ConvertFFmpegToAudioFormat(AVSampleFormat FFmpegFormat);
      static std::optional<AVSampleFormat> ConvertAudioFormatToFFmpeg(UMusic::TAudioSampleFormat Format);
      std::unique_lock<std::mutex>&& LockAVCodec();
      int AVFormatOpenInput(AVFormatContextU& ps, const std::filesystem::path& filename);
      void AVFormatCloseInput(ps: PPAVFormatContext);
      function GetCodecContext(stream: PAVStream; codec: PAVCodec): PAVCodecContext;
  };

/*
implementation

uses
  SysUtils,
  UConfig;
*/
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
/* redeclaration of constants with the same names as deprecated
  constants in order to reuse old callback definitions */
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

TMediaCore_FFmpeg::TMediaCore_FFmpeg()
{
  //CheckVersions();

  #if LIBAVFORMAT_VERSION_INT <= AV_VERSION_INT(52, 111, 0) // 52.110.0 ??
  av_register_protocol(@UTF8FileProtocol);
  #elif LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(54, 29, 104)
  av_register_protocol2(@UTF8FileProtocol, sizeof(UTF8FileProtocol));
  #endif

}

TMediaCore_FFmpeg::~TMediaCore_FFmpeg()
{
  AVCodecLock.unlock();
}

class TMediaCore_FFmpeg TMediaCore_FFmpeg::GetInstance()
{
  if (not Assigned(Instance)) 
    Instance = TMediaCore_FFmpeg::Create();
  Result = Instance;
}

std::unique_lock<std::mutex>&& TMediaCore_FFmpeg::LockAVCodec()
{
  return std::unique_lock(AVCodecLock);
}

std::string TMediaCore_FFmpeg::GetErrorString(int ErrorNum)
{
#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(50, 43, 0) // < 50.43.0
  case ErrorNum of
    AVERROR_IO:           Result = "AVERROR_IO";
    AVERROR_NUMEXPECTED:  Result = "AVERROR_NUMEXPECTED";
    AVERROR_INVALIDDATA:  Result = "AVERROR_INVALIDDATA";
    AVERROR_NOMEM:        Result = "AVERROR_NOMEM";
    AVERROR_NOFMT:        Result = "AVERROR_NOFMT";
    AVERROR_NOTSUPP:      Result = "AVERROR_NOTSUPP";
    AVERROR_NOENT:        Result = "AVERROR_NOENT";
    AVERROR_PATCHWELCOME: Result = "AVERROR_PATCHWELCOME";
    else                  Result = "AVERROR_#"+inttostr(ErrorNum);
  }
#else
#endif
  return "";
}

/*
  @param(FormatCtx is a PAVFormatContext returned from av_open_input_file )
  @param(FirstVideoStream is an OUT value of type int, this is the index of the video stream)
  @param(FirstAudioStream is an OUT value of type int, this is the index of the audio stream)
  @returns(@true on success, @false otherwise)
*/
bool TMediaCore_FFmpeg::FindStreamIDs(const AVFormatContext& FormatCtx, int& FirstVideoStream, int& FirstAudioStream)
{
  // find the first video stream
  FirstAudioStream = -1;
  FirstVideoStream = -1;

  for (size_t i = 0; i < FormatCtx.nb_streams; ++i)
  {
    auto& Stream = *FormatCtx.streams[i];

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(52, 64, 0) // < 52.64.0
    if (Stream.codec.codec_type = CODEC_TYPE_VIDEO) and
       (FirstVideoStream < 0) 
    {
      FirstVideoStream = i;
    }

    if (Stream.codec.codec_type = CODEC_TYPE_AUDIO) and
       (FirstAudioStream < 0) 
    {
      FirstAudioStream = i;
    }
#elif LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(59, 0, 0)
    if (Stream.codec.codec_type = AVMEDIA_TYPE_VIDEO) and
       (FirstVideoStream < 0) 
    {
      FirstVideoStream = i;
    }

    if (Stream.codec.codec_type = AVMEDIA_TYPE_AUDIO) and
       (FirstAudioStream < 0) 
    {
      FirstAudioStream = i;
    }
#else
    if (Stream.codecpar->codec_type == AVMEDIA_TYPE_VIDEO &&
       FirstVideoStream < 0)
    {
      FirstVideoStream = i;
    }

    if (Stream.codecpar->codec_type == AVMEDIA_TYPE_AUDIO &&
       FirstAudioStream < 0)
    {
      FirstAudioStream = i;
    }
#endif
  }
  // return true if either an audio- or video-stream was found
  Result = (FirstAudioStream > -1 ||
            FirstVideoStream > -1);
}

int TMediaCore_FFmpeg::FindAudioStreamIndex(const AVFormatContext& FormatCtx)
{
  // find the first audio stream
  for (size_t i = 0; i < FormatCtx.nb_streams; ++i)
  {
    auto& Stream = *FormatCtx.streams[i];

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(52, 64, 0) // < 52.64.0
    if (Stream.codec^.codec_type = CODEC_TYPE_AUDIO) 
#elif LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(59, 0, 0)
    if (Stream.codec^.codec_type = AVMEDIA_TYPE_AUDIO) 
#else
    if (Stream.codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
#endif
    {
      return i;
    }
  }
  return -1;
}

std::optional<UMusic::TAudioSampleFormat> TMediaCore_FFmpeg::ConvertFFmpegToAudioFormat(AVSampleFormat FFmpegFormat)
{
  switch (FFmpegFormat)
  {
    case AV_SAMPLE_FMT_U8:  
      return UMusic::TAudioSampleFormat::asfU8;
    case AV_SAMPLE_FMT_S16: 
       return UMusic::TAudioSampleFormat::asfS16;
    case AV_SAMPLE_FMT_S32: 
      return UMusic::TAudioSampleFormat::asfS32;
    case AV_SAMPLE_FMT_FLT: 
      return UMusic::TAudioSampleFormat::asfFloat;
    case AV_SAMPLE_FMT_DBL: 
      return UMusic::TAudioSampleFormat::asfDouble;
    default:               
      return std::nullopt;
  }
}

std::optional<AVSampleFormat> TMediaCore_FFmpeg::ConvertAudioFormatToFFmpeg(UMusic::TAudioSampleFormat Format)
{
  switch (Format)
  {
    case UMusic::TAudioSampleFormat::asfU8:     
      return AV_SAMPLE_FMT_U8;
    case UMusic::TAudioSampleFormat::asfS16:
      return AV_SAMPLE_FMT_S16;
    case UMusic::TAudioSampleFormat::asfS32: 
      return AV_SAMPLE_FMT_S32;
    case UMusic::TAudioSampleFormat::asfFloat:  
      return AV_SAMPLE_FMT_FLT;
    case UMusic::TAudioSampleFormat::asfDouble: 
      return AV_SAMPLE_FMT_DBL;
    default:
      return std::nullopt;
  }
}


/**
 * UTF-8 Filename wrapper based on:
 * http://www.mail-archive.com/libav-user@mplayerhq.hu/msg02460.html
 */
/*
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
int FFmpegStreamOpen(void*& h, const std::filesystem::path& filename, int flags)
#else
int FFmpegStreamOpen(h: PURLContext; filename: PAnsiChar; flags: cint)
#endif
{
  // check for protocol prefix ("ufile:") and strip it
  std::filesystem::path FilePath;

  std::string ProtPrefix = "ufile:";
  if (filename.string().starts_with(ProtPrefix))
    FilePath = {filename.string().substr(ProtPrefix.size())};
  else
    FilePath = filename;

  std::ios_base::openmode Mode;

  if ((flags & URL_RDWR) == URL_RDWR) 
    Mode = std::ios::in | std::ios::out;
  else if ((flags & URL_WRONLY) = URL_WRONLY) 
    Mode = std::ios::in | std::ios::out; // TODO: fmCreate is Read+Write -> reopen with fmOpenWrite
  else
    Mode = std::ios::in; //| //TODO::reimplement in c++ fmShareDenyWrite;

  Result = 0;

  try
  {
    auto Stream = std::fstream(FilePath, Mode | std::ios_base::binary);
    if (!Stream.is_open())
      throw std::exception();

    #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
      h = Stream;
    #else
      h.priv_data = Stream;
    #endif
  }
  catch (...)
  {
#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(50, 43, 0) // < 50.43.0
    return AVERROR_NOENT;
#else
    return -1;
#endif
  }
  return 0;
}

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
function FFmpegStreamRead(h: Pointer; buf: PByteArray; size: cint): cint; cdecl;
#else
function FFmpegStreamRead(h: PURLContext; buf: PByteArray; size: cint): cint; cdecl;
#endif
//var
  //Stream: TStream;
{
  #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
  Stream = TStream(h);
  #else
  Stream = TStream(h.priv_data);
  #endif
  if (Stream = nil) 
    raise EInvalidContainer.Create("FFmpegStreamRead on nil");
  try
  {
    Result = Stream.Read(buf[0], size);
  }
  catch (...)
  {
    Result = -1;
  }
  if (Result == 0)
    Result = AVERROR_EOF;
}

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
int FFmpegStreamWrite(h: Pointer; buf: PByteArray; size: cint)
#else
int FFmpegStreamWrite(h: PURLContext; buf: PByteArray; size: cint)
#endif
//var
  //Stream: TStream;
{
  #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
  Stream = TStream(h);
  #else
  Stream = TStream(h.priv_data);
  #endif
  if (Stream = nil) 
    throw EInvalidContainer("FFmpegStreamWrite on nil");
  try
  {
    Result = Stream.Write(buf[0], size);
  }
  catch (...)
  {
    Result = -1;
  }
}

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
function FFmpegStreamSeek(h: Pointer; pos: cint64; whence: cint): cint64
#else
function FFmpegStreamSeek(h: PURLContext; pos: cint64; whence: cint): cint64
#endif
//var
  //Stream : TStream;
  //Origin : TSeekOrigin;
{
  #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
  Stream = TStream(h);
  #else
  Stream = TStream(h.priv_data);
  #endif
  if (Stream = nil) 
    throw EInvalidContainer("FFmpegStreamSeek on nil");
  switch (whence)
  {
    0 {SEEK_SET}: Origin = soBeginning;
    1 {SEEK_CUR}: Origin = soCurrent;
    2 {SEEK_END}: Origin = soEnd;
    AVSEEK_SIZE: {
      Result = Stream.Size;
      Exit;
    }
  else
    Origin = soBeginning;
  }
  Result = Stream.Seek(pos, Origin);
}

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
int FFmpegStreamClose(h: Pointer)
#else
int FFmpegStreamClose(h: PURLContext): cint; cdecl;
#endif
//var
  //Stream : TStream;
{
  #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
  Stream = TStream(h);
  #else
  Stream = TStream(h.priv_data);
  #endif
  try
  {
    Stream.Free;
  }
  catch (...)
  {

  }
  Result = 0;
}
*/
int TMediaCore_FFmpeg::AVFormatOpenInput(AVFormatContextU& ps, const std::filesystem::path& filename)
{
  #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
  //TODO:: must move Stream into class
  typedef std::basic_ifstream<uint8_t> TypeStream;
  auto Stream = std::basic_ifstream<uint8_t>(filename, std::ios::binary);
  if (!Stream.is_open())
    return AVERROR_STREAM_NOT_FOUND;

  ps = make<AVFormatContextU>(avformat_alloc_context());
  auto buffer = static_cast<uint8_t*>(av_malloc(BLOCKSIZE));
  //FFmpegStreamOpen(h, filename, URL_RDONLY);
  ps->pb = avio_alloc_context(buffer, BLOCKSIZE, 0, reinterpret_cast<void*>(&Stream), 
  [](void* opaque, uint8_t* buf, int buf_size) -> int 
  {
    //Read
    auto& Stream = *reinterpret_cast<TypeStream*>(opaque);
    Stream.read(buf, buf_size);

    if (Stream.fail())
      return -1;
    if (Stream.eof())
      return AVERROR_EOF;
    return 0;
  }, 
  nullptr, //Write
  [](void* opaque, int64_t offset, int whence) -> int64_t
  {
    //Seek
    auto& Stream = *reinterpret_cast<TypeStream*>(opaque);
    switch (whence)
    {
      case SEEK_SET:

      break;
      case SEEK_CUR:
      break;
      case SEEK_END:
      break;
      case AVSEEK_SIZE:
        auto pos = Stream.tellg();
        Stream.seekg(0, std::ios::end);
        
        break;
    }
  });
    return avformat_open_input(ps.release(), filename.c_str(), nullptr, nullptr);
  #else
    return 0;
  #endif
}
/*#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
var
  h: Pointer;
  buffer: Pointer;
#endif*/

void TMediaCore_FFmpeg::AVFormatCloseInput(ps: PPAVFormatContext);
{
  #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
  av_free(ps^^.pb.buffer);
  FFmpegStreamClose(ps^^.pb.opaque);
  //{ avformat_close_input frees AVIOContext pb, no avio_close needed }
  //{ avformat_close_input frees AVFormatContext, no additional avformat_free_context needed }
  avformat_close_input(ps);
  #endif
}

function TMediaCore_FFmpeg::GetCodecContext(stream: PAVStream; codec: PAVCodec): PAVCodecContext;
{
  #if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(59, 0, 0)
  Result = stream^.codec;
  #else
  Result = avcodec_alloc_context3(codec);
  if (Result <> nil) and (avcodec_parameters_to_context(Result, stream^.codecpar) < 0) 
    avcodec_free_context(@Result);
  if Result = nil 
    Log.LogError("Failed to allocate context", "TMediaCore_FFmpeg::GetCodecContext");
  #endif
}

{ TPacketQueue }

constructor TPacketQueue.Create();
{
  inherited;

  FirstListEntry = nil;
  LastListEntry  = nil;
  PacketCount = 0;
  Size = 0;

  Mutex = SDL_CreateMutex();
  Condition = SDL_CreateCond();
}

destructor TPacketQueue.Destroy();
{
  Flush();
  SDL_DestroyMutex(Mutex);
  Mutex=nil;
  SDL_DestroyCond(Condition);
  inherited;
}

void TPacketQueue.Abort();
{
  SDL_LockMutex(Mutex);

  AbortRequest = true;

  SDL_CondBroadcast(Condition);
  SDL_UnlockMutex(Mutex);
}

function TPacketQueue.IsAborted(): bool;
{
  SDL_LockMutex(Mutex);
  Result = AbortRequest;
  SDL_UnlockMutex(Mutex);
}

function TPacketQueue.Put(Packet : PAVPacket): int;
var
  CurrentListEntry : PAVPacketList;
{
  Result = -1;

  if (Packet = nil) 
    Exit;

  CurrentListEntry = av_malloc(SizeOf(TAVPacketList));
  if (CurrentListEntry = nil) 
    Exit;

  #if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(59, 0, 0)
  CurrentListEntry^.pkt = Packet^;
  if (PChar(Packet^.data) <> STATUS_PACKET) 
  {
    if (av_dup_packet(@(CurrentListEntry^.pkt)) < 0) 
    {
      av_free(CurrentListEntry);
      Exit;
    }
  }
  #else
  if (PChar(Packet^.data) <> STATUS_PACKET) 
    {
      if (av_packet_ref(@CurrentListEntry^.pkt, Packet) < 0) 
      {
        av_free(CurrentListEntry);
        Exit;
      }
    }
  else
    CurrentListEntry^.pkt = Packet^;
  #endif

  CurrentListEntry^.next = nil;

  SDL_LockMutex(Mutex);
  try
    if (LastListEntry = nil) 
      FirstListEntry = CurrentListEntry
    else
      LastListEntry^.next = CurrentListEntry;

    LastListEntry = CurrentListEntry;
    Inc(PacketCount);

    Size = Size + CurrentListEntry^.pkt.size;
    SDL_CondSignal(Condition);
  finally
    SDL_UnlockMutex(Mutex);
  }

  Result = 0;
}

/**
 * Adds a status packet (EOF, Flush, etc.) to the end of the queue.
 * StatusInfo can be used to pass additional information to the decoder.
 * Only assign nil or a valid pointer to data allocated with Getmem() to
 * StatusInfo because the pointer will be disposed with Freemem() on a call
 * to Flush(). If the packet is removed from the queue it is the decoder"s
 * responsibility to free the StatusInfo data with FreeStatusInfo().
 */
function TPacketQueue.PutStatus(StatusFlag: int; StatusInfo: Pointer): int;
var
  TempPacket: PAVPacket;
{
  // create temp. package
  TempPacket = av_malloc(SizeOf(TAVPacket));
  if (TempPacket = nil) 
  {
    Result = -1;
    Exit;
  }
  // init package
  av_init_packet(TempPacket^);
  TempPacket^.data  = Pointer(STATUS_PACKET);
  TempPacket^.flags = StatusFlag;
#if FFMPEG_VERSION_INT < AV_VERSION_INT(20, 0, 0)
  TempPacket^.priv  = StatusInfo;
#endif
  // put a copy of the package into the queue
  Result = Put(TempPacket);
  // data has been copied -> delete temp. package
  av_free(TempPacket);
}

void TPacketQueue.FreeStatusInfo(var Packet: TAVPacket);
{
#if FFMPEG_VERSION_INT < AV_VERSION_INT(20, 0, 0)
  if (Packet.priv <> nil) 
    FreeMem(Packet.priv);
#endif
}

function TPacketQueue.GetStatusInfo(var Packet: TAVPacket): Pointer;
{
#if FFMPEG_VERSION_INT < AV_VERSION_INT(20, 0, 0)
  Result = Packet.priv;
#endif
}

function TPacketQueue.Get(var Packet: TAVPacket; Blocking: bool): int;
var
  CurrentListEntry: PAVPacketList;
const
  WAIT_TIMEOUT = 10; // timeout in ms
{
  Result = -1;

  SDL_LockMutex(Mutex);
  try
    while (true) do
    {
      if (AbortRequest) 
        Exit;

      CurrentListEntry = FirstListEntry;
      if (CurrentListEntry <> nil) 
      {
        FirstListEntry = CurrentListEntry^.next;
        if (FirstListEntry = nil) 
          LastListEntry = nil;
        Dec(PacketCount);

        Size = Size - CurrentListEntry^.pkt.size;
        Packet = CurrentListEntry^.pkt;
        av_free(CurrentListEntry);

        Result = 1;
        break;
      }
      else if (not Blocking) 
      {
        Result = 0;
        break;
      }
      else
      {
        // block until a new package arrives,
        // but do not wait till infinity to avoid deadlocks
        if (SDL_CondWaitTimeout(Condition, Mutex, WAIT_TIMEOUT) = SDL_MUTEX_TIMEDOUT) 
        {
          Result = 0;
          break;
        }
      }
    }
  finally
    SDL_UnlockMutex(Mutex);
  }
}

function TPacketQueue.GetSize(): int;
{
  SDL_LockMutex(Mutex);
  Result = Size;
  SDL_UnlockMutex(Mutex);
}

void TPacketQueue.Flush();
var
  CurrentListEntry, TempListEntry: PAVPacketList;
{
  SDL_LockMutex(Mutex);

  CurrentListEntry = FirstListEntry;
  while(CurrentListEntry <> nil) do
  {
    TempListEntry = CurrentListEntry^.next;
    // free status data
    if (PChar(CurrentListEntry^.pkt.data) = STATUS_PACKET) 
      FreeStatusInfo(CurrentListEntry^.pkt);
    // free packet data
    #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(59, 0, 0)
    if (PChar(CurrentListEntry^.pkt.data) <> STATUS_PACKET) 
      av_packet_unref(@CurrentListEntry^.pkt);
    #else
      av_free_packet(@CurrentListEntry^.pkt);
    #endif
    // Note: param must be a pointer to a pointer!
    av_freep(@CurrentListEntry);
    CurrentListEntry = TempListEntry;
  }
  LastListEntry = nil;
  FirstListEntry = nil;
  PacketCount = 0;
  Size = 0;

  SDL_UnlockMutex(Mutex);
}
}