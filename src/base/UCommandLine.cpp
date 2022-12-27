#include "UCommandLine.h"

namespace UCommandLine {

/**
 * Resets variables and reads info
 */
TCMDParams::TCMDParams(int argc, char** argv)
{
  //ResetVariables();
  ReadParamInfo(argc, argv);
}

void TCMDParams::ShowHelp()
{
  auto Fmt = [](std::string aString) -> std::string
  {
    return std::format("{: <15}", aString);
  };

  std::cout 
    << std::endl
    << "**************************************************************" << std::endl
    << "  UltraStar Deluxe - Command line switches                    " << std::endl
    << "**************************************************************" << std::endl
    << std::endl
    << "  " + Fmt("Switch") +" : Purpose" << std::endl
    << "  ----------------------------------------------------------" << std::endl
    << "  "+ Fmt(cMediaInterfaces) +" : Show in-use media interfaces" << std::endl
    << "  "+ Fmt(cDebug) +" : Display Debugging info" << std::endl
    << std::endl;

  exit(0);
}

/**
 * Reset Class Variables
 */
/*void TCMDParams::ResetVariables()
{
  Debug       = false;
  Benchmark   = false;
  NoLog       = false;
  ScreenMode  = scmDefault;
  Joypad      = false;
  Split       = spmDefault;

  // some value variables set when reading infos {-1: Not Set, others: Value}
  fResolution = "";
  fLanguage   = "";
  Depth       = -1;
  Screens     = -1;

  // some strings set when reading infos {Length=0 Not Set}
  SongPath    = std::nullopt;
  ConfigFile  = std::nullopt;
  ScoreFile   = std::nullopt;
}*/

/**
 * Read command-line parameters
 */
void TCMDParams::ReadParamInfo(int argc, char** argv)
{
  ScreenMode = scmDefault;
  Split = spmDefault;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", po::value<void>()->notifier(&ShowHelp), "Produce help message")
    ("h", po::value<void>()->notifier(&ShowHelp), "Alias for help")

    ("debug", po::value<bool>(&Debug)->default_value(false), "Display Debugging info")
    ("benchmark", po::value<bool>(&Benchmark)->default_value(false), "Create a benchmark.log file with start timings")
    ("nolog", po::value<bool>(&NoLog)->default_value(false), "Do not create any .log files")
    ("fullscreen", po::value<void>()->notifier([this](){ ScreenMode = scmFullscreen; }), "Start the game in full screen mode")
    ("window", po::value<void>()->notifier([this](){ ScreenMode = scmWindowed; }), "Start the game in window mode")
    ("joypad", po::value<bool>(&Joypad)->default_value(false), "Start with joypad support")
    ("split", po::value<void>()->notifier([this](){ Split = spmSplit; }), "Enable splitscreen")
    ("nosplit", po::value<void>()->notifier([this](){ Split = spmNoSplit; }), "Disable splitscreen")

    ("depth", po::value<int>()->default_value(-1)->notifier([this](int val)
    {
      if (val == 16)
        Depth = 0;
      else if (val == 32)
        Depth = 1;
      else
        Depth = -1;
    }), "Force depth to 16 or 32. Example: -Depth 16")
    ("screens", po::value<int>()->default_value(-1), "Force 1 or 2 screens. Example: -Screens 2")

    ("language", po::value<std::string>(&fLanguage)->default_value("")->notifier([this](const std::string& str)
    {
      try
      {
        Language = std::stoi(str);
      }
      catch (...)
      {}
    }), "Load language [ID] on startup. Example: -Language german")
    ("resolution", po::value<std::string>(&fResolution)->default_value("")->notifier([this](const std::string& str)
    {
      CustomResolution = str;
      try
      {
        Resolution = std::stoi(str);
      }
      catch (...)
      {}
    }), "Force resolution. Either by ID (matching an entry of the possible resolution list) or custom resolution (with the format of WIDTHxHEIGHT). Example: -Resolution 800x600")

    ("songpath", po::value<std::string>()->default_value("")->notifier([this](const std::string& str)
      { 
        std::filesystem::path file = str;
        if (std::filesystem::exists(file) && std::filesystem::is_regular_file(file))
          SongPath = file;
        else
          SongPath = std::nullopt;
      }), "Same as config Songpath. Example: -SongPath \"C:\\Ultrastar Songs\"")
    ("configfile", po::value<std::string>()->default_value("")->notifier([this](const std::string& str)
      { 
        std::filesystem::path file = str;
        if (std::filesystem::exists(file) && std::filesystem::is_regular_file(file))
          ConfigFile = file;
        else
          ConfigFile = std::nullopt;
      }), "Load configuration file [File] instead of config.ini. The path to the file has to exist. Example: -ConfigFile config.SongCreation.ini")
    ("scorefile", po::value<std::string>()->default_value("")->notifier([this](const std::string& str)
      { 
        std::filesystem::path file = str;
        if (std::filesystem::exists(file) && std::filesystem::is_regular_file(file))
          ScoreFile = file;
        else
          ScoreFile = std::nullopt;
      }), "Use [File] instead of Ultrastar.db The path to the file has to exist. Example: -ScoreFile HouseParty.db")
  ;  

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).style(po::command_line_style::case_insensitive).run(), vm);
  po::notify(vm);

/*{
  Log.LogInfo("Screens: " + Inttostr(Screens));
  Log.LogInfo("Depth: " + Inttostr(Depth));

  Log.LogInfo("Resolution: " + Inttostr(Resolution));
  Log.LogInfo("Language: " + Inttostr(Language));

  Log.LogInfo("sResolution: " + sResolution);
  Log.LogInfo("sLanguage: " + sLanguage);

  Log.LogInfo("ConfigFile: " + ConfigFile);
  Log.LogInfo("SongPath: " + SongPath);
  Log.LogInfo("ScoreFile: " + ScoreFile);
}*/
}
}