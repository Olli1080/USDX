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
  int ReadArrayIndex(const std::vector<std::string> SearchArray, TCustomIniFile IniFile,
      std::string IniSection, std::string IniProperty, int Default, bool CaseInsensitive = false);
  int ReadArrayIndex(const std::vector<std::string> SearchArray, TCustomIniFile IniFile,
      std::string IniSection, std::string IniProperty, int Default, std::string DefaultValue, bool CaseInsensitive = false);

  void LoadInputDeviceCfg(TMemIniFile IniFile);
  void SaveInputDeviceCfg(TIniFile IniFile);
  void LoadThemes(TCustomIniFile IniFile);

  void LoadPaths(TCustomIniFile IniFile);
  void LoadScreenModes(TCustomIniFile IniFile);
  void LoadWebcamSettings(TCustomIniFile IniFile);

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
  int WebCamID;
  int WebcamResolution;
  int WebCamFPS;
  int WebCamFlip;
  int WebCamBrightness;
  int WebCamSaturation;
  int WebCamHue;
  int WebCamEffect;

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
const std::vector<std::string> IPlayers = { u8"1", u8"2", u8"3", u8"4", u8"6", u8"8", u8"12" };
const std::vector<int> IPlayersVals = { 1, 2, 3, 4, 6, 8, 12 };

/* Variables */
TIni Ini;
std::vector<std::string> IResolution;
std::vector<std::string> IResolutionFullScreen;
std::vector<std::string> IResolutionCustom;
std::vector<std::string> ILanguage;
std::vector<std::string> ITheme;
std::vector<std::string> ISkin;

/*
 * Options
 */

const std::array<std::string, 3> IDifficulty = { u8"Easy", u8"Medium", u8"Hard" };
const std::array<std::string, 2> ITabs       = { u8"Off", u8"On" };

const std::array<std::string, 11> ISorting   = { u8"Edition", u8"Genre", u8"Language", u8"Folder", u8"Title", u8"Artist", u8"Artist2", u8"Year", u8"Year Reversed", u8"Decade", u8"Playlist" };
const std::array<std::string, 7> ISongMenuMode = { u8"Roulette", u8"Chessboard", u8"Carousel", u8"Slot Machine", u8"Slide", u8"List", u8"Mosaic" };

const std::array<std::string, 3> IShowScores = { u8"Off", u8"When exists", u8"On" };

const std::array<std::string, 2> IDebug = { u8"Off", u8"On" };

const std::array<std::string, 12> IMaxFramerate = { u8"10", u8"20", u8"30", u8"40", u8"50", u8"60", u8"70", u8"80", u8"90", u8"100", u8"150", u8"200" };
const std::array<std::string, 2> IScreens = { u8"1", u8"2" };
const std::array<std::string, 2> ISplit = { u8"Off", u8"On" };
const std::array<std::string, 3> IFullScreen = { u8"Off", u8"On", u8"Borderless" };
const std::array<std::string, 2> IDepth = { u8"16 bit", u8"32 bit" };
const std::array<std::string, 4> IVisualizer = { u8"Off", u8"WhenNoVideo", u8"WhenNoVideoAndImage", u8"On" };

const std::array<std::string, 2> IBackgroundMusic = { u8"Off", u8"On" };

const std::array<std::string, 4> ITextureSize = { u8"64", u8"128", u8"256", u8"512" };
const std::array<int, 4> ITextureSizeVals = { 64, 128, 256, 512 };

const std::array<std::string, 2> ISingWindow = { u8"Small", u8"Big" };

  // SingBar Mod
const std::array<std::string, 2> IOscilloscope = { u8"Off", u8"On" };

const std::array<std::string, 2> ISpectrum = { u8"Off", u8"On" };
const std::array<std::string, 2> ISpectrograph = { u8"Off", u8"On" };
const std::array<std::string, 3> IMovieSize = { u8"Half", u8"Full [Vid]", u8"Full [BG+Vid]" };
const std::array<std::string, 2> IVideoPreview = { u8"Off", u8"On" };
const std::array<std::string, 2> IVideoEnabled = { u8"Off", u8"On" };

const std::array<std::string, 2> IClickAssist = { u8"Off", u8"On" };
const std::array<std::string, 2> IBeatClick = { u8"Off", u8"On" };
const std::array<std::string, 2> ISavePlayback = { u8"Off", u8"On" };

const std::array<std::string, 4> IThreshold = { u8"5%", u8"10%", u8"15%", u8"20%" };
const std::array<double, 4> IThresholdVals = { 0.05, 0.10,  0.15,  0.20 };

const std::array<std::string, 2> IVoicePassthrough = { u8"Off", u8"On" };
  
const std::array<std::string, 4> IMusicAutoGain = { u8"Off", u8"Soft", u8"Medium", u8"Hard" };
const std::array<int, 4> IMusicAutoGainVals = { -1, 0, 1, 2 };


const std::array<std::string, 3> ISyncTo = { u8"Music", u8"Lyrics", u8"Off" };  


const std::array<std::string, 10> IAudioOutputBufferSize = { u8"Auto", u8"256", u8"512", u8"1024", u8"2048", u8"4096", u8"8192", u8"16384", u8"32768", u8"65536" };
const std::array<int, 10> IAudioOutputBufferSizeVals = { 0, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 };

const std::array<std::string, 10> IAudioInputBufferSize = { u8"Auto", u8"256", u8"512", u8"1024", u8"2048", u8"4096", u8"8192", u8"16384", u8"32768", u8"65536" };
const std::array<int, 10> IAudioInputBufferSizeVals = { 0, 256, 512 , 1024, 2048, 4096, 8192, 16384, 32768, 65536 };

// Song Preview
const std::array<std::string, 11> IPreviewVolume = { u8"Off", u8"10%", u8"20%", u8"30%", u8"40%", u8"50%", u8"60%", u8"70%", u8"80%", u8"90%", u8"100%" };
const std::array<double, 11> IPreviewVolumeVals = { 0, 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00 };

const std::array<std::string, 6> IPreviewFading = { u8"Off", u8"1 Sec", u8"2 Secs", u8"3 Secs", u8"4 Secs", u8"5 Secs" };
const std::array<int, 6> IPreviewFadingVals = { 0, 1, 2, 3, 4, 5 };

const std::array<std::string, 3> ILyricsStyleCompat = { u8"Plain", u8"Oline1", u8"Oline2" };
const std::array<std::string, 3> ILyricsStyle = { u8"Regular", u8"Bold", u8"Outline" };
const std::array<std::string, 5> ILyricsEffect = { u8"Simple", u8"Zoom", u8"Slide", u8"Ball", u8"Shift" };
const std::array<std::string, 21> ILyricsAlpha = { u8"0.00", u8"0.05", u8"0.10", u8"0.15", u8"0.20", u8"0.25", u8"0.30", u8"0.35", u8"0.40", u8"0.45", u8"0.50",
                                                u8"0.55", u8"0.60", u8"0.65", u8"0.70", u8"0.75", u8"0.80", u8"0.85", u8"0.90", u8"0.95", u8"1.00" };
const std::array<double, 21> ILyricsAlphaVals = { 0, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50,
                                              0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95, 1.00 };

const std::array<std::string, 2> INoteLines = { "Off", u8"On" };

  // for lyric colors
const std::array<std::string, 3> ILine = { u8"Sing", u8"Actual", u8"Next" };
const std::array<std::string, 2> IAttribute = { u8"Fill", u8"Outline" };
const std::array<std::string, 21> ISingLineColor = { u8"Blue", u8"Green", u8"Pink", u8"Red", u8"Violet", u8"Orange", u8"Yellow", u8"Brown", u8"Black", u8"Turquoise", u8"Salmon", u8"GreenYellow", u8"Lavender", u8"Beige", u8"Teal", u8"Orchid", u8"SteelBlue", u8"Plum", u8"Chocolate", u8"Gold", u8"Other" };
const std::array<std::string, 10> IActualLineColor = { u8"Black", u8"Gray +3", u8"Gray +2", u8"Gray +1", u8"Gray", u8"Gray -1", u8"Gray -2", u8"Gray -3", u8"White", u8"Other" };
const std::array<std::string, 10> INextLineColor = { u8"Black", u8"Gray +3", u8"Gray +2", u8"Gray +1", u8"Gray", u8"Gray -1", u8"Gray -2", u8"Gray -3", u8"White", u8"Other" };
  // outline
const std::array<std::string, 3> ISingLineOColor = { u8"Black", u8"White", u8"Other" };
const std::array<std::string, 3> IActualLineOColor = { u8"Black", u8"White", u8"Other" };
const std::array<std::string, 3> INextLineOColor = { u8"Black", u8"White", u8"Other" };

const std::array<std::string, 21> IHexSingColor = { u8"0096FF", u8"3FBF3F", u8"FF3FC0", u8"DC0000", u8"B43FE6", u8"FF9000", u8"FFFF00", u8"C07F1F", u8"000000", u8"00FFE6", u8"FF7F66",
                                                u8"99FF66", u8"CCCCFF", u8"FFE6CC", u8"339999", u8"9900CC", u8"336699", u8"FF99FF", u8"8A5C2E", u8"FFCC33", u8"" };
const std::array<std::string, 10> IHexGrayColor = { u8"000000", u8"202020", u8"404040", u8"606060", u8"808080", u8"A0A0A0", u8"C0C0C0", u8"D6D6D6", u8"FFFFFF", u8"" };
const std::array<std::string, 3> IHexOColor = { u8"000000", u8"FFFFFF", u8"" };

