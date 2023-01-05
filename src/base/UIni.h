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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UIni.pas $
 * $Id: UIni.pas 2630 2010-09-04 10:18:40Z brunzelchen $
 */

//TODO: lots of parts in this code should be rewritten in a more object oriented way.
#include "../switches.h"

#include <string>
#include <vector>
#include <array>
#include <list>
#include <memory>
#include <filesystem>
#include <cstdint>
#include <map>

#include <yaml-cpp/node/node.h>
#include <boost/bimap.hpp>
#include <boost/assign.hpp>

#include "IniFileHelper.hpp"
#include "UCommon.h"
#include "UWebcam.h"

namespace UIni
{
static constexpr int IMaxPlayerCount = 12;
static constexpr int IMaxTeamCount = 3;

/**
 * TInputDeviceConfig stores the configuration for an input device.
 * Configurations will be stored in the InputDeviceConfig array.
 * Note that not all devices listed in InputDeviceConfig are active devices.
 * Some might be unplugged and hence unavailable.
 * Available devices are held in TAudioInputProcessor.DeviceList. Each
 * TAudioInputDevice listed there has a CfgIndex field which is the index to
 * its configuration in the InputDeviceConfig array.
 */
struct TInputDeviceConfig 
{
  std::string Name;  //**< Name of the input device
  int Input; //**< Index of the input source to use for recording
  int Latency; //**< Latency in ms, or LATENCY_AUTODETECT for default

  /**
   * Mapping of recording channels to players, e.g. ChannelToPlayerMap[0] = 2
   * maps the channel 0 (left) to player 2.
   * A player index of 0 (CHANNEL_OFF) means that the channel is not assigned
   * to any player (the channel is off).
   */
  std::vector<int> ChannelToPlayerMap;
};
typedef std::shared_ptr<TInputDeviceConfig> PInputDeviceConfig;

//Options

enum TVisualizerOption
{
    voOff, voWhenNoVideo, voWhenNoVideoAndImage, voOn
};
enum TBackgroundMusicOption
{
    bmoOff, bmoOn
};
enum TSongMenuMode
{
    smRoulette, smChessboard, smCarousel, smSlotMachine, smSlide, smList, smMosaic
};

enum class DepthMode
{
    BIT_16, BIT_32
};

enum class FullScreenMode
{
    OFF, ON, BORDERLESS
};

enum class Difficulty
{
    EASY, MEDIUM, HARD,
	Size
};

enum class PartyMode
{
	CLASSIC, CLASSIC_FREE,
    CHALLENGE, TOURNAMENT,
    Size
};

// Jukebox Lyric Fill Color
struct JukeBoxFillColors
{
    UCommon::TRGB<uint8_t> SingLineColor;
    UCommon::TRGB<uint8_t> ActualLineColor;
    UCommon::TRGB<uint8_t> NextLineColor;

    UCommon::TRGB<uint8_t> SingLineOutlineColor;
    UCommon::TRGB<uint8_t> ActualLineOutlineColor;
    UCommon::TRGB<uint8_t> NextLineOutlineColor;

    /*
    int CurrentSingLineOutlineColor;
    int CurrentActualLineOutlineColor;
    int CurrentNextLineOutlineColor;
    */
};

// WebCam
struct Webcam
{
    int ID;
    struct 
    {
        int width;
        int height;
    } resolution;
    int FPS;
    bool Flip;
    int Brightness;
    int Saturation;
    int Hue;
    UWebcam::Effect Effect;
};

struct TResolution
{
    int width;
    int height;

    [[nodiscard]] bool isValid() const { return width > 0 && height > 0; }

    [[nodiscard]] TResolution max(const TResolution& rhs) const
    {
        return { std::max(width, rhs.width), std::max(height, rhs.height) }
    }

    [[nodiscard]] bool contains(const TResolution& rhs) const
    {
        return width >= rhs.width && height >= rhs.height;
    }

    bool operator<(const TResolution& rhs) const
    {
        if (width > rhs.width)
            return false;
        return height < rhs.height;
    }

    [[nodiscard]] std::string to_string() const
    {
        return std::format("{}x{}", width, height);
    }
};

struct ScreenInfo
{
    uint8_t MaxFramerate;
    int Screens;
    bool Split;
    std::unique_ptr<TResolution> Resolution;             // Resolution for windowed mode
    std::unique_ptr<TResolution> ResolutionFullscreen;   // Resolution for double fullscreen (changing Video mode)
    DepthMode Depth;
    FullScreenMode FullScreen;
};

class TIni
{
private:

  int ExtractKeyIndex(const std::string Key, std::string Prefix, std::string Suffix);
  int GetMaxKeyIndex(std::list<std::string> Keys, const std::string Prefix, std::string Suffix);

/*
  int ReadArrayIndex(const std::vector<std::string>& SearchArray, const TCustomIniFile& IniFile,
      std::string IniSection, std::string IniProperty, int Default, bool CaseInsensitive = false);

  int ReadArrayIndex(const std::vector<std::string>& SearchArray, const TCustomIniFile& IniFile,
      std::string IniSection, std::string IniProperty, int Default, std::string DefaultValue, bool CaseInsensitive = false);

  template<std::size_t N>
  int ReadArrayIndex(const std::array<std::string, N>& SearchArray, const TCustomIniFile& IniFile,
      std::string IniSection, std::string IniProperty, int Default, bool CaseInsensitive = false);

  template<std::size_t N>
  int ReadArrayIndex(const std::array<std::string, N>& SearchArray, const TCustomIniFile& IniFile,
      std::string IniSection, std::string IniProperty, int Default, std::string DefaultValue, bool CaseInsensitive = false);
    */
  void LoadInputDeviceCfg(TMemIniFile IniFile);
  void SaveInputDeviceCfg(TIniFile IniFile);
  void LoadThemes(TCustomIniFile IniFile);

