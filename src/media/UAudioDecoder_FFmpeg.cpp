#include "UAudioDecoder_FFmpeg.h"

namespace UAudioDecoder_FFmpeg
{
// TFFmpegDecodeStream
TFFmpegDecodeStream::TFFmpegDecodeStream()
{
inherited Create();

#ifdef UseFrameDecoderAPI
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(52, 19, 101}
fAudioBufferFrame = av_frame_alloc();
#else
fAudioBufferFrame = avcodec_alloc_frame();
#endif
#else
// according to the documentation of avcodec_decode_audio(2), sample-data
// should be aligned on a 16 byte boundary. Otherwise internal calls
// (e.g. to SSE or Altivec operations) might fail or lack performance on some
// CPUs. Although GetMem() in Delphi and FPC seems to use a 16 byte or higher
// alignment for buffers of this size (alignment depends on the size of the
// requested buffer), we will set the alignment explicitly as the minimum
// alignment used by Delphi and FPC is on an 8 byte boundary.
// 
// Note: AudioBuffer was previously defined as a field of type TAudioBuffer
// (array[0..AUDIO_BUFFER_SIZE-1] of byte) and hence statically allocated.
// Fields of records are aligned different to memory allocated with GetMem(),
// aligning depending on the type but will be at least 2 bytes.
// AudioBuffer was not aligned to a 16 byte boundary. The {$ALIGN x} directive
// was not applicable as Delphi in contrast to FPC provides at most 8 byte
// alignment ({$ALIGN 16} is not supported) by this directive.
fAudioBuffer = GetAlignedMem(AUDIO_BUFFER_SIZE, 16);
#endif

Reset();
}

void TFFmpegDecodeStream::Reset()
{
fParseThread.reset();

fEOFState = false;
fErrorState = false;
fLoop = false;
fQuitRequest = false;

fAudioPaketData.clear();
fAudioPaketSize = 0;
fAudioPaketSilence = 0;

fAudioBufferPos = 0;
fAudioBufferSize = 0;

fParserLocked = false;
fParserPauseRequestCount = 0;
fDecoderLocked = false;
fDecoderPauseRequestCount = 0;

fAudioPaket.reset();
#if (LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(58, 0, 0)
// avoid calling av_packet_unref before fetching first frame
fAudioPaket->data = reinterpret_cast<uint8_t*>(const_cast<char*>(UMediaCore_FFmpeg::STATUS_PACKET.data()));
#endif
}

/*
*Frees the decode - stream data.
*/
TFFmpegDecodeStream::~TFFmpegDecodeStream()
{
    Close();
    /*
    SDL_DestroyMutex(fStateLock);
    fStateLock = nil;
    SDL_DestroyCond(fParserUnlockedCond);
    SDL_DestroyCond(fParserResumeCond);
    SDL_DestroyCond(fParserIdleCond);
    SDL_DestroyCond(SeekFinishedCond);
    SDL_DestroyCond(fDecoderUnlockedCond);
    SDL_DestroyCond(fDecoderResumeCond);
    */
#if defined(UseFrameDecoderAPI)
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(52, 19, 101)
    av_frame_free(@fAudioBufferFrame);
#else
    avcodec_free_frame(@fAudioBufferFrame);
#endif
#else
    FreeAlignedMem(fAudioBuffer);
#endif
}

bool TFFmpegDecodeStream::Open(const std::filesystem::path& Filename)
/*var
SampleFormat : TAudioSampleFormat;
PackedSampleFormat: TAVSampleFormat;
TestFrame: TAVPacket;
AVResult: int;
CodecID: TAVCodecID;*/
{
    Close();
    Reset();

    if (!is_regular_file(Filename))
    {
        ULog::Log.LogErrorLocation("Audio-file does not exist: \"" + Filename.string() + "\"");
        return false;
    }

    fFilename = Filename;

    // use custom "ufile" protocol for UTF-8 support
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(53, 1, 3)
    AVResult = av_open_input_file(fFormatCtx, PAnsiChar("ufile:" + FileName.ToUTF8), nil, 0, nil);
#elif LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(54, 29, 104)
    AVResult = avformat_open_input(@fFormatCtx, PAnsiChar("ufile:" + FileName.ToUTF8), nil, nil);
#else

#endif
    if (auto AVResult = fileStream.AVFormatOpenInput(Filename); AVResult != 0)
    {
        ULog::Log.LogErrorLocation("Failed to open file \"" + Filename.string() + "\" (" + UMediaCore_FFmpeg::GetErrorString(AVResult) + ")");
        return false;
    }

    // generate PTS values if they do not exist
    fileStream.flags() |= AVFMT_FLAG_GENPTS;

    // retrieve stream information
    //#if LIBAVFORMAT_VERSION >= 54006000)}
    // av_find_stream_info is deprecated and should be replaced by av_read_frame. Untested.
    //AVResult := av_read_frame(fFormatCtx, TestFrame);


    if (fileStream.findStreamInfo() < 0)
    {
        ULog::Log.LogErrorLocation("No stream info found: \"" + Filename.string() + "\"");
        Close();
        return false;
    }

    // FIXME: hack used by ffplay. Maybe should not use url_feof() to test for the end
    fFormatCtx->pb->eof_reached = 0;

#ifdef DebugFFmpegDecode
    dump_format(fFormatCtx, 0, PAnsiChar(Filename.ToNative), 0);
#endif

    const auto audioStIdxOpt = fileStream.findAudioStreamIndex();
    if (!audioStIdxOpt)
    {
        ULog::Log.LogErrorLocation("FindAudioStreamIndex: No Audio-stream found \"" + Filename.string() + "\"");
        Close();
        return false;
    }
    fAudioStreamIndex = audioStIdxOpt.value();

    //Log.LogStatus("AudioStreamIndex is: "+ inttostr(ffmpegStreamID), "UAudio_FFmpeg");

    fAudioStream = fileStream.get_stream(fAudioStreamIndex);
    fAudioStreamPos = 0;

    auto
#if FF_API_LAVF_AVCTX
        CodecID = fAudioStream->codec->codec_id;
#else
        CodecID = fAudioStream->codecpar->codec_id;
#endif

    fCodec = avcodec_find_decoder(CodecID);
    if (!fCodec)
    {
        ULog::Log.LogErrorLocation("Unsupported codec!");
        fCodecCtx = nullptr;
        Close();
        return false;
    }

    fCodecCtx = UMediaCore_FFmpeg::GetCodecContext<UMediaCore_FFmpeg::AVCodecContextU>(fAudioStream, fCodec);
    if (!fCodecCtx)
    {
        Close();
        return false;
    }

    // TODO: should we use this or not? Should we allow 5.1 channel audio?

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(56, 42, 0)
    fCodecCtx->request_channel_layout = AV_CH_LAYOUT_STEREO_DOWNMIX; //avcodec.c AV_CH_LAYOUT_STEREO_DOWNMIX;
#elif LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(51, 42, 0)
    if (fCodecCtx ^ .channels > 0) then
        fCodecCtx^ .request_channels = Min(2, fCodecCtx ^ .channels)
    else
        fCodecCtx^ .request_channels = 2;
#endif

    // set debug options
    //fCodecCtx->debug_mv = 0;
    fCodecCtx->debug = 0;

#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(10, 1, 0)
    // request required sample format
    // reference:
    // http://stackoverflow.com/questions/16479662/ffmpeg-1-0-causing-audio-playback-issues
    // without this avcodec_open2 returns AV_SAMPLE_FMT_S16P
    fCodecCtx->request_sample_fmt = AV_SAMPLE_FMT_S16;
#endif

    // detect bug-workarounds automatically
    fCodecCtx->workaround_bugs = FF_BUG_AUTODETECT;
    // error resilience strategy (careful/compliant/agressive/very_aggressive)
    //CodecCtx^.error_resilience := FF_ER_CAREFUL; //FF_ER_COMPLIANT;
    // allow non spec compliant speedup tricks.
    //CodecCtx^.flags2 := CodecCtx^.flags2 or CODEC_FLAG2_FAST;

    // Note: avcodec_open() and avcodec_close() are not thread-safe and will
    // fail if called concurrently by different threads.
    if (UMediaCore_FFmpeg::open_avcodec(fCodecCtx, fCodec) < 0)
    {
        ULog::Log.LogErrorLocation("avcodec_open failed!");
        Close();
        return false;
    }

    // now initialize the audio-format
    AVSampleFormat PackedSampleFormat;
#ifdef ConvertPlanar
    PackedSampleFormat = av_get_packed_sample_fmt(fCodecCtx->sample_fmt);
    if (PackedSampleFormat != fCodecCtx->sample_fmt)
    {
        // There is no point in leaving PackedSampleFormat as is.
        // av_audio_resample_init as used by TAudioConverter_FFmpeg will internally
        // convert to AV_SAMPLE_FMT_S16 anyway and most architectures have assembly
        // optimized conversion routines from AV_SAMPLE_FMT_FLTP to AV_SAMPLE_FMT_S16.
        PackedSampleFormat = AV_SAMPLE_FMT_S16;
        fSwrContext = makeSwrContext<SwrContextU>(swr_alloc_set_opts(nullptr, fCodecCtx->channel_layout, PackedSampleFormat, fCodecCtx->sample_rate,
            fCodecCtx->channel_layout, fCodecCtx->sample_fmt, fCodecCtx->sample_rate, 0, nullptr));
        if (!fSwrContext)
            ULog::Log.LogStatusLocation("Error: Failed to create SwrContext");
        else
        {
            av_opt_set_int(fSwrContext.get(), "ich", fCodecCtx->channels, 0);
            av_opt_set_int(fSwrContext.get(), "och", fCodecCtx->channels, 0);
            if (swr_init(fSwrContext.get()) < 0)
            {
                fSwrContext.reset();
                ULog::Log.LogStatusLocation("Error: Failed to initialize SwrContext");
            }
        }
    }
#else
    PackedSampleFormat = fCodecCtx->sample_fmt;
#endif

    auto SampleFormat = UMediaCore_FFmpeg::ConvertFFmpegToAudioFormat(PackedSampleFormat).
							value_or(UMusic::TAudioSampleFormat::asfS16);
    if (fCodecCtx->channels > 255)
        ULog::Log.LogStatusLocation("Error: CodecCtx->channels > 255");
    fFormatInfo = UMusic::TAudioFormatInfo(
        fCodecCtx->channels,
        fCodecCtx->sample_rate,
        SampleFormat
    );
#ifdef UseFrameDecoderAPI
    fBytesPerSample = av_get_bytes_per_sample(PackedSampleFormat) * fCodecCtx->channels;
#endif

    // finally start the decode thread
    fParseThread = std::make_unique<std::thread>(ParseThreadMain); //SDL_CreateThread(@ParseThreadMain, nil, Self);

    return true;
}

void TFFmpegDecodeStream::Close()
/*var
ThreadResult : int;*/
{
// wake threads waiting for packet-queue data
// Note: normally, there are no waiting threads. If there were waiting
// ones, they would block the audio-callback thread.
fPacketQueue.Abort();

// send quit request (to parse-thread etc)
{
    std::unique_lock lock(fStateLock);
    fQuitRequest = true;
    fParserIdleCond.notify_all();
}

// abort parse-thread
if (fParseThread)
{
	// and wait until it terminates
    fParseThread->join();
    std::unique_lock lock(fStateLock);
	fParseThread.reset();
    SeekFinishedCond.notify_all();
}

#ifdef ConvertPlanar
// Free the swresample context
fSwrContext.reset();
#endif

// Close the codec
fCodecCtx.reset();

fileStream.AVFormatCloseInput();
close(*this);

fPacketQueue.Flush();
fFormatInfo = {};
}

std::optional<UMusic::AudioDuration> TFFmpegDecodeStream::GetLength() const
/*var
start_time : cint64;*/
{
auto start_time = fileStream.get_start_time();
// AV_NOPTS_VALUE is returned if no explicit start_time is available.
if (start_time == UMediaCore_FFmpeg::Stream_Wrapper::invalid_time())
	start_time = UMediaCore_FFmpeg::Stream_Wrapper::AVDuration::zero();

// there is a type size mismatch warning because start_time and duration are cint64.
// So, in principle there could be an overflow when doing the sum.
	return start_time + fileStream.get_duration();
}

UMusic::TAudioFormatInfo TFFmpegDecodeStream::GetAudioFormatInfo()
{
 return fFormatInfo;
}

bool TFFmpegDecodeStream::IsEOF()
{
std::unique_lock lock(fStateLock);
return fEOFState;
}

void TFFmpegDecodeStream::SetEOF(bool State)
{
    std::unique_lock lock(fStateLock);
	fEOFState = State;
}

bool TFFmpegDecodeStream::IsError()
{
    std::unique_lock lock(fStateLock);
return fErrorState;
}

void TFFmpegDecodeStream::SetError(bool State)
{
    std::unique_lock lock(fStateLock);
fErrorState = State;
}

bool TFFmpegDecodeStream::IsSeeking()
{
    std::unique_lock lock(fStateLock);
return fSeekRequest;
}

bool TFFmpegDecodeStream::IsQuit()
{
return fQuitRequest;
}

std::optional<UMusic::AudioDuration> TFFmpegDecodeStream::GetPosition() const
/*var
BufferSizeSec : double;*/
{
	PauseDecoder();

	// ReadData() does not return all of the buffer retrieved by DecodeFrame().
	// Determine the size of the unused part of the decode-buffer.
	auto BufferSize = SecDouble((fAudioBufferSize - fAudioBufferPos) / fFormatInfo.BytesPerSec());

	// subtract the size of unused buffer-data from the audio clock.
	auto Result = fAudioStreamPos - std::chrono::duration_cast<UMusic::AudioDuration>(BufferSize);

	ResumeDecoder();

    return Result;
}

void TFFmpegDecodeStream::SetPosition(double Time)
{
    // - Pause the parser first to prevent it from putting obsolete packages
    //   into the queue after the queue was flushed and before seeking is done.
    //   Otherwise we will hear fragments of old data, if the stream was seeked
    //   in stopped mode and resumed afterwards (applies to non-blocking mode only).
    std::unique_lock lock(fStateLock);
    if (!PauseParser())
        return;

    SetPositionIntern(Time, true);
    ResumeParser();

    // wait until seeking is done
    SeekFinishedCond.wait(lock, [this]()
        {
            return !(fParseThread || fSeekRequest);
        });
}

bool TFFmpegDecodeStream::GetLoopInternal() const
{
    return fLoop;
}

bool TFFmpegDecodeStream::GetLoop() const
{
    std::unique_lock lock(fStateLock);
	return GetLoopInternal();
}

void TFFmpegDecodeStream::SetLoop(bool Enabled)
{
    std::unique_lock lock(fStateLock);
	fLoop = Enabled;
}


/********************************************
 * Parser section
 ********************************************/

bool TFFmpegDecodeStream::PauseParser()
{
    ++fParserPauseRequestCount;
    while (fParserLocked) do
        SDL_CondWait(fParserUnlockedCond, fStateLock);

    if (!fParseThread)
    {
	    --fParserPauseRequestCount;
	    return false;
    }
    return true;
}

void TFFmpegDecodeStream::ResumeParser()
{
Dec(fParserPauseRequestCount);
SDL_CondSignal(fParserResumeCond);
}

void TFFmpegDecodeStream::SetPositionIntern(Time: real; Flush: bool)
{
// - The state lock has already been locked.
// - Pause the decoder to avoid race-condition that might occur otherwise.
PauseDecoderUnlocked();
try
{
    fEOFState = false;
    fErrorState = false;

    // do not seek if we are already at the correct position.
    // This is important especially for seeking to position 0 if we already are
    // at the beginning. Although seeking with AVSEEK_FLAG_BACKWARD for pos 0 works,
    // it is still a bit choppy (although much better than w/o AVSEEK_FLAG_BACKWARD).
    if (Time = fAudioStreamPos) then
        Exit;

    // configure seek parameters
    fSeekPos = Time;
    fSeekFlush = Flush;
    fSeekFlags = AVSEEK_FLAG_ANY;
    fSeekRequest = true;

    // Note: the BACKWARD-flag seeks to the first position <= the position
    // searched for. Otherwise e.g. position 0 might not be seeked correct.
    // For some reason ffmpeg sometimes doesn"t use position 0 but the key-frame
    // following. In streams with few key-frames (like many flv-files) the next
    // key-frame after 0 might be 5secs ahead.
    if (Time <= fAudioStreamPos) then
        fSeekFlags = fSeekFlags or AVSEEK_FLAG_BACKWARD;

    // send a reuse signal in case the parser was stopped (e.g. because of an EOF)
    SDL_CondSignal(fParserIdleCond);
}
catch (...)
{}
ResumeDecoderUnlocked();
}

int ParseThreadMain(Data: Pointer)
var
Stream : TFFmpegDecodeStream;
{
Stream = TFFmpegDecodeStream(Data);
if (Stream <> nil) then
Stream.Parse();
Result = 0;
}

void TFFmpegDecodeStream::Parse()
{
// reuse thread as long as the stream is not terminated
	std::unique_lock lock(fStateLock);
	while (ParseLoop())
	{
        // wait for reuse or destruction of stream
        fParserIdleCond.wait(lock, [this]()
            {
                return fSeekRequest || fQuitRequest;
            });
	}
}

/**
 * Parser main loop.
 * Will not return until parsing of the stream is finished.
 * Reasons for the parser to return are:
 * - the end-of-file is reached
 * - an error occured
 * - the stream was quited (received a quit-request)
 * Returns true if the stream can be resumed or false if the stream has to
 * be terminated.
 * Must be called and returns with fStateLock locked but temporarily
 * unlocks it.
 */
bool TFFmpegDecodeStream::ParseLoop()
var
Packet : TAVPacket;
SeekTarget: int64;
#if FFMPEG_VERSION_INT < 1001000}
ByteIOCtx: PByteIOContext;
#else
ByteIOCtx: PAVIOContext;
#endif
ErrorCode: int;
StartSilence: double;       // duration of silence at start of stream
StartSilencePtr: PDouble;  // pointer for the EMPTY status packet 
fileSize: int;
urlError: int;

// Note: pthreads wakes threads waiting on a mutex in the order of their
// priority and not in FIFO order. SDL does not provide any option to
// control priorities. This might (and already did) starve threads waiting
// on the mutex (e.g. SetPosition) making usdx look like it was froozen.
// Instead of simply locking the critical section we set a ParserLocked flag
// instead and give priority to the threads requesting the parser to pause.
function LockParser() : bool;
{
while ((fParserPauseRequestCount > 0) and not fQuitRequest) do
SDL_CondWait(fParserResumeCond, fStateLock);
if (not fQuitRequest) then
fParserLocked = true;
Result = fParserLocked;
}

void UnlockParser();
{
fParserLocked = false;
SDL_CondBroadcast(fParserUnlockedCond);
}

{
Result = true;

while LockParser() do
{
SDL_UnlockMutex(fStateLock);
try
// handle seek-request (Note: no need to lock SeekRequest here)
if (fSeekRequest) then
{
// first try: seek on the audio stream
SeekTarget = Round(fSeekPos / av_q2d(fAudioStream ^ .time_base));
StartSilence = 0;
if (SeekTarget < fAudioStream ^ .start_time) then
    StartSilence = (fAudioStream ^ .start_time - SeekTarget) * av_q2d(fAudioStream ^ .time_base);
ErrorCode = av_seek_frame(fFormatCtx, fAudioStreamIndex, SeekTarget, fSeekFlags);

if (ErrorCode < 0) then
    {
    // second try: seek on the default stream (necessary for flv-videos and some ogg-files)
    SeekTarget = Round(fSeekPos * AV_TIME_BASE);
StartSilence = 0;
if (SeekTarget < fFormatCtx ^ .start_time) then
    StartSilence = (fFormatCtx ^ .start_time - SeekTarget) / AV_TIME_BASE;
ErrorCode = av_seek_frame(fFormatCtx, -1, SeekTarget, fSeekFlags);
}

// pause decoder and lock state (keep the lock-order to avoid deadlocks).
// Note that the decoder does not block in the packet-queue in seeking state,
// so locking the decoder here does not cause a dead-lock.
SDL_LockMutex(fStateLock);
PauseDecoderUnlocked();
try
{
    if (ErrorCode < 0) then
    {
        // seeking failed
        fErrorState = true;
    #if LIBAVFORMAT_VERSION <= 58007100}
    Log.LogError("Seek Error in "" + fFormatCtx ^ .filename + """, "UAudioDecoder_FFmpeg");
#else
    Log.LogError("Seek Error in "" + fFormatCtx ^ .url + """, "UAudioDecoder_FFmpeg");
#endif
    }
    else
    {
        if (fSeekFlush) then
        {
            // flush queue (we will send a Flush-Packet when seeking is finished)
            fPacketQueue.Flush();

        // flush the decode buffers
        fAudioBufferSize = 0;
        fAudioBufferPos = 0;
        fAudioPaketSize = 0;
        fAudioPaketSilence = 0;
        FlushCodecBuffers();

        // Set preliminary stream position. The position will be set to
        // the correct value as soon as the first packet is decoded.
        fAudioStreamPos = fSeekPos;
        }
        else
        {
            // request avcodec buffer flush
            fPacketQueue.PutStatus(PKT_STATUS_FLAG_FLUSH, nil);
        }

        // fill the gap between position 0 and start_time with silence
        // but not if we are in loop mode
        if ((StartSilence > 0) and (not fLoop)) then
        {
        GetMem(StartSilencePtr, SizeOf(StartSilence));
        StartSilencePtr^ = StartSilence;
        fPacketQueue.PutStatus(PKT_STATUS_FLAG_EMPTY, StartSilencePtr);
        }
    }

    fSeekRequest = false;
    SDL_CondBroadcast(SeekFinishedCond);
}
catch (...)
{}
ResumeDecoderUnlocked();
SDL_UnlockMutex(fStateLock);
}
}

if (fPacketQueue.GetSize() > MAX_AUDIOQ_SIZE) then
{
SDL_Delay(10);
Continue;
}

if (av_read_frame(fFormatCtx, Packet) < 0) then
    {
    // failed to read a frame, check reason
#if (LIBAVFORMAT_VERSION_MAJOR >= 52)}
ByteIOCtx = fFormatCtx ^ .pb;
#else
ByteIOCtx = @fFormatCtx ^ .pb;
#endif

// check for end-of-file (eof is not an error)
#if (LIBAVFORMAT_VERSION_MAJOR < 56)}
if (url_feof(ByteIOCtx) < > 0) then
#else
if (avio_feof(ByteIOCtx) < > 0) then
#endif
{
SDL_LockMutex(fStateLock);
if (GetLoopInternal()) then
{
// rewind stream (but do not flush)
SetPositionIntern(0, false);
SDL_UnlockMutex(fStateLock);
Continue;
}
else
{
SDL_UnlockMutex(fStateLock);
// signal end-of-file
fPacketQueue.PutStatus(PKT_STATUS_FLAG_EOF, nil);
Break;
}
}

// check for errors
#if (LIBAVFORMAT_VERSION >= 52103000)}
urlError = ByteIOCtx ^ .error;
#else
urlError = url_ferror(ByteIOCtx);
#endif
if (urlError < > 0) then
{
// an error occured -> abort and wait for repositioning or termination
fPacketQueue.PutStatus(PKT_STATUS_FLAG_ERROR, nil);
Break;
}

// url_feof() does not detect an EOF for some files
// so we have to do it this way.
#if (LIBAVFORMAT_VERSION >= 53009000)}
fileSize = avio_size(fFormatCtx ^ .pb);
#else
fileSize = fFormatCtx ^ .file_size;
#endif
if ((fileSize < > 0) and (ByteIOCtx ^ .pos >= fileSize)) then
{
fPacketQueue.PutStatus(PKT_STATUS_FLAG_EOF, nil);
Break;
}

// unknown error occured, exit
fPacketQueue.PutStatus(PKT_STATUS_FLAG_ERROR, nil);
Break;
}

if (Packet.stream_index = fAudioStreamIndex) then
fPacketQueue.Put(@Packet)
else
#if (LIBAVFORMAT_VERSION < 59000000)}
    av_free_packet(@Packet);
#else
    av_packet_unref(@Packet);
#endif

finally
SDL_LockMutex(fStateLock);
UnlockParser();
}
}
if (IsQuit()) then
{
Result = false;
}
}