const std::array<std::string, 2> IJukeboxSongMenu = { u8"Off", u8"On" };

const std::array<std::string, 9> IColor = { u8"Blue", u8"Green", u8"Pink", u8"Red", u8"Violet", u8"Orange", u8"Yellow", u8"Brown", u8"Black" };

  // Advanced
const std::array<std::string, 2> ILoadAnimation = { u8"Off", u8"On" };
const std::array<std::string, 2> IEffectSing = { u8"Off", u8"On" };
const std::array<std::string, 2> IScreenFade = { u8"Off", u8"On" };
const std::array<std::string, 2> IAskbeforeDel = { u8"Off", u8"On" };
const std::array<std::string, 2> ISingScores = { u8"Off", u8"On" };
const std::array<std::string, 2> ITopScores = { u8"All", u8"Player" };
const std::array<std::string, 3> IOnSongClick = { u8"Sing", u8"Select Players", u8"Open Menu" };
const int sStartSing = 0;
const int sSelectPlayer = 1;
const int sOpenMenu = 2;

const std::array<std::string, 2> ILineBonus = { u8"Off", u8"On" };
const std::array<std::string, 2> IPartyPopup = { u8"Off", u8"On" };

const std::array<std::string, 2> IJoypad = { u8"Off", u8"On" };
const std::array<std::string, 3> IMouse = { u8"Off", u8"System", u8"Game" };
const std::array<std::string, 3> IMouseLegacy = { u8"Off", u8"Hardware Cursor", u8"Software Cursor" }; // use to convert old config option to new

const std::array<std::string, 3> ISingTimebarMode = { u8"Current", u8"Remaining", u8"Total" };
const std::array<std::string, 3> IJukeboxTimebarMode = { u8"Current", u8"Remaining", u8"Total" };

  // Recording options
const std::array<std::string, 7> IChannelPlayer = { u8"Off", u8"1", u8"2", u8"3", u8"4", u8"5", u8"6" };
const std::array<std::string, 4> IMicBoost = { u8"Off", u8"+6dB", u8"+12dB", u8"+18dB" };

  // Webcam
const std::array<std::string, 6> IWebcamResolution = { u8"160x120", u8"176x144", u8"320x240", u8"352x288", u8"640x480", u8"800x600" };
const std::array<std::string, 9> IWebcamFPS = { u8"10", u8"12", u8"15", u8"18", u8"20", u8"22", u8"25", u8"28", u8"30" };
const std::array<std::string, 2> IWebcamFlip = { u8"Off", u8"On" };

/*
 * Translated options
 */

std::vector<std::string> ILanguageTranslated;
std::vector<std::string> ILyricsFont;

const std::array<std::string, 3> IDifficultyTranslated = { u8"Easy", u8"Medium", u8"Hard" };
const std::array<std::string, 2> ITabsTranslated = { u8"Off", u8"On" };

const std::array<std::string, 7> ISongMenuTranslated = { u8"Roulette", u8"Chessboard", u8"Carousel", u8"Slot Machine", u8"Slide", u8"List", u8"Mosaic" };

//const std::array<std::string, > ISortingTranslated = { u8"Edition", u8"Genre", u8"Language", u8"Folder", u8"Title", u8"Artist", u8"Artist2", u8"Year", u8"Decade", u8"Playlist" };
const std::array<std::string, 10> ISortingTranslated = { u8"Edition", u8"Genre", u8"Language", u8"Folder", u8"Title", u8"Artist", u8"Artist2", u8"Year", u8"Year Reversed", u8"Decade" };

const std::array<std::string, 3> IShowScoresTranslated = { u8"Off", u8"WhenExists", u8"On" };

const std::array<std::string, 2> IDebugTranslated = { u8"Off", u8"On" };
std::vector<std::string> IAVDelay;
std::vector<std::string> IMicDelay;

const std::array<std::string, 3> IFullScreenTranslated = { u8"Off", u8"On", u8"Borderless" };
const std::array<std::string, 4> IVisualizerTranslated = { u8"Off", u8"WhenNoVideo", u8"WhenNoVideoAndImage", u8"On" };

const std::array<std::string, 2> IBackgroundMusicTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> ISingWindowTranslated = { u8"Small", u8"Big" };

  // SingBar Mod
const std::array<std::string, 2> IOscilloscopeTranslated = { u8"Off", u8"On" };

const std::array<std::string, 2> ISpectrumTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> ISpectrographTranslated = { u8"Off", u8"On" };
const std::array<std::string, 3> IMovieSizeTranslated = { u8"Half", u8"Full [Vid]", u8"Full [BG+Vid]" };
const std::array<std::string, 2> IVideoPreviewTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> IVideoEnabledTranslated = { u8"Off", u8"On" };

const std::array<std::string, 2> IClickAssistTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> IBeatClickTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> ISavePlaybackTranslated = { u8"Off", u8"On" };

const std::array<std::string, 2> IVoicePassthroughTranslated = { u8"Off", u8"On" };
  
const std::array<std::string, 4> IMusicAutoGainTranslated = { u8"Off", u8"Soft", u8"Medium", u8"Hard" };

const std::array<std::string, 3> ISyncToTranslated = { u8"Music", u8"Lyrics", u8"Off" };

  // Song Preview
const std::array<std::string, 11> IPreviewVolumeTranslated = { u8"Off", u8"10%", u8"20%", u8"30%", u8"40%", u8"50%", u8"60%", u8"70%", u8"80%", u8"90%", u8"100%" };

const std::array<std::string, 10> IAudioOutputBufferSizeTranslated = { u8"Auto", u8"256", u8"512", u8"1024", u8"2048", u8"4096", u8"8192", u8"16384", u8"32768", u8"65536" };

const std::array<std::string, 10> IAudioInputBufferSizeTranslated = { u8"Auto", u8"256", u8"512", u8"1024", u8"2048", u8"4096", u8"8192", u8"16384", u8"32768", u8"65536" };

const std::array<std::string, 6> IPreviewFadingTranslated = { u8"Off", u8"1 Sec", u8"2 Secs", u8"3 Secs", u8"4 Secs", u8"5 Secs" };

const std::array<std::string, 3> ILyricsStyleTranslated = { u8"Regular", u8"Bold", u8"Outline" };
const std::array<std::string, 5> ILyricsEffectTranslated = { u8"Simple", u8"Zoom", u8"Slide", u8"Ball", u8"Shift" };
const std::array<std::string, 2> INoteLinesTranslated = { u8"Off", u8"On" };
const std::array<std::string, 9> IColorTranslated = { u8"Blue", u8"Green", u8"Pink", u8"Red", u8"Violet", u8"Orange", u8"Yellow", u8"Brown", u8"Black" };
const std::array<std::string, 16> IPlayerColorTranslated = { u8"Blue", u8"Red", u8"Green", u8"Yellow", u8"Orange", u8"Pink", u8"Violet", u8"Brown", u8"Gray", u8"Dark Blue", u8"Sky", u8"Cyan", u8"Flame", u8"Orchid", u8"Harlequin", u8"Lime" };

  // for lyric colors
const std::array<std::string, 3> ILineTranslated = { u8"Sing", u8"Actual", u8"Next" };
const std::array<std::string, 2> IPropertyTranslated = { u8"Fill", u8"Outline" };

const std::array<std::string, 21> ISingLineColorTranslated = { u8"Blue", u8"Green", u8"Pink", u8"Red", u8"Violet", u8"Orange", u8"Yellow", u8"Brown", u8"Black", u8"Turquoise", u8"Salmon", u8"GreenYellow", u8"Lavender", u8"Beige", u8"Teal", u8"Orchid", u8"SteelBlue", u8"Plum", u8"Chocolate", u8"Gold", u8"Other" };
const std::array<std::string, 10> IActualLineColorTranslated = { u8"Black", u8"Gray +3", u8"Gray +2", u8"Gray +1", u8"Gray", u8"Gray -1", u8"Gray -2", u8"Gray -3", u8"White", u8"Other" };
const std::array<std::string, 10> INextLineColorTranslated = { u8"Black", u8"Gray +3", u8"Gray +2", u8"Gray +1", u8"Gray", u8"Gray -1", u8"Gray -2", u8"Gray -3", u8"White", u8"Other" };
const std::array<std::string, 3> ISingLineOColorTranslated = { u8"Black", u8"White", u8"Other" };
const std::array<std::string, 3> IActualLineOColorTranslated = { u8"Black", u8"White", u8"Other" };
const std::array<std::string, 3> INextLineOColorTranslated = { u8"Black", u8"White", u8"Other" };

// Advanced
const std::array<std::string, 2> ILoadAnimationTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> IEffectSingTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> IScreenFadeTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> IAskbeforeDelTranslated = { u8"Off", u8"On" };
const std::array<std::string, 3> IOnSongClickTranslated = { u8"Sing", u8"Select Players", u8"Open Menu" };
const std::array<std::string, 2> ILineBonusTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> IPartyPopupTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> ISingScoresTranslated = { u8"Off", u8"On" };
const std::array<std::string, 2> ITopScoresTranslated = { u8"All", u8"Player" };

const std::array<std::string, 2> IJoypadTranslated = { u8"Off", u8"On" };
const std::array<std::string, 3> IMouseTranslated = { u8"Off", u8"On [System Cursor]", u8"On [Game Cursor]" };
const std::array<std::string, 3> IMouseTranslatedLegacy = { u8"Off", u8"Hardware Cursor", u8"Software Cursor" };

