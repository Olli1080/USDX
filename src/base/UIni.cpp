#include "UIni.h"

#include <format>

#include <SDL2/SDL.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <yaml-cpp/yaml.h>

#include "ULanguage.h"
#include "UCommon.h"
#include "ULog.h"
#include "UPathUtils.h"
#include "UDLLManager.h"
#include "UCommandLine.h"
#include "UDataBase.h"
#include "UThemes.h"

namespace UIni
{
/**
 * Translate && set the values of options, which need translation.
 */
void TIni::TranslateOptionValues()
{
    // Load language file, fallback to config language if param is invalid
    if ((UCommandLine::Params.Language > -1) && (UCommandLine::Params.Language < ILanguage.size()))
        ULanguage::Language.ChangeLanguage(ILanguage[UCommandLine::Params.Language]);
    else
        ULanguage::Language.ChangeLanguage(ILanguage[Language]);

    IDifficultyTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_EASY");
    IDifficultyTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_MEDIUM");
    IDifficultyTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_HARD");

    ITabsTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    ITabsTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    ISongMenuTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_ROULETTE");
    ISongMenuTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_CHESSBOARD");
    ISongMenuTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_CAROUSEL");
    ISongMenuTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_SLOTMACHINE");
    ISongMenuTranslated[4] = ULanguage::Language.Translate("OPTION_VALUE_SLIDE");
    ISongMenuTranslated[5] = ULanguage::Language.Translate("OPTION_VALUE_LIST");
    ISongMenuTranslated[6] = ULanguage::Language.Translate("OPTION_VALUE_MOSAIC");

    ISortingTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_EDITION");
    ISortingTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_GENRE");
    ISortingTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_LANGUAGE");
    ISortingTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_FOLDER");
    ISortingTranslated[4] = ULanguage::Language.Translate("OPTION_VALUE_TITLE");
    ISortingTranslated[5] = ULanguage::Language.Translate("OPTION_VALUE_ARTIST");
    ISortingTranslated[6] = ULanguage::Language.Translate("OPTION_VALUE_ARTIST2");
    ISortingTranslated[7] = ULanguage::Language.Translate("OPTION_VALUE_YEAR");
    ISortingTranslated[8] = ULanguage::Language.Translate("OPTION_VALUE_YEAR_REVERSED");
    ISortingTranslated[9] = ULanguage::Language.Translate("OPTION_VALUE_DECADE");
    //ISortingTranslated[10]               = ULanguage::Language.Translate("OPTION_VALUE_PLAYLIST");

    IShowScoresTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IShowScoresTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_WHENEXIST");
    IShowScoresTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IDebugTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IDebugTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IFullScreenTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IFullScreenTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");
    IFullScreenTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_BORDERLESS");

    IVisualizerTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IVisualizerTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_WHENNOVIDEO");
    IVisualizerTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_WHENNOVIDEOANDIMAGE");
    IVisualizerTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IBackgroundMusicTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IBackgroundMusicTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    ISingWindowTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_SMALL");
    ISingWindowTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_BIG");

    IOscilloscopeTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IOscilloscopeTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    ISpectrumTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    ISpectrumTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    ISpectrographTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    ISpectrographTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IMovieSizeTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_HALF");
    IMovieSizeTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_FULL_VID");
    IMovieSizeTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_FULL_VID_BG");

    IVideoPreviewTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IVideoPreviewTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IVideoEnabledTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IVideoEnabledTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IClickAssistTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IClickAssistTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IBeatClickTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IBeatClickTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    ISavePlaybackTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    ISavePlaybackTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IVoicePassthroughTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IVoicePassthroughTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IMusicAutoGainTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IMusicAutoGainTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_GAIN_SOFT");
    IMusicAutoGainTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_GAIN_MEDIUM");
    IMusicAutoGainTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_GAIN_HARD");

    ISyncToTranslated[stMusic] = ULanguage::Language.Translate("OPTION_VALUE_MUSIC");
    ISyncToTranslated[stLyrics] = ULanguage::Language.Translate("OPTION_VALUE_LYRICS");
    ISyncToTranslated[stOff] = ULanguage::Language.Translate("OPTION_VALUE_OFF");

    ILyricsStyleTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_REGULAR");
    ILyricsStyleTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_BOLD");
    ILyricsStyleTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_TEXTOUTLINE");

    ILyricsEffectTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_SIMPLE");
    ILyricsEffectTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ZOOM");
    ILyricsEffectTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_SLIDE");
    ILyricsEffectTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_BALL");
    ILyricsEffectTranslated[4] = ULanguage::Language.Translate("OPTION_VALUE_SHIFT");

    INoteLinesTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    INoteLinesTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    for (int i = 0; i < 256; ++i)
    {
        IRed[i] = std::to_string(i);
        IGreen[i] = std::to_string(i);
        IBlue[i] = std::to_string(i);
    }

    ILineTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_TO_SING");
    ILineTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ACTUAL");
    ILineTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_NEXT");

    IPropertyTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_FILL");
    IPropertyTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_OUTLINE");

    ISingLineColorTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_BLUE");
    ISingLineColorTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_GREEN");
    ISingLineColorTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_PINK");
    ISingLineColorTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_RED");
    ISingLineColorTranslated[4] = ULanguage::Language.Translate("OPTION_VALUE_VIOLET");
    ISingLineColorTranslated[5] = ULanguage::Language.Translate("OPTION_VALUE_ORANGE");
    ISingLineColorTranslated[6] = ULanguage::Language.Translate("OPTION_VALUE_YELLOW");
    ISingLineColorTranslated[7] = ULanguage::Language.Translate("OPTION_VALUE_BROWN");
    ISingLineColorTranslated[8] = ULanguage::Language.Translate("OPTION_VALUE_BLACK");
    ISingLineColorTranslated[9] = ULanguage::Language.Translate("OPTION_VALUE_TURQUOISE");
    ISingLineColorTranslated[10] = ULanguage::Language.Translate("OPTION_VALUE_SALMON");
    ISingLineColorTranslated[11] = ULanguage::Language.Translate("OPTION_VALUE_GREENYELLOW");
    ISingLineColorTranslated[12] = ULanguage::Language.Translate("OPTION_VALUE_LAVENDER");
    ISingLineColorTranslated[13] = ULanguage::Language.Translate("OPTION_VALUE_BEIGE");
    ISingLineColorTranslated[14] = ULanguage::Language.Translate("OPTION_VALUE_TEAL");
    ISingLineColorTranslated[15] = ULanguage::Language.Translate("OPTION_VALUE_ORCHID");
    ISingLineColorTranslated[16] = ULanguage::Language.Translate("OPTION_VALUE_STEELBLUE");
    ISingLineColorTranslated[17] = ULanguage::Language.Translate("OPTION_VALUE_PLUM");
    ISingLineColorTranslated[18] = ULanguage::Language.Translate("OPTION_VALUE_CHOCOLATE");
    ISingLineColorTranslated[19] = ULanguage::Language.Translate("OPTION_VALUE_GOLD");
    ISingLineColorTranslated[20] = ULanguage::Language.Translate("OPTION_VALUE_OTHER");

    IActualLineColorTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_BLACK");
    IActualLineColorTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " +3";
    IActualLineColorTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " +2";
    IActualLineColorTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " +1";
    IActualLineColorTranslated[4] = ULanguage::Language.Translate("OPTION_VALUE_GRAY");
    IActualLineColorTranslated[5] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " -1";
    IActualLineColorTranslated[6] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " -2";
    IActualLineColorTranslated[7] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " -3";
    IActualLineColorTranslated[8] = ULanguage::Language.Translate("OPTION_VALUE_WHITE");
    IActualLineColorTranslated[9] = ULanguage::Language.Translate("OPTION_VALUE_OTHER");

    INextLineColorTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_BLACK");
    INextLineColorTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " +3";
    INextLineColorTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " +2";
    INextLineColorTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " +1";
    INextLineColorTranslated[4] = ULanguage::Language.Translate("OPTION_VALUE_GRAY");
    INextLineColorTranslated[5] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " -1";
    INextLineColorTranslated[6] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " -2";
    INextLineColorTranslated[7] = ULanguage::Language.Translate("OPTION_VALUE_GRAY") + " -3";
    INextLineColorTranslated[8] = ULanguage::Language.Translate("OPTION_VALUE_WHITE");
    INextLineColorTranslated[9] = ULanguage::Language.Translate("OPTION_VALUE_OTHER");

    ISingLineOColorTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_BLACK");
    ISingLineOColorTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_WHITE");
    ISingLineOColorTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_OTHER");

    IActualLineOColorTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_BLACK");
    IActualLineOColorTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_WHITE");
    IActualLineOColorTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_OTHER");

    INextLineOColorTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_BLACK");
    INextLineOColorTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_WHITE");
    INextLineOColorTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_OTHER");

    IColorTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_BLUE");
    IColorTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_GREEN");
    IColorTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_PINK");
    IColorTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_RED");
    IColorTranslated[4] = ULanguage::Language.Translate("OPTION_VALUE_VIOLET");
    IColorTranslated[5] = ULanguage::Language.Translate("OPTION_VALUE_ORANGE");
    IColorTranslated[6] = ULanguage::Language.Translate("OPTION_VALUE_YELLOW");
    IColorTranslated[7] = ULanguage::Language.Translate("OPTION_VALUE_BROWN");
    IColorTranslated[8] = ULanguage::Language.Translate("OPTION_VALUE_BLACK");

    IPlayerColorTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_BLUE");
    IPlayerColorTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_RED");
    IPlayerColorTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_GREEN");
    IPlayerColorTranslated[3] = ULanguage::Language.Translate("OPTION_VALUE_YELLOW");
    IPlayerColorTranslated[4] = ULanguage::Language.Translate("OPTION_VALUE_ORANGE");
    IPlayerColorTranslated[5] = ULanguage::Language.Translate("OPTION_VALUE_PINK");
    IPlayerColorTranslated[6] = ULanguage::Language.Translate("OPTION_VALUE_VIOLET");
    IPlayerColorTranslated[7] = ULanguage::Language.Translate("OPTION_VALUE_BROWN");
    IPlayerColorTranslated[8] = ULanguage::Language.Translate("OPTION_VALUE_GRAY");
    IPlayerColorTranslated[9] = ULanguage::Language.Translate("OPTION_VALUE_DARKBLUE");
    IPlayerColorTranslated[10] = ULanguage::Language.Translate("OPTION_VALUE_SKY");
    IPlayerColorTranslated[11] = ULanguage::Language.Translate("OPTION_VALUE_CYAN");
    IPlayerColorTranslated[12] = ULanguage::Language.Translate("OPTION_VALUE_FLAME");
    IPlayerColorTranslated[13] = ULanguage::Language.Translate("OPTION_VALUE_ORCHID");
    IPlayerColorTranslated[14] = ULanguage::Language.Translate("OPTION_VALUE_HARLEQUIN");
    IPlayerColorTranslated[15] = ULanguage::Language.Translate("OPTION_VALUE_GREENYELLOW");

    // Advanced
    ILoadAnimationTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    ILoadAnimationTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IEffectSingTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IEffectSingTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IScreenFadeTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IScreenFadeTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IAskbeforeDelTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IAskbeforeDelTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IOnSongClickTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_SING");
    IOnSongClickTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_SELECT_PLAYERS");
    IOnSongClickTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_OPEN_MENU");

    ILineBonusTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    ILineBonusTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IPartyPopupTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IPartyPopupTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    ISingScoresTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    ISingScoresTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    ITopScoresTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_ALL");
    ITopScoresTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_PLAYER");

    IJoypadTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IJoypadTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_ON");

    IMouseTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IMouseTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_SYSTEM_CURSOR");
    IMouseTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_GAME_CURSOR");

    IMouseTranslatedLegacy[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IMouseTranslatedLegacy[1] = ULanguage::Language.Translate("OPTION_VALUE_HARDWARE_CURSOR");
    IMouseTranslatedLegacy[2] = ULanguage::Language.Translate("OPTION_VALUE_SOFTWARE_CURSOR");

    ISingTimebarModeTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_CURRENT");
    ISingTimebarModeTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_REMAINING");
    ISingTimebarModeTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_TOTAL");

    IJukeboxTimebarModeTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_CURRENT");
    IJukeboxTimebarModeTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_REMAINING");
    IJukeboxTimebarModeTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_TOTAL");

    IAudioBufferSizeTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_AUTO");
    //for (int i = 1, val = 256; i < IAudioBufferSizeTranslated.size(); ++i, val <<= 1)
      //IAudioBufferSizeTranslated[i]  = std::to_string(val);

    // Song Preview
    IPreviewVolumeTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    //for (int i = 1, val = 10; i < IPreviewVolumeTranslated.size(); ++i, val += 10)
      //  IPreviewVolumeTranslated[i] = std::to_string(val) + "%";


    IPreviewFadingTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IPreviewFadingTranslated[1] = "1 " + ULanguage::Language.Translate("OPTION_VALUE_SEC");
    IPreviewFadingTranslated[2] = "2 " + ULanguage::Language.Translate("OPTION_VALUE_SECS");
    IPreviewFadingTranslated[3] = "3 " + ULanguage::Language.Translate("OPTION_VALUE_SECS");
    IPreviewFadingTranslated[4] = "4 " + ULanguage::Language.Translate("OPTION_VALUE_SECS");
    IPreviewFadingTranslated[5] = "5 " + ULanguage::Language.Translate("OPTION_VALUE_SECS");

    // Recording options
    IChannelPlayerTranslated[0] = ULanguage::Language.Translate("SING_OPTIONS_RECORD_NOONE");

    //for (int i = 1; i <= IMaxPlayerCount; ++i)
      //IChannelPlayerTranslated[i]       = std::to_string(i);

    IMicBoostTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IMicBoostTranslated[1] = "+6dB";
    IMicBoostTranslated[2] = "+12dB";
    IMicBoostTranslated[3] = "+18dB";

    // Network
    IAutoModeTranslated[0] = ULanguage::Language.Translate("OPTION_VALUE_OFF");
    IAutoModeTranslated[1] = ULanguage::Language.Translate("OPTION_VALUE_SEND");
    IAutoModeTranslated[2] = ULanguage::Language.Translate("OPTION_VALUE_SAVE");

    for (int i = 0; i < IMaxPlayerCount; ++i)
        IAutoPlayerTranslated[i] = ULanguage::Language.Translate("OPTION_PLAYER_" + std::to_string(i));
    IAutoPlayerTranslated[12] = ULanguage::Language.Translate("OPTION_ALL_PLAYERS");

    // Webcam
    SetWebcamFlipTranslated(false, ULanguage::Language.Translate("OPTION_VALUE_OFF"));
    SetWebcamFlipTranslated(true, ULanguage::Language.Translate("OPTION_VALUE_ON"));

    IWebcamEffectTranslated[0] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_NORMAL");
    IWebcamEffectTranslated[1] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_GRAYSCALE");
    IWebcamEffectTranslated[2] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_BLACK_WHITE");
    IWebcamEffectTranslated[3] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_NEGATIVE");
    IWebcamEffectTranslated[4] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_BINARY_IMAGE");
    IWebcamEffectTranslated[5] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_DILATE");
    IWebcamEffectTranslated[6] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_THRESHOLD");
    IWebcamEffectTranslated[7] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_EDGES");
    IWebcamEffectTranslated[8] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_GAUSSIAN_BLUR");
    IWebcamEffectTranslated[9] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_EQUALIZED");
    IWebcamEffectTranslated[10] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_ERODE");

    IAutoScoreEasyTranslated.resize(10000);
    IAutoScoreMediumTranslated.resize(10000);
    IAutoScoreHardTranslated.resize(10000);

    for (int i = 0; i < 10000; ++i)
    {
        std::string zeros = "";
        if (i < 10)
        {
            zeros = "000";
        }
        else if (i < 100)
        {
            zeros = "00";
        }
        else if (i < 1000)
        {
            zeros = "0";
        }

        IAutoScoreEasyTranslated[i] = "+" + zeros + std::to_string(i);
        IAutoScoreMediumTranslated[i] = "+" + zeros + std::to_string(i);
        IAutoScoreHardTranslated[i] = "+" + zeros + std::to_string(i);
    }
}

/*
void TIni::LoadWebcamSettings(TCustomIniFile IniFile)
{
  for (int i = 100; i >= 1; --i)
  {
    IWebcamBrightness[100 - i]   = "-" + std::to_string(i);
    IWebcamSaturation[100 - i]   = "-" + std::to_string(i);
  }
  IWebcamBrightness[100]   = std::string("0");
  IWebcamSaturation[100]   = "0";

  for (int i = 1; i <= 100; ++i)
  {
    IWebcamBrightness[i + 100]   = "+" + std::to_string(i);
    IWebcamSaturation[i + 100]   = "+" + std::to_string(i);
  }

  for (int i = 180; i >= 1; --i)
    IWebcamHue[180 - i]   = "-" + std::to_string(i);
  IWebcamHue[180]   = "0";

  for (int i = 1; i <= 180; ++i)
    IWebcamHue[i + 180]   = "+" + std::to_string(i);
}

*/
/**
 * Extracts an index of a key that is surrounded by a Prefix/Suffix pair.
 * Example: ExtractKeyIndex("MyKey[1]", "[", "]") will return 1.
 */
int TIni::ExtractKeyIndex(const std::string Key, std::string Prefix, std::string Suffix)
{
    int PrefixPos = Key.find_first_of(Prefix);
    if (PrefixPos <= 0)
        return -1;
    int SuffixPos = Key.find_last_of(Suffix);
    if (SuffixPos <= 0)
        return -1;

    auto Start = PrefixPos + Prefix.size();

    // copy all between prefix && suffix
    std::string Value = Key.substr(Start, SuffixPos - Start);

    try
    {
        return std::stoi(Value);
    }
    catch (const std::exception& e)
    {
        return -1;
    }
}

/**
 * Finds the maximum key-index in a key-list.
 * The indexes of the list are surrounded by Prefix/Suffix,
 * e.g. MyKey[1] (Prefix="[", Suffix="]")
 */
int TIni::GetMaxKeyIndex(std::list<std::string> Keys, const std::string Prefix, std::string Suffix)
{
  int Result = -1;

  for (const auto& Key : Keys)
  {
    int KeyIndex = ExtractKeyIndex(Key, Prefix, Suffix);
    Result = std::max(KeyIndex, Result);
  }
}

/**
 * Reads the property IniSeaction:IniProperty from IniFile &&
 * finds its corresponding index in SearchArray.
 * If SearchArray does not contain the property value, the default value is
 * returned.
 */
    /*
int TIni::ReadArrayIndex(const std::vector<std::string>& SearchArray, const TCustomIniFile& IniFile,
      std::string IniSection, std::string IniProperty, int Default, bool CaseInsensitive)
      {
        return ReadArrayIndex(SearchArray, IniFile, IniSection, IniProperty, Default, "", CaseInsensitive);
      }

int TIni::ReadArrayIndex(const std::vector<std::string>& SearchArray, const TCustomIniFile& IniFile,
    std::string IniSection, std::string IniProperty, int Default, std::string DefaultValue, bool CaseInsensitive)
    {
      std::string StrValue = IniFile.ReadString(IniSection, IniProperty, "");
      int Result = UCommon::GetArrayIndex(SearchArray, StrValue, CaseInsensitive);
      if (Result < 0)
      {
        if (Default == IGNORE_INDEX && !UCommon::Equals(StrValue, DefaultValue, !CaseInsensitive))
        {
          // prioritize default string value
          Result = UCommon::GetArrayIndex(SearchArray, DefaultValue, CaseInsensitive);
        }

        if (Result < 0 || Result > SearchArray.size())
          Result = Default;
      }
      return Result;
    }

template<std::size_t N>
int TIni::ReadArrayIndex(const std::array<std::string, N>& SearchArray, const TCustomIniFile& IniFile,
    std::string IniSection, std::string IniProperty, int Default, bool CaseInsensitive)
{
    return ReadArrayIndex<N>(SearchArray, IniFile, IniSection, IniProperty, Default, "", CaseInsensitive);
}

template<std::size_t N>
int TIni::ReadArrayIndex(const std::array<std::string, N>& SearchArray, const TCustomIniFile& IniFile,
    std::string IniSection, std::string IniProperty, int Default, std::string DefaultValue, bool CaseInsensitive)
{
    std::string StrValue = IniFile.ReadString(IniSection, IniProperty, "");
    int Result = UCommon::GetArrayIndex(SearchArray, StrValue, CaseInsensitive);
    if (Result < 0)
    {
        if (Default == IGNORE_INDEX && !UCommon.Equals(StrValue, DefaultValue, !CaseInsensitive))
        {
            // prioritize default string value
            Result = UCommon::GetArrayIndex(SearchArray, DefaultValue, CaseInsensitive);
        }

        if (Result < 0 || Result > SearchArray.size())
            Result = Default;
    }
    return Result;
}
*/

void TIni::LoadInputDeviceCfg(TMemIniFile IniFile)
{
    /*
    var
      DeviceCfg:    PInputDeviceConfig;
      DeviceIndex:  integer;
      ChannelCount: integer;
      ChannelIndex: integer;
      RecordKeys:   TStringList;
      i:            integer;
    */
    std::list<std::string> RecordKeys;

    // read all record-keys for filtering
    IniFile.ReadSection("Record", RecordKeys);
    InputDeviceConfig.resize(0);

    for (const auto& key : RecordKeys)
    {
        // find next device-name
        int DeviceIndex = ExtractKeyIndex(key, "DeviceName[", "]");
        if (DeviceIndex >= 0)
        {
            if (!IniFile.ValueExists("Record", std::format("DeviceName[{}]", DeviceIndex)))
                continue;

            // resize list
            InputDeviceConfig.resize(InputDeviceConfig.size() + 1);

            // read an input device"s config.
            // Note: All devices are appended to the list whether they exist ||!.
            //   Otherwise an external device"s config will be lost if it is!
            //   connected (e.g. singstar mics || USB-Audio devices).
            auto& DeviceCfg = InputDeviceConfig.back();
            DeviceCfg.Name = IniFile.ReadString("Record", std::format("DeviceName[{}]", DeviceIndex), "");
            DeviceCfg.Input = IniFile.ReadInteger("Record", std::format("Input[{}]", DeviceIndex), 0);
            DeviceCfg.Latency = IniFile.ReadInteger("Record", std::format("Latency[{}]", DeviceIndex), LATENCY_AUTODETECT);

            // find the largest channel-number of the current device in the ini-file
            int ChannelCount = std::max(0, GetMaxKeyIndex(RecordKeys, "Channel", std::format("[{}]", DeviceIndex)));

            DeviceCfg.ChannelToPlayerMap.resize(ChannelCount);

            // read channel-to-player mapping for every channel of the current device
            // || set non-configured channels to no player (=0).
            for (int ChannelIndex = 0; ChannelIndex < DeviceCfg.ChannelToPlayerMap.size(); ++ChannelIndex)
            {
                DeviceCfg.ChannelToPlayerMap[ChannelIndex] =
                    IniFile.ReadInteger("Record", std::format("Channel{}[{}]", ChannelIndex + 1, DeviceIndex), CHANNEL_OFF);
            }
        }
    }

    // MicBoost
    MicBoost = ReadArrayIndex(IMicBoost, IniFile, "Record", "MicBoost", IGNORE_INDEX, "Off");
    // Threshold
    ThresholdIndex = ReadArrayIndex(IThreshold, IniFile, "Record", "Threshold", 1);
}

void TIni::SaveInputDeviceCfg(TIniFile IniFile)
{
/*
var
  DeviceIndex:  integer;
  ChannelIndex: integer;
  PlayerNumber: integer;
*/
  for (int DeviceIndex = 0; DeviceIndex < InputDeviceConfig.size(); ++DeviceIndex)
  {
    // DeviceName && DeviceInput
    IniFile.WriteString("Record", std::format("DeviceName[{}]", DeviceIndex+1),
                        InputDeviceConfig[DeviceIndex].Name);
    IniFile.WriteInteger("Record", std::format("Input[{}]", DeviceIndex+1),
                        InputDeviceConfig[DeviceIndex].Input);
    IniFile.WriteInteger("Record", std::format("Latency[{}]", DeviceIndex+1),
                        InputDeviceConfig[DeviceIndex].Latency);

    // Channel-to-Player Mapping
    for (int ChannelIndex = 0; ChannelIndex < InputDeviceConfig[DeviceIndex].ChannelToPlayerMap.size(); ++ChannelIndex)
    {
      int PlayerNumber = InputDeviceConfig[DeviceIndex].ChannelToPlayerMap[ChannelIndex];
      if (PlayerNumber > 0)
      {
        IniFile.WriteInteger("Record",
            std::format("Channel{}[{}]", ChannelIndex+1, DeviceIndex+1),
            PlayerNumber);
      }
      else
      {
        IniFile.DeleteKey("Record",
            std::format("Channel{}[{}]", ChannelIndex+1, DeviceIndex+1));
      }
    }
  }

  // MicBoost
  IniFile.WriteString("Record", "MicBoost", IMicBoost[MicBoost]);
  // Threshold
  IniFile.WriteString("Record", "Threshold", IThreshold[ThresholdIndex]);
}

void TIni::LoadPaths(TCustomIniFile IniFile)
{
/*
var
  PathStrings: TStringList;
  I:           integer;
*/
  std::list<std::string> PathStrings;
  IniFile.ReadSection("Directories", PathStrings);

  // Load song-paths
  for (const auto& pStr : PathStrings)
  {
    if (boost::algorithm::to_upper_copy(pStr).find("SONGDIR") == 0)
      UPathUtils::AddSongPath(std::filesystem::path(IniFile.ReadString("Directories", pStr, "")));
  }
}

void TIni::LoadThemes(TCustomIniFile IniFile)
{
  // No Theme Found
  if (ITheme.empty())
    ULog::Log.CriticalError("Could!find any valid Themes.");

  Theme = ReadArrayIndex(ITheme, IniFile, "Themes", "Theme", IGNORE_INDEX, DEFAULT_THEME, true);
  if (Theme = -1)
    Theme = 0;

  // Skin
  Skin.onThemeChange;

  SkinNo = ReadArrayIndex(ISkin, IniFile, "Themes", "Skin", UThemes::Theme.Themes[Theme].DefaultSkin);

  /* there may be a!existing skin in the ini file
    e.g. due to manual edit || corrupted file.
    in this case we load the first Skin */
  if (SkinNo = -1)
    SkinNo = 0;

  // Color
  Color = ReadArrayIndex(IColor, IniFile, "Themes", "Color", Skin.GetDefaultColor(SkinNo));
}
/*
void TIni::LoadScreenModes(TCustomIniFile IniFile)
{
    //MaxFramerate = ReadArrayIndex(IMaxFramerate, IniFile, "Graphics", "MaxFramerate", IGNORE_INDEX, "60");
    //MaxFramerateGet = std::stoi(IMaxFramerate[MaxFramerate]);
    // Screens
    //Screens = ReadArrayIndex(IScreens, IniFile, "Graphics", "Screens", 0);

    // Split mode
    //Split = ReadArrayIndex(ISplit, IniFile, "Graphics", "Split", 0);

    // FullScreen
    //FullScreen = ReadArrayIndex(IFullScreen, IniFile, "Graphics", "FullScreen", IGNORE_INDEX, "Borderless");

    // standard fallback resolutions
    IResolution =
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

    TResolution CurrentRes;
    // Check if there are any modes available

    // retrieve currently used Video Display
    std::optional<int> DisplayIndexOpt;
    SDL_DisplayMode CurrentMode;
    TResolution MaxRes = { 0, 0 };
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
            
            auto it = IResolutionFullScreen.emplace(ModeIter.w, ModeIter.h);
            if (it.second)
            {
                ULog::Log.LogStatus("Found Video Mode: " + it.first->to_string(), "Video");
                MaxRes = MaxRes.max(*it.first);
            }
        }
    }

    // if display modes are found, override fallback ones
    if (!IResolutionFullScreen.empty())
    {
        ULog::Log.LogStatus("Found resolutions: " + std::to_string(IResolutionFullScreen.size()), "Video");
        IResolution = IResolutionFullScreen;

        // reverse order TODO::Why?
        //std::ranges::reverse(IResolution);
    }

    // read fullscreen resolution && verify if possible
    std::string ResString = IniFile.ReadString("Graphics", "ResolutionFullscreen", CurrentRes);
    ResolutionFullscreen = UCommon::GetArrayIndex(IResolutionFullScreen, ResString);

    // Check if there is a resolution configured, try using it
    ResString = IniFile.ReadString("Graphics", "Resolution", CurrentRes);
    // either store desktop resolution || invalid which results into DEFAULT
    // check if stored resolution is valid
    Resolution = UCommon::GetArrayIndex(IResolution, ResString);

    // if resolution cannot be found, check if is larger than max resolution
    if ((Resolution < 0) && (MaxMode.w > 0) && (MaxMode.h > 0) &&
        (UCommon::ParseResolutionString(ResString, ModeIter.w, ModeIter.h)) &&
        ((ModeIter.w > MaxMode.w) || (ModeIter.h > MaxMode.h)))
    {
        ULog::Log.LogInfo(std::format("Exceeding resolution found ({}). Reverting to standard resolution.", ResString), "Video");
        ResString = CurrentRes;
        Resolution = UCommon::GetArrayIndex(IResolution, ResString);
    }

    // append unknown mode to list
    if (Resolution = -1 && ResString.size() >= 3)
    {
        IResolution.emplace_back(ResString);
        Resolution = IResolution.size() - 1;

        // store also as custom resolution to eventually remove it upon window size change
        IResolutionCustom.emplace_back(ResString);
    }

    if (IResolution.empty() || Resolution < 0)
    {
        // if no modes were set,  failback to DEFAULT_RESOLUTION (800x600)
        // as per http://sourceforge.net/forum/message.php?msg_id=4544965
        // THANKS : linnex at users.sourceforge.net
        IResolution.emplace_back(DEFAULT_RESOLUTION);
        Resolution = UCommon::GetArrayIndex(IResolution, DEFAULT_RESOLUTION);

        if (Resolution < 0)
            Resolution = 0;

        ULog::Log.LogStatus(std::format("No video mode found! Default to: {} ", IResolution[Resolution]), "Video");
        FullScreen = 0; // default to fullscreen OFF in this case
    }

    // Depth
    Depth = ReadArrayIndex(IDepth, IniFile, "Graphics", "Depth", IGNORE_INDEX, "32 bit");
}*/

void TIni::Load()
{
    /*
  var
    IniFile: TMemIniFile;
    I:       integer;
    IShowWebScore: array of UTF8String;
    HexColor: string;
    Col: TRGB;
    */
    LoadFontFamilyNames();
    ILyricsFont = FontFamilyNames;
    UPathUtils::GamePath = Platform.GetGameUserPath;

    ULog::Log.LogStatus("GamePath : " + UPathUtils::GamePath.string(), "");

    if (UCommandLine::Params.ConfigFile.has_value())
        Filename = UCommandLine::Params.ConfigFile.value();
    else
        Filename = UPathUtils::GamePath / "config.yaml";

    ULog::Log.LogStatus("Using config : " + Filename.string(), "Yaml");
    auto IniFile = YAML::LoadFile(Filename.string());

    for (int i = 0; i < IMaxPlayerCount; ++i)
    {
        // Name
        Name[i] = IniFile.ReadString("Name", "P" + std::to_string(i + 1), "Player" + std::to_string(i + 1));
        // Color Player
        PlayerColor[i] = IniFile.ReadInteger("PlayerColor", "P" + std::to_string(i + 1), i + 1);
        // Avatar Player
        PlayerAvatar[i] = IniFile.ReadString("PlayerAvatar", "P" + std::to_string(i + 1), "");
        // Level Player
        PlayerLevel[i] = IniFile.ReadInteger("PlayerLevel", "P" + std::to_string(i + 1), 0);
        // Templates for Names Mod
        NameTemplate[i] = IniFile.ReadString("NameTemplate", "Name" + std::to_string(i + 1), "Template" + std::to_string(i + 1));
    }

    // Color Team
    for (int i = 0; i <= 2; ++i)
        TeamColor[i] = IniFile.ReadInteger("TeamColor", "T" + std::to_string(i + 1), i + 1);

    // Templates for Names Mod
    for (int i = 0; i <= 2; ++i)
        NameTeam[i] = IniFile.ReadString("NameTeam", "T" + std::to_string(i + 1), "Team" + std::to_string(i + 1));

    // Players
    Players = ReadArrayIndex(IPlayers, IniFile, "Game", "Players", 0);

    // Difficulty
    Difficulty = ReadArrayIndex(IDifficulty, IniFile, "Game", "Difficulty", IGNORE_INDEX, "Easy");

    // Language
    Language = ReadArrayIndex(ILanguage, IniFile, "Game", "Language", IGNORE_INDEX, "English");
    if (Language < 0)
        Language = UCommon::GetArrayIndex(ILanguage, "English"); // Default to english
    if (Language < 0)
        Language = 0; // Default to first available

    // SongMenu
    SongMenu = ReadArrayIndex(ISongMenuMode, IniFile, "Game", "SongMenu", smRoulette);

    // Tabs
    Tabs = ReadArrayIndex(ITabs, IniFile, "Game", "Tabs", 0);
    TabsAtStartup = Tabs;	//Tabs at Startup fix

    // Song Sorting
    Sorting = ReadArrayIndex(ISorting, IniFile, "Game", "Sorting", sEdition);

    // Show Score
    ShowScores = ReadArrayIndex(IShowScores, IniFile, "Game", "ShowScores", IGNORE_INDEX, "On");

    AVDelay = IniFile.ReadInteger("Game", "AVDelay", 0);

    MicDelay = IniFile.ReadInteger("Game", "MicDelay", 140);

    // Read Users Info (Network)
    UDataBase::DataBase.ReadUsers();

    // Update Webs Scores
    UDataBase::DataBase.AddWebsite();

    // Webs Scores Path
    UPathUtils::WebScoresPath = std::filesystem::path(IniFile.ReadString("Directories", "WebScoresDir", UPathUtils::WebsitePath.string()));
    if (!std::filesystem::exists(UPathUtils::WebScoresPath.string()))
        UPathUtils::WebScoresPath = UPathUtils::WebsitePath;


    // ShowWebScore
    if (!UDLLManager::DLLMan.Websites.empty())
    {
        std::vector<std::string> IShowWebScore;
        IShowWebScore.reserve(UDLLManager::DLLMan.Websites.size());
        for (const auto& website : UDLLManager::DLLMan.Websites)
            IShowWebScore.emplace_back(website.Name);
        ShowWebScore = ReadArrayIndex(IShowWebScore, IniFile, "Game", "ShowWebScore", 0);
        if (ShowWebScore = -1)
            ShowWebScore = 0;
    }

    // Debug
    Debug = ReadArrayIndex(IDebug, IniFile, "Game", "Debug", 0);

    LoadScreenModes(IniFile);

    LoadWebcamSettings(IniFile);

    // TextureSize (aka CachedCoverSize)
    TextureSize = ReadArrayIndex(ITextureSize, IniFile, "Graphics", "TextureSize", IGNORE_INDEX, "256");

    // SingWindow
    SingWindow = ReadArrayIndex(ISingWindow, IniFile, "Graphics", "SingWindow", IGNORE_INDEX, "Big");

    // Oscilloscope
    Oscilloscope = ReadArrayIndex(IOscilloscope, IniFile, "Graphics", "Oscilloscope", 0);

    // Spectrum
    //Spectrum = ReadArrayIndex(ISpectrum, IniFile, "Graphics", "Spectrum", IGNORE_INDEX, "Off");

    // Spectrograph
    //Spectrograph = ReadArrayIndex(ISpectrograph, IniFile, "Graphics", "Spectrograph", IGNORE_INDEX, "Off");

    // MovieSize
    MovieSize = ReadArrayIndex(IMovieSize, IniFile, "Graphics", "MovieSize", 2);

    // VideoPreview
    VideoPreview = ReadArrayIndex(IVideoPreview, IniFile, "Graphics", "VideoPreview", 1);

    // VideoEnabled
    VideoEnabled = ReadArrayIndex(IVideoEnabled, IniFile, "Graphics", "VideoEnabled", 1);

    PreferredCodecNames = IniFile.ReadString("Graphics", "PreferredCodecs", "");

    // ClickAssist
    ClickAssist = ReadArrayIndex(IClickAssist, IniFile, "Sound", "ClickAssist", IGNORE_INDEX, "Off");

    // BeatClick
    BeatClick = ReadArrayIndex(IBeatClick, IniFile, "Sound", "BeatClick", 0);

    // SavePlayback
    SavePlayback = ReadArrayIndex(ISavePlayback, IniFile, "Sound", "SavePlayback", 0);

    // AudioOutputBufferSize
    AudioOutputBufferSizeIndex = ReadArrayIndex(IAudioOutputBufferSize, IniFile, "Sound", "AudioOutputBufferSize", 0);

    //Preview Volume
    PreviewVolume = ReadArrayIndex(IPreviewVolume, IniFile, "Sound", "PreviewVolume", 7);

    //Preview Fading
    PreviewFading = ReadArrayIndex(IPreviewFading, IniFile, "Sound", "PreviewFading", 3);

    //AudioRepeat aka VoicePassthrough
    VoicePassthrough = ReadArrayIndex(IVoicePassthrough, IniFile, "Sound", "VoicePassthrough", 0);

    // ReplayGain aka MusicAutoGain
    MusicAutoGain = ReadArrayIndex(IMusicAutoGain, IniFile, "Sound", "MusicAutoGain", 0);

    SoundFont = IniFile.ReadString("Sound", "SoundFont", "");

    // Lyrics Font
    LyricsFont = ReadArrayIndex(ILyricsFont, IniFile, "Lyrics", "LyricsFont", 0);

    // Lyrics Style
    LyricsStyle = ReadArrayIndex(ILyricsStyleCompat, IniFile, "Lyrics", "LyricsStyle", -1);
    if (LyricsStyle = -1)
        LyricsStyle = ReadArrayIndex(ILyricsStyle, IniFile, "Lyrics", "LyricsStyle", 0);

    // Lyrics Effect
    LyricsEffect = ReadArrayIndex(ILyricsEffect, IniFile, "Lyrics", "LyricsEffect", 2);

    // NoteLines
    NoteLines = ReadArrayIndex(INoteLines, IniFile, "Lyrics", "NoteLines", 1);

    // DefaultEncoding
    DefaultEncoding = ParseEncoding(IniFile.ReadString("Lyrics", "Encoding", ""), encAuto);

    LoadThemes(IniFile);

    LoadInputDeviceCfg(IniFile);

    // LoadAnimation
    LoadAnimation = ReadArrayIndex(ILoadAnimation, IniFile, "Advanced", "LoadAnimation", IGNORE_INDEX, "On");

    // ScreenFade
    ScreenFade = ReadArrayIndex(IScreenFade, IniFile, "Advanced", "ScreenFade", IGNORE_INDEX, "On");

    // Visualizations
    // <mog> this could be of use later..
    //  VisualizerOption =
    //    TVisualizerOption(GetEnumValue(TypeInfo(TVisualizerOption),
    //            IniFile.ReadString("Graphics", "Visualization", "Off")));
    // || VisualizerOption = TVisualizerOption(UCommon::GetArrayIndex(IVisualizer, IniFile.ReadString("Graphics", "Visualization", "Off")));
    VisualizerOption = ReadArrayIndex(IVisualizer, IniFile, "Graphics", "Visualization", IGNORE_INDEX, "Off");

    /**
     * Background music
     */
    BackgroundMusicOption = ReadArrayIndex(IBackgroundMusic, IniFile, "Sound", "BackgroundMusic", IGNORE_INDEX, "On");

    // EffectSing
    EffectSing = ReadArrayIndex(IEffectSing, IniFile, "Advanced", "EffectSing", IGNORE_INDEX, "On");

    // AskbeforeDel
    AskBeforeDel = ReadArrayIndex(IAskbeforeDel, IniFile, "Advanced", "AskbeforeDel", IGNORE_INDEX, "On");

    // OnSongClick
    OnSongClick = ReadArrayIndex(IOnSongClick, IniFile, "Advanced", "OnSongClick", IGNORE_INDEX, "Sing");

    // Linebonus
    LineBonus = ReadArrayIndex(ILineBonus, IniFile, "Advanced", "LineBonus", 1);

    // PartyPopup
    PartyPopup = ReadArrayIndex(IPartyPopup, IniFile, "Advanced", "PartyPopup", IGNORE_INDEX, "On");

    // SingScores
    SingScores = ReadArrayIndex(ISingScores, IniFile, "Advanced", "SingScores", IGNORE_INDEX, "On");

    // TopScores
    TopScores = ReadArrayIndex(ITopScores, IniFile, "Advanced", "TopScores", IGNORE_INDEX, "All");

    // SyncTo
    SyncTo = ReadArrayIndex(ISyncTo, IniFile, "Advanced", "SyncTo", stMusic);

    // Joypad
    Joypad = ReadArrayIndex(IJoypad, IniFile, "Controller", "Joypad", 0);

    // Mouse
    Mouse = ReadArrayIndex(IMouse, IniFile, "Controller", "Mouse", 2);
    if (Mouse < 0) // try finding legacy option
        Mouse = ReadArrayIndex(IMouseLegacy, IniFile, "Controller", "Mouse", 2);

    // SingTimebarMode
    SingTimebarMode = ReadArrayIndex(ISingTimebarMode, IniFile, "Advanced", "SingTimebarMode", IGNORE_INDEX, "Remaining");

    // JukeboxTimebarMode
    JukeboxTimebarMode = ReadArrayIndex(IJukeboxTimebarMode, IniFile, "Advanced", "JukeboxTimebarMode", IGNORE_INDEX, "Current");

    // TODO::WebCam replace read
  /*
    WebcamID = IniFile.ReadInteger("Webcam", "ID", 0);
    WebcamResolution = ReadArrayIndex(IWebcamResolution, IniFile, "Webcam", "Resolution", IGNORE_INDEX, "320x240");
    if (WebcamResolution = -1)
      WebcamResolution = 2;
    WebcamFPS = ReadArrayIndex(IWebcamFPS, IniFile, "Webcam", "FPS", 4);
    WebcamFlip = ReadArrayIndex(IWebcamFlipTranslated, IniFile, "Webcam", "Flip", IGNORE_INDEX, "On");
    WebcamBrightness = ReadArrayIndex(IWebcamBrightness, IniFile, "Webcam", "Brightness", IGNORE_INDEX, "0");
    WebcamSaturation = ReadArrayIndex(IWebcamSaturation, IniFile, "Webcam", "Saturation", IGNORE_INDEX, "0");
    WebcamHue = ReadArrayIndex(IWebcamHue, IniFile, "Webcam", "Hue", IGNORE_INDEX, "0");
    WebcamEffect = IniFile.ReadInteger("Webcam", "Effect", 0);
    */
    // Jukebox
    JukeboxFont = ReadArrayIndex(ILyricsFont, IniFile, "Jukebox", "LyricsFont", 0);
    JukeboxStyle = ReadArrayIndex(ILyricsStyle, IniFile, "Jukebox", "LyricsStyle", 0);
    JukeboxEffect = ReadArrayIndex(ILyricsEffect, IniFile, "Jukebox", "LyricsEffect", 1);
    JukeboxAlpha = ReadArrayIndex(ILyricsAlpha, IniFile, "Jukebox", "LyricsAlpha", 20);

    JukeboxSongMenu = ReadArrayIndex(IJukeboxSongMenu, IniFile, "Jukebox", "SongMenu", IGNORE_INDEX, "On");

    //TODO:: replace with:: struct convert<UIni::JukeBoxFillColors> {
  /*
  JukeboxSingLineColor = ReadArrayIndex(IHexSingColor, IniFile, "Jukebox", "SingLineColor", IHexSingColor.size() - 1);

  // other color
  if ((JukeboxSingLineColor < 0) || (JukeboxSingLineColor == IHexSingColor.size() - 1))
  {
    JukeboxSingLineColor = IHexSingColor.size() - 1;

    std::string HexColor = IniFile.ReadString("Jukebox", "SingLineColor", IHexSingColor[0]);
    auto Col = UCommon::HexToRGB(HexColor);

    Ini.JukeboxSingLineOtherColorR = std::round(Col.R);
    Ini.JukeboxSingLineOtherColorG = std::round(Col.G);
    Ini.JukeboxSingLineOtherColorB = std::round(Col.B);
  }

  JukeboxActualLineColor = ReadArrayIndex(IHexGrayColor, IniFile, "Jukebox", "ActualLineColor", IHexGrayColor.size() - 1);

  // other color
  if ((JukeboxActualLineColor == -1) || (JukeboxActualLineColor == IHexGrayColor.size() - 1))
  {
    JukeboxActualLineColor = IHexGrayColor.size() - 1;

    auto HexColor = IniFile.ReadString("Jukebox", "ActualLineColor", IHexGrayColor[8]);
    auto Col = UCommon::HexToRGB(HexColor);

    Ini.JukeboxActualLineOtherColorR = std::round(Col.R);
    Ini.JukeboxActualLineOtherColorG = std::round(Col.G);
    Ini.JukeboxActualLineOtherColorB = std::round(Col.B);
  }

  JukeboxNextLineColor = ReadArrayIndex(IHexGrayColor, IniFile, "Jukebox", "NextLineColor", IHexGrayColor.size() - 1);
  // other color
  if ((JukeboxNextLineColor == -1) || (JukeboxNextLineColor == IHexGrayColor.size() - 1))
  {
    JukeboxNextLineColor = IHexGrayColor.size() - 1;

    auto HexColor = IniFile.ReadString("Jukebox", "NextLineColor", IHexGrayColor[6]);
    auto Col = UCommon::HexToRGB(HexColor);

    Ini.JukeboxNextLineOtherColorR = std::round(Col.R);
    Ini.JukeboxNextLineOtherColorG = std::round(Col.G);
    Ini.JukeboxNextLineOtherColorB = std::round(Col.B);
  }

  JukeboxSingLineOutlineColor = ReadArrayIndex(IHexOColor, IniFile, "Jukebox", "SingLineOColor", 0);
  // other color
  if (JukeboxSingLineOutlineColor == -1)
  {
    JukeboxSingLineOutlineColor = IHexOColor.size() - 1;

    auto HexColor = IniFile.ReadString("Jukebox", "SingLineOColor", IHexOColor[0]);
    auto Col = UCommon::HexToRGB(HexColor);

    Ini.JukeboxSingLineOtherOColorR = std::round(Col.R);
    Ini.JukeboxSingLineOtherOColorG = std::round(Col.G);
    Ini.JukeboxSingLineOtherOColorB = std::round(Col.B);
  }

  JukeboxActualLineOutlineColor = ReadArrayIndex(IHexOColor, IniFile, "Jukebox", "ActualLineOColor", 0);
  // other color
  if (JukeboxActualLineOutlineColor == -1)
  {
    JukeboxActualLineOutlineColor = IHexOColor.size() - 1;

    auto HexColor = IniFile.ReadString("Jukebox", "ActualLineOColor", IHexOColor[0]);
    auto Col = UCommon::HexToRGB(HexColor);

    Ini.JukeboxActualLineOtherOColorR = std::round(Col.R);
    Ini.JukeboxActualLineOtherOColorG = std::round(Col.G);
    Ini.JukeboxActualLineOtherOColorB = std::round(Col.B);
  }

  JukeboxNextLineOutlineColor = ReadArrayIndex(IHexOColor, IniFile, "Jukebox", "NextLineOColor", 0);
  // other color
  if (JukeboxNextLineOutlineColor == -1)
  {
    JukeboxNextLineOutlineColor = IHexOColor.size() - 1;

    auto HexColor = IniFile.ReadString("Jukebox", "NextLineOColor", IHexOColor[0]);
    auto Col = UCommon::HexToRGB(HexColor);

    Ini.JukeboxNextLineOtherOColorR = std::round(Col.R);
    Ini.JukeboxNextLineOtherOColorG = std::round(Col.G);
    Ini.JukeboxNextLineOtherOColorB = std::round(Col.B);
  }
  */
    LoadPaths(IniFile);

    TranslateOptionValues();
}

void TIni::Save()
/*
var
  IniFile: TIniFile;
  HexColor: string;
  I: integer;
  C: TRGB;*/
{
  try
  {
    if (std::filesystem::is_regular_file(Filename) && !UPathUtils::IsWritable(Filename))
    {
      ULog::Log.LogError("Config-file is read-only", "TIni.Save");
      return;
    }

    TIniFile IniFile = TIniFile(Filename);

    // Players
    IniFile.WriteString("Game", "Players", IPlayers[Players]);

    // Difficulty
    IniFile.WriteString("Game", "Difficulty", IDifficulty[Difficulty]);

    // Language
    IniFile.WriteString("Game", "Language", ILanguage[Language]);

    // Tabs
    IniFile.WriteString("Game", "Tabs", ITabs[Tabs]);

    // SongMenu
    IniFile.WriteString("Game", "SongMenu", ISongMenuMode[SongMenu]);

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
    IniFile.WriteString("Graphics", "Resolution", GetResolution());
    IniFile.WriteString("Graphics", "ResolutionFullscreen", GetResolutionFullscreen());

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
    // Note: Value must be " " &&!"", otherwise no key is generated on Linux
    if (!IniFile.SectionExists("Directories")) 
      IniFile.WriteString("Directories", "SongDir1", " ");

    if (!IniFile.ValueExists("Directories", "WebScoresDir")) 
      IniFile.WriteString("Directories", "WebScoresDir", " ");

    // Jukebox
    IniFile.WriteString("Jukebox", "LyricsFont", ILyricsFont[JukeboxFont]);
    IniFile.WriteString("Jukebox", "LyricsStyle", ILyricsStyle[JukeboxStyle]);
    IniFile.WriteString("Jukebox", "LyricsEffect", ILyricsEffect[JukeboxEffect]);
    IniFile.WriteString("Jukebox", "LyricsAlpha", ILyricsAlpha[JukeboxAlpha]);

    /*
    AssignJukeboxColor(JukeboxSingLineColor, ISingLineColor, JukeboxSingLineOtherColorR, JukeboxSingLineOtherColorG, JukeboxSingLineOtherColorB, IniFile, "SingLineColor");
    AssignJukeboxColor(JukeboxActualLineColor, IActualLineColor, JukeboxActualLineOtherColorR, JukeboxActualLineOtherColorG, JukeboxActualLineOtherColorB, IniFile, "ActualLineColor");
    AssignJukeboxColor(JukeboxNextLineColor, INextLineColor, JukeboxNextLineOtherColorR, JukeboxNextLineOtherColorG, JukeboxNextLineOtherColorB, IniFile, "NextLineColor");
    AssignJukeboxColor(JukeboxSingLineOutlineColor, ISingLineOColor, JukeboxSingLineOtherOColorR, JukeboxSingLineOtherOColorG, JukeboxSingLineOtherOColorB, IniFile, "SingLineOColor");
    AssignJukeboxColor(JukeboxActualLineOutlineColor, IActualLineOColor, JukeboxActualLineOtherOColorR, JukeboxActualLineOtherOColorG, JukeboxActualLineOtherOColorB, IniFile, "ActualLineOColor");
    AssignJukeboxColor(JukeboxNextLineOutlineColor, INextLineOColor, JukeboxNextLineOtherOColorR, JukeboxNextLineOtherOColorG, JukeboxNextLineOtherOColorB, IniFile, "NextLineOColor");
    */
  }
  catch (const std::exception& e)
  {
    ULog::Log.LogWarn("Saving InputDeviceConfig failed: " + std::string(e.what()), "UIni.Save");
  }
}

void TIni::SaveNames()
{
  if(UPathUtils::IsWritable(Filename))
  {
    auto IniFile = TIniFile(Filename);

    //Name Templates for Names Mod
    for (int i = 0; i < Name.size(); ++i)
      IniFile.WriteString("Name", "P" + std::to_string(i+1), Name[i]);
    for (int i = 0; i < NameTeam.size(); ++i)
      IniFile.WriteString("NameTeam", "T" + std::to_string(i+1), NameTeam[i]);
    for (int i = 0; i < NameTemplate.size(); ++i)
      IniFile.WriteString("NameTemplate", "Name" + std::to_string(i+1), NameTemplate[i]);
  }
}

void TIni::SaveLevel()
{
  if (UPathUtils::IsWritable(Filename))
  {
    auto IniFile = TIniFile(Filename);

    // Difficulty
    IniFile.WriteString("Game", "Difficulty", IDifficulty[Difficulty]);
  }
}

void TIni::SaveJukeboxSongMenu()
/*var
  IniFile: TIniFile;*/
{
  if (UPathUtils::IsWritable(Filename)) 
  {
    auto IniFile = TIniFile(Filename);

    IniFile.WriteString("Jukebox", "SongMenu", IJukeboxSongMenu[JukeboxSongMenu]);
  }
}


void TIni::SaveShowWebScore()
{
  if (UPathUtils::IsWritable(Filename))
  {
    auto IniFile = TIniFile(Filename);

    // ShowWebScore
    IniFile.WriteString("Game", "ShowWebScore", UDLLManager::DLLMan.Websites[ShowWebScore].Name);
  }
}


void TIni::SavePlayerColors()
{
  if(UPathUtils::IsWritable(Filename))
  {
    auto IniFile = TIniFile(Filename);

    //Colors for Names Mod
    for (int i = 1; i <= IMaxPlayerCount; ++i)
      IniFile.WriteString("PlayerColor", "P" + std::to_string(i), std::to_string(PlayerColor[i-1]));
  }
}

void TIni::SavePlayerAvatars()
{
  if(UPathUtils::IsWritable(Filename)) 
  {
    auto IniFile = TIniFile(Filename);

    //Colors for Names Mod
    for (int i = 1; i <= IMaxPlayerCount; ++i)
      IniFile.WriteString("PlayerAvatar", "P" + std::to_string(i), PlayerAvatar[i-1]);
  }
}

void TIni::SavePlayerLevels()
{
  if(UPathUtils::IsWritable(Filename))
  {
    auto IniFile = TIniFile(Filename);

    for (int i = 1; i <= IMaxPlayerCount; ++i)
      IniFile.WriteInteger("PlayerLevel", "P" + std::to_string(i), PlayerLevel[i-1]);
  }
}

void TIni::SaveTeamColors()
{
  if(UPathUtils::IsWritable(Filename))
  {
    auto IniFile = TIniFile(Filename);

    //Colors for Names Mod
    for (int i = 1; i <= 3; ++i)
      IniFile.WriteString("TeamColor", "T" + std::to_string(i), std::to_string(TeamColor[i-1]));
  }
}

void TIni::SaveSoundFont(std::string Name)
{
  if(UPathUtils::IsWritable(Filename))
  {
    auto IniFile = TIniFile(Filename);

    IniFile.WriteString("Sound", "SoundFont", Name);
  }
}
/*
void TIni::SaveWebcamSettings()
{
    if (UPathUtils::IsWritable(Filename))
    {
        auto IniFile = TIniFile(Filename);

        // WebCam
        IniFile.WriteInteger("Webcam", "ID", WebcamID);
        IniFile.WriteString("Webcam", "Resolution", IWebcamResolution[WebcamResolution]);
        IniFile.WriteInteger("Webcam", "FPS", std::stoi(IWebcamFPS[WebcamFPS]));

        IniFile.WriteString("Webcam", "Flip", IWebcamFlip[WebcamFlip]);
        IniFile.WriteString("Webcam", "Brightness", IWebcamBrightness[WebcamBrightness]);
        IniFile.WriteString("Webcam", "Saturation", IWebcamSaturation[WebcamSaturation]);
        IniFile.WriteString("Webcam", "Hue", IWebcamHue[WebcamHue]);
        IniFile.WriteInteger("Webcam", "Effect", WebcamEffect);
    }
}
*/
void TIni::SaveNumberOfPlayers()
{
    if (UPathUtils::IsWritable(Filename))
    {
        auto IniFile = TIniFile(Filename);

        // Players
        IniFile.WriteString("Game", "Players", IPlayers[Players]);
    }
}

void TIni::SaveSingTimebarMode()
{
    if (UPathUtils::IsWritable(Filename))
    {
        auto IniFile = TIniFile(Filename);

        // Players
        IniFile.WriteString("Advanced", "SingTimebarMode", ISingTimebarMode[SingTimebarMode]);
    }
}

void TIni::SaveJukeboxTimebarMode()
{
    if (UPathUtils::IsWritable(Filename))
    {
        auto IniFile = TIniFile(Filename);

        // Players
        IniFile.WriteString("Advanced", "JukeboxTimebarMode", IJukeboxTimebarMode[JukeboxTimebarMode]);
    }
}

bool TIni::SetResolution(std::string ResolutionString, bool RemoveCurrent = false, bool NoSave = false)
{
    /*var
      Index: integer;
      Dirty: boolean;*/

    bool Result = false;
    bool Dirty = false;

    // check if current resolution is custom &&  remove anyway (no matter what RemoveCurrent is set)
    if (Resolution >= 0)
    {
        int Index = UCommon::GetArrayIndex(IResolutionCustom, IResolution[Resolution]);
        if (Index >= 0)
        {
            IResolutionCustom.erase(IResolutionCustom.begin() + Index);
            IResolution.erase(IResolution.begin() + Resolution);
        }
    }

    int Index = UCommon::GetArrayIndex(IResolution, ResolutionString);
    if (!NoSave && (Resolution != Index))
        Dirty = true;
    if (Resolution >= 0 && RemoveCurrent)
        IResolution.erase(IResolution.begin() + Resolution);
    if (Index < 0)
    {
        IResolution.emplace_back(ResolutionString);
        Index = IResolution.size() - 1;
        Result = true;

        if (UCommon::GetArrayIndex(IResolutionCustom, ResolutionString) < 0)
            IResolutionCustom.emplace_back(ResolutionString);
    }

    if (SetResolution(Index) && Dirty)
    {
        ULog::Log.LogStatus("Resolution overridden to: " + ResolutionString, "Video");
        Save();
    }
}

bool TIni::SetResolution(int w, int h, bool RemoveCurrent = false, bool NoSave = false)
{
    // hacky fix to support multiplied resolution (in width) in multiple screen setup (Screens=2 && more)
    // TODO: RattleSN4K3: Improve the way multiplied screen resolution is applied && stored (see UGraphics::InitializeScreen; W = W * Screens)
    if (Screens > 0 && !((UCommandLine::Params.Split = UCommandLine::spmSplit) || (Split > 0)))
    {
        w = w / (Screens + 1); // integral div
    }
    else if (UCommandLine::Params.Screens > 0 && !((UCommandLine::Params.Split = UCommandLine::spmSplit) || (Split > 0)))
        w = w / (UCommandLine::Params.Screens + 1);

    return SetResolution(UCommon::BuildResolutionString(w, h), RemoveCurrent, NoSave);
}

bool TIni::SetResolution(int index)
{
    bool Result = false;
    if ((index >= 0) && (index < IResolution.size()))
    {
        Resolution = index;
        Result = true;
    }
    return Result;
}

std::string TIni::GetResolution()
{
    if (Resolution >= 0)
        return IResolution[Resolution];
    else if (IResolution.empty())
        return DEFAULT_RESOLUTION;
    else
        return IResolution[0];
}

std::string TIni::GetResolution(int& w, int& h)
{
    auto Result = GetResolution();
    UCommon::ParseResolutionString(Result, w, h);

    // hacky fix to support multiplied resolution (in width) in multiple screen setup (Screens=2 && more)
    // TODO: RattleSN4K3: Improve the way multiplied screen resolution is applied && stored (see UGraphics::InitializeScreen; W = W * Screens)
    if ((Screens > 0) && !((UCommandLine::Params.Split = UCommandLine::spmSplit) || (Split > 0)))
        w = w * (Screens + 1);
    else if ((UCommandLine::Params.Screens > 0) && !((UCommandLine::Params.Split = UCommandLine::spmSplit) || (Split > 0)))
        w = w * (UCommandLine::Params.Screens + 1);

    return Result;
}

bool TIni::GetResolution(int index, std::string& ResolutionString)
{
    bool Result = false;
    if ((index >= 0) && (index < IResolution.size()))
    {
        ResolutionString = IResolution[index];
        Result = true;
    }
    return Result;
}

std::string TIni::GetResolutionFullscreen()
{
    if (ResolutionFullscreen >= 0)
        return IResolutionFullScreen[ResolutionFullscreen];
    else if (IResolutionFullScreen.empty())
        return DEFAULT_RESOLUTION;
    else
        return IResolutionFullScreen[0];
}

std::string TIni::GetResolutionFullscreen(int& w, int& h)
{
    auto Result = GetResolutionFullscreen();
    UCommon::ParseResolutionString(Result, w, h);

    return Result;
}

bool TIni::GetResolutionFullscreen(int index, std::string& ResolutionString)
{
    bool Result = false;
    if ((index >= 0) && (index < IResolutionFullScreen.size()))
    {
        ResolutionString = IResolutionFullScreen[index];
        Result = true;
    }
    return Result;
}

void TIni::ClearCustomResolutions()
{
    if (Resolution < 0)
        return;

    // check if current resolution is a custom one
    std::string ResString = IResolution[Resolution];
    int Index = UCommon::GetArrayIndex(IResolutionCustom, ResString);

    for (int i = IResolutionCustom.size() - 1; i >= 0; --i)
    {
        int custom = UCommon::GetArrayIndex(IResolution, IResolutionCustom[i]);
        if ((custom >= 0) && (Index != i))
        {
            IResolution.erase(IResolution.begin() + custom);
            IResolutionCustom.erase(IResolutionCustom.begin() + i);
        }
    }
    // update index
    Resolution = UCommon::GetArrayIndex(IResolution, ResString);
}
}