/********************************************
 * Decoder section
 ********************************************/

void TFFmpegDecodeStream::PauseDecoderUnlocked()
{
Inc(fDecoderPauseRequestCount);
while (fDecoderLocked) do
SDL_CondWait(fDecoderUnlockedCond, fStateLock);
}

void TFFmpegDecodeStream::PauseDecoder();
{
SDL_LockMutex(fStateLock);
PauseDecoderUnlocked();
SDL_UnlockMutex(fStateLock);
}

void TFFmpegDecodeStream::ResumeDecoderUnlocked();
{
Dec(fDecoderPauseRequestCount);
SDL_CondSignal(fDecoderResumeCond);
}

void TFFmpegDecodeStream::ResumeDecoder();
{
SDL_LockMutex(fStateLock);
ResumeDecoderUnlocked();
SDL_UnlockMutex(fStateLock);
}

void TFFmpegDecodeStream::FlushCodecBuffers();
#if LIBAVFORMAT_VERSION >= 59000000}
var
NewCtx : PAVCodecContext;
#endif
{
// if no flush operation is specified, avcodec_flush_buffers will not do anything.
if (@fCodecCtx.codec.flush <> nil) then
{
// flush buffers used by avcodec_decode_audio, etc.
avcodec_flush_buffers(fCodecCtx);
}
else
{
// we need a Workaround to avoid plopping noise with ogg-vorbis and
// mp3 (in older versions of FFmpeg).
// We will just reopen the codec.
FFmpegCore.LockAVCodec();
try
#if LIBAVFORMAT_VERSION < 59000000}
    avcodec_close(fCodecCtx);
#if LIBAVCODEC_VERSION >= 53005000}
avcodec_open2(fCodecCtx, fCodec, nil);
#else
avcodec_open(fCodecCtx, fCodec);
#endif
#else
NewCtx = FFmpegCore.GetCodecContext(fAudioStream, fCodec);
if NewCtx <> nil then
{
avcodec_free_context(@fCodecCtx);
fCodecCtx = NewCtx;
avcodec_open2(fCodecCtx, fCodec, nil);
}
else
avcodec_flush_buffers(fCodecCtx);
#endif
finally
FFmpegCore.UnlockAVCodec();
}
}
}

