#include "UMediaCore_FFmpeg.h"

namespace UMediaCore_FFmpeg
{
    int Stream_Wrapper::AVFormatOpenInput(const std::filesystem::path& filename)
    {
        auto Stream = std::basic_ifstream<uint8_t>(filename, std::ios::binary);
        if (!Stream.is_open())
            return AVERROR_STREAM_NOT_FOUND;

        formatCtx = makeFormatContext<AVFormatContextU>(avformat_alloc_context());
        buffer = makeMem<AVMemU<uint8_t>>(static_cast<uint8_t*>(av_malloc(BLOCKSIZE)));
        //FFmpegStreamOpen(h, filename, URL_RDONLY);

        formatCtx->pb = avio_alloc_context(buffer.get(), BLOCKSIZE, 0, this,
            &Stream_Wrapper::readStreamWrapper,
            nullptr,
            &Stream_Wrapper::seekWrapper
        );

        auto i = formatCtx.release(); //Safe since pointer gets deleted anyway if next step fails
        const auto res = avformat_open_input(&i, filename.string().c_str(), nullptr, nullptr);
        formatCtx = makeFormatContext<AVFormatContextU>(i);

        return res;
    }

    void Stream_Wrapper::AVFormatCloseInput()
    {
        formatCtx.reset();
        stream.close();
        buffer.reset();
    }

    int Stream_Wrapper::readStreamWrapper(void* opaque, uint8_t* buf, int buf_size)
    {
        auto instance = static_cast<Stream_Wrapper*>(opaque);
        return instance->readStream(buf, buf_size);
    }

    int Stream_Wrapper::readStream(uint8_t* buf, int buf_size)
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

    int64_t Stream_Wrapper::seekWrapper(void* opaque, int64_t offset, int whence)
    {
        auto instance = static_cast<Stream_Wrapper*>(opaque);
        return instance->seek(offset, whence);
    }

    int64_t Stream_Wrapper::seek(int64_t offset, int whence)
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

    TMediaCore_FFmpeg TMediaCore_FFmpeg::GetInstance()
    {
        if (!Instance)
            Instance = TMediaCore_FFmpeg();
        return Instance;
    }

    std::unique_lock<std::mutex>&& TMediaCore_FFmpeg::LockAVCodec()
    {
        return std::move(std::unique_lock<std::mutex>(AVCodecLock));
    }

    std::string TMediaCore_FFmpeg::GetErrorString(int ErrorNum)
    {
#if LIBAVUTIL_VERSION_INT < AV_VERSION_INT(50, 43, 0) // < 50.43.0
        case ErrorNum of
            AVERROR_IO : Result = "AVERROR_IO";
        AVERROR_NUMEXPECTED:  Result = "AVERROR_NUMEXPECTED";
        AVERROR_INVALIDDATA:  Result = "AVERROR_INVALIDDATA";
        AVERROR_NOMEM:        Result = "AVERROR_NOMEM";
        AVERROR_NOFMT:        Result = "AVERROR_NOFMT";
        AVERROR_NOTSUPP:      Result = "AVERROR_NOTSUPP";
        AVERROR_NOENT:        Result = "AVERROR_NOENT";
        AVERROR_PATCHWELCOME: Result = "AVERROR_PATCHWELCOME";
    else                  Result = "AVERROR_#" + inttostr(ErrorNum);
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
#elif defined(FF_API_LAVF_AVCTX) && FF_API_LAVF_AVCTX
            if (Stream.codec.codec_type == AVMEDIA_TYPE_VIDEO &&
                FirstVideoStream < 0)
            {
                FirstVideoStream = i;
            }

            if (Stream.codec.codec_type == AVMEDIA_TYPE_AUDIO &&
                FirstAudioStream < 0)
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
        return FirstAudioStream > -1 || FirstVideoStream > -1;
    }