const std::array<std::string, 3> ISingTimebarModeTranslated = { u8"Current", u8"Remaining", u8"Total" };
const std::array<std::string, 3> IJukeboxTimebarModeTranslated = { u8"Current", u8"Remaining", u8"Total" };

  // Recording options
const std::array<std::string, IMaxPlayerCount + 1> IChannelPlayerTranslated = { u8"Off", u8"1", u8"2", u8"3", u8"4", u8"5", u8"6", u8"7", u8"8", u8"9", u8"10", u8"11", u8"12" };
const std::array<std::string, 4> IMicBoostTranslated = { u8"Off", u8"+6dB", u8"+12dB", u8"+18dB" };

// Network
const std::array<std::string, 2> ISendNameTranslated = { u8"Off", u8"On" };
const std::array<std::string, 3> IAutoModeTranslated = { u8"Off", u8"Send", u8"Guardar" };
const std::array<std::string, IMaxPlayerCount + 1> IAutoPlayerTranslated = { u8"Player 1", u8"Player 2", u8"Player 3", u8"Player 4", u8"Player 5", u8"Player 6", u8"Player 7", u8"Player 8", u8"Player 9", u8"Player 10", u8"Player 11", u8"Player 12", u8"All" };
std::vector<std::string> IAutoScoreEasyTranslated;
std::vector<std::string> IAutoScoreMediumTranslated;
std::vector<std::string> IAutoScoreHardTranslated;

// Webcam
const std::array<std::string, 2> IWebcamFlipTranslated = { u8"Off", u8"On" };
const std::array<std::string, 201> IWebcamBrightness;
const std::array<std::string, 201> IWebcamSaturation;
const std::array<std::string, 361> IWebcamHue;
const std::array<std::string, 11> IWebcamEffectTranslated;

// Name
const std::array<std::string, IMaxPlayerCount> IPlayerTranslated = { u8"Player 1", u8"Player 2", u8"Player 3", u8"Player 4", u8"Player 5", u8"Player 6", u8"Player 7", u8"Player 8", u8"Player 9", u8"Player 10", u8"Player 11", u8"Player 12" };

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

void TIni.LoadWebcamSettings(IniFile: TCustomIniFile);
var
  I: integer;
begin
  for I:= 100 downto 1 do
  begin
    IWebcamBrightness[100 - I]   := "-" + IntToStr(I);
    IWebcamSaturation[100 - I]   := "-" + IntToStr(I);
  end;

  IWebcamBrightness[100]   := "0";
  IWebcamSaturation[100]   := "0";

  for I:= 1 to 100 do
  begin
    IWebcamBrightness[I + 100]   := "+" + IntToStr(I);
    IWebcamSaturation[I + 100]   := "+" + IntToStr(I);
  end;

  for I:= 180 downto 1 do
    IWebcamHue[180 - I]   := "-" + IntToStr(I);

  IWebcamHue[180]   := "0";

  for I:= 1 to 180 do
    IWebcamHue[I + 180]   := "+" + IntToStr(I);

end;

/**
 * Extracts an index of a key that is surrounded by a Prefix/Suffix pair.
 * Example: ExtractKeyIndex("MyKey[1]", "[", "]") will return 1.
 */
function TIni.ExtractKeyIndex(const Key, Prefix, Suffix: string): integer;
var
  Value: string;
  Start: integer;
  PrefixPos, SuffixPos: integer;
begin
  Result := -1;

  PrefixPos := Pos(Prefix, Key);
  if (PrefixPos <= 0) then
    Exit;
  SuffixPos := Pos(Suffix, Key);
  if (SuffixPos <= 0) then
    Exit;

  Start := PrefixPos + Length(Prefix);

  // copy all between prefix and suffix
  Value  := Copy(Key, Start, SuffixPos - Start);
  Result := StrToIntDef(Value, -1);
end;

/**
 * Finds the maximum key-index in a key-list.
 * The indexes of the list are surrounded by Prefix/Suffix,
 * e.g. MyKey[1] (Prefix="[", Suffix="]")
 */
function TIni.GetMaxKeyIndex(Keys: TStringList; const Prefix, Suffix: string): integer;
var
  i:        integer;
  KeyIndex: integer;
begin
  Result := -1;

  for i := 0 to Keys.Count-1 do
  begin
    KeyIndex := ExtractKeyIndex(Keys[i], Prefix, Suffix);
    if (KeyIndex > Result) then
      Result := KeyIndex;
  end;
end;

/**
 * Reads the property IniSeaction:IniProperty from IniFile and
 * finds its corresponding index in SearchArray.
 * If SearchArray does not contain the property value, the default value is
 * returned.
 */
function TIni.ReadArrayIndex(const SearchArray: array of UTF8String; IniFile: TCustomIniFile;
    IniSection: string; IniProperty: string; Default: integer; CaseInsensitive: boolean = false): integer;
begin
  Result := ReadArrayIndex(SearchArray, IniFile, IniSection, IniProperty, Default, "", CaseInsensitive);
end;

function TIni.ReadArrayIndex(const SearchArray: array of UTF8String; IniFile: TCustomIniFile;
          IniSection: string; IniProperty: string; Default: integer; DefaultValue: UTF8String; CaseInsensitive: boolean = false): integer;
var
  StrValue: string;
begin
  StrValue := IniFile.ReadString(IniSection, IniProperty, "");
  Result := GetArrayIndex(SearchArray, StrValue, CaseInsensitive);
  if (Result < 0) then
  begin
    if (Default = IGNORE_INDEX) and (not UCommon.Equals(StrValue, DefaultValue, not CaseInsensitive)) then
    begin
      // prioritize default string value
      Result := GetArrayIndex(SearchArray, DefaultValue, CaseInsensitive);
    end;

    if (Result < 0) or (Result > High(SearchArray)) then Result := Default;
  end;
end;

void TIni.LoadInputDeviceCfg(IniFile: TMemIniFile);
var
  DeviceCfg:    PInputDeviceConfig;
  DeviceIndex:  integer;
  ChannelCount: integer;
  ChannelIndex: integer;
  RecordKeys:   TStringList;
  i:            integer;
begin
  RecordKeys := TStringList.Create();

  // read all record-keys for filtering
  IniFile.ReadSection("Record", RecordKeys);

  SetLength(InputDeviceConfig, 0);

  for i := 0 to RecordKeys.Count-1 do
  begin
    // find next device-name
    DeviceIndex := ExtractKeyIndex(RecordKeys[i], "DeviceName[", "]");
    if (DeviceIndex >= 0) then
    begin
      if not IniFile.ValueExists("Record", Format("DeviceName[%d]", [DeviceIndex])) then
        Continue;

      // resize list
      SetLength(InputDeviceConfig, Length(InputDeviceConfig)+1);

      // read an input device"s config.
      // Note: All devices are appended to the list whether they exist or not.
      //   Otherwise an external device"s config will be lost if it is not
      //   connected (e.g. singstar mics or USB-Audio devices).
      DeviceCfg := @InputDeviceConfig[High(InputDeviceConfig)];
      DeviceCfg.Name := IniFile.ReadString("Record", Format("DeviceName[%d]", [DeviceIndex]), "");
      DeviceCfg.Input := IniFile.ReadInteger("Record", Format("Input[%d]", [DeviceIndex]), 0);
      DeviceCfg.Latency := IniFile.ReadInteger("Record", Format("Latency[%d]", [DeviceIndex]), LATENCY_AUTODETECT);

      // find the largest channel-number of the current device in the ini-file
      ChannelCount := GetMaxKeyIndex(RecordKeys, "Channel", Format("[%d]", [DeviceIndex]));
      if (ChannelCount < 0) then
        ChannelCount := 0;

      SetLength(DeviceCfg.ChannelToPlayerMap, ChannelCount);

      // read channel-to-player mapping for every channel of the current device
      // or set non-configured channels to no player (=0).
      for ChannelIndex := 0 to High(DeviceCfg.ChannelToPlayerMap) do
      begin
        DeviceCfg.ChannelToPlayerMap[ChannelIndex] :=
          IniFile.ReadInteger("Record", Format("Channel%d[%d]", [ChannelIndex+1, DeviceIndex]), CHANNEL_OFF);
      end;
    end;
  end;

  RecordKeys.Free();

  // MicBoost
  MicBoost := ReadArrayIndex(IMicBoost, IniFile, "Record", "MicBoost", IGNORE_INDEX, "Off");
  // Threshold
  ThresholdIndex := ReadArrayIndex(IThreshold, IniFile, "Record", "Threshold", 1);
end;

void TIni.SaveInputDeviceCfg(IniFile: TIniFile);
var
  DeviceIndex:  integer;
  ChannelIndex: integer;
  PlayerNumber: integer;
begin
  for DeviceIndex := 0 to High(InputDeviceConfig) do
  begin
    // DeviceName and DeviceInput
    IniFile.WriteString("Record", Format("DeviceName[%d]", [DeviceIndex+1]),
                        InputDeviceConfig[DeviceIndex].Name);
    IniFile.WriteInteger("Record", Format("Input[%d]", [DeviceIndex+1]),
                        InputDeviceConfig[DeviceIndex].Input);
    IniFile.WriteInteger("Record", Format("Latency[%d]", [DeviceIndex+1]),
                        InputDeviceConfig[DeviceIndex].Latency);

    // Channel-to-Player Mapping
    for ChannelIndex := 0 to High(InputDeviceConfig[DeviceIndex].ChannelToPlayerMap) do
    begin
      PlayerNumber := InputDeviceConfig[DeviceIndex].ChannelToPlayerMap[ChannelIndex];
      if PlayerNumber > 0 then
      begin
        IniFile.WriteInteger("Record",
            Format("Channel%d[%d]", [ChannelIndex+1, DeviceIndex+1]),
            PlayerNumber);
      end
      else
      begin
        IniFile.DeleteKey("Record",
            Format("Channel%d[%d]", [ChannelIndex+1, DeviceIndex+1]));
      end;
    end;
  end;

  // MicBoost
  IniFile.WriteString("Record", "MicBoost", IMicBoost[MicBoost]);
  // Threshold
  IniFile.WriteString("Record", "Threshold", IThreshold[ThresholdIndex]);