  void LoadPaths(TCustomIniFile IniFile);
  void LoadScreenModes(TCustomIniFile IniFile);
  //void LoadWebcamSettings(TCustomIniFile IniFile);

public:
  // Players or Teams colors
  std::array<int, IMaxPlayerCount> SingColor;
  std::array<std::string, IMaxPlayerCount> Name;
  std::array<int, IMaxPlayerCount> PlayerColor;
  std::array<std::string, IMaxPlayerCount> PlayerAvatar;
  std::array<int, IMaxPlayerCount> PlayerLevel;

  std::array<int, IMaxTeamCount> TeamColor;

  // Templates for Names Mod
  std::array<std::string, IMaxTeamCount> NameTeam;
  std::array<std::string, IMaxPlayerCount> NameTemplate;

  // Filename of the opened iniFile
  std::filesystem::path Filename;

  // Game
  int Players;
  int Difficulty;
  int Language;
  int SongMenu;
  int Tabs;
  int TabsAtStartup; // Tabs at Startup fix
  int Sorting;
  int ShowScores;
  int ShowWebScore;
  int Debug;
  int AVDelay;
  int MicDelay;

  // Graphics
  ScreenInfo screen_info;

  int VisualizerOption;
  int TextureSize;
  int SingWindow;
  int Oscilloscope;
  // not used
  //Spectrum:       int;
  //Spectrograph:   int;
  int MovieSize;
  int VideoPreview;
  int VideoEnabled;
  std::string PreferredCodecNames;

  // Sound
  int MicBoost;
  int ClickAssist;
  int BeatClick;
  int SavePlayback;
  int ThresholdIndex;
  int AudioOutputBufferSizeIndex;
  int VoicePassthrough;
  int MusicAutoGain;
  std::string SoundFont;

  int SyncTo;

  // Song Preview
  int PreviewVolume;
  int PreviewFading;

  // Lyrics
  int LyricsFont;
  int LyricsStyle;
  int LyricsEffect;
  int NoteLines;

  // Themes
  int Theme;
  int SkinNo;
  int Color;
  int BackgroundMusicOption;

  // Record
  std::vector<TInputDeviceConfig> InputDeviceConfig;

  // Advanced
  int LoadAnimation;
  int EffectSing;
  int ScreenFade;
  int AskBeforeDel;
  int OnSongClick;
  int LineBonus;
  int PartyPopup;
  int SingScores;
  int TopScores;
  int SingTimebarMode;
  int JukeboxTimebarMode;

  // Controller
  int Joypad;
  int Mouse;

  Webcam webcam;

  // Jukebox
  int JukeboxSongMenu;

  int JukeboxFont;
  int JukeboxStyle;
  int JukeboxEffect;
  int JukeboxAlpha;

  int JukeboxLine;
  int JukeboxProperty;

  JukeBoxFillColors juke_box_fill_colors;


  // default encoding for texts (lyrics, song-name, ...)
  TEncoding DefaultEncoding;

  void Load();
  void Save();
  void SaveNames();
  void SaveLevel();
  void SavePlayerColors();
  void SavePlayerAvatars();
  void SavePlayerLevels();
  void SaveTeamColors();
  void SaveShowWebScore();
  void SaveJukeboxSongMenu();

  void SaveSoundFont(std::string Name);
  void SaveWebcamSettings();
  void SaveNumberOfPlayers();
  void SaveSingTimebarMode();
  void SaveJukeboxTimebarMode();

  void TranslateOptionValues();

  /*{ Sets resolution.
    @return (@true when resolution was added, @false otherwise) }*/
  bool SetResolution(std::string ResolutionString, bool RemoveCurrent = false, bool NoSave = false);
  /*{ Sets resolution.
    @return (@true when resolution was added, @false otherwise) }*/
  bool SetResolution(int w, int h, bool RemoveCurrent = false, bool NoSave = false);
  /*{ Sets resolution given by the index pointing to a resolution in IResolution.
    @return (@true when resolution ID was found, @false otherwise) }*/
  bool SetResolution(int index);
  std::string GetResolution();
  std::string GetResolution(int& w, int& h);
  bool GetResolution(int index, std::string& ResolutionString);

  std::string GetResolutionFullscreen(); 
  std::string GetResolutionFullscreen(int& w, int& h);
  bool GetResolutionFullscreen(int index, std::string& ResolutionString); 

