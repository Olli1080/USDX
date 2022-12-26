#include "UIni.h"

namespace UIni
{
/**
 * Translate and set the values of options, which need translation.
 */
void TIni::TranslateOptionValues()
{
  int I;
  std::string Zeros;

  // Load language file, fallback to config language if param is invalid
  if (Params.Language > -1) and (Params.Language < Length(ILanguage)) then
    ULanguage.Language.ChangeLanguage(ILanguage[Params.Language])
  else
    ULanguage.Language.ChangeLanguage(ILanguage[Language]);

  IDifficultyTranslated[0]            = ULanguage.Language.Translate("OPTION_VALUE_EASY");
  IDifficultyTranslated[1]            = ULanguage.Language.Translate("OPTION_VALUE_MEDIUM");
  IDifficultyTranslated[2]            = ULanguage.Language.Translate("OPTION_VALUE_HARD");

  ITabsTranslated[0]                  = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  ITabsTranslated[1]                  = ULanguage.Language.Translate("OPTION_VALUE_ON");

  ISongMenuTranslated[0]              = ULanguage.Language.Translate("OPTION_VALUE_ROULETTE");
  ISongMenuTranslated[1]              = ULanguage.Language.Translate("OPTION_VALUE_CHESSBOARD");
  ISongMenuTranslated[2]              = ULanguage.Language.Translate("OPTION_VALUE_CAROUSEL");
  ISongMenuTranslated[3]              = ULanguage.Language.Translate("OPTION_VALUE_SLOTMACHINE");
  ISongMenuTranslated[4]              = ULanguage.Language.Translate("OPTION_VALUE_SLIDE");
  ISongMenuTranslated[5]              = ULanguage.Language.Translate("OPTION_VALUE_LIST");
  ISongMenuTranslated[6]              = ULanguage.Language.Translate("OPTION_VALUE_MOSAIC");

  ISortingTranslated[0]               = ULanguage.Language.Translate("OPTION_VALUE_EDITION");
  ISortingTranslated[1]               = ULanguage.Language.Translate("OPTION_VALUE_GENRE");
  ISortingTranslated[2]               = ULanguage.Language.Translate("OPTION_VALUE_LANGUAGE");
  ISortingTranslated[3]               = ULanguage.Language.Translate("OPTION_VALUE_FOLDER");
  ISortingTranslated[4]               = ULanguage.Language.Translate("OPTION_VALUE_TITLE");
  ISortingTranslated[5]               = ULanguage.Language.Translate("OPTION_VALUE_ARTIST");
  ISortingTranslated[6]               = ULanguage.Language.Translate("OPTION_VALUE_ARTIST2");
  ISortingTranslated[7]               = ULanguage.Language.Translate("OPTION_VALUE_YEAR");
  ISortingTranslated[8]               = ULanguage.Language.Translate("OPTION_VALUE_YEAR_REVERSED");
  ISortingTranslated[9]               = ULanguage.Language.Translate("OPTION_VALUE_DECADE");
  //ISortingTranslated[10]               = ULanguage.Language.Translate("OPTION_VALUE_PLAYLIST");

  IShowScoresTranslated[0]            = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IShowScoresTranslated[1]            = ULanguage.Language.Translate("OPTION_VALUE_WHENEXIST");
  IShowScoresTranslated[2]            = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IDebugTranslated[0]                 = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IDebugTranslated[1]                 = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IFullScreenTranslated[0]            = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IFullScreenTranslated[1]            = ULanguage.Language.Translate("OPTION_VALUE_ON");
  IFullScreenTranslated[2]            = ULanguage.Language.Translate("OPTION_VALUE_BORDERLESS");

  IVisualizerTranslated[0]            = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IVisualizerTranslated[1]            = ULanguage.Language.Translate("OPTION_VALUE_WHENNOVIDEO");
  IVisualizerTranslated[2]            = ULanguage.Language.Translate("OPTION_VALUE_WHENNOVIDEOANDIMAGE");
  IVisualizerTranslated[3]            = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IBackgroundMusicTranslated[0]       = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IBackgroundMusicTranslated[1]       = ULanguage.Language.Translate("OPTION_VALUE_ON");

  ISingWindowTranslated[0]            = ULanguage.Language.Translate("OPTION_VALUE_SMALL");
  ISingWindowTranslated[1]            = ULanguage.Language.Translate("OPTION_VALUE_BIG");

  IOscilloscopeTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IOscilloscopeTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_ON");

  ISpectrumTranslated[0]              = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  ISpectrumTranslated[1]              = ULanguage.Language.Translate("OPTION_VALUE_ON");

  ISpectrographTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  ISpectrographTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IMovieSizeTranslated[0]             = ULanguage.Language.Translate("OPTION_VALUE_HALF");
  IMovieSizeTranslated[1]             = ULanguage.Language.Translate("OPTION_VALUE_FULL_VID");
  IMovieSizeTranslated[2]             = ULanguage.Language.Translate("OPTION_VALUE_FULL_VID_BG");

  IVideoPreviewTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IVideoPreviewTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IVideoEnabledTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IVideoEnabledTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IClickAssistTranslated[0]           = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IClickAssistTranslated[1]           = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IBeatClickTranslated[0]             = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IBeatClickTranslated[1]             = ULanguage.Language.Translate("OPTION_VALUE_ON");

  ISavePlaybackTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  ISavePlaybackTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IVoicePassthroughTranslated[0]      = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IVoicePassthroughTranslated[1]      = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IMusicAutoGainTranslated[0]      = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IMusicAutoGainTranslated[1]      = ULanguage.Language.Translate("OPTION_VALUE_GAIN_SOFT");
  IMusicAutoGainTranslated[2]      = ULanguage.Language.Translate("OPTION_VALUE_GAIN_MEDIUM");
  IMusicAutoGainTranslated[3]      = ULanguage.Language.Translate("OPTION_VALUE_GAIN_HARD");

  ISyncToTranslated[Ord(stMusic)]     = ULanguage.Language.Translate("OPTION_VALUE_MUSIC");
  ISyncToTranslated[Ord(stLyrics)]    = ULanguage.Language.Translate("OPTION_VALUE_LYRICS");
  ISyncToTranslated[Ord(stOff)]       = ULanguage.Language.Translate("OPTION_VALUE_OFF");

  ILyricsStyleTranslated[0]           = ULanguage.Language.Translate("OPTION_VALUE_REGULAR");
  ILyricsStyleTranslated[1]           = ULanguage.Language.Translate("OPTION_VALUE_BOLD");
  ILyricsStyleTranslated[2]           = ULanguage.Language.Translate("OPTION_VALUE_TEXTOUTLINE");

  ILyricsEffectTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_SIMPLE");
  ILyricsEffectTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_ZOOM");
  ILyricsEffectTranslated[2]          = ULanguage.Language.Translate("OPTION_VALUE_SLIDE");
  ILyricsEffectTranslated[3]          = ULanguage.Language.Translate("OPTION_VALUE_BALL");
  ILyricsEffectTranslated[4]          = ULanguage.Language.Translate("OPTION_VALUE_SHIFT");

  INoteLinesTranslated[0]             = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  INoteLinesTranslated[1]             = ULanguage.Language.Translate("OPTION_VALUE_ON");

  for (int I = 0; I < 256; ++i)
  {
    IRed[I] = std::to_string(I);
    IGreen[I] = std::to_string(I);
    IBlue[I] = std::to_string(I);
  }

  ILineTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_TO_SING");
  ILineTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_ACTUAL");
  ILineTranslated[2] = ULanguage.Language.Translate("OPTION_VALUE_NEXT");

  IPropertyTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_FILL");
  IPropertyTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_OUTLINE");

  ISingLineColorTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_BLUE");
  ISingLineColorTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_GREEN");
  ISingLineColorTranslated[2] = ULanguage.Language.Translate("OPTION_VALUE_PINK");
  ISingLineColorTranslated[3] = ULanguage.Language.Translate("OPTION_VALUE_RED");
  ISingLineColorTranslated[4] = ULanguage.Language.Translate("OPTION_VALUE_VIOLET");
  ISingLineColorTranslated[5] = ULanguage.Language.Translate("OPTION_VALUE_ORANGE");
  ISingLineColorTranslated[6] = ULanguage.Language.Translate("OPTION_VALUE_YELLOW");
  ISingLineColorTranslated[7] = ULanguage.Language.Translate("OPTION_VALUE_BROWN");
  ISingLineColorTranslated[8] = ULanguage.Language.Translate("OPTION_VALUE_BLACK");
  ISingLineColorTranslated[9] = ULanguage.Language.Translate("OPTION_VALUE_TURQUOISE");
  ISingLineColorTranslated[10] = ULanguage.Language.Translate("OPTION_VALUE_SALMON");
  ISingLineColorTranslated[11] = ULanguage.Language.Translate("OPTION_VALUE_GREENYELLOW");
  ISingLineColorTranslated[12] = ULanguage.Language.Translate("OPTION_VALUE_LAVENDER");
  ISingLineColorTranslated[13] = ULanguage.Language.Translate("OPTION_VALUE_BEIGE");
  ISingLineColorTranslated[14] = ULanguage.Language.Translate("OPTION_VALUE_TEAL");
  ISingLineColorTranslated[15] = ULanguage.Language.Translate("OPTION_VALUE_ORCHID");
  ISingLineColorTranslated[16] = ULanguage.Language.Translate("OPTION_VALUE_STEELBLUE");
  ISingLineColorTranslated[17] = ULanguage.Language.Translate("OPTION_VALUE_PLUM");
  ISingLineColorTranslated[18] = ULanguage.Language.Translate("OPTION_VALUE_CHOCOLATE");
  ISingLineColorTranslated[19] = ULanguage.Language.Translate("OPTION_VALUE_GOLD");
  ISingLineColorTranslated[20] = ULanguage.Language.Translate("OPTION_VALUE_OTHER");

  IActualLineColorTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_BLACK");
  IActualLineColorTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " +3";
  IActualLineColorTranslated[2] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " +2";
  IActualLineColorTranslated[3] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " +1";
  IActualLineColorTranslated[4] = ULanguage.Language.Translate("OPTION_VALUE_GRAY");
  IActualLineColorTranslated[5] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " -1";
  IActualLineColorTranslated[6] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " -2";
  IActualLineColorTranslated[7] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " -3";
  IActualLineColorTranslated[8] = ULanguage.Language.Translate("OPTION_VALUE_WHITE");
  IActualLineColorTranslated[9] = ULanguage.Language.Translate("OPTION_VALUE_OTHER");

  INextLineColorTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_BLACK");
  INextLineColorTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " +3";
  INextLineColorTranslated[2] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " +2";
  INextLineColorTranslated[3] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " +1";
  INextLineColorTranslated[4] = ULanguage.Language.Translate("OPTION_VALUE_GRAY");
  INextLineColorTranslated[5] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " -1";
  INextLineColorTranslated[6] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " -2";
  INextLineColorTranslated[7] = ULanguage.Language.Translate("OPTION_VALUE_GRAY") + " -3";
  INextLineColorTranslated[8] = ULanguage.Language.Translate("OPTION_VALUE_WHITE");
  INextLineColorTranslated[9] = ULanguage.Language.Translate("OPTION_VALUE_OTHER");

  ISingLineOColorTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_BLACK");
  ISingLineOColorTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_WHITE");
  ISingLineOColorTranslated[2] = ULanguage.Language.Translate("OPTION_VALUE_OTHER");

  IActualLineOColorTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_BLACK");
  IActualLineOColorTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_WHITE");
  IActualLineOColorTranslated[2] = ULanguage.Language.Translate("OPTION_VALUE_OTHER");

  INextLineOColorTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_BLACK");
  INextLineOColorTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_WHITE");
  INextLineOColorTranslated[2] = ULanguage.Language.Translate("OPTION_VALUE_OTHER");

  IColorTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_BLUE");
  IColorTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_GREEN");
  IColorTranslated[2] = ULanguage.Language.Translate("OPTION_VALUE_PINK");
  IColorTranslated[3] = ULanguage.Language.Translate("OPTION_VALUE_RED");
  IColorTranslated[4] = ULanguage.Language.Translate("OPTION_VALUE_VIOLET");
  IColorTranslated[5] = ULanguage.Language.Translate("OPTION_VALUE_ORANGE");
  IColorTranslated[6] = ULanguage.Language.Translate("OPTION_VALUE_YELLOW");
  IColorTranslated[7] = ULanguage.Language.Translate("OPTION_VALUE_BROWN");
  IColorTranslated[8] = ULanguage.Language.Translate("OPTION_VALUE_BLACK");

  IPlayerColorTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_BLUE");
  IPlayerColorTranslated[1] = ULanguage.Language.Translate("OPTION_VALUE_RED");
  IPlayerColorTranslated[2] = ULanguage.Language.Translate("OPTION_VALUE_GREEN");
  IPlayerColorTranslated[3] = ULanguage.Language.Translate("OPTION_VALUE_YELLOW");
  IPlayerColorTranslated[4] = ULanguage.Language.Translate("OPTION_VALUE_ORANGE");
  IPlayerColorTranslated[5] = ULanguage.Language.Translate("OPTION_VALUE_PINK");
  IPlayerColorTranslated[6] = ULanguage.Language.Translate("OPTION_VALUE_VIOLET");
  IPlayerColorTranslated[7] = ULanguage.Language.Translate("OPTION_VALUE_BROWN");
  IPlayerColorTranslated[8] = ULanguage.Language.Translate("OPTION_VALUE_GRAY");
  IPlayerColorTranslated[9] = ULanguage.Language.Translate("OPTION_VALUE_DARKBLUE");
  IPlayerColorTranslated[10] = ULanguage.Language.Translate("OPTION_VALUE_SKY");
  IPlayerColorTranslated[11] = ULanguage.Language.Translate("OPTION_VALUE_CYAN");
  IPlayerColorTranslated[12] = ULanguage.Language.Translate("OPTION_VALUE_FLAME");
  IPlayerColorTranslated[13] = ULanguage.Language.Translate("OPTION_VALUE_ORCHID");
  IPlayerColorTranslated[14] = ULanguage.Language.Translate("OPTION_VALUE_HARLEQUIN");
  IPlayerColorTranslated[15] = ULanguage.Language.Translate("OPTION_VALUE_GREENYELLOW");

  // Advanced
  ILoadAnimationTranslated[0]         = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  ILoadAnimationTranslated[1]         = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IEffectSingTranslated[0]            = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IEffectSingTranslated[1]            = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IScreenFadeTranslated[0]            = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IScreenFadeTranslated[1]            = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IAskbeforeDelTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IAskbeforeDelTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IOnSongClickTranslated[0]           = ULanguage.Language.Translate("OPTION_VALUE_SING");
  IOnSongClickTranslated[1]           = ULanguage.Language.Translate("OPTION_VALUE_SELECT_PLAYERS");
  IOnSongClickTranslated[2]           = ULanguage.Language.Translate("OPTION_VALUE_OPEN_MENU");

  ILineBonusTranslated[0]             = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  ILineBonusTranslated[1]             = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IPartyPopupTranslated[0]            = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IPartyPopupTranslated[1]            = ULanguage.Language.Translate("OPTION_VALUE_ON");

  ISingScoresTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  ISingScoresTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_ON");

  ITopScoresTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_ALL");
  ITopScoresTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_PLAYER");

  IJoypadTranslated[0]                = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IJoypadTranslated[1]                = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IMouseTranslated[0]                 = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IMouseTranslated[1]                 = ULanguage.Language.Translate("OPTION_VALUE_SYSTEM_CURSOR");
  IMouseTranslated[2]                 = ULanguage.Language.Translate("OPTION_VALUE_GAME_CURSOR");

  IMouseTranslatedLegacy[0]           = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IMouseTranslatedLegacy[1]           = ULanguage.Language.Translate("OPTION_VALUE_HARDWARE_CURSOR");
  IMouseTranslatedLegacy[2]           = ULanguage.Language.Translate("OPTION_VALUE_SOFTWARE_CURSOR");

  ISingTimebarModeTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_CURRENT");
  ISingTimebarModeTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_REMAINING");
  ISingTimebarModeTranslated[2]          = ULanguage.Language.Translate("OPTION_VALUE_TOTAL");

  IJukeboxTimebarModeTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_CURRENT");
  IJukeboxTimebarModeTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_REMAINING");
  IJukeboxTimebarModeTranslated[2]          = ULanguage.Language.Translate("OPTION_VALUE_TOTAL");

  IAudioOutputBufferSizeTranslated[0] = ULanguage.Language.Translate("OPTION_VALUE_AUTO");
  IAudioOutputBufferSizeTranslated[1] = "256";
  IAudioOutputBufferSizeTranslated[2] = "512";
  IAudioOutputBufferSizeTranslated[3] = "1024";
  IAudioOutputBufferSizeTranslated[4] = "2048";
  IAudioOutputBufferSizeTranslated[5] = "4096";
  IAudioOutputBufferSizeTranslated[6] = "8192";
  IAudioOutputBufferSizeTranslated[7] = "16384";
  IAudioOutputBufferSizeTranslated[8] = "32768";
  IAudioOutputBufferSizeTranslated[9] = "65536";


  IAudioInputBufferSizeTranslated[0]  = ULanguage.Language.Translate("OPTION_VALUE_AUTO");
  IAudioInputBufferSizeTranslated[1]  = "256";
  IAudioInputBufferSizeTranslated[2]  = "512";
  IAudioInputBufferSizeTranslated[3]  = "1024";
  IAudioInputBufferSizeTranslated[4]  = "2048";
  IAudioInputBufferSizeTranslated[5]  = "4096";
  IAudioInputBufferSizeTranslated[6]  = "8192";
  IAudioInputBufferSizeTranslated[7]  = "16384";
  IAudioInputBufferSizeTranslated[8]  = "32768";
  IAudioInputBufferSizeTranslated[9]  = "65536";

  // Song Preview
  IPreviewVolumeTranslated[0]         = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IPreviewVolumeTranslated[1]         = "10%";
  IPreviewVolumeTranslated[2]         = "20%";
  IPreviewVolumeTranslated[3]         = "30%";
  IPreviewVolumeTranslated[4]         = "40%";
  IPreviewVolumeTranslated[5]         = "50%";
  IPreviewVolumeTranslated[6]         = "60%";
  IPreviewVolumeTranslated[7]         = "70%";
  IPreviewVolumeTranslated[8]         = "80%";
  IPreviewVolumeTranslated[9]         = "90%";
  IPreviewVolumeTranslated[10]        = "100%";


  IPreviewFadingTranslated[0]         =        ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IPreviewFadingTranslated[1]         = "1 " + ULanguage.Language.Translate("OPTION_VALUE_SEC");
  IPreviewFadingTranslated[2]         = "2 " + ULanguage.Language.Translate("OPTION_VALUE_SECS");
  IPreviewFadingTranslated[3]         = "3 " + ULanguage.Language.Translate("OPTION_VALUE_SECS");
  IPreviewFadingTranslated[4]         = "4 " + ULanguage.Language.Translate("OPTION_VALUE_SECS");
  IPreviewFadingTranslated[5]         = "5 " + ULanguage.Language.Translate("OPTION_VALUE_SECS");

  // Recording options
  IChannelPlayerTranslated[0]         = ULanguage.Language.Translate("SING_OPTIONS_RECORD_NOONE");
  for I=1 to IMaxPlayerCount do
  begin
    IChannelPlayerTranslated[I]       =std::to_string(I);
  end;

  IMicBoostTranslated[0]              = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IMicBoostTranslated[1]              = "+6dB";
  IMicBoostTranslated[2]              = "+12dB";
  IMicBoostTranslated[3]              = "+18dB";

  // Network
  IAutoModeTranslated[0]         = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IAutoModeTranslated[1]         = ULanguage.Language.Translate("OPTION_VALUE_SEND");
  IAutoModeTranslated[2]         = ULanguage.Language.Translate("OPTION_VALUE_SAVE");

  for I=0 to IMaxPlayerCount-1 do
  begin
    IAutoPlayerTranslated[I]       =ULanguage.Language.Translate("OPTION_PLAYER_" + std::to_string(I));
  end;
  IAutoPlayerTranslated[12]         = ULanguage.Language.Translate("OPTION_ALL_PLAYERS");

  // Webcam
  IWebcamFlipTranslated[0]          = ULanguage.Language.Translate("OPTION_VALUE_OFF");
  IWebcamFlipTranslated[1]          = ULanguage.Language.Translate("OPTION_VALUE_ON");

  IWebcamEffectTranslated[0] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_NORMAL");
  IWebcamEffectTranslated[1] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_GRAYSCALE");
  IWebcamEffectTranslated[2] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_BLACK_WHITE");
  IWebcamEffectTranslated[3] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_NEGATIVE");
  IWebcamEffectTranslated[4] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_BINARY_IMAGE");
  IWebcamEffectTranslated[5] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_DILATE");
  IWebcamEffectTranslated[6] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_THRESHOLD");
  IWebcamEffectTranslated[7] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_EDGES");
  IWebcamEffectTranslated[8] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_GAUSSIAN_BLUR");
  IWebcamEffectTranslated[9] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_EQUALIZED");
  IWebcamEffectTranslated[10] = ULanguage.Language.Translate("SING_OPTIONS_WEBCAM_EFFECT_ERODE");

  SetLength(IAutoScoreEasyTranslated, 10000);
  SetLength(IAutoScoreMediumTranslated, 10000);
  SetLength(IAutoScoreHardTranslated, 10000);

  for I = 0 to 9999 do
  begin
    case (I) of
      0..9 : Zeros = "000";
      10..99 : Zeros = "00";
      100..999 : Zeros = "0";
      1000..9999 : Zeros = "";
    end;

    IAutoScoreEasyTranslated[I]   = "+" + Zeros + std::to_string(I);
    IAutoScoreMediumTranslated[I] = "+" + Zeros + std::to_string(I);
    IAutoScoreHardTranslated[I]   = "+" + Zeros + std::to_string(I);
  end;
}

}