function TFFmpegDecodeStream::DecodeFrame() : int;
var
PaketDecodedSize : int; // size of packet data used for decoding
DataSize: int;         // size of output data decoded by FFmpeg
BlockQueue: bool;
SilenceDuration: double;
#if (LIBAVCODEC_VERSION >= 52122000) and (LIBAVCODEC_VERSION < 57037100)}
AVPacket: TAVPacket;
#endif
#ifdef UseFrameDecoderAPI}
got_frame_ptr: int;
#endif
#ifdef DebugFFmpegDecode}
TmpPos: double;
#endif
{
Result = -1;

if (EOF) then
Exit;

while (true) do
{
#if (LIBAVCODEC_VERSION >= 52122000) and (LIBAVCODEC_VERSION < 57037100)}
AVPacket = fAudioPaket;
#endif

// for titles with start_time > 0 we have to generate silence
// until we reach the pts of the first data packet.
if (fAudioPaketSilence > 0) then
{
DataSize = Min(fAudioPaketSilence, AUDIO_BUFFER_SIZE);
FillChar(fAudioBuffer[0], DataSize, 0);
Dec(fAudioPaketSilence, DataSize);
fAudioStreamPos = fAudioStreamPos + DataSize / fFormatInfo.BytesPerSec;
Result = DataSize;
Exit;
}

// read packet data
while (fAudioPaketSize > 0) do
{
DataSize = AUDIO_BUFFER_SIZE;
#if (LIBAVCODEC_VERSION >= 52122000) and (LIBAVCODEC_VERSION < 57037100)}
AVPacket.data = fAudioPaketData;
AVPacket.size = fAudioPaketSize;
#endif

#ifdef UseFrameDecoderAPI}
#if LIBAVCODEC_VERSION >= 57037100}
got_frame_ptr = avcodec_receive_frame(fCodecCtx, fAudioBufferFrame);
if (got_frame_ptr = AVERROR(EAGAIN)) then
PaketDecodedSize = fAudioPaketSize
else
PaketDecodedSize = 0;
got_frame_ptr = ord(got_frame_ptr = 0);
#else
PaketDecodedSize = avcodec_decode_audio4(fCodecCtx, fAudioBufferFrame,
    @got_frame_ptr, @AVPacket);
#endif
if (got_frame_ptr < > 0) then
{
DataSize = fAudioBufferFrame.nb_samples * fBytesPerSample;
fAudioBuffer = PByteArray(fAudioBufferFrame.data[0]);
}
else
DataSize = 0;
#else
#if LIBAVCODEC_VERSION >= 52122000} // 52.122.0
PaketDecodedSize = avcodec_decode_audio3(fCodecCtx, PSmallint(fAudioBuffer),
    DataSize, @AVPacket);
#elif LIBAVCODEC_VERSION >= 51030000} // 51.30.0
PaketDecodedSize = avcodec_decode_audio2(fCodecCtx, PSmallint(fAudioBuffer),
    DataSize, fAudioPaketData, fAudioPaketSize);
#else
PaketDecodedSize = avcodec_decode_audio(fCodecCtx, PSmallint(fAudioBuffer),
    DataSize, fAudioPaketData, fAudioPaketSize);
#endif
#endif

if (PaketDecodedSize < 0) then
    {
    // if error, skip frame
#ifdef DebugFFmpegDecode}
    DebugWriteln("Skip audio frame");
#endif
fAudioPaketSize = 0;
Break;
}

Inc(PByte(fAudioPaketData), PaketDecodedSize);
Dec(fAudioPaketSize, PaketDecodedSize);

// check if avcodec_decode_audio returned data, otherwise fetch more frames
if (DataSize <= 0) then
Continue;

// update stream position by the amount of fetched data
fAudioStreamPos = fAudioStreamPos + DataSize / fFormatInfo.BytesPerSec;

// we have data, return it and come back for more later
Result = DataSize;
Exit;
}

// free old packet data
#if (LIBAVFORMAT_VERSION < 59000000)}
if (fAudioPaket.data <> nil) then
av_free_packet(@fAudioPaket);
#else
if (PAnsiChar(fAudioPaket.data) < > STATUS_PACKET) then
av_packet_unref(@fAudioPaket);
#endif

// do not block queue on seeking (to avoid deadlocks on the DecoderLock)
if (IsSeeking()) then
BlockQueue = false
else
BlockQueue = true;

// request a new packet and block if none available.
// If this fails, the queue was aborted.
if (fPacketQueue.Get(fAudioPaket, BlockQueue) <= 0) then
Exit;

// handle Status-packet
if (PAnsiChar(fAudioPaket.data) = STATUS_PACKET) then
{
#if (LIBAVFORMAT_VERSION < 59000000)}
    fAudioPaket.data = nil;
#endif
fAudioPaketData = nil;
fAudioPaketSize = 0;

case (fAudioPaket.flags) of
PKT_STATUS_FLAG_FLUSH :
    {
        // just used if SetPositionIntern was called without the flush flag.
        FlushCodecBuffers;
    }
PKT_STATUS_FLAG_EOF: // end-of-file
    {
        // ignore EOF while seeking
        if (not IsSeeking()) then
            SetEOF(true);
    // buffer contains no data -> result = -1
    Exit;
    }
PKT_STATUS_FLAG_ERROR:
    {
        SetError(true);
    Log.LogStatus("I/O Error", "TFFmpegDecodeStream::DecodeFrame");
    Exit;
    }
PKT_STATUS_FLAG_EMPTY:
    {
        SilenceDuration = PDouble(fPacketQueue.GetStatusInfo(fAudioPaket))^;
fAudioPaketSilence = Round(SilenceDuration * fFormatInfo.SampleRate) * fFormatInfo.FrameSize;
    fPacketQueue.FreeStatusInfo(fAudioPaket);
    }
        else
            {
            Log.LogStatus("Unknown status", "TFFmpegDecodeStream::DecodeFrame");
    }
    }

    Continue;
    }

fAudioPaketData = fAudioPaket.data;
fAudioPaketSize = fAudioPaket.size;

#if LIBAVCODEC_VERSION >= 57037100}
    avcodec_send_packet(fCodecCtx, @fAudioPaket);
#endif

    // if available, update the stream position to the presentation time of this package
    if (fAudioPaket.pts <> AV_NOPTS_VALUE) then
        {
#ifdef DebugFFmpegDecode}
    TmpPos = fAudioStreamPos;
#endif
fAudioStreamPos = av_q2d(fAudioStream ^ .time_base) * fAudioPaket.pts;
#ifdef DebugFFmpegDecode}
    DebugWriteln("Timestamp: " + floattostrf(fAudioStreamPos, ffFixed, 15, 3) + " " +
        "(Calc: " + floattostrf(TmpPos, ffFixed, 15, 3) + "), " +
        "Diff: " + floattostrf(fAudioStreamPos - TmpPos, ffFixed, 15, 3));
#endif
    }
    }
    }

    function TFFmpegDecodeStream::ReadData(Buffer: PByteArray; BufferSize: int) : int;
    var
        CopyByteCount : int; // number of bytes to copy
