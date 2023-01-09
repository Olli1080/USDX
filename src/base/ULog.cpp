#include "ULog.h"

#include <iostream>
#include <cstdint>

#include "UCommandLine.h"
#include "UPathUtils.h"

namespace ULog
{
    
/*
 * Write to console if in debug mode (Thread-safe).
 * If debug-mode is disabled nothing is done. 
 */
void DebugWriteln(const std::string aString)
{
    #ifndef _DEBUG
    if ! assigned(Params) || Params.Debug then
    {
    #endif
    std::cout << aString;
    #ifndef _DEBUG
    }
    #endif
}

TLog::TLog() : LogLevel(DEFAULT), LogFileLevel(LOG_FILE_LEVEL_DEFAULT), FileOutputEnabled(false)
{
}

TLog::~TLog()
{
  /*
  std::unique_lock lock(mtx);
  if (BenchmarkFileOpened)
    CloseFile(BenchmarkFile);
  //if AnalyzeFileOpened then
  //  CloseFile(AnalyzeFile);
  if (LogFileOpened)
    CloseFile(LogFile);
  
  ConsoleBuffer.Free;
  */
}

void TLog::BenchmarkStart(int Number)
{
  BenchmarkTimeStart[Number] = std::chrono::utc_clock::now(); //Time;
}

void TLog::BenchmarkEnd(int Number)
{
  BenchmarkTimeLength[Number] = std::chrono::utc_clock::now() - BenchmarkTimeStart[Number];
}

void TLog::LogBenchmark(const std::string Text, int Number)
/*var
  Minutes:      integer;
  Seconds:      integer;
  Miliseconds:  integer;

  MinutesS:     string;
  SecondsS:     string;
  MilisecondsS: string;

  ValueText:    string;*/
{
  std::unique_lock lock(mtx);
  
  if (FileOutputEnabled && UCommandLine::Params.Benchmark)
  {
    if (!BenchmarkFileOpened)
    {
      BenchmarkFile.open(UPathUtils::LogPath / "Benchmark.log");
      BenchmarkFileOpened = BenchmarkFile.is_open();

      //If File is opened write Date to Benchmark File
      if (BenchmarkFileOpened)
      {
        BenchmarkFile << Title + " Benchmark File" << std::endl;
        BenchmarkFile << std::format("Date: {0:%d/%m/%y} Time: {0:%T}", std::chrono::utc_clock::now()) << std::endl;
        BenchmarkFile << "-------------------" << std::endl;

        BenchmarkFile.flush();
      }
    }

    if (BenchmarkFileOpened)
    {
      const auto& benchLength = BenchmarkTimeLength[Number];
      auto structured_time = std::chrono::hh_mm_ss(benchLength);
/*
      Miliseconds = Trunc(Frac(BenchmarkTimeLength[Number]) * 1000);
      Seconds = Trunc(BenchmarkTimeLength[Number]) mod 60;
      Minutes = Trunc((BenchmarkTimeLength[Number] - Seconds) / 60);
      */
      //ValueText = FloatToStr(BenchmarkTimeLength[Number]);

      auto mili = std::chrono::duration_cast<std::chrono::milliseconds>(structured_time.subseconds()).count();

      std::string ValueText;
      if (structured_time.minutes() == std::chrono::minutes::zero())
      {
        if (structured_time.seconds() == std::chrono::seconds::zero())
        {
            
            ValueText = std::to_string(mili) + " miliseconds";
        }
        else
        {
            std::stringstream ststr;
            ststr << std::to_string(structured_time.seconds().count()) << ","
                << std::setw(3) << std::setfill("0") << mili
                << " seconds";
            ValueText = ststr.str();
        }
      }
      else
      {
        std::stringstream ststr;
        ststr << std::to_string(structured_time.minutes().count()) << ":"
            << std::setw(2) << std::setfill('0') << structured_time.seconds().count() << ","
            << std::setw(3) << std::setfill('0') << mili
            << " minutes";
        ValueText = ststr.str();
      }

      BenchmarkFile << Text + ": " + ValueText << std::endl;
      BenchmarkFile.flush();
    }
  }
}

void TLog::LogToFile(const std::string Text)
{
  std::unique_lock lock(mtx);
  if (FileOutputEnabled && ! LogFileOpened)
  {
    LogFile.open(UPathUtils::LogPath / "Error.log");
    LogFileOpened = LogFile.is_open();

    //If File is opened write Date to Error File
    if (LogFileOpened)
    {
      LogFile << Title + " Error Log" << std::endl;
      LogFile << std::format("Date: {0:%d/%m/%y} Time: {0:%T}", std::chrono::utc_clock::now()) << std::endl;
      LogFile << "-------------------" << std::endl;

      LogFile.flush();
    }
  }

  if (LogFileOpened)
  {
    try
    {
      LogFile << Text << std::endl;
      LogFile.flush();
    }
    catch(...)
    {
      LogFileOpened = false;
    }
  }
}

void TLog::SetLogLevel(int Level)
{
  LogLevel = Level;
}

int TLog::GetLogLevel()
{
  return LogLevel;
}

void TLog::SetLogFileLevel(int Level)
{
  LogFileLevel = Level;
}

int TLog::GetLogFileLevel()
{
  return LogFileLevel;
}

void TLog::LogMsg(const std::string Text, int Level)
{
  std::string LogMsg;

  if (((Level <= LogLevel) || (Level <= LogFileLevel)))
  {
    if (Level <= CRITICAL_MAX)
      LogMsg = "CRITICAL: " + Text;
    else if (Level <= ERROR_MAX)
      LogMsg = "ERROR:  " + Text;
    else if (Level <= WARN_MAX)
      LogMsg = "WARN:   " + Text;
    else if (Level <= STATUS_MAX)
      LogMsg = "STATUS: " + Text;
    else if (Level <= INFO_MAX)
      LogMsg = "INFO:   " + Text;
    else
      LogMsg = "DEBUG:  " + Text;

    // output log-message
    if (Level <= LogLevel)
    {
      DebugWriteln(LogMsg);
      LogConsole(LogMsg);
    }
    
    // write message to log-file
    if (Level <= LogFileLevel)
    {
      LogToFile(LogMsg);
    }
  }

  // exit application on criticial errors (cannot be turned off)
  if (Level <= CRITICAL_MAX)
  {
    // Show information (window)
    //ShowMessage(Text, mtError);
    exit(Level);
  }
}

void TLog::LogMsg(const std::string Msg, const std::string Context, int Level)
{
  LogMsg(Msg + " ["+Context+"]", Level);
}

void TLog::LogDebug(const std::string Msg, const std::string Context)
{
  LogMsg(Msg, Context, DEBUG);
}

void TLog::LogInfo(const std::string Msg, const std::string Context)
{
  LogMsg(Msg, Context, INFO);
}

void TLog::LogStatus(const std::string Msg, const std::string Context)
{
  LogMsg(Msg, Context, STATUS);
}

void TLog::LogWarn(const std::string Msg, const std::string Context)
{
  LogMsg(Msg, Context, WARN);
}

void TLog::LogError(const std::string Msg, const std::string Context)
{
  LogMsg(Msg, Context, ERROR);
}

void TLog::LogError(const std::string Text)
{
  LogMsg(Text, ERROR);
}

void TLog::CriticalError(const std::string Text)
{
  LogMsg(Text, CRITICAL);
}

void TLog::LogCritical(const std::string Msg, const std::string Context)
{
  LogMsg(Msg, Context, CRITICAL);
}

void TLog::LogVoice(int SoundNr)
/*var
  Stream: TBinaryFileStream;
  Prefix: string;
  FileName: IPath;
  Num: integer;
  CaptureBuffer: TCaptureBuffer;
  Buffer: TMemoryStream;
  FormatInfo: TAudioFormatInfo;
  UseWavFile: boolean;
  FileExt: string;*/
{
  const int Channels = 1;
  const int SampleRate = 44100;
  const TRiffChunkID RIFF_CHUNK_HDR = { uint8_t('R'), uint8_t('I'), uint8_t('F'), uint8_t('F') };
  const TRiffChunkID RIFF_CHUNK_FMT = { uint8_t('f'), uint8_t('m'), uint8_t('t'), uint8_t(' ') };
  const TRiffChunkID RIFF_CHUNK_DATA = { uint8_t('d'), uint8_t('a'), uint8_t('t'), uint8_t('a') };
  const TRiffChunkID RIFF_TYPE_WAVE = { uint8_t('W'), uint8_t('A'), uint8_t('V'), uint8_t('E') };
  const int WAVE_FORMAT_PCM = 1; // PCM (uncompressed)


  CaptureBuffer = AudioInputProcessor.Sound[SoundNr];
  Buffer = CaptureBuffer.LogBuffer;
  FormatInfo = CaptureBuffer.AudioFormat;

  // ! all formats can be stored in a wav-file
  UseWavFile = (FormatInfo.Format in [asfU8, asfS16, asfS16LSB]);

  // create output filename
  for (int i = 1; i < 10000; ++i)
  {
    Prefix = Format("Voice%.4d", [Num]);
    if (UseWavFile)
      FileExt = ".wav"
    else
      FileExt = ".raw";
    FileName = LogPath.Append(Prefix + FileExt);
    if (!FileName.Exists())
      break;
  }

  // open output file
  Stream = TBinaryFileStream.Create(FileName, fmCreate);
  
  // write wav-file header
  if (UseWavFile)
  {
    TRiffHeader WaveHdr;
    WaveHdr.ChunkInfo.ID = RIFF_CHUNK_HDR;
    WaveHdr.ChunkInfo.DataSize = (sizeof(TRiffHeader) - 8) +
        sizeof(TWaveFmtChunk) + sizeof(TRiffChunk) + Buffer.Size;
    WaveHdr.RiffType = RIFF_TYPE_WAVE;
    Stream.Write(WaveHdr, sizeof(TRiffHeader));

    TWaveFmtChunk WaveFmt;
    WaveFmt.ChunkInfo.ID = RIFF_CHUNK_FMT;
    WaveFmt.ChunkInfo.DataSize = sizeof(TWaveFmtChunk) - 8;
    WaveFmt.FormatTag = WAVE_FORMAT_PCM;
    WaveFmt.NumChannels = FormatInfo.Channels;
    WaveFmt.SamplesPerSec = std::round(FormatInfo.SampleRate);
    WaveFmt.AvgBytesPerSec = std::round(FormatInfo.BytesPerSec);
    WaveFmt.BlockAlign = FormatInfo.FrameSize;
    WaveFmt.BitsPerSample = FormatInfo.SampleSize * 8;
    Stream.Write(WaveFmt, sizeof(TWaveFmtChunk));

    TRiffChunk DataChunk;
    DataChunk.ID = RIFF_CHUNK_DATA;
    DataChunk.DataSize = Buffer.Size;
    Stream.Write(DataChunk, sizeof(TRiffChunk));
  }

  Buffer.Seek(0, soBeginning);
  Stream.CopyFrom(Buffer, Buffer.Size);

  Stream.Free;
}

void TLog::LogBuffer(const std::vector<uint8_t>& buffer, const std::filesystem::path& filename)
{
    try
    {
        auto f = std::basic_ofstream<uint8_t>(filename, std::fstream::binary);
        f.write(buffer.data(), buffer.size());
    }
    catch (const std::exception& e)
    {
        Log.LogError("TLog::LogBuffer: Failed to log buffer into file \"" + filename.string() + "\". ErrMsg: " + std::string(e.what()));
    }
}

void TLog::ClearConsoleLog()
{
  std::unique_lock lock(mtx);
  ConsoleBuffer.resize(0);
}

std::string TLog::GetConsole(const int index, bool FromTheBeginning)
{
  std::unique_lock lock(mtx);

  if (FromTheBeginning)
  {
    auto it = ConsoleBuffer.begin();
    std::advance(it, index);
    return *it;
  }
  else
  {
    auto it = ConsoleBuffer.rbegin();
    std::advance(it, index);
    return *it;
    //return ConsoleBuffer[ConsoleBuffer.Count-1-index];
  }
}

int TLog::GetConsoleCount()
{
  std::unique_lock lock(mtx);
  return ConsoleBuffer.size();
}

void TLog::LogConsole(const std::string Text)
{
  std::unique_lock lock(mtx);
  ConsoleBuffer.emplace_front(Text);
  if (ConsoleBuffer.size() > CONSOLE_SCROLLBACK_SIZE)
    ConsoleBuffer.pop_front();
}

}