  void ClearCustomResolutions();

};

enum TSortingType
{
  sEdition, sGenre, sLanguage, sFolder, sTitle, sArtist, sArtist2, sYear, sYearReversed, sDecade, sPlaylist, Size
};

enum TSyncToType 
{
  stMusic, stLyrics, stOff
};

/*
uses
  Classes,
  IniFiles,
  SysUtils,
  TextGL,
  UCommon,
  ULog,
  UTextEncoding,
  UFilesystem,
  UPath;
*/

/* Constants for TInputDeviceConfig */
const int CHANNEL_OFF = 0;         // for field ChannelToPlayerMap
const int LATENCY_AUTODETECT = -1; // for field Latency
const TResolution DEFAULT_RESOLUTION = { 800, 600 };
const std::string DEFAULT_THEME = "Modern";
const std::vector<std::string> IPlayers = { "1", "2", "3", "4", "6", "8", "12" };
const std::vector<int> IPlayersVals = { 1, 2, 3, 4, 6, 8, 12 };

/* Variables */
inline TIni Ini;
std::set<TResolution> IResolution;
std::set<TResolution> IResolutionFullScreen;
std::set<TResolution> IResolutionCustom;
const std::set<TResolution> IFallbackResolution =
{
    { 640, 480 }, // VGA
    { 720, 480 }, // SDTV 480i, EDTV 480p [TV]
    { 720, 576 }, // SDTV 576i, EDTV 576p [TV]
    { 768, 576 }, // SDTV 576i, EDTV 576p [TV]
    { 800, 600 }, // SVGA
    { 960, 540 }, // Quarter FHD
    { 1024, 768 }, // XGA
    { 1152, 666 },
    { 1152, 864 }, // XGA+
    { 1280, 720 }, // WXGA-H
    { 1280, 800 }, // WXGA
    { 1280, 960 }, // WXGA
    { 1280, 1024 }, // SXGA
    { 1366, 768 }, // HD
    { 1400, 1050 }, // SXGA+
    { 1440, 900 }, // WXGA+
    { 1600, 900 }, // HD+
    { 1600, 1200 }, // UXGA
    { 1680, 1050 }, // WSXGA+
    { 1920, 1080 }, // FHD
    { 1920, 1200 }, // WUXGA
    { 2048, 1152 }, // QWXGA
    { 2560, 1440 }, // WQHD
    { 2560, 1600 }, // WQXGA
    { 3840, 2160 }, // 4K UHD
    { 4096, 2304 }, // 4K
    { 4096, 3072 } // HXGA
};

std::vector<std::string> ILanguage;
std::vector<std::string> ITheme;
std::vector<std::string> ISkin;

/*
 * Options
 */

const std::array<std::string, 3> IDifficulty = { "Easy", "Medium", "Hard" };
const std::array<std::string, 2> ITabs       = { "Off", "On" };

const std::array<std::string, 11> ISorting   = { "Edition", "Genre", "Language", "Folder", "Title", "Artist", "Artist2", "Year", "Year Reversed", "Decade", "Playlist" };
const std::array<std::string, 7> ISongMenuMode = { "Roulette", "Chessboard", "Carousel", "Slot Machine", "Slide", "List", "Mosaic" };

const std::array<std::string, 3> IShowScores = { "Off", "When exists", "On" };

const std::array<std::string, 2> IDebug = { "Off", "On" };

const std::set<int> IMaxFramerate = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 150, 200 };
int IScreens;
const std::array<std::string, 2> ISplit = { "Off", "On" };
const std::array<std::string, 3> IFullScreen = { "Off", "On", "Borderless" };
const std::array<std::string, 2> IDepth = { "16 bit", "32 bit" };
const std::array<std::string, 4> IVisualizer = { "Off", "WhenNoVideo", "WhenNoVideoAndImage", "On" };

const std::array<std::string, 2> IBackgroundMusic = { "Off", "On" };

const std::array<std::string, 4> ITextureSize = { "64", "128", "256", "512" };
const std::array<int, 4> ITextureSizeVals = { 64, 128, 256, 512 };

const std::array<std::string, 2> ISingWindow = { "Small", "Big" };

  // SingBar Mod
const std::array<std::string, 2> IOscilloscope = { "Off", "On" };

const std::array<std::string, 2> ISpectrum = { "Off", "On" };
const std::array<std::string, 2> ISpectrograph = { "Off", "On" };
const std::array<std::string, 3> IMovieSize = { "Half", "Full [Vid]", "Full [BG+Vid]" };
const std::array<std::string, 2> IVideoPreview = { "Off", "On" };
const std::array<std::string, 2> IVideoEnabled = { "Off", "On" };

const std::array<std::string, 2> IClickAssist = { "Off", "On" };
const std::array<std::string, 2> IBeatClick = { "Off", "On" };
const std::array<std::string, 2> ISavePlayback = { "Off", "On" };

const std::array<std::string, 4> IThreshold = { "5%", "10%", "15%", "20%" };
const std::array<double, 4> IThresholdVals = { 0.05, 0.10,  0.15,  0.20 };

const std::array<std::string, 2> IVoicePassthrough = { "Off", "On" };
  
const std::array<std::string, 4> IMusicAutoGain = { "Off", "Soft", "Medium", "Hard" };
const std::array<int, 4> IMusicAutoGainVals = { -1, 0, 1, 2 };


const std::array<std::string, 3> ISyncTo = { "Music", "Lyrics", "Off" };  


const std::array<std::string, 10> IAudioOutputBufferSize = { "Auto", "256", "512", "1024", "2048", "4096", "8192", "16384", "32768", "65536" };
const std::array<int, 10> IAudioOutputBufferSizeVals = { 0, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 };

const std::array<std::string, 10> IAudioInputBufferSize = { "Auto", "256", "512", "1024", "2048", "4096", "8192", "16384", "32768", "65536" };
const std::array<int, 10> IAudioInputBufferSizeVals = { 0, 256, 512 , 1024, 2048, 4096, 8192, 16384, 32768, 65536 };