end;

void TIni.LoadPaths(IniFile: TCustomIniFile);
var
  PathStrings: TStringList;
  I:           integer;
begin
  PathStrings := TStringList.Create;
  IniFile.ReadSection("Directories", PathStrings);

  // Load song-paths
  for I := 0 to PathStrings.Count-1 do
  begin
    if (Pos("SONGDIR", UpperCase(PathStrings[I])) = 1) then
    begin
      AddSongPath(Path(IniFile.ReadString("Directories", PathStrings[I], "")));
    end;
  end;

  PathStrings.Free;
end;

void TIni.LoadThemes(IniFile: TCustomIniFile);
begin
  // No Theme Found
  if (Length(ITheme) = 0) then
  begin
    Log.CriticalError("Could not find any valid Themes.");
  end;

  Theme := ReadArrayIndex(ITheme, IniFile, "Themes", "Theme", IGNORE_INDEX, DEFAULT_THEME, true);
  if (Theme = -1) then
    Theme := 0;

  // Skin
  Skin.onThemeChange;

  SkinNo := ReadArrayIndex(ISkin, IniFile, "Themes", "Skin", UThemes.Theme.Themes[Theme].DefaultSkin);

  { there may be a not existing skin in the ini file
    e.g. due to manual edit or corrupted file.
    in this case we load the first Skin }
  if SkinNo = -1 then
    SkinNo := 0;

  // Color
  Color := ReadArrayIndex(IColor, IniFile, "Themes", "Color", Skin.GetDefaultColor(SkinNo));
end;

void TIni.LoadScreenModes(IniFile: TCustomIniFile);

  // swap two strings
  void swap(var s1, s2: UTF8String);
  var
    s3: string;
  begin
    s3 := s1;
    s1 := s2;
    s2 := s3;
  end;

var
  I, Success, DisplayIndex:     integer;
  CurrentMode, ModeIter, MaxMode: TSDL_DisplayMode;
  CurrentRes, ResString: string;
begin
  MaxFramerate := ReadArrayIndex(IMaxFramerate, IniFile, "Graphics", "MaxFramerate", IGNORE_INDEX, "60");
  MaxFramerateGet:= StrToInt(IMaxFramerate[MaxFramerate]);
  // Screens
  Screens := ReadArrayIndex(IScreens, IniFile, "Graphics", "Screens", 0);

  // Split mode
  Split := ReadArrayIndex(ISplit, IniFile, "Graphics", "Split", 0);

  // FullScreen
  FullScreen := ReadArrayIndex(IFullScreen, IniFile, "Graphics", "FullScreen", IGNORE_INDEX, "Borderless");

  // standard fallback resolutions
  SetLength(IResolution, 27);
  IResolution[0] := "640x480"; // VGA
  IResolution[1] := "720x480"; // SDTV 480i, EDTV 480p [TV]
  IResolution[2] := "720x576"; // SDTV 576i, EDTV 576p [TV]
  IResolution[3] := "768x576"; // SDTV 576i, EDTV 576p [TV]
  IResolution[4] := "800x600"; // SVGA
  IResolution[5] := "960x540"; // Quarter FHD
  IResolution[6] := "1024x768"; // XGA
  IResolution[7] := "1152x666";
  IResolution[8] := "1152x864"; // XGA+
  IResolution[9] := "1280x720"; // WXGA-H
  IResolution[10] := "1280x800"; // WXGA
  IResolution[11] := "1280x960"; // WXGA
  IResolution[12] := "1280x1024"; // SXGA
  IResolution[13] := "1366x768"; // HD
  IResolution[14] := "1400x1050"; // SXGA+
  IResolution[15] := "1440x900"; // WXGA+
  IResolution[16] := "1600x900"; // HD+
  IResolution[17] := "1600x1200"; // UXGA
  IResolution[18] := "1680x1050"; // WSXGA+
  IResolution[19] := "1920x1080"; // FHD
  IResolution[20] := "1920x1200"; // WUXGA
  IResolution[21] := "2048x1152"; // QWXGA
  IResolution[22] := "2560x1440"; // WQHD
  IResolution[23] := "2560x1600"; // WQXGA
  IResolution[24] := "3840x2160"; // 4K UHD
  IResolution[25] := "4096x2304"; // 4K
  IResolution[26] := "4096x3072"; // HXGA

  // Check if there are any modes available

  // retrieve currently used Video Display
  DisplayIndex := -1;
  MaxMode.h := 0; MaxMode.w := 0;
  CurrentMode.h := -1; CurrentMode.w := -1;
  for I := 0 to SDL_GetNumVideoDisplays() - 1 do
  begin
    Success := SDL_GetCurrentDisplayMode(I,  @CurrentMode);
    if Success = 0 then
    begin
      DisplayIndex := I;
      CurrentRes := BuildResolutionString(CurrentMode.w, CurrentMode.h);
      Break
    end;
  end;

  // retrieve available display modes, store into separate array
  if DisplayIndex >= 0 then
  begin
    for I := 0 to SDL_GetNumDisplayModes(DisplayIndex) - 1 do
    begin
      Success := SDL_GetDisplayMode(DisplayIndex, I, @ModeIter);
      if Success <> 0 then continue;

      ResString := BuildResolutionString(ModeIter.w, ModeIter.h);
      if GetArrayIndex(IResolutionFullScreen, ResString) < 0 then
      begin
        Log.LogStatus("Found Video Mode: " + ResString, "Video");
        SetLength(IResolutionFullScreen, Length(IResolutionFullScreen) + 1);
        IResolutionFullScreen[High(IResolutionFullScreen)] := ResString;

        if (ModeIter.w > MaxMode.w) or (ModeIter.h > ModeIter.h) then
        begin
          MaxMode := ModeIter;
        end;
      end;
    end;
  end;

  // if display modes are found, override fallback ones
  if Length(IResolutionFullScreen) > 0 then
  begin
    Log.LogStatus( "Found resolutions: " + IntToStr(Length(IResolutionFullScreen)), "Video");
    IResolution := IResolutionFullScreen;

    // reverse order
    for I := 0 to (Length(IResolution) div 2) - 1 do swap(IResolution[I], IResolution[High(IResolution)-I]);
  end;

  // read fullscreen resolution and verify if possible
  ResString := IniFile.ReadString("Graphics", "ResolutionFullscreen", CurrentRes);
  ResolutionFullscreen := GetArrayIndex(IResolutionFullScreen, ResString);

  // Check if there is a resolution configured, try using it
  ResString := IniFile.ReadString("Graphics", "Resolution", "");
  if ResString = "" then
  begin
    ResString := CurrentRes; // either store desktop resolution or invalid which results into DEFAULT
  end;

  // check if stored resolution is valid
  Resolution := GetArrayIndex(IResolution, ResString);

  // if resolution cannot be found, check if is larger than max resolution
  if (Resolution < 0) and (MaxMode.w > 0) and (MaxMode.h > 0) and
     (ParseResolutionString(ResString, ModeIter.w, ModeIter.h)) and
     ((ModeIter.w > MaxMode.w) or (ModeIter.h > MaxMode.h)) then
  begin
    Log.LogInfo(Format("Exceeding resoluton found (%s). Reverting to standard resolution.", [ResString]), "Video");
    ResString := CurrentRes;
    Resolution := GetArrayIndex(IResolution, ResString);
  end;

  // append unknown mode to list
  if (Resolution = -1) and (Length(ResString) >= 3) then
  begin
    SetLength(IResolution, Length(IResolution) + 1);
    IResolution[High(IResolution)] := ResString;
    Resolution := High(IResolution);

    // store also as custom resolution to eventually remove it upon window size change
    SetLength(IResolutionCustom, Length(IResolutionCustom) + 1);
    IResolutionCustom[High(IResolutionCustom)] := ResString;
  end;

  if (Length(IResolution) = 0) or (Resolution < 0) then
  begin
    // if no modes were set, then failback to DEFAULT_RESOLUTION (800x600)
    // as per http://sourceforge.net/forum/message.php?msg_id=4544965
    // THANKS : linnex at users.sourceforge.net
    SetLength(IResolution, Length(IResolution) + 1);
    IResolution[High(IResolution)] := DEFAULT_RESOLUTION;
    Resolution := GetArrayIndex(IResolution, DEFAULT_RESOLUTION);
    if Resolution < 0 then Resolution := 0;

    Log.LogStatus( Format("No video mode found! Default to: %s ", [IResolution[Resolution]]), "Video");
    FullScreen := 0; // default to fullscreen OFF in this case
  end;

  // Depth
  Depth := ReadArrayIndex(IDepth, IniFile, "Graphics", "Depth", IGNORE_INDEX, "32 bit");
end;

void TIni.Load();
var
  IniFile: TMemIniFile;
  I:       integer;
  IShowWebScore: array of UTF8String;
  HexColor: string;
  Col: TRGB;
