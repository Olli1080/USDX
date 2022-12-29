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

#include <string>
#include <vector>
#include <array>
#include <list>
#include <memory>
#include <filesystem>
#include <cstdint>

#include "switches.h"
#include "IniFileHelper.hpp"

namespace UIni
{
static const int IMaxPlayerCount = 12;

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

class TIni
{
private:

  int ExtractKeyIndex(const std::string Key, std::string Prefix, std::string Suffix);
  int GetMaxKeyIndex(std::list<std::string> Keys, const std::string Prefix, std::string Suffix);
  
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

  void LoadInputDeviceCfg(TMemIniFile IniFile);
  void SaveInputDeviceCfg(TIniFile IniFile);
  void LoadThemes(TCustomIniFile IniFile);

  void LoadPaths(TCustomIniFile IniFile);
  void LoadScreenModes(TCustomIniFile IniFile);
  void LoadWebcamSettings(TCustomIniFile IniFile);

  template<std::size_t N>
  void AssignJukeboxColor(int JukeboxColor, std::array<std::string, N> LineColor, int R, int G, int B, TIniFile IniFile, std::string Key);

public:
  // Players or Teams colors
  std::array<int, IMaxPlayerCount> SingColor;
  
  std::array<std::string, 16> Name;
  std::array<int, IMaxPlayerCount> PlayerColor;
  std::array<int, 3> TeamColor;

  std::array<std::string, IMaxPlayerCount> PlayerAvatar;
  std::array<int, IMaxPlayerCount> PlayerLevel;

  // Templates for Names Mod
  std::array<std::string, 3> NameTeam;
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
  uint8_t MaxFramerate;
  uint8_t MaxFramerateGet;
  int Screens;
  int Split;
  int Resolution;             // Resolution for windowed mode
  int ResolutionFullscreen;   // Resolution for real fullscreen (changing Video mode)
  int Depth;
  int VisualizerOption;
  int FullScreen;
  int TextureSize;
  int SingWindow;
  int Oscilloscope;
  // not used
  //Spectrum:       integer;
  //Spectrograph:   integer;
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

  // WebCam
  int WebcamID;
  int WebcamResolution;
  int WebcamFPS;
  int WebcamFlip;
  int WebcamBrightness;
  int WebcamSaturation;
  int WebcamHue;
  int WebcamEffect;

  // Jukebox
  int JukeboxSongMenu;

  int JukeboxFont;
  int JukeboxStyle;
  int JukeboxEffect;
  int JukeboxAlpha;

  int JukeboxLine;
  int JukeboxProperty;

  // Jukebox Lyric Fill Color
  int JukeboxSingLineColor;
  int JukeboxActualLineColor;
  int JukeboxNextLineColor;

  int JukeboxSingLineOutlineColor;
  int JukeboxActualLineOutlineColor;
  int JukeboxNextLineOutlineColor;

  int CurrentJukeboxSingLineOutlineColor;
  int CurrentJukeboxActualLineOutlineColor;
  int CurrentJukeboxNextLineOutlineColor;

  int JukeboxSingLineOtherColorR;
  int JukeboxSingLineOtherColorG;
  int JukeboxSingLineOtherColorB;

  int JukeboxActualLineOtherColorR;
  int JukeboxActualLineOtherColorG;
  int JukeboxActualLineOtherColorB;

  int JukeboxNextLineOtherColorR;
  int JukeboxNextLineOtherColorG;
  int JukeboxNextLineOtherColorB;

  int JukeboxSingLineOtherOColorR;
  int JukeboxSingLineOtherOColorG;
  int JukeboxSingLineOtherOColorB;

  int JukeboxActualLineOtherOColorR;
  int JukeboxActualLineOtherOColorG;
  int JukeboxActualLineOtherOColorB;

  int JukeboxNextLineOtherOColorR;
  int JukeboxNextLineOtherOColorG;
  int JukeboxNextLineOtherOColorB;


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
  sEdition, sGenre, sLanguage, sFolder, sTitle, sArtist, sArtist2, sYear, sYearReversed, sDecade, sPlaylist
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
const std::string DEFAULT_RESOLUTION = "800x600";
const std::string DEFAULT_THEME = "Modern";
const std::vector<std::string> IPlayers = { "1", "2", "3", "4", "6", "8", "12" };
const std::vector<int> IPlayersVals = { 1, 2, 3, 4, 6, 8, 12 };

/* Variables */
inline TIni Ini;
std::vector<std::string> IResolution;
std::vector<std::string> IResolutionFullScreen;
std::vector<std::string> IResolutionCustom;
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

const std::array<std::string, 12> IMaxFramerate = { "10", "20", "30", "40", "50", "60", "70", "80", "90", "100", "150", "200" };
const std::array<std::string, 2> IScreens = { "1", "2" };
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

std::array<std::string, 21> IHexSingColor = { "0096FF", "3FBF3F", "FF3FC0", "DC0000", "B43FE6", "FF9000", "FFFF00", "C07F1F", "000000", "00FFE6", "FF7F66",
                                                "99FF66", "CCCCFF", "FFE6CC", "339999", "9900CC", "336699", "FF99FF", "8A5C2E", "FFCC33", "" };
std::array<std::string, 10> IHexGrayColor = { "000000", "202020", "404040", "606060", "808080", "A0A0A0", "C0C0C0", "D6D6D6", "FFFFFF", "" };
std::array<std::string, 3> IHexOColor = { "000000", "FFFFFF", "" };

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
std::array<std::string, 6> IWebcamResolution = { "160x120", "176x144", "320x240", "352x288", "640x480", "800x600" };
std::array<std::string, 9> IWebcamFPS = { "10", "12", "15", "18", "20", "22", "25", "28", "30" };
std::array<std::string, 2> IWebcamFlip = { "Off", "On" };

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

std::array<std::string, 10> IAudioOutputBufferSizeTranslated = { "Auto", "256", "512", "1024", "2048", "4096", "8192", "16384", "32768", "65536" };

std::array<std::string, 10> IAudioInputBufferSizeTranslated = { "Auto", "256", "512", "1024", "2048", "4096", "8192", "16384", "32768", "65536" };

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
std::array<std::string, 201> IWebcamBrightness;
std::array<std::string, 201> IWebcamSaturation;
std::array<std::string, 361> IWebcamHue;
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
const int IGNORE_INDEX = -1;

};