    int TMediaCore_FFmpeg::FindAudioStreamIndex(const AVFormatContext& FormatCtx)
    {
        // find the first audio stream
        for (size_t i = 0; i < FormatCtx.nb_streams; ++i)
        {
            auto& Stream = *FormatCtx.streams[i];

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(52, 64, 0) // < 52.64.0
            if (Stream.codec^ .codec_type = CODEC_TYPE_AUDIO)
#elif defined(FF_API_LAVF_AVCTX) && FF_API_LAVF_AVCTX
            if (Stream.codec.codec_type = AVMEDIA_TYPE_AUDIO)
#else
            if (Stream.codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
#endif
            {
                return i;
            }
        }
        return -1;
    }

    std::optional<UMusic::TAudioSampleFormat> ConvertFFmpegToAudioFormat(AVSampleFormat FFmpegFormat)
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

    std::optional<AVSampleFormat> ConvertAudioFormatToFFmpeg(UMusic::TAudioSampleFormat Format)
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
     /*int TMediaCore_FFmpeg::AVFormatOpenInput(AVFormatContextU& ps, const std::filesystem::path& filename)
     {
       #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
       //TODO:: must move Stream into class
       typedef std::basic_ifstream<uint8_t> TypeStream;
       auto Stream = std::basic_ifstream<uint8_t>(filename, std::ios::binary);
       if (!Stream.is_open())
         return AVERROR_STREAM_NOT_FOUND;

       ps = make<AVFormatContextU>(avformat_alloc_context());
       auto buffer = static_cast<uint8_t*>(av_malloc(Stream_Wrapper::BLOCKSIZE));
       //FFmpegStreamOpen(h, filename, URL_RDONLY);
       ps->pb = avio_alloc_context(buffer, Stream_Wrapper::BLOCKSIZE, 0, &Stream,
       [](void* opaque, uint8_t* buf, int buf_size) -> int
       {
         //Read
         auto& Stream = *static_cast<TypeStream*>(opaque);
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
         auto& Stream = *static_cast<TypeStream*>(opaque);
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
     }*/
     /*#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
     var
       h: Pointer;
       buffer: Pointer;
     #endif*/
     /*
     void TMediaCore_FFmpeg::AVFormatCloseInput(ps: PPAVFormatContext);
     {
       #if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 29, 104)
       av_free(ps^^.pb.buffer);
       FFmpegStreamClose(ps^^.pb.opaque);
       // avformat_close_input frees AVIOContext pb, no avio_close needed
       // avformat_close_input frees AVFormatContext, no additional avformat_free_context needed
       avformat_close_input(ps);
       #endif
     }
     */
     /*
     AVCodecContext* TMediaCore_FFmpeg::GetCodecContext(AVStream* stream, AVCodec* codec)
     {
       #if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 76, 100)
         return stream->codec;
       #else
         auto Result = avcodec_alloc_context3(codec);
       if (Result && avcodec_parameters_to_context(Result, stream->codecpar) < 0)
         avcodec_free_context(&Result);
       if (!Result)
         ULog::Log.LogError("Failed to allocate context", "TMediaCore_FFmpeg::GetCodecContext");
       #endif
       return Result;
     }
     */

     // TPacketQueue
    TPacketQueue::~TPacketQueue()
    {
        Flush();
    }

    void TPacketQueue::Abort()
    {
        std::unique_lock lock(Mutex);
        AbortRequest = true;

        Condition.notify_all();
    }

    bool TPacketQueue::IsAborted()
    {
        std::unique_lock lock(Mutex);
        return AbortRequest;
    }

    int TPacketQueue::Put(AVPacketU&& Packet)
    {
#if defined(FF_API_AVPACKET_OLD_API) && FF_API_AVPACKET_OLD_API
        CurrentListEntry->pkt = *Packet;
        if (PChar(Packet ^ .data) < > STATUS_PACKET)
        {
            if (av_dup_packet(@(CurrentListEntry->pkt)) < 0)
            {
                av_free(CurrentListEntry);
                return -1;
            }
        }
#else
        AVPacket packet;
        if (std::string(reinterpret_cast<const char*>(Packet->data)) != STATUS_PACKET)
        {
            if (av_packet_ref(&packet, Packet.get()) < 0)
            {
                return -1;
            }
        }
        else
            packet = Packet;
#endif

        try
        {
            std::unique_lock lock(Mutex);
            entries.push(makePacket<AVPacketU>(packet));

            Size += entries.back()->size;
            Condition.notify_one();
        }
        catch (...)
        {
        }

        return 0;
    }

    /**
     * Adds a status packet (EOF, Flush, etc.) to the end of the queue.
     * StatusInfo can be used to pass additional information to the decoder.
     * Only assign nil or a valid pointer to data allocated with Getmem() to
     * StatusInfo because the pointer will be disposed with Freemem() on a call
     * to Flush(). If the packet is removed from the queue it is the decoder's
     * responsibility to free the StatusInfo data with FreeStatusInfo().
     */
    int TPacketQueue::PutStatus(int StatusFlag, void* StatusInfo)
        //var
          //TempPacket: PAVPacket;
    {
        // create temp. package
        auto TempPacket = makePacket<AVPacketU>(av_packet_alloc());
        // init package
        //av_init_packet(&TempPacket);
        TempPacket->data = reinterpret_cast<uint8_t*>(const_cast<char*>(STATUS_PACKET.c_str()));
        TempPacket->flags = StatusFlag;
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(20, 0, 0)
        TempPacket^ .priv = StatusInfo;
#endif
        // put a copy of the package into the queue
        return Put(std::move(TempPacket));
        // data has been copied -> delete temp. package
    }

    void TPacketQueue::FreeStatusInfo(AVPacket& Packet)
    {
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(20, 0, 0)
        if (Packet.priv <> nil)
            FreeMem(Packet.priv);
#endif
    }

    void* TPacketQueue::GetStatusInfo(AVPacket& Packet)
    {
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(20, 0, 0)
        Result = Packet.priv;
#endif
    }

    int TPacketQueue::Get(AVPacketU& Packet, bool Blocking)
    {
        constexpr auto WAIT_TIMEOUT = std::chrono::milliseconds(10); // timeout
        try
        {
            std::unique_lock lock(Mutex);
            while (true)
            {
                if (AbortRequest)
                    return -1;

                if (!entries.empty())
                {
                    Packet = std::move(entries.front());
                    entries.pop();

                    Size -= Packet->size;

                    return 1;
                }
                if (!Blocking)
                {
                    return 0;
                }
                // block until a new package arrives,
                // but do not wait till infinity to avoid deadlocks
                if (Condition.wait_for(lock, WAIT_TIMEOUT) == std::cv_status::timeout)
                    return 0;
            }
        }
        catch (...)
        {
        }
        return -1;
    }

    int TPacketQueue::GetSize()
    {
        std::unique_lock lock(Mutex);
        return Size;
    }

    void TPacketQueue::Flush()
    {
        std::unique_lock lock(Mutex);

        entries = {};
        /*TempListEntry = CurrentListEntry^.next;
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
        // Note: param must be a pointer to a pointer!*/

        Size = 0;
    }
}