begin
  LoadFontFamilyNames;
  ILyricsFont := FontFamilyNames;
  GamePath := Platform.GetGameUserPath;

  Log.LogStatus( "GamePath : " +GamePath.ToNative , "" );

  if (Params.ConfigFile.IsSet) then
    FileName := Params.ConfigFile
  else
    FileName := GamePath.Append("config.ini");

  Log.LogStatus("Using config : " + FileName.ToNative, "Ini");
  IniFile := TMemIniFile.Create(FileName.ToNative);

  for I := 0 to IMaxPlayerCount-1 do
  begin
    // Name
    Name[I] := IniFile.ReadString("Name", "P"+IntToStr(I+1), "Player"+IntToStr(I+1));
    // Color Player
    PlayerColor[I] := IniFile.ReadInteger("PlayerColor", "P"+IntToStr(I+1), I + 1);
    // Avatar Player
    PlayerAvatar[I] := IniFile.ReadString("PlayerAvatar", "P"+IntToStr(I+1), "");
    // Level Player
    PlayerLevel[I] := IniFile.ReadInteger("PlayerLevel", "P"+IntToStr(I+1), 0);
  end;

  // Color Team
  for I := 0 to 2 do
    TeamColor[I] := IniFile.ReadInteger("TeamColor", "T"+IntToStr(I+1), I + 1);

  // Templates for Names Mod
  for I := 0 to 2 do
    NameTeam[I] := IniFile.ReadString("NameTeam", "T"+IntToStr(I+1), "Team"+IntToStr(I+1));
  for I := 0 to 11 do
    NameTemplate[I] := IniFile.ReadString("NameTemplate", "Name"+IntToStr(I+1), "Template"+IntToStr(I+1));

  // Players
  Players := ReadArrayIndex(IPlayers, IniFile, "Game", "Players", 0);

  // Difficulty
  Difficulty := ReadArrayIndex(IDifficulty, IniFile, "Game", "Difficulty", IGNORE_INDEX, "Easy");

  // Language
  Language := ReadArrayIndex(ILanguage, IniFile, "Game", "Language", IGNORE_INDEX, "English");
  if Language < 0 then Language := GetArrayIndex(ILanguage, "English"); // Default to english
  if Language < 0 then Language := 0; // Default to first available

  // SongMenu
  SongMenu := ReadArrayIndex(ISongMenuMode, IniFile, "Game", "SongMenu", Ord(smRoulette));

  // Tabs
  Tabs := ReadArrayIndex(ITabs, IniFile, "Game", "Tabs", 0);
  TabsAtStartup := Tabs;	//Tabs at Startup fix

  // Song Sorting
  Sorting := ReadArrayIndex(ISorting, IniFile, "Game", "Sorting", Ord(sEdition));

  // Show Score
  ShowScores := ReadArrayIndex(IShowScores, IniFile, "Game", "ShowScores", IGNORE_INDEX, "On");

  AVDelay := IniFile.ReadInteger("Game", "AVDelay", 0);

  MicDelay := IniFile.ReadInteger("Game", "MicDelay", 140);

  // Read Users Info (Network)
  DataBase.ReadUsers;

  // Update Webs Scores
  DataBase.AddWebsite;

  // Webs Scores Path
  WebScoresPath := Path(IniFile.ReadString("Directories", "WebScoresDir", WebsitePath.ToNative));
  if not(DirectoryExists(WebScoresPath.ToNative)) then
    WebScoresPath :=  WebsitePath;

  // ShowWebScore
  if (Length(DllMan.Websites) > 0) then
  begin
    SetLength(IShowWebScore, Length(DLLMan.Websites));
    for I:= 0 to High(DllMan.Websites) do
      IShowWebScore[I] := DllMan.Websites[I].Name;
    ShowWebScore := ReadArrayIndex(IShowWebScore, IniFile, "Game", "ShowWebScore", 0);
    if (ShowWebScore = -1) then
      ShowWebScore := 0;
  end;

  // Debug
  Debug := ReadArrayIndex(IDebug, IniFile, "Game", "Debug", 0);

  LoadScreenModes(IniFile);

  LoadWebcamSettings(IniFile);

  // TextureSize (aka CachedCoverSize)
  TextureSize := ReadArrayIndex(ITextureSize, IniFile, "Graphics", "TextureSize", IGNORE_INDEX, "256");

  // SingWindow
  SingWindow := ReadArrayIndex(ISingWindow, IniFile, "Graphics", "SingWindow", IGNORE_INDEX, "Big");

  // Oscilloscope
  Oscilloscope := ReadArrayIndex(IOscilloscope, IniFile, "Graphics", "Oscilloscope", 0);

  // Spectrum
  //Spectrum := ReadArrayIndex(ISpectrum, IniFile, "Graphics", "Spectrum", IGNORE_INDEX, "Off");

  // Spectrograph
  //Spectrograph := ReadArrayIndex(ISpectrograph, IniFile, "Graphics", "Spectrograph", IGNORE_INDEX, "Off");

  // MovieSize
  MovieSize := ReadArrayIndex(IMovieSize, IniFile, "Graphics", "MovieSize", 2);

  // VideoPreview
  VideoPreview := ReadArrayIndex(IVideoPreview, IniFile, "Graphics", "VideoPreview", 1);

  // VideoEnabled
  VideoEnabled := ReadArrayIndex(IVideoEnabled, IniFile, "Graphics", "VideoEnabled", 1);

  PreferredCodecNames := IniFile.ReadString("Graphics", "PreferredCodecs", "");

  // ClickAssist
  ClickAssist := ReadArrayIndex(IClickAssist, IniFile, "Sound", "ClickAssist", IGNORE_INDEX, "Off");

  // BeatClick
  BeatClick := ReadArrayIndex(IBeatClick, IniFile, "Sound", "BeatClick", 0);

  // SavePlayback
  SavePlayback := ReadArrayIndex(ISavePlayback, IniFile, "Sound", "SavePlayback", 0);

  // AudioOutputBufferSize
  AudioOutputBufferSizeIndex := ReadArrayIndex(IAudioOutputBufferSize, IniFile, "Sound", "AudioOutputBufferSize", 0);

  //Preview Volume
  PreviewVolume := ReadArrayIndex(IPreviewVolume, IniFile, "Sound", "PreviewVolume", 7);

  //Preview Fading
  PreviewFading := ReadArrayIndex(IPreviewFading, IniFile, "Sound", "PreviewFading", 3);

  //AudioRepeat aka VoicePassthrough
  VoicePassthrough := ReadArrayIndex(IVoicePassthrough, IniFile, "Sound", "VoicePassthrough", 0);
  
  // ReplayGain aka MusicAutoGain
  MusicAutoGain := ReadArrayIndex(IMusicAutoGain, IniFile, "Sound", "MusicAutoGain", 0);

  SoundFont := IniFile.ReadString("Sound", "SoundFont", "");

  // Lyrics Font
  LyricsFont := ReadArrayIndex(ILyricsFont, IniFile, "Lyrics", "LyricsFont", 0);

  // Lyrics Style
  LyricsStyle := ReadArrayIndex(ILyricsStyleCompat, IniFile, "Lyrics", "LyricsStyle", -1);
  if (LyricsStyle = -1) then
    LyricsStyle := ReadArrayIndex(ILyricsStyle, IniFile, "Lyrics", "LyricsStyle", 0);

  // Lyrics Effect
  LyricsEffect := ReadArrayIndex(ILyricsEffect, IniFile, "Lyrics", "LyricsEffect", 2);

  // NoteLines
  NoteLines := ReadArrayIndex(INoteLines, IniFile, "Lyrics", "NoteLines", 1);

  // DefaultEncoding
  DefaultEncoding := ParseEncoding(IniFile.ReadString("Lyrics", "Encoding", ""), encAuto);

  LoadThemes(IniFile);

  LoadInputDeviceCfg(IniFile);

  // LoadAnimation
  LoadAnimation := ReadArrayIndex(ILoadAnimation, IniFile, "Advanced", "LoadAnimation", IGNORE_INDEX, "On");

  // ScreenFade
  ScreenFade := ReadArrayIndex(IScreenFade, IniFile, "Advanced", "ScreenFade", IGNORE_INDEX, "On");

  // Visualizations
  // <mog> this could be of use later..
  //  VisualizerOption :=
  //    TVisualizerOption(GetEnumValue(TypeInfo(TVisualizerOption),
  //            IniFile.ReadString("Graphics", "Visualization", "Off")));
  // || VisualizerOption := TVisualizerOption(GetArrayIndex(IVisualizer, IniFile.ReadString("Graphics", "Visualization", "Off")));
  VisualizerOption := ReadArrayIndex(IVisualizer, IniFile, "Graphics", "Visualization", IGNORE_INDEX, "Off");

