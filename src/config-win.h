/*****************************************************************
 * Configuration file for UltraStar Deluxe
 *****************************************************************/

//* Libraries *

#ifdef IncludeConstants
  #define LUA_LIB_NAME        "lua"
  #define LUA_VERSION_INT     5001005
  #define LUA_VERSION_RELEASE "5"
  #define LUA_VERSION_MINOR   "1"
  #define LUA_VERSION_MAJOR   "5"
  #define LUA_VERSION         "5.1.5"
  #define LUA_INTEGER_BITS    32
#endif

#define HaveFFmpeg
//the required DLLs can be downloaded here: https://ffmpeg.zeranoe.com/builds/win32/shared/
#ifdef HaveFFmpeg
  //{$MACRO ON}
  #ifndef FFMPEG_DIR
    #if (0 > 0) or (4 > 0)
      #define FFMPEG_DIR "ffmpeg-4.0"
      #define FF_API_OLD_DECODE_AUDIO
    #else
      #define FFMPEG_DIR "ffmpeg"
      #define useOLD_FFMPEG
    #endif
  #endif
  #ifdef IncludeConstants
    #define FFMPEG_VERSION_INT = 4002001;
    #define av__codec "avcodec-58"
    #define LIBAVCODEC_VERSION_MAJOR 58
    #define LIBAVCODEC_VERSION_MINOR 54
    #define LIBAVCODEC_VERSION_RELEASE 100

    #define av__format "avformat-58"
    #define LIBAVFORMAT_VERSION_MAJOR 58
    #define LIBAVFORMAT_VERSION_MINOR 29
    #define LIBAVFORMAT_VERSION_RELEASE 100

    #define av__util "avutil-56"
    #define LIBAVUTIL_VERSION_MAJOR 56
    #define LIBAVUTIL_VERSION_MINOR 31
    #define LIBAVUTIL_VERSION_RELEASE 100
  #endif
#endif

#define HaveSWResample
#if defined(HaveSWResample) && defined(IncludeConstants)
  #define sw__resample "swresample-3"
  #define LIBSWRESAMPLE_VERSION_MAJOR   3
  #define LIBSWRESAMPLE_VERSION_MINOR   5
  #define LIBSWRESAMPLE_VERSION_RELEASE 100
#endif

#define HaveSWScale
#if defined(HaveSWScale) && defined(IncludeConstants)
  #define sw__scale "swscale-5"
  #define LIBSWSCALE_VERSION_MAJOR   5
  #define LIBSWSCALE_VERSION_MINOR   5
  #define LIBSWSCALE_VERSION_RELEASE 100
#endif

#define HaveProjectM
#if defined(HaveProjectM) && defined(IncludeConstants)
  #define ProjectM_DataDir "Visuals\projectM"
  #define PROJECTM_VERSION_MAJOR   1
  #define PROJECTM_VERSION_MINOR   1
  #define PROJECTM_VERSION_RELEASE 0
#endif

#undef HavePortaudio
#if defined(HavePortaudio) && defined(IncludeConstants)
  #define PORTAUDIO_VERSION_MAJOR   19
  #define PORTAUDIO_VERSION_MINOR   0
  #define PORTAUDIO_VERSION_RELEASE 0
#endif

#undef HavePortmixer

#undef HaveLibsamplerate
#if defined(HaveLibsamplerate) && defined(IncludeConstants)
  #define LIBSAMPLERATE_VERSION_MAJOR   0
  #define LIBSAMPLERATE_VERSION_MINOR   1
  #define LIBSAMPLERATE_VERSION_RELEASE 3
#endif
#define UseMIDIPort