RemainByteCount: int; // number of bytes left (remain) to read
BufferPos: int;
BufferPtr: PByte;

    // prioritize pause requests
    void LockDecoder();
    {
        SDL_LockMutex(fStateLock);
    while (fDecoderPauseRequestCount > 0) do
        SDL_CondWait(fDecoderResumeCond, fStateLock);
fDecoderLocked = true;
    SDL_UnlockMutex(fStateLock);
    }

    void UnlockDecoder();
    {
        SDL_LockMutex(fStateLock);
fDecoderLocked = false;
    SDL_CondBroadcast(fDecoderUnlockedCond);
    SDL_UnlockMutex(fStateLock);
    }

    {
        Result = -1;

    // set number of bytes to copy to the output buffer
BufferPos = 0;

#ifdef ConvertPlanar}
    if (fSwrContext <> nil) then
        {
        RemainByteCount = BufferSize mod fBytesPerSample;
BufferSize = BufferSize - RemainByteCount;
    }
#endif

    LockDecoder();
    try
        // leave if end-of-file is reached
        if (EOF) then
            Exit;

BufferPtr = nil;
#ifdef ConvertPlanar}
    if ((fSwrContext <> nil) and (fAudioBufferSize > 0)) then
        {
        BufferPtr = @Buffer[0];
BufferPos = swr_convert(fSwrContext, BufferPtr, BufferSize div fBytesPerSample,
    fAudioBufferFrame.extended_data^, 0);
if (BufferPos < 0) then // might happen if out of memory
    Exit;
BufferPos = BufferPos * fBytesPerSample;
Inc(fAudioBufferPos, BufferPos);
}
#endif

// copy data to output buffer
while (BufferPos < BufferSize) do
    {
    // check if we need more data
    if ((fAudioBufferPos >= fAudioBufferSize) or (BufferPtr <> nil)) then
        {
        fAudioBufferPos = 0;

// we have already sent all our data; get more
fAudioBufferSize = DecodeFrame();

// check for errors or EOF
if (fAudioBufferSize < 0) then
    {
    Result = BufferPos;
Exit;
}
}

RemainByteCount = BufferSize - BufferPos;

#ifdef ConvertPlanar}
if (fSwrContext <> nil) then
{
BufferPtr = @Buffer[BufferPos];
CopyByteCount = swr_convert(fSwrContext, BufferPtr, RemainByteCount div fBytesPerSample,
    fAudioBufferFrame.extended_data^, fAudioBufferFrame.nb_samples);
if (CopyByteCount < 0) then
    Exit;
CopyByteCount = CopyByteCount * fBytesPerSample;

Inc(BufferPos, CopyByteCount);
Inc(fAudioBufferPos, CopyByteCount);
continue;
}
#endif

// calc number of new bytes in the decode-buffer
CopyByteCount = fAudioBufferSize - fAudioBufferPos;
// resize copy-count if more bytes available than needed (remaining bytes are used the next time)
if (CopyByteCount > RemainByteCount) then
CopyByteCount = RemainByteCount;

Move(fAudioBuffer[fAudioBufferPos], Buffer[BufferPos], CopyByteCount);

Inc(BufferPos, CopyByteCount);
Inc(fAudioBufferPos, CopyByteCount);
}
finally
UnlockDecoder();
}

Result = BufferSize;
}


// TAudioDecoder_FFmpeg }

function TAudioDecoder_FFmpeg.GetName: String;
{
Result = "FFmpeg_Decoder";
}

function TAudioDecoder_FFmpeg.InitializeDecoder: bool;
{
//Log.LogStatus("InitializeDecoder", "UAudioDecoder_FFmpeg");
FFmpegCore = TMediaCore_FFmpeg.GetInstance();
#if LIBAVFORMAT_VERSION < 58027100}
av_register_all();
#endif

// Do not show uninformative error messages by default.
// FFmpeg prints all error-infos on the console by default what
// is very confusing as the playback of the files is correct.
// We consider these errors to be internal to FFMpeg. They can be fixed
// by the FFmpeg guys only and do not provide any useful information in
// respect to USDX.
{$IFNDEF EnableFFmpegErrorOutput}
#if LIBAVUTIL_VERSION_MAJOR >= 50}
av_log_set_level(AV_LOG_FATAL);
#else
// FATAL and ERROR share one log-level, so we have to use QUIET
av_log_set_level(AV_LOG_QUIET);
#endif
#endif

Result = true;
}

function TAudioDecoder_FFmpeg.FinalizeDecoder() : bool;
{
Result = true;
}

function TAudioDecoder_FFmpeg.Open(const Filename : IPath) : TAudioDecodeStream;
var
Stream : TFFmpegDecodeStream;
{
Result = nil;

Stream = TFFmpegDecodeStream::Create();
if (not Stream.Open(Filename)) then
{
Stream.Free;
Exit;
}

Result = Stream;
}

initialization
MediaManager.Add(TAudioDecoder_FFmpeg.Create);
}