/**
 * Background music
 */
  BackgroundMusicOption := ReadArrayIndex(IBackgroundMusic, IniFile, "Sound", "BackgroundMusic", IGNORE_INDEX, "On");

  // EffectSing
  EffectSing := ReadArrayIndex(IEffectSing, IniFile, "Advanced", "EffectSing", IGNORE_INDEX, "On");

  // AskbeforeDel
  AskBeforeDel := ReadArrayIndex(IAskbeforeDel, IniFile, "Advanced", "AskbeforeDel", IGNORE_INDEX, "On");

  // OnSongClick
  OnSongClick := ReadArrayIndex(IOnSongClick, IniFile, "Advanced", "OnSongClick", IGNORE_INDEX, "Sing");

  // Linebonus
  LineBonus := ReadArrayIndex(ILineBonus, IniFile, "Advanced", "LineBonus", 1);

  // PartyPopup
  PartyPopup := ReadArrayIndex(IPartyPopup, IniFile, "Advanced", "PartyPopup", IGNORE_INDEX, "On");

  // SingScores
  SingScores := ReadArrayIndex(ISingScores, IniFile, "Advanced", "SingScores", IGNORE_INDEX, "On");

  // TopScores
  TopScores := ReadArrayIndex(ITopScores, IniFile, "Advanced", "TopScores", IGNORE_INDEX, "All");

  // SyncTo
  SyncTo := ReadArrayIndex(ISyncTo, IniFile, "Advanced", "SyncTo", Ord(stMusic));

  // Joypad
  Joypad := ReadArrayIndex(IJoypad, IniFile, "Controller", "Joypad", 0);

  // Mouse
  Mouse := ReadArrayIndex(IMouse, IniFile, "Controller", "Mouse", 2);
  if Mouse < 0 then // try finding legacy option
  begin
    Mouse := ReadArrayIndex(IMouseLegacy, IniFile, "Controller", "Mouse", 2);
  end;

  // SingTimebarMode
  SingTimebarMode := ReadArrayIndex(ISingTimebarMode, IniFile, "Advanced", "SingTimebarMode", IGNORE_INDEX, "Remaining");

  // JukeboxTimebarMode
  JukeboxTimebarMode := ReadArrayIndex(IJukeboxTimebarMode, IniFile, "Advanced", "JukeboxTimebarMode", IGNORE_INDEX, "Current");

  // WebCam
  WebCamID := IniFile.ReadInteger("Webcam", "ID", 0);
  WebCamResolution := ReadArrayIndex(IWebcamResolution, IniFile, "Webcam", "Resolution", IGNORE_INDEX, "320x240");
  if (WebCamResolution = -1) then
    WebcamResolution := 2;
  WebCamFPS := ReadArrayIndex(IWebcamFPS, IniFile, "Webcam", "FPS", 4);
  WebCamFlip := ReadArrayIndex(IWebcamFlipTranslated, IniFile, "Webcam", "Flip", IGNORE_INDEX, "On");
  WebCamBrightness := ReadArrayIndex(IWebcamBrightness, IniFile, "Webcam", "Brightness", IGNORE_INDEX, "0");
  WebCamSaturation := ReadArrayIndex(IWebcamSaturation, IniFile, "Webcam", "Saturation", IGNORE_INDEX, "0");
  WebCamHue := ReadArrayIndex(IWebcamHue, IniFile, "Webcam", "Hue", IGNORE_INDEX, "0");
  WebCamEffect := IniFile.ReadInteger("Webcam", "Effect", 0);

  // Jukebox
  JukeboxFont := ReadArrayIndex(ILyricsFont, IniFile, "Jukebox", "LyricsFont", 0);
  JukeboxStyle := ReadArrayIndex(ILyricsStyle, IniFile, "Jukebox", "LyricsStyle", 0);
  JukeboxEffect := ReadArrayIndex(ILyricsEffect, IniFile, "Jukebox", "LyricsEffect", 1);
  JukeboxAlpha := ReadArrayIndex(ILyricsAlpha, IniFile, "Jukebox", "LyricsAlpha", 20);

  JukeboxSongMenu := ReadArrayIndex(IJukeboxSongMenu, IniFile, "Jukebox", "SongMenu", IGNORE_INDEX, "On");


  JukeboxSingLineColor := ReadArrayIndex(IHexSingColor, IniFile, "Jukebox", "SingLineColor", High(IHexSingColor));

  // other color
  if (JukeboxSingLineColor = -1) or (JukeboxSingLineColor = High(IHexSingColor)) then
  begin
    JukeboxSingLineColor := High(IHexSingColor);

    HexColor := IniFile.ReadString("Jukebox", "SingLineColor", IHexSingColor[0]);
    Col := HexToRGB(HexColor);

    Ini.JukeboxSingLineOtherColorR := Round(Col.R);
    Ini.JukeboxSingLineOtherColorG := Round(Col.G);
    Ini.JukeboxSingLineOtherColorB := Round(Col.B);
  end;

  JukeboxActualLineColor := ReadArrayIndex(IHexGrayColor, IniFile, "Jukebox", "ActualLineColor", High(IHexGrayColor));

  // other color
  if (JukeboxActualLineColor = -1) or (JukeboxActualLineColor = High(IHexGrayColor)) then
  begin
    JukeboxActualLineColor := High(IHexGrayColor);

    HexColor := IniFile.ReadString("Jukebox", "ActualLineColor", IHexGrayColor[8]);
    Col := HexToRGB(HexColor);

    Ini.JukeboxActualLineOtherColorR := Round(Col.R);
    Ini.JukeboxActualLineOtherColorG := Round(Col.G);
    Ini.JukeboxActualLineOtherColorB := Round(Col.B);
  end;

  JukeboxNextLineColor := ReadArrayIndex(IHexGrayColor, IniFile, "Jukebox", "NextLineColor", High(IHexGrayColor));
  // other color
  if (JukeboxNextLineColor = -1) or (JukeboxNextLineColor = High(IHexGrayColor)) then
  begin
    JukeboxNextLineColor := High(IHexGrayColor);

    HexColor := IniFile.ReadString("Jukebox", "NextLineColor", IHexGrayColor[6]);
    Col := HexToRGB(HexColor);

    Ini.JukeboxNextLineOtherColorR := Round(Col.R);
    Ini.JukeboxNextLineOtherColorG := Round(Col.G);
    Ini.JukeboxNextLineOtherColorB := Round(Col.B);
  end;

  JukeboxSingLineOutlineColor := ReadArrayIndex(IHexOColor, IniFile, "Jukebox", "SingLineOColor", 0);
  // other color
  if (JukeboxSingLineOutlineColor = -1) then
  begin
    JukeboxSingLineOutlineColor := High(IHexOColor);

    HexColor := IniFile.ReadString("Jukebox", "SingLineOColor", IHexOColor[0]);
    Col := HexToRGB(HexColor);

    Ini.JukeboxSingLineOtherOColorR := Round(Col.R);
    Ini.JukeboxSingLineOtherOColorG := Round(Col.G);
    Ini.JukeboxSingLineOtherOColorB := Round(Col.B);
  end;

  JukeboxActualLineOutlineColor := ReadArrayIndex(IHexOColor, IniFile, "Jukebox", "ActualLineOColor", 0);
  // other color
  if (JukeboxActualLineOutlineColor = -1) then
  begin
    JukeboxActualLineOutlineColor := High(IHexOColor);

    HexColor := IniFile.ReadString("Jukebox", "ActualLineOColor", IHexOColor[0]);
    Col := HexToRGB(HexColor);

    Ini.JukeboxActualLineOtherOColorR := Round(Col.R);
    Ini.JukeboxActualLineOtherOColorG := Round(Col.G);
    Ini.JukeboxActualLineOtherOColorB := Round(Col.B);
  end;

  JukeboxNextLineOutlineColor := ReadArrayIndex(IHexOColor, IniFile, "Jukebox", "NextLineOColor", 0);
  // other color
  if (JukeboxNextLineOutlineColor = -1) then
  begin
    JukeboxNextLineOutlineColor := High(IHexOColor);

    HexColor := IniFile.ReadString("Jukebox", "NextLineOColor", IHexOColor[0]);
    Col := HexToRGB(HexColor);

    Ini.JukeboxNextLineOtherOColorR := Round(Col.R);
    Ini.JukeboxNextLineOtherOColorG := Round(Col.G);
    Ini.JukeboxNextLineOtherOColorB := Round(Col.B);
  end;

  LoadPaths(IniFile);

  TranslateOptionValues;

  IniFile.Free;
end;

void TIni.Save;
var
  IniFile: TIniFile;
  HexColor: string;
  I: integer;
  C: TRGB;
