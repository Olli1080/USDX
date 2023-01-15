// prevent pasdoc from parsing this file
#pragma once

#include <chrono>


typedef std::chrono::duration<double, std::milli> MiliSecDouble;
typedef std::chrono::duration<double> SecDouble;
typedef std::chrono::duration<double, std::ratio<60, 1>> MinDouble;
#ifndef PASDOC

// compiler/IDE dependent config
/*
#ifdef FPC
  {$H+} // use AnsiString instead of ShortString as String-type (default in Delphi)

  // if -dDEBUG is specified on the command-line, FPC uses some default
  // compiler-flags specified in fpc.cfg -> use -dDEBUG_MODE instead
  #ifdef DEBUG_MODE}
    #define DEBUG}
  #endif

  #define HasInline}
{$ELSE}
  #define Delphi}

  // Delphi version numbers (ignore Delphi < 7 and Delphi 8 (VER160))

  #if defined(VER180)} // Delphi 2006 (=10)
    #define DELPHI_10}
    #define DELPHI_7_UP}
    #define DELPHI_9_UP}
    #define DELPHI_10_UP}
  #elif defined(VER170)} // Delphi 2005 (=9)
    #define DELPHI_9}
    #define DELPHI_7_UP}
    #define DELPHI_9_UP}    
  #elif defined(VER150)}
    #define DELPHI_7}
    #define DELPHI_7_UP}
  {$ELSE} // unsupported
    {$WARN ERROR 'Unsupported compiler version'}
  #endif

  // inline directive introduced with Delphi 2005
  #ifdef DELPHI_9_UP}
    #define HasInline}
  #endif
#endif
*/
// platform dependent config
#if defined(_WIN32)
  // include defines but no constants
  #include "config-win.h"

  #ifdef _DEBUG
      #define CONSOLE
  #endif
  #define HaveBASS
#elif defined(__APPLE__)
  // include defines but no constants
  #include "config-darwin.h"

  #define CONSOLE
  //#define HaveBASS
  #define UTF8_FILENAMES
#elif defined(__unix__)
  // include defines but no constants
  #if defined(__FreeBSD__)
    #include "config-freebsd.h"
  #elif defined(__linux__)
    //#include "config-linux.h"
  #endif
  #define CONSOLE
#endif

#include "config-win.h"

// audio config
#if defined(HaveBASS)
  #define UseBASSPlayback
  #define UseBASSDecoder
  #define UseBASSInput
#elif defined(HavePortaudio)
  #define UseSDLPlayback
  //.$DEFINE UsePortaudioPlayback
  #define UsePortaudioInput
  #ifdef HavePortmixer
    #define UsePortmixer
  #endif
#else
  #define UseSDLPlayback
  #define UseSDLInput
#endif

// ffmpeg config
#ifdef HaveFFmpeg
  #define UseFFmpegDecoder
  #define UseFFmpegVideo
  #ifdef HaveSWResample
    #define UseSWResample
  #endif
  #ifdef HaveSWScale
    #define UseSWScale
  #endif
#endif

#ifdef HaveLibsamplerate
  #define UseSRCResample
#endif

// projectM config
#if defined(HaveProjectM)
  #define UseProjectM
#endif

// specify some useful defines

#if defined(UseFFmpegVideo) or defined(UseFFmpegDecoder)
  #define UseFFmpeg
#endif

#if defined(UseBASSInput) or defined(UseBASSPlayback) or defined(UseBASSDecoder)
  #define UseBASS
#endif

#if defined(UsePortaudioInput) or defined(UsePortaudioPlayback)
  #define UsePortaudio
#endif

#endif