// Song Preview
const std::array<std::string, 11> IPreviewVolume = { "Off", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%" };
const std::array<double, 11> IPreviewVolumeVals = { 0, 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00 };

const std::array<std::string, 6> IPreviewFading = { "Off", "1 Sec", "2 Secs", "3 Secs", "4 Secs", "5 Secs" };
const std::array<int, 6> IPreviewFadingVals = { 0, 1, 2, 3, 4, 5 };

const std::array<std::string, 3> ILyricsStyleCompat = { "Plain", "Oline1", "Oline2" };
const std::array<std::string, 3> ILyricsStyle = { "Regular", "Bold", "Outline" };
const std::array<std::string, 5> ILyricsEffect = { "Simple", "Zoom", "Slide", "Ball", "Shift" };
const std::array<std::string, 21> ILyricsAlpha = { "0.00", "0.05", "0.10", "0.15", "0.20", "0.25", "0.30", "0.35", "0.40", "0.45", "0.50",
                                                "0.55", "0.60", "0.65", "0.70", "0.75", "0.80", "0.85", "0.90", "0.95", "1.00" };
const std::array<double, 21> ILyricsAlphaVals = { 0, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50,
                                              0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95, 1.00 };

const std::array<std::string, 2> INoteLines = { "Off", "On" };

  // for lyric colors
std::array<std::string, 3> ILine = { "Sing", "Actual", "Next" };
std::array<std::string, 2> IAttribute = { "Fill", "Outline" };
std::array<std::string, 21> ISingLineColor = { "Blue", "Green", "Pink", "Red", "Violet", "Orange", "Yellow", "Brown", "Black", "Turquoise", "Salmon", "GreenYellow", "Lavender", "Beige", "Teal", "Orchid", "SteelBlue", "Plum", "Chocolate", "Gold", "Other" };
std::array<std::string, 10> IActualLineColor = { "Black", "Gray +3", "Gray +2", "Gray +1", "Gray", "Gray -1", "Gray -2", "Gray -3", "White", "Other" };
std::array<std::string, 10> INextLineColor = { "Black", "Gray +3", "Gray +2", "Gray +1", "Gray", "Gray -1", "Gray -2", "Gray -3", "White", "Other" };
  // outline
std::array<std::string, 3> ISingLineOColor = { "Black", "White", "Other" };
std::array<std::string, 3> IActualLineOColor = { "Black", "White", "Other" };
std::array<std::string, 3> INextLineOColor = { "Black", "White", "Other" };

std::array<UCommon::TRGB<uint8_t>, 20> IHexSingColor = { 
UCommon::TRGB<uint8_t>{0x00, 0x96, 0xFF}, 
UCommon::TRGB<uint8_t>{0x3F, 0xBF, 0x3F}, 
UCommon::TRGB<uint8_t>{0xFF, 0x3F, 0xC0}, 
UCommon::TRGB<uint8_t>{0xDC, 0x00, 0x00}, 
UCommon::TRGB<uint8_t>{0xB4, 0x3F, 0xE6}, 
UCommon::TRGB<uint8_t>{0xFF, 0x90, 0x00}, 
UCommon::TRGB<uint8_t>{0xFF, 0xFF, 0x00}, 
UCommon::TRGB<uint8_t>{0xC0, 0x7F, 0x1F}, 
UCommon::TRGB<uint8_t>{0x00, 0x00, 0x00}, 
UCommon::TRGB<uint8_t>{0x00, 0xFF, 0xE6}, 
UCommon::TRGB<uint8_t>{0xFF, 0x7F, 0x66},
UCommon::TRGB<uint8_t>{0x99, 0xFF, 0x66}, 
UCommon::TRGB<uint8_t>{0xCC, 0xCC, 0xFF}, 
UCommon::TRGB<uint8_t>{0xFF, 0xE6, 0xCC}, 
UCommon::TRGB<uint8_t>{0x33, 0x99, 0x99}, 
UCommon::TRGB<uint8_t>{0x99, 0x00, 0xCC}, 
UCommon::TRGB<uint8_t>{0x33, 0x66, 0x99}, 
UCommon::TRGB<uint8_t>{0xFF, 0x99, 0xFF}, 
UCommon::TRGB<uint8_t>{0x8A, 0x5C, 0x2E}, 
UCommon::TRGB<uint8_t>{0xFF, 0xCC, 0x33}, 
 };

std::array<UCommon::TRGB<uint8_t>, 9> IHexGrayColor = {
	UCommon::TRGB<uint8_t>{0x00, 0x00, 0x00}
, UCommon::TRGB<uint8_t>{0x20, 0x20, 0x20}
, UCommon::TRGB<uint8_t>{0x40, 0x40, 0x40}
, UCommon::TRGB<uint8_t>{0x60, 0x60, 0x60}
, UCommon::TRGB<uint8_t>{0x80, 0x80, 0x80}
, UCommon::TRGB<uint8_t>{0xA0, 0xA0, 0xA0}
, UCommon::TRGB<uint8_t>{0xC0, 0xC0, 0xC0}
, UCommon::TRGB<uint8_t>{0xD6, 0xD6, 0xD6}
, UCommon::TRGB<uint8_t>{0xFF, 0xFF, 0xFF}
 };
std::array<UCommon::TRGB<uint8_t>, 2> IHexOColor = {
	UCommon::TRGB<uint8_t>{0x00, 0x00, 0x00}
, UCommon::TRGB<uint8_t>{0xFF, 0xFF, 0xFF}
 };

std::array<std::string, 2> IJukeboxSongMenu = { "Off", "On" };

std::array<std::string, 9> IColor = { "Blue", "Green", "Pink", "Red", "Violet", "Orange", "Yellow", "Brown", "Black" };

  // Advanced
std::array<std::string, 2> ILoadAnimation = { "Off", "On" };
std::array<std::string, 2> IEffectSing = { "Off", "On" };
std::array<std::string, 2> IScreenFade = { "Off", "On" };
std::array<std::string, 2> IAskbeforeDel = { "Off", "On" };
std::array<std::string, 2> ISingScores = { "Off", "On" };
std::array<std::string, 2> ITopScores = { "All", "Player" };
std::array<std::string, 3> IOnSongClick = { "Sing", "Select Players", "Open Menu" };
const int sStartSing = 0;
const int sSelectPlayer = 1;
const int sOpenMenu = 2;

std::array<std::string, 2> ILineBonus = { "Off", "On" };
std::array<std::string, 2> IPartyPopup = { "Off", "On" };

std::array<std::string, 2> IJoypad = { "Off", "On" };
std::array<std::string, 3> IMouse = { "Off", "System", "Game" };
std::array<std::string, 3> IMouseLegacy = { "Off", "Hardware Cursor", "Software Cursor" }; // use to convert old config option to new

std::array<std::string, 3> ISingTimebarMode = { "Current", "Remaining", "Total" };
std::array<std::string, 3> IJukeboxTimebarMode = { "Current", "Remaining", "Total" };

  // Recording options
std::array<std::string, 7> IChannelPlayer = { "Off", "1", "2", "3", "4", "5", "6" };
std::array<std::string, 4> IMicBoost = { "Off", "+6dB", "+12dB", "+18dB" };

// Webcam
//std::array<std::string, 6> IWebcamResolution = { "160x120", "176x144", "320x240", "352x288", "640x480", "800x600" };
//std::array<std::string, 9> IWebcamFPS = { "10", "12", "15", "18", "20", "22", "25", "28", "30" };
//std::array<std::string, 2> IWebcamFlip = { "Off", "On" };

/*
 * Translated options
 */

std::vector<std::string> ILanguageTranslated;
std::vector<std::string> ILyricsFont;

std::array<std::string, 3> IDifficultyTranslated = { "Easy", "Medium", "Hard" };
std::array<std::string, 2> ITabsTranslated = { "Off", "On" };

std::array<std::string, 7> ISongMenuTranslated = { "Roulette", "Chessboard", "Carousel", "Slot Machine", "Slide", "List", "Mosaic" };

//const std::array<std::string, > ISortingTranslated = { "Edition", "Genre", "Language", "Folder", "Title", "Artist", "Artist2", "Year", "Decade", "Playlist" };
std::array<std::string, 10> ISortingTranslated = { "Edition", "Genre", "Language", "Folder", "Title", "Artist", "Artist2", "Year", "Year Reversed", "Decade" };

std::array<std::string, 3> IShowScoresTranslated = { "Off", "WhenExists", "On" };

std::array<std::string, 2> IDebugTranslated = { "Off", "On" };
std::vector<std::string> IAVDelay;
std::vector<std::string> IMicDelay;

std::array<std::string, 3> IFullScreenTranslated = { "Off", "On", "Borderless" };
std::array<std::string, 4> IVisualizerTranslated = { "Off", "WhenNoVideo", "WhenNoVideoAndImage", "On" };

std::array<std::string, 2> IBackgroundMusicTranslated = { "Off", "On" };
std::array<std::string, 2> ISingWindowTranslated = { "Small", "Big" };

  // SingBar Mod
std::array<std::string, 2> IOscilloscopeTranslated = { "Off", "On" };

std::array<std::string, 2> ISpectrumTranslated = { "Off", "On" };
std::array<std::string, 2> ISpectrographTranslated = { "Off", "On" };
std::array<std::string, 3> IMovieSizeTranslated = { "Half", "Full [Vid]", "Full [BG+Vid]" };
std::array<std::string, 2> IVideoPreviewTranslated = { "Off", "On" };
std::array<std::string, 2> IVideoEnabledTranslated = { "Off", "On" };

std::array<std::string, 2> IClickAssistTranslated = { "Off", "On" };
std::array<std::string, 2> IBeatClickTranslated = { "Off", "On" };
std::array<std::string, 2> ISavePlaybackTranslated = { "Off", "On" };

std::array<std::string, 2> IVoicePassthroughTranslated = { "Off", "On" };
  
std::array<std::string, 4> IMusicAutoGainTranslated = { "Off", "Soft", "Medium", "Hard" };

std::array<std::string, 3> ISyncToTranslated = { "Music", "Lyrics", "Off" };

  // Song Preview
std::array<std::string, 11> IPreviewVolumeTranslated = { "Off", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%" };

std::array<std::string, 10> IAudioBufferSizeTranslated = { "Auto", "256", "512", "1024", "2048", "4096", "8192", "16384", "32768", "65536" };

std::array<std::string, 6> IPreviewFadingTranslated = { "Off", "1 Sec", "2 Secs", "3 Secs", "4 Secs", "5 Secs" };

std::array<std::string, 3> ILyricsStyleTranslated = { "Regular", "Bold", "Outline" };
std::array<std::string, 5> ILyricsEffectTranslated = { "Simple", "Zoom", "Slide", "Ball", "Shift" };
std::array<std::string, 2> INoteLinesTranslated = { "Off", "On" };
std::array<std::string, 9> IColorTranslated = { "Blue", "Green", "Pink", "Red", "Violet", "Orange", "Yellow", "Brown", "Black" };
std::array<std::string, 16> IPlayerColorTranslated = { "Blue", "Red", "Green", "Yellow", "Orange", "Pink", "Violet", "Brown", "Gray", "Dark Blue", "Sky", "Cyan", "Flame", "Orchid", "Harlequin", "Lime" };

  // for lyric colors
std::array<std::string, 3> ILineTranslated = { "Sing", "Actual", "Next" };
std::array<std::string, 2> IPropertyTranslated = { "Fill", "Outline" };

std::array<std::string, 21> ISingLineColorTranslated = { "Blue", "Green", "Pink", "Red", "Violet", "Orange", "Yellow", "Brown", "Black", "Turquoise", "Salmon", "GreenYellow", "Lavender", "Beige", "Teal", "Orchid", "SteelBlue", "Plum", "Chocolate", "Gold", "Other" };
std::array<std::string, 10> IActualLineColorTranslated = { "Black", "Gray +3", "Gray +2", "Gray +1", "Gray", "Gray -1", "Gray -2", "Gray -3", "White", "Other" };
std::array<std::string, 10> INextLineColorTranslated = { "Black", "Gray +3", "Gray +2", "Gray +1", "Gray", "Gray -1", "Gray -2", "Gray -3", "White", "Other" };
std::array<std::string, 3> ISingLineOColorTranslated = { "Black", "White", "Other" };
std::array<std::string, 3> IActualLineOColorTranslated = { "Black", "White", "Other" };
std::array<std::string, 3> INextLineOColorTranslated = { "Black", "White", "Other" };

// Advanced
std::array<std::string, 2> ILoadAnimationTranslated = { "Off", "On" };
std::array<std::string, 2> IEffectSingTranslated = { "Off", "On" };
std::array<std::string, 2> IScreenFadeTranslated = { "Off", "On" };
std::array<std::string, 2> IAskbeforeDelTranslated = { "Off", "On" };
std::array<std::string, 3> IOnSongClickTranslated = { "Sing", "Select Players", "Open Menu" };
std::array<std::string, 2> ILineBonusTranslated = { "Off", "On" };
std::array<std::string, 2> IPartyPopupTranslated = { "Off", "On" };
std::array<std::string, 2> ISingScoresTranslated = { "Off", "On" };
std::array<std::string, 2> ITopScoresTranslated = { "All", "Player" };

std::array<std::string, 2> IJoypadTranslated = { "Off", "On" };
std::array<std::string, 3> IMouseTranslated = { "Off", "On [System Cursor]", "On [Game Cursor]" };
std::array<std::string, 3> IMouseTranslatedLegacy = { "Off", "Hardware Cursor", "Software Cursor" };

std::array<std::string, 3> ISingTimebarModeTranslated = { "Current", "Remaining", "Total" };
std::array<std::string, 3> IJukeboxTimebarModeTranslated = { "Current", "Remaining", "Total" };

  // Recording options
std::array<std::string, IMaxPlayerCount + 1> IChannelPlayerTranslated = { "Off", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12" };
std::array<std::string, 4> IMicBoostTranslated = { "Off", "+6dB", "+12dB", "+18dB" };

// Network
std::array<std::string, 2> ISendNameTranslated = { "Off", "On" };
std::array<std::string, 3> IAutoModeTranslated = { "Off", "Send", "Guardar" };
std::array<std::string, IMaxPlayerCount + 1> IAutoPlayerTranslated = { "Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6", "Player 7", "Player 8", "Player 9", "Player 10", "Player 11", "Player 12", "All" };
std::vector<std::string> IAutoScoreEasyTranslated;
std::vector<std::string> IAutoScoreMediumTranslated;
std::vector<std::string> IAutoScoreHardTranslated;

// Webcam

std::array<std::string, 2> IWebcamFlipTranslated = { "Off", "On" };
std::string GetWebcamFlipTranslated(bool flip) { return  IWebcamFlipTranslated[flip ? 1 : 0]; };
std::string SetWebcamFlipTranslated(bool flip, const std::string& translation) { return  IWebcamFlipTranslated[flip ? 1 : 0] = translation; };
//std::array<std::string, 201> IWebcamBrightness;
//std::array<std::string, 201> IWebcamSaturation;
//std::array<std::string, 361> IWebcamHue;
std::array<std::string, 11> IWebcamEffectTranslated;

// Name
std::array<std::string, IMaxPlayerCount> IPlayerTranslated = { "Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6", "Player 7", "Player 8", "Player 9", "Player 10", "Player 11", "Player 12" };

std::array<std::string, 256> IRed;
std::array<std::string, 256> IGreen;
std::array<std::string, 256> IBlue;

/*
uses
  StrUtils,
  sdl2,
  UCommandLine,
  UDataBase,
  UDllManager,
  ULanguage,
  UPlatform,
  UMain,
  URecord,
  USkins,
  UThemes,
  UPathUtils,
  UUnicodeUtils;
*/
//const int IGNORE_INDEX = -1;

}

namespace YAML {
template<>
struct convert<UIni::Webcam> {
    typedef boost::bimap<UWebcam::Effect, std::string> bimapEffectType;

    static inline boost::bimap<UWebcam::Effect, std::string> EffectMapping = 
        boost::assign::list_of<bimapEffectType::relation>
        (UWebcam::Effect::NORMAL, "NORMAL")
        (UWebcam::Effect::GRAYSCALE, "GRAYSCALE")
        (UWebcam::Effect::BLACK_AND_WHITE, "BLACK_AND_WHITE")
        (UWebcam::Effect::NEGATIVE, "NEGATIVE")
        (UWebcam::Effect::BINARY_IMAGE, "BINARY_IMAGE")
        (UWebcam::Effect::DILATE, "DILATE")
        (UWebcam::Effect::THRESHOLD, "THRESHOLD")
        (UWebcam::Effect::EDGES, "EDGES")
        (UWebcam::Effect::GAUSSIAN_BLUR, "GAUSSIAN_BLUR")
        (UWebcam::Effect::EQUALIZED, "EQUALIZED")
        (UWebcam::Effect::ERODE, "ERODE");

    static Node encode(const UIni::Webcam& rhs) {
        Node node;
        node["ID"] = rhs.ID;
        Node resolutionNode;
        resolutionNode["Width"] = rhs.resolution.width;
        resolutionNode["Height"] = rhs.resolution.height;
        node["Resolution"] = resolutionNode;
        node["FPS"] = rhs.FPS;
        node["Flip"] = rhs.Flip;
        node["Brightness"] = rhs.Brightness - 100;
        node["Saturation"] = rhs.Saturation - 100;
        node["Hue"] = rhs.Hue - 180;
        node["Effect"] = rhs.Effect;
        return node;
    }

    static bool decode(const Node& node, UIni::Webcam& rhs) {
        if (!node.IsMap() || node["ID"].IsNull()) {
            return false;
        }
        rhs.ID = node["ID"].as<int>(0);
        auto& resNode = node["Resolution"];
        if (resNode.IsDefined() && resNode.IsMap())
        {
            rhs.resolution.width = resNode["Width"].as<int>(800);
            rhs.resolution.height = resNode["Height"].as<int>(600);
        }
        rhs.FPS = node["FPS"].as<int>(30);
        rhs.Flip = node["Flip"].as<bool>(true);
        rhs.Brightness = std::clamp(node["Brightness"].as<int>(0), -100, 100) + 100;
        rhs.Saturation = std::clamp(node["Saturation"].as<int>(0), -100, 100) + 100;
        rhs.Hue = std::clamp(node["Hue"].as<int>(0), -180, 180) + 180;

        const std::string& effect = node["Effect"].as<std::string>("NORMAL");
        const auto it = EffectMapping.right.find(effect);
        if (it == EffectMapping.right.end())
            rhs.Effect = UWebcam::Effect::NORMAL;
        else
            rhs.Effect = it->second;
        
        return true;
    }
};
template<>
struct convert<UCommon::TRGB<uint8_t>> {

    static Node encode(const UCommon::TRGB<uint8_t>& rhs) {
        Node node;
        node["r"] = rhs.R;
        node["g"] = rhs.G;
        node["b"] = rhs.B;

        return node;
    }

    static bool decode(const Node& node, UCommon::TRGB<uint8_t>& rhs) {
        if (!node.IsMap()) {
            return false;
        }
        auto r = node["r"];
        auto g = node["g"];
        auto b = node["b"];

        if (r.IsNull() || g.IsNull() || b.IsNull())
            return false;

        rhs.R = r.as<uint8_t>();
        rhs.G = g.as<uint8_t>();
        rhs.B = b.as<uint8_t>();

        return true;
    }
};
/*
struct convert<a> {

    static Node encode(const a& rhs) {
        Node node;
        return node;
    }

    static bool decode(const Node& node, a& rhs) {
        if (!node.IsMap()) {
            return false;
        }


        return true;
    }
};
*/
template<>
struct convert<UIni::JukeBoxFillColors> {

    typedef UCommon::TRGB<uint8_t> Color;

    static Node encode(const UIni::JukeBoxFillColors& rhs) {
        Node node;

        node["SingLineColor"] = rhs.SingLineColor;
        node["ActualLineColor"] = rhs.ActualLineColor;
        node["NextLineColor"] = rhs.NextLineColor;
        node["SingLineOutlineColor"] = rhs.SingLineOutlineColor;
        node["ActualLineOutlineColor"] = rhs.ActualLineOutlineColor;
        node["NextLineOutlineColor"] = rhs.NextLineOutlineColor;

        return node;
    }

    static bool decode(const Node& node, UIni::JukeBoxFillColors& rhs) {
        if (!node.IsMap()) {
            return false;
        }
        if (
            !convert<Color>::decode(node["SingLineColor"], rhs.SingLineColor) ||
            std::find(UIni::IHexSingColor.begin(), UIni::IHexSingColor.end(), rhs.SingLineColor) == UIni::IHexSingColor.end()
            )
            rhs.SingLineColor = UIni::IHexSingColor[0];

        if (
            !convert<Color>::decode(node["ActualLineColor"], rhs.ActualLineColor) ||
            std::find(UIni::IHexGrayColor.begin(), UIni::IHexGrayColor.end(), rhs.ActualLineColor) == UIni::IHexGrayColor.end()
            )
            rhs.ActualLineColor = UIni::IHexGrayColor[8];

        if (
            !convert<Color>::decode(node["NextLineColor"], rhs.NextLineColor) ||
            std::find(UIni::IHexGrayColor.begin(), UIni::IHexGrayColor.end(), rhs.NextLineColor) == UIni::IHexGrayColor.end()
            )
            rhs.NextLineColor = UIni::IHexGrayColor[6];
        
        if (
            !convert<Color>::decode(node["SingLineOutlineColor"], rhs.SingLineOutlineColor) ||
            std::find(UIni::IHexOColor.begin(), UIni::IHexOColor.end(), rhs.SingLineOutlineColor) == UIni::IHexOColor.end()
            )
            rhs.SingLineOutlineColor = UIni::IHexOColor[0];

        if (
            !convert<Color>::decode(node["ActualLineOutlineColor"], rhs.ActualLineOutlineColor) ||
            std::find(UIni::IHexOColor.begin(), UIni::IHexOColor.end(), rhs.ActualLineOutlineColor) == UIni::IHexOColor.end()
            )
            rhs.ActualLineOutlineColor = UIni::IHexOColor[0];

        if (
            !convert<Color>::decode(node["NextLineOutlineColor"], rhs.NextLineOutlineColor) ||
            std::find(UIni::IHexOColor.begin(), UIni::IHexOColor.end(), rhs.NextLineOutlineColor) == UIni::IHexOColor.end()
            )
            rhs.NextLineOutlineColor = UIni::IHexOColor[0];

        return true;
    }
};
template<>
struct convert<UIni::TResolution> {

    static Node encode(const UIni::TResolution& rhs) {
        Node node;

        node["width"] = rhs.width;
        node["height"] = rhs.height;

        return node;
    }

    static bool decode(const Node& node, UIni::TResolution& rhs) {
        if (!node.IsMap()) {
            return false;
        }
        const auto width = node["width"];
        const auto height = node["height"];

        if (width.IsNull() || height.IsNull())
            return false;

        rhs.width = width.as<int>();
        rhs.height = height.as<int>();

        return true;
    }
};
template<>
struct convert<UIni::ScreenInfo> {

    typedef boost::bimap<UIni::FullScreenMode, std::string> bimapFullScreenType;

    static inline bimapFullScreenType FullScreenMapping =
        boost::assign::list_of<bimapFullScreenType::relation>
        (UIni::FullScreenMode::OFF, "OFF")
        (UIni::FullScreenMode::ON, "ON")
        (UIni::FullScreenMode::BORDERLESS, "BORDERLESS");

    static Node encode(const UIni::ScreenInfo& rhs) {
        Node node;
        return node;
    }

    static bool decode(const Node& node, UIni::ScreenInfo& rhs) {
        if (!node.IsMap()) {
            return false;
        }
        
        auto it = UIni::IMaxFramerate.lower_bound(node["MaxFrameRate"].as<int>(60));
        if (it == UIni::IMaxFramerate.end())
            --it;
        rhs.MaxFramerate = *it;
        rhs.Screens = std::clamp<uint8_t>(node["Screens"].as<uint8_t>(1), 1, 2);
        rhs.Split = node["Split"].as<bool>(false);
        const auto& fullscreenString = node["FullScreen"].as<std::string>("BORDERLESS");

        const auto it2 = FullScreenMapping.right.find(fullscreenString);
        if (it2 == FullScreenMapping.right.end())
            rhs.FullScreen = UIni::FullScreenMode::BORDERLESS;
        else
            rhs.FullScreen = it2->second;

        std::optional<UIni::TResolution> CurrentRes;
        // Check if there are any modes available

        // retrieve currently used Video Display
        std::optional<int> DisplayIndexOpt;
        SDL_DisplayMode CurrentMode;
        UIni::TResolution MaxRes = { 0, 0 };
        SDL_DisplayMode ModeIter;
        CurrentMode.h = -1; CurrentMode.w = -1;
        for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
        {
            if (SDL_GetCurrentDisplayMode(i, &CurrentMode) == 0)
            {
                DisplayIndexOpt = i;
                CurrentRes = { CurrentMode.w, CurrentMode.h };
                break;
            }
        }

        // retrieve available display modes, store into separate array
        if (DisplayIndexOpt.has_value())
        {
            const auto& DisplayIndex = DisplayIndexOpt.value();
            for (int i = 0; i < SDL_GetNumDisplayModes(DisplayIndex); ++i)
            {
                if (SDL_GetDisplayMode(DisplayIndex, i, &ModeIter) != 0)
                    continue;

                auto it = UIni::IResolutionFullScreen.emplace(ModeIter.w, ModeIter.h);
                if (it.second)
                {
                    ULog::Log.LogStatus("Found Video Mode: " + it.first->to_string(), "Video");
                    MaxRes = MaxRes.max(*it.first);
                }
            }
        }

        // if display modes are found, override fallback ones
        if (!UIni::IResolutionFullScreen.empty())
        {
            ULog::Log.LogStatus("Found resolutions: " + std::to_string(UIni::IResolutionFullScreen.size()), "Video");
            UIni::IResolution = UIni::IResolutionFullScreen;

            // reverse order TODO::Why?
            //std::ranges::reverse(IResolution);
        }
        else
            UIni::IResolution = UIni::IFallbackResolution;

        // read fullscreen resolution && verify if possible
        UIni::TResolution tempRes;
        bool valid = true;
        if (!convert<UIni::TResolution>::decode(node["ResolutionFullscreen"], tempRes))
        {
            if (CurrentRes.has_value())
                tempRes = CurrentRes.value();
            else
                valid = false;
        }
        const auto itFSC = valid ? UIni::IResolutionFullScreen.find(tempRes) : UIni::IResolutionFullScreen.end();
        if (itFSC != UIni::IResolutionFullScreen.end())
            rhs.ResolutionFullscreen = std::make_unique<UIni::TResolution>(*itFSC);

        // Check if there is a resolution configured, try using it
        valid = true;
        if (!convert<UIni::TResolution>::decode(node["Resolution"], tempRes))
        {
            // either store desktop resolution or invalid which results into DEFAULT
            if (CurrentRes.has_value())
                tempRes = CurrentRes.value();
            else
                valid = false;
        }
        const auto itR = valid ? UIni::IResolution.find(tempRes) : UIni::IResolution.end();
        if // check if stored resolution is valid
        	(itR != UIni::IResolution.end())
            rhs.Resolution = std::make_unique<UIni::TResolution>(*itR);
        else if // if resolution cannot be found, check if is larger than max resolution
        	(tempRes = { ModeIter.w, ModeIter.h }; MaxRes.isValid() && !MaxRes.contains(tempRes))
        {

            ULog::Log.LogInfo(std::format("Exceeding resolution found ({}). Reverting to standard resolution.", tempRes.to_string()), "Video");
            const auto itR2 = UIni::IResolution.find(tempRes);
            if (itR2 != UIni::IResolution.end())
                rhs.Resolution = std::make_unique<UIni::TResolution>(*itR2);
        }
        else if // append unknown mode to list
            (tempRes.isValid())
        {
            // store also as custom resolution to eventually remove it upon window size change
            //TODO:: keep in mind that we don't store custom one's in the main resolution anymore
            rhs.Resolution = std::make_unique<UIni::TResolution>(*UIni::IResolutionCustom.insert(tempRes).first);
        }
        else
        {
            // if no modes were set, failback to DEFAULT_RESOLUTION (800x600)
	        // as per http://sourceforge.net/forum/message.php?msg_id=4544965
	        // THANKS : linnex at users.sourceforge.net

            rhs.Resolution = std::make_unique<UIni::TResolution>(UIni::DEFAULT_RESOLUTION);
            ULog::Log.LogStatus(std::format("No video mode found! Default to: {} ", UIni::DEFAULT_RESOLUTION.to_string()), "Video");
            rhs.FullScreen = UIni::FullScreenMode::OFF; // default to fullscreen OFF in this case
        }
        const auto& depthString = node["Depth"].as<std::string>("BIT_32");
        switch (depthString)
        {
        case "BIT_16":
            rhs.Depth = UIni::DepthMode::BIT_16;
            break;
        default:
            rhs.Depth = UIni::DepthMode::BIT_32;
        }
        return true;
    }
};
}