begin
  try
  begin
    if (Filename.IsFile and Filename.IsReadOnly) then
    begin
      Log.LogError("Config-file is read-only", "TIni.Save");
      Exit;
    end;

    IniFile := TIniFile.Create(Filename.ToNative);

    // Players
    IniFile.WriteString("Game", "Players", IPlayers[Players]);

    // Difficulty
    IniFile.WriteString("Game", "Difficulty", IDifficulty[Difficulty]);

    // Language
    IniFile.WriteString("Game", "Language", ILanguage[Language]);

    // Tabs
    IniFile.WriteString("Game", "Tabs", ITabs[Tabs]);

    // SongMenu
    IniFile.WriteString("Game", "SongMenu", ISongMenuMode[Ord(SongMenu)]);

    // Sorting
    IniFile.WriteString("Game", "Sorting", ISorting[Sorting]);

    // Show Scores
    IniFile.WriteString("Game", "ShowScores", IShowScores[ShowScores]);

    // Debug
    IniFile.WriteString("Game", "Debug", IDebug[Debug]);

    IniFile.WriteInteger("Game", "AVDelay", AVDelay);
    IniFile.WriteInteger("Game", "MicDelay", MicDelay);

    // MaxFramerate
    IniFile.WriteString("Graphics", "MaxFramerate", IMaxFramerate[MaxFramerate]);

    // Screens
    IniFile.WriteString("Graphics", "Screens", IScreens[Screens]);

    // Split
    IniFile.WriteString("Graphics", "Split", ISplit[Split]);

    // FullScreen
    IniFile.WriteString("Graphics", "FullScreen", IFullScreen[FullScreen]);

    // Visualization
    IniFile.WriteString("Graphics", "Visualization", IVisualizer[VisualizerOption]);

    // Resolution
    IniFile.WriteString("Graphics", "Resolution", GetResolution);
    IniFile.WriteString("Graphics", "ResolutionFullscreen", GetResolutionFullscreen);

    // Depth
    IniFile.WriteString("Graphics", "Depth", IDepth[Depth]);

    // TextureSize
    IniFile.WriteString("Graphics", "TextureSize", ITextureSize[TextureSize]);

    // Sing Window
    IniFile.WriteString("Graphics", "SingWindow", ISingWindow[SingWindow]);

    // Oscilloscope
    IniFile.WriteString("Graphics", "Oscilloscope", IOscilloscope[Oscilloscope]);

    // Spectrum
    //IniFile.WriteString("Graphics", "Spectrum", ISpectrum[Spectrum]);

    // Spectrograph
    //IniFile.WriteString("Graphics", "Spectrograph", ISpectrograph[Spectrograph]);

    // Movie Size
    IniFile.WriteString("Graphics", "MovieSize", IMovieSize[MovieSize]);

    // VideoPreview
    IniFile.WriteString("Graphics", "VideoPreview", IVideoPreview[VideoPreview]);

    // VideoEnabled
    IniFile.WriteString("Graphics", "VideoEnabled", IVideoEnabled[VideoEnabled]);

    // ClickAssist
    IniFile.WriteString("Sound", "ClickAssist", IClickAssist[ClickAssist]);

    // BeatClick
    IniFile.WriteString("Sound", "BeatClick", IBeatClick[BeatClick]);

    // AudioOutputBufferSize
    IniFile.WriteString("Sound", "AudioOutputBufferSize", IAudioOutputBufferSize[AudioOutputBufferSizeIndex]);

    // Background music
    IniFile.WriteString("Sound", "BackgroundMusic", IBackgroundMusic[BackgroundMusicOption]);

    // Song Preview
    IniFile.WriteString("Sound", "PreviewVolume", IPreviewVolume[PreviewVolume]);

    // PreviewFading
    IniFile.WriteString("Sound", "PreviewFading", IPreviewFading[PreviewFading]);

    // SavePlayback
    IniFile.WriteString("Sound", "SavePlayback", ISavePlayback[SavePlayback]);

    // VoicePasstrough
    IniFile.WriteString("Sound", "VoicePassthrough", IVoicePassthrough[VoicePassthrough]);

    // MusicAutoGain
    IniFile.WriteString("Sound", "MusicAutoGain", IMusicAutoGain[MusicAutoGain]);

    // Lyrics Font
    IniFile.WriteString("Lyrics", "LyricsFont", ILyricsFont[LyricsFont]);

    // Lyrics Style
    IniFile.WriteString("Lyrics", "LyricsStyle", ILyricsStyle[LyricsStyle]);

    // Lyrics Effect
    IniFile.WriteString("Lyrics", "LyricsEffect", ILyricsEffect[LyricsEffect]);

    // NoteLines
    IniFile.WriteString("Lyrics", "NoteLines", INoteLines[NoteLines]);

    //Encoding default
    IniFile.WriteString("Lyrics", "Encoding", EncodingName(DefaultEncoding));

    // Theme
    IniFile.WriteString("Themes", "Theme", ITheme[Theme]);

    // Skin
    IniFile.WriteString("Themes", "Skin", ISkin[SkinNo]);

    // Color
    IniFile.WriteString("Themes", "Color", IColor[Color]);

    SaveInputDeviceCfg(IniFile);

    //LoadAnimation
    IniFile.WriteString("Advanced", "LoadAnimation", ILoadAnimation[LoadAnimation]);

    //EffectSing
    IniFile.WriteString("Advanced", "EffectSing", IEffectSing[EffectSing]);

    //ScreenFade
    IniFile.WriteString("Advanced", "ScreenFade", IScreenFade[ScreenFade]);

    //AskbeforeDel
    IniFile.WriteString("Advanced", "AskbeforeDel", IAskbeforeDel[AskBeforeDel]);

    //OnSongClick
    IniFile.WriteString("Advanced", "OnSongClick", IOnSongClick[OnSongClick]);

    //Line Bonus
    IniFile.WriteString("Advanced", "LineBonus", ILineBonus[LineBonus]);

    //Party Popup
    IniFile.WriteString("Advanced", "PartyPopup", IPartyPopup[PartyPopup]);

    //SingScores
    IniFile.WriteString("Advanced", "SingScores", ISingScores[SingScores]);

    //TopScores
    IniFile.WriteString("Advanced", "TopScores", ITopScores[TopScores]);

    //SyncTo
    IniFile.WriteString("Advanced", "SyncTo", ISyncTo[SyncTo]);

    // Joypad
    IniFile.WriteString("Controller", "Joypad", IJoypad[Joypad]);

    // Mouse
    IniFile.WriteString("Controller", "Mouse", IMouse[Mouse]);

    // SingTimebarMode
    IniFile.WriteString("Advanced", "SingTimebarMode", ISingTimebarMode[SingTimebarMode]);

    // JukeboxTimebarMode
    IniFile.WriteString("Advanced", "JukeboxTimebarMode", IJukeboxTimebarMode[JukeboxTimebarMode]);

    // Directories (add a template if section is missing)
    // Note: Value must be " " and not "", otherwise no key is generated on Linux
    if (not IniFile.SectionExists("Directories")) then
      IniFile.WriteString("Directories", "SongDir1", " ");

    if (not IniFile.ValueExists("Directories", "WebScoresDir")) then
      IniFile.WriteString("Directories", "WebScoresDir", " ");

    // Jukebox
    IniFile.WriteString("Jukebox", "LyricsFont", ILyricsFont[JukeboxFont]);
    IniFile.WriteString("Jukebox", "LyricsStyle", ILyricsStyle[JukeboxStyle]);
    IniFile.WriteString("Jukebox", "LyricsEffect", ILyricsEffect[JukeboxEffect]);
    IniFile.WriteString("Jukebox", "LyricsAlpha", ILyricsAlpha[JukeboxAlpha]);

    if (JukeboxSingLineColor <> High(ISingLineColor)) then
    begin
      C := GetLyricColor(JukeboxSingLineColor);
      HexColor := RGBToHex(Round(C.R * 255), Round(C.G * 255), Round(C.B * 255));
    end
    else
      HexColor := RGBToHex(JukeboxSingLineOtherColorR, JukeboxSingLineOtherColorG, JukeboxSingLineOtherColorB);

    IniFile.WriteString("Jukebox", "SingLineColor", HexColor);

    if (JukeboxActualLineColor <> High(IActualLineColor)) then
    begin
      C := GetLyricGrayColor(JukeboxActualLineColor);
      HexColor := RGBToHex(Round(C.R * 255), Round(C.G * 255), Round(C.B * 255));
    end
    else
      HexColor := RGBToHex(JukeboxActualLineOtherColorR, JukeboxActualLineOtherColorG, JukeboxActualLineOtherColorB);

    IniFile.WriteString("Jukebox", "ActualLineColor", HexColor);

    if (JukeboxNextLineColor <> High(INextLineColor)) then
    begin
      C := GetLyricGrayColor(JukeboxNextLineColor);
      HexColor := RGBToHex(Round(C.R * 255), Round(C.G * 255), Round(C.B * 255));
    end
    else
      HexColor := RGBToHex(JukeboxNextLineOtherColorR, JukeboxNextLineOtherColorG, JukeboxNextLineOtherColorB);

    IniFile.WriteString("Jukebox", "NextLineColor", HexColor);

    if (JukeboxSingLineOutlineColor <> High(ISingLineOColor)) then
    begin
      C := GetLyricOutlineColor(JukeboxSingLineOutlineColor);
      HexColor := RGBToHex(Round(C.R * 255), Round(C.G * 255), Round(C.B * 255));
    end
    else
      HexColor := RGBToHex(JukeboxSingLineOtherOColorR, JukeboxSingLineOtherOColorG, JukeboxSingLineOtherOColorB);

    IniFile.WriteString("Jukebox", "SingLineOColor", HexColor);

    if (JukeboxActualLineOutlineColor <> High(IActualLineOColor)) then
    begin
      C := GetLyricOutlineColor(JukeboxActualLineOutlineColor);
      HexColor := RGBToHex(Round(C.R * 255), Round(C.G * 255), Round(C.B * 255));
    end
    else
      HexColor := RGBToHex(JukeboxActualLineOtherOColorR, JukeboxActualLineOtherOColorG, JukeboxActualLineOtherOColorB);

    IniFile.WriteString("Jukebox", "ActualLineOColor", HexColor);

    if (JukeboxNextLineOutlineColor <> High(INextLineOColor)) then
    begin
      C := GetLyricOutlineColor(JukeboxNextLineOutlineColor);
      HexColor := RGBToHex(Round(C.R * 255), Round(C.G * 255), Round(C.B * 255));
    end
    else
      HexColor := RGBToHex(JukeboxNextLineOtherOColorR, JukeboxNextLineOtherOColorG, JukeboxNextLineOtherOColorB);

    IniFile.WriteString("Jukebox", "NextLineOColor", HexColor);

    IniFile.Free;

  end
  except
    On e :Exception do begin
      Log.LogWarn("Saving InputDeviceConfig failed: " + e.Message, "UIni.Save");
    end;
  end;
end;

void TIni.SaveNames;
var
  IniFile: TIniFile;
  I:       integer;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    //Name Templates for Names Mod
    for I := 0 to High(Name) do
      IniFile.WriteString("Name", "P" + IntToStr(I+1), Name[I]);
    for I := 0 to High(NameTeam) do
      IniFile.WriteString("NameTeam", "T" + IntToStr(I+1), NameTeam[I]);
    for I := 0 to High(NameTemplate) do
      IniFile.WriteString("NameTemplate", "Name" + IntToStr(I+1), NameTemplate[I]);

    IniFile.Free;
  end;
end;

void TIni.SaveLevel;
var
  IniFile: TIniFile;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    // Difficulty
    IniFile.WriteString("Game", "Difficulty", IDifficulty[Difficulty]);

    IniFile.Free;
  end;
end;

void TIni.SaveJukeboxSongMenu;
var
  IniFile: TIniFile;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    IniFile.WriteString("Jukebox", "SongMenu", IJukeboxSongMenu[JukeboxSongMenu]);

    IniFile.Free;
  end;
end;


void TIni.SaveShowWebScore;
var
  IniFile: TIniFile;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    // ShowWebScore
    IniFile.WriteString("Game", "ShowWebScore", DllMan.Websites[ShowWebScore].Name);

    IniFile.Free;
  end;
end;


void TIni.SavePlayerColors;

var
  IniFile: TIniFile;
  I: integer;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    //Colors for Names Mod
    for I := 1 to IMaxPlayerCount do
      IniFile.WriteString("PlayerColor", "P" + IntToStr(I), IntToStr(PlayerColor[I-1]));

    IniFile.Free;
  end;
end;

void TIni.SavePlayerAvatars;
var
  IniFile: TIniFile;
  I: integer;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    //Colors for Names Mod
    for I := 1 to IMaxPlayerCount do
      IniFile.WriteString("PlayerAvatar", "P" + IntToStr(I), PlayerAvatar[I-1]);

    IniFile.Free;
  end;
end;

void TIni.SavePlayerLevels;
var
  IniFile: TIniFile;
  I: integer;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    for I := 1 to IMaxPlayerCount do
      IniFile.WriteInteger("PlayerLevel", "P" + IntToStr(I), PlayerLevel[I-1]);

    IniFile.Free;
  end;
end;

void TIni.SaveTeamColors;
var
  IniFile: TIniFile;
  I: integer;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    //Colors for Names Mod
    for I := 1 to 3 do
      IniFile.WriteString("TeamColor", "T" + IntToStr(I), IntToStr(TeamColor[I-1]));

    IniFile.Free;
  end;
end;

void TIni.SaveSoundFont(Name: string);
var
  IniFile: TIniFile;
  I: integer;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    IniFile.WriteString("Sound", "SoundFont", Name);

    IniFile.Free;
  end;
end;

void TIni.SaveWebcamSettings;
var
  IniFile: TIniFile;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    // WebCam
    IniFile.WriteInteger("Webcam", "ID", WebCamID);
    IniFile.WriteString("Webcam", "Resolution", IWebcamResolution[WebcamResolution]);
    IniFile.WriteInteger("Webcam", "FPS", StrToInt(IWebcamFPS[WebCamFPS]));

    IniFile.WriteString("Webcam", "Flip", IWebcamFlip[WebcamFlip]);
    IniFile.WriteString("Webcam", "Brightness", IWebcamBrightness[WebcamBrightness]);
    IniFile.WriteString("Webcam", "Saturation", IWebcamSaturation[WebcamSaturation]);
    IniFile.WriteString("Webcam", "Hue", IWebcamHue[WebcamHue]);
    IniFile.WriteInteger("Webcam", "Effect", WebcamEffect);

    IniFile.Free;
  end;

end;

void TIni.SaveNumberOfPlayers;
var
  IniFile: TIniFile;
  I: integer;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    // Players
    IniFile.WriteString("Game", "Players", IPlayers[Players]);

    IniFile.Free;
  end;
end;

void TIni.SaveSingTimebarMode;
var
  IniFile: TIniFile;
  I: integer;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    // Players
    IniFile.WriteString("Advanced", "SingTimebarMode", ISingTimebarMode[SingTimebarMode]);

    IniFile.Free;
  end;
end;

void TIni.SaveJukeboxTimebarMode;
var
  IniFile: TIniFile;
  I: integer;
begin
  if not Filename.IsReadOnly() then
  begin
    IniFile := TIniFile.Create(Filename.ToNative);

    // Players
    IniFile.WriteString("Advanced", "JukeboxTimebarMode", IJukeboxTimebarMode[JukeboxTimebarMode]);

    IniFile.Free;
  end;
end;


function TIni.SetResolution(ResolutionString: string; RemoveCurrent: boolean; NoSave: boolean): boolean;
  var
    Index: integer;
    Dirty: boolean;
begin
  Result := false;
  Dirty := false;

  // check if current resolution is custom and then remove anyway (no matter what RemoveCurrent is set)
  if (Resolution >= 0) then
  begin
    Index := GetArrayIndex(IResolutionCustom, IResolution[Resolution]);
    if Index >= 0 then
    begin
      StringDeleteFromArray(IResolutionCustom, Index);
      StringDeleteFromArray(IResolution, Resolution);
    end;
  end;

  Index := GetArrayIndex(IResolution, ResolutionString);
  if not NoSave and (Resolution <> Index) then Dirty := true;
  if (Resolution >= 0) and (RemoveCurrent) then StringDeleteFromArray(IResolution, Resolution);
  if Index < 0 then
  begin
    SetLength(IResolution, Length(IResolution) + 1);
    IResolution[High(IResolution)] := ResolutionString;
    index := High(IResolution);
    Result := true;

    if GetArrayIndex(IResolutionCustom, ResolutionString) < 0 then
    begin
      SetLength(IResolutionCustom, Length(IResolutionCustom) + 1);
      IResolutionCustom[High(IResolutionCustom)] := ResolutionString;
    end;
  end;

  if SetResolution(index) and Dirty then
  begin
    Log.LogStatus("Resolution overridden to: " + ResolutionString, "Video");
    Save();
  end;
end;

function TIni.SetResolution(w,h: integer; RemoveCurrent: boolean; NoSave: boolean): boolean;
begin

  // hacky fix to support multiplied resolution (in width) in multiple screen setup (Screens=2 and more)
  // TODO: RattleSN4K3: Improve the way multiplied screen resolution is applied and stored (see UGraphics::InitializeScreen; W := W * Screens)
  if (Screens > 0) and not ((Params.Split = spmSplit ) or (Split > 0)) then w := w div (Screens+1) // integral div
  else if (Params.Screens > 0) and not ((Params.Split = spmSplit ) or (Split > 0)) then w := w div (Params.Screens+1);

  Result := SetResolution(BuildResolutionString(w, h), RemoveCurrent, NoSave);
end;

function TIni.SetResolution(index: integer): boolean;
begin
  Result := false;
  if (index >= 0) and (index < Length(IResolution)) then
  begin
      Resolution := index;
      Result := true;
  end;
end;

function TIni.GetResolution(): string;
begin
  if Resolution >= 0 then Result := IResolution[Resolution]
  else if Length(IResolution) = 0 then Result := DEFAULT_RESOLUTION
  else Result := IResolution[0];
end;

function TIni.GetResolution(out w,h: integer): string;
begin
  Result := GetResolution();
  ParseResolutionString(Result, w, h);

  // hacky fix to support multiplied resolution (in width) in multiple screen setup (Screens=2 and more)
  // TODO: RattleSN4K3: Improve the way multiplied screen resolution is applied and stored (see UGraphics::InitializeScreen; W := W * Screens)
  if (Screens > 0) and not ((Params.Split = spmSplit ) or (Split > 0)) then w := w * (Screens+1)
  else if (Params.Screens > 0) and not ((Params.Split = spmSplit ) or (Split > 0)) then w := w * (Params.Screens+1);
end;

function TIni.GetResolution(index: integer; out ResolutionString: string): boolean;
begin
  Result := false;
  if (index >= 0) and (index < Length(IResolution)) then
  begin
      ResolutionString := IResolution[index];
      Result := true;
  end;
end;

function TIni.GetResolutionFullscreen(): string;
begin
  if ResolutionFullscreen >= 0 then Result := IResolutionFullScreen[ResolutionFullscreen]
  else if Length(IResolutionFullScreen) = 0 then Result := DEFAULT_RESOLUTION
  else Result := IResolutionFullScreen[0];
end;

function TIni.GetResolutionFullscreen(out w,h: integer): string;
begin
  Result := GetResolutionFullscreen();
  ParseResolutionString(Result, w, h);
end;

function TIni.GetResolutionFullscreen(index: integer; out ResolutionString: string): boolean;
begin
  Result := false;
  if (index >= 0) and (index < Length(IResolutionFullScreen)) then
  begin
      ResolutionString := IResolutionFullScreen[index];
      Result := true;
  end;
end;

void TIni.ClearCustomResolutions();
  var
    Index, i, custom: integer;
    ResString: string;
begin
  if Resolution < 0 then Exit;

  // check if current resolution is a custom one
  ResString := IResolution[Resolution];
  Index := GetArrayIndex(IResolutionCustom, ResString);
  for i := High(IResolutionCustom) downto 0 do
  begin
    custom := GetArrayIndex(IResolution, IResolutionCustom[i]);
    if (custom >= 0) and (Index <> i) then
    begin
      StringDeleteFromArray(IResolution, custom);
      StringDeleteFromArray(IResolutionCustom, i);
    end;
  end;

  // update index
  Resolution := GetArrayIndex(IResolution, ResString);
end;

};