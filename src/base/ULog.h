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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/ULog.pas $
 * $Id: ULog.pas 3117 2015-08-15 01:23:56Z basisbit $
 */
#include "../switches.h"

#include <string>
#include <array>
#include <list>
#include <filesystem>
#include <limits>
#include <mutex>
#include <fstream>
#include <chrono>
#include <source_location>

namespace ULog
{
/*
uses
  Classes,
  UPath;
*/
/*
 * LOG_LEVEL_[TYPE] defines the "minimum" index for logs of type TYPE. Each
 * level greater than this BUT less or equal than LOG_LEVEL_[TYPE]_MAX is of this type.  
 * This means a level "LOG_LEVEL_ERROR >= Level <= LOG_LEVEL_ERROR_MAX" e.g.
 * "Level := LOG_LEVEL_ERROR+2" is considered an error level.
 * This is nice for debugging if you have more or less important debug messages.
 * For example you can assign LOG_LEVEL_DEBUG+10 for the more important ones and
 * LOG_LEVEL_DEBUG+20 for less important ones and so on. By changing the log-level
 * you can hide the less important ones.  
 */
enum LogLevel
{
	DEBUG_MAX = std::numeric_limits<int>::max(),
	DEBUG = 50,
	INFO_MAX = DEBUG_MAX - 1,
	INFO = 40,
	STATUS_MAX = INFO - 1,
	STATUS = 30,
	WARN_MAX = STATUS - 1,
	WARN = 20,
	ERROR_MAX = WARN - 1,
	ERROR = 10,
	CRITICAL_MAX = ERROR - 1,
	CRITICAL = 0,
	NONE = -1,
};

  // define level that Log(File)Level is initialized with
constexpr LogLevel LOG_LEVEL_DEFAULT      = WARN;
constexpr LogLevel LOG_FILE_LEVEL_DEFAULT = ERROR;

constexpr int CONSOLE_SCROLLBACK_SIZE = 512;

typedef std::array<uint8_t, 4> TRiffChunkID;

struct TRiffChunk
{
  TRiffChunkID ID;
  uint32_t DataSize;
};

struct TRiffHeader
{
  TRiffChunk ChunkInfo;
  TRiffChunkID RiffType;
};

struct TWaveFmtChunk
{
  TRiffChunk ChunkInfo;
  uint16_t FormatTag;
  uint16_t NumChannels;
  uint32_t SamplesPerSec;
  uint32_t AvgBytesPerSec;
  uint16_t BlockAlign;
  uint16_t BitsPerSample;
};

void DebugWriteln(const std::string aString);

class TLog
{
private:
  std::ofstream LogFile;
  bool LogFileOpened;
  std::ofstream BenchmarkFile;
  bool BenchmarkFileOpened;
  std::list<std::string> ConsoleBuffer; // stores logged messages for in-game console, capped to CONSOLE_SCROLLBACK_SIZE
  std::mutex mtx;

  int LogLevel;
  // level of messages written to the log-file
  int LogFileLevel;

  void LogToFile(const std::string Text);

  int GetConsoleCount();

public:
  std::array<std::chrono::utc_clock::time_point, 32> BenchmarkTimeStart;
  std::array<std::chrono::utc_clock::duration, 32> BenchmarkTimeLength;//TDateTime;

  std::string Title; //Application Title

  // Write log message to log-file
  bool FileOutputEnabled;

  TLog();
  ~TLog();

  // benchmark
  void BenchmarkStart(int Number);
  void BenchmarkEnd(int Number);
  void LogBenchmark(const std::string Text, int Number);

  void SetLogLevel(int Level);
  int GetLogLevel();
  void SetLogFileLevel(int Level);
  int GetLogFileLevel();

  void LogMsg(const std::string Text, int Level);
  inline void LogMsg(const std::string Msg, const std::string Context, int Level);
  inline void LogDebug(const std::string Msg, const std::string Context);
  inline void LogInfo(const std::string Msg, const std::string Context);
  inline void LogStatus(const std::string Msg, const std::string Context);
  inline void LogWarn(const std::string Msg, const std::string Context);
  inline void LogError(const std::string Text);
  inline void LogError(const std::string Msg, const std::string Context);
  //Critical Error (Halt + MessageBox)
  inline void LogCritical(const std::string Msg, const std::string Context);
  inline void CriticalError(const std::string Text);

	template<typename T = decltype(&std::source_location::function_name)>
  inline void LogMsgLocation(const std::string Msg, int Level, T member = &std::source_location::function_name,
	  const std::source_location& location = std::source_location::current())
	{
	  LogMsg(Msg, std::invoke(member, location), Level);
	}

  template<typename T = decltype(&std::source_location::function_name)>
  inline void LogDebugLocation(const std::string Msg, T member = &std::source_location::function_name,
	  const std::source_location& location = std::source_location::current())
	{
	  LogDebug(Msg, std::invoke(member, location));
	}

  template<typename T = decltype(&std::source_location::function_name)>
  inline void LogInfoLocation(const std::string Msg, T member = &std::source_location::function_name,
	  const std::source_location& location = std::source_location::current())
	{
	  LogInfo(Msg, std::invoke(member, location));
	}

  template<typename T = decltype(&std::source_location::function_name)>
  inline void LogStatusLocation(const std::string Msg, T member = &std::source_location::function_name,
	  const std::source_location& location = std::source_location::current())
	{
	  LogStatus(Msg, std::invoke(member, location));
	}

  template<typename T = decltype(&std::source_location::function_name)>
  inline void LogWarnLocation(const std::string Msg, T member = &std::source_location::function_name,
	  const std::source_location& location = std::source_location::current())
	{
	  LogWarn(Msg, std::invoke(member, location));
	}

  template<typename T = decltype(&std::source_location::function_name)>
  inline void LogErrorLocation(const std::string Msg, T member = &std::source_location::function_name,
	  const std::source_location& location = std::source_location::current())
	{
	  LogError(Msg, std::invoke(member, location));
	}
  
  template<typename T = decltype(&std::source_location::function_name)>
  inline void LogCriticalLocation(const std::string Msg, T member = &std::source_location::function_name,
	  const std::source_location& location = std::source_location::current())
	{
	  LogCritical(Msg, std::invoke(member, location));
	}

  // voice
  void LogVoice(int SoundNr);
  // buffer
  void LogBuffer(const Pointer buf, const int bufLength, const std::filesystem::path filename);

  // console
  property ConsoleCount: integer read GetConsoleCount;
  std::string GetConsole(const int index, bool FromTheBeginning = false);
  void LogConsole(const std::string Text);
  void ClearConsoleLog();

};

TLog Log;

/*
uses
  SysUtils,
  DateUtils,
  URecord,
  UMain,
  UMusic,  
  UTime,
  UCommon,
  UCommandLine,
  UPathUtils;
*/
};