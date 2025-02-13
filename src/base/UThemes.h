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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/UThemes.pas $
 * $Id: UThemes.pas 3131 2015-09-07 00:11:32Z basisbit $
 */

//regex for converting types "([^\s()]+):\s*([^();]+)" "$2 $1"
//for == classes "([^ ]*) == class(?>\(([^)]*)\))?" "class $2 : public $1 {"
//for arrays "([^\s]+):\s*array\[([^\]]+)\] of ([^\s;]+)" "std::array<$3, $2> $1"
//for vectors "array of ([^\s]+)\s+([^\s;]+)" "std::vector<$1> $2"

#include "../switches.h"

#include <string>
#include <array>
#include <vector>
#include <memory>
#include <filesystem>
#include <cstdint>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "TextGL.h"
#include "UCommon.h"
#include "UIni.h"
#include "ULanguage.h"
#include "UPathUtils.h"
#include "UTexture.h"

namespace UThemes
{
/*uses
  IniFiles,
  SysUtils,
  Classes,
  UCommon,
  ULog,
  UIni,
  UTexture,
  UPath;*/
enum class TBackgroundType
{
    bgtNone, bgtColor, bgtTexture, bgtVideo, bgtFade, bgtAuto, Size
};

const std::array<std::string, static_cast<size_t>(TBackgroundType::Size)> BGT_Names =
{ "none", "color", "texture", "video", "fade", "auto" };

struct TThemeBackground
{
    TBackgroundType BGType;
    UCommon::TRGBA Color;
    std::string Tex;
};

template<typename T>
struct TThemeColor<T>
{
    T R;
    T G;
    T B;
};

//Defaul Background for Screens w/o Theme e.g. editor
const TThemeBackground DEFAULT_BACKGROUND = {
 TBackgroundType::bgtColor,
    { 1, 1, 1, 1 },
    "",
};

constexpr int OPTIONS_DESC_INDEX_BACK = 0;
constexpr int OPTIONS_DESC_INDEX_GAME = 1;
constexpr int OPTIONS_DESC_INDEX_GRAPHICS = 2;
constexpr int OPTIONS_DESC_INDEX_SOUND = 3;
constexpr int OPTIONS_DESC_INDEX_LYRICS = 4;
constexpr int OPTIONS_DESC_INDEX_THEMES = 5;
constexpr int OPTIONS_DESC_INDEX_RECORD = 6;
constexpr int OPTIONS_DESC_INDEX_ADVANCED = 7;
constexpr int OPTIONS_DESC_INDEX_NETWORK = 8;
constexpr int OPTIONS_DESC_INDEX_WEBCAM = 9;
constexpr int OPTIONS_DESC_INDEX_JUKEBOX = 10;
constexpr int OPTIONS_DESC_INDEX_INPUT = 11;

struct TThemePosition
{
    int X;
    int Y;
    int H;
    int W;
};

struct Reflection
{
    bool active;
    double spacing;
};

struct TThemeStatic
{
    std::string Color;
    std::string Tex;

    TThemePosition pos;
    double Z;
    UCommon::TRGBA Col;
    UTexture::TTextureType Typ;
    UCommon::TTexCoords TexC;
    //Reflection    TODO::check if Text only!
    Reflection reflection;
};
typedef std::vector<TThemeStatic> AThemeStatic;

struct TThemeText
{
    TThemePosition pos;
    double Z;
    std::string Color;
    std::string DColor;
    UCommon::TRGB<double> Col;
    UCommon::TRGB<double> DCol;
    int Font;
    int Style;
    int Size;
    int Align;
    std::string Text;
    bool Writable; // true -> add a blink char (|) at the }
    //Reflection
    bool Reflection;
    double ReflectionSpacing;
};
typedef std::vector<TThemeText> AThemeText;

struct TThemeButton
{
    AThemeText Text;
    TThemePosition pos;
    double Z;
    std::string Color;
    UCommon::TRGB<double> Col;
    double Int;
    std::string DColor;
    UCommon::TRGB<double> DCol;
    double DInt;
    std::string Tex;
    UTexture::TTextureType Typ;

    bool Visible;

    //Reflection Mod
    bool Reflection;
    double Reflectionspacing;
    //Fade Mod
    int SelectH;
    int SelectW;
    bool Fade;
    bool FadeText;
    double DeSelectReflectionspacing;
    std::string FadeTex;
    int FadeTexPos;

    //Button Collection Mod
    uint8_t Parent; //Number of the Button Collection this Button is assigned to. IF 0: No Assignement
};

//Button Collection Mod
struct TThemeButtonCollection
{
    TThemeButton Style;
    uint8_t ChildCount; //No of assigned Childs
    uint8_t FirstChild; //No of Child on whose Interaction Position the Button should be
};
typedef std::vector<TThemeButtonCollection> AThemeButtonCollection;
typedef std::shared_ptr<AThemeButtonCollection> PAThemeButtonCollection;

struct TThemeSelectSlide
{
    std::string Tex;
    UTexture::TTextureType Typ;
    std::string TexSBG;
    UTexture::TTextureType TypSBG;
    TThemePosition pos;
    double Z;
    int SBGW;

    int TextSize;

    bool showArrows;
    bool oneItemOnly;

    std::string Text;
    UCommon::TRGB<double> Col; double Int;
    UCommon::TRGB<double> DCol; double DInt;
    UCommon::TRGB<double> TCol; double TInt;
    UCommon::TRGB<double> SBGCol; double SBGInt;
    UCommon::TRGB<double> SBGDCol; double SBGDInt;
    UCommon::TRGB<double> STCol; double STInt;
    UCommon::TRGB<double> STDCol; double STDInt;
    int SkipX;
};

struct TThemeEqualizer
{
    bool Visible;
    bool Direction;
    TThemePosition pos;
    double Z;
    int Space;
    int Bands;
    int Length;
    UCommon::TRGBA Col;
    bool Reflection;
    double Reflectionspacing;
};

struct TThemeBasic
{
    TThemeBackground Background;
    AThemeText Text;
    AThemeStatic Statics;

    //Button Collection Mod
    AThemeButtonCollection ButtonCollection;
};

typedef std::shared_ptr<TThemeBasic> PThemeBasic;

using TThemeBox = TThemePosition;

struct TThemeLoading : public TThemeBasic
{
    TThemeStatic StaticAnimation;
    TThemeText TextLoading;
};

struct TThemeMain : public TThemeBasic
{
    TThemeButton ButtonSolo;
    TThemeButton ButtonMulti;
    TThemeButton ButtonJukebox;
    TThemeButton ButtonStat;
    TThemeButton ButtonEditor;
    TThemeButton ButtonOptions;
    TThemeButton ButtonAbout;
    TThemeButton ButtonExit;

    TThemeText TextDescription;
    TThemeText TextDescriptionLong;
    std::array<std::string, 8> Description;
    std::array<std::string, 8> DescriptionLong;
};

struct TThemeName : public TThemeBasic
{
    TThemeButton PlayerButtonName;
    TThemeButton PlayerButtonAvatar;

    struct
	{
        int NumAvatars;
        int DistanceAvatars;
    } PlayerScrollAvatar;

    TThemeButton PlayerAvatar;

    std::array<TThemeStatic, UIni::IMaxPlayerCount> PlayerSelect;
    std::array<TThemeText, UIni::IMaxPlayerCount> PlayerSelectText;
    std::array<TThemeStatic, UIni::IMaxPlayerCount> PlayerSelectAvatar;
    TThemeButton PlayerSelectCurrent;

    TThemeSelectSlide SelectPlayersCount;
    TThemeSelectSlide SelectPlayerColor;
    TThemeSelectSlide SelectPlayerLevel;
};

struct TThemeLevel : public TThemeBasic
{
    std::array<UIni::Difficulty, static_cast<size_t>(UIni::Difficulty::Size)> Button;
};

struct TThemeSong : public TThemeBasic
{
    TThemeText TextArtist;
    TThemeText TextTitle;
    TThemeText TextNumber;
    TThemeText TextYear;

    int TextMedleyMax;

    std::vector<TThemeText> TextArtistMedley;
    std::vector<TThemeText> TextTitleMedley;
    std::vector<TThemeStatic> StaticMedley;
    std::vector<TThemeText> TextNumberMedley;

    //Video Icon Mod
    TThemeStatic VideoIcon;

    //Medley Icons
    TThemeStatic MedleyIcon;
    TThemeStatic CalculatedMedleyIcon;

    //Duet Icon
    TThemeStatic DuetIcon;

    //Rap Icon
    TThemeStatic RapIcon;

    //Show Cat in TopLeft Mod
    TThemeText TextCat;
    TThemeStatic StaticCat;

    //Cover Mod
    struct
    {
        bool Reflections;
        TThemePosition pos;
        int Z;
        int Rows;
        int Cols;
        int Padding;
        TThemePosition Select;
        bool SelectReflection;
        int SelectReflectionSpacing;
        int ZoomThumbW;
        int ZoomThumbH;
        int ZoomThumbStyle;
        std::string Tex;
    } Cover;

    //Equalizer Mod
    TThemeEqualizer Equalizer;

    //List Song Mod
    struct
    {
        TThemePosition pos;
        int Z;
        int Rows;
        int Padding;
        bool Reflection;
        int ReflectionSpacing;
        UTexture::TTextureType Typ;
        std::string Tex;
        std::string DTex;
        std::string Color;
        std::string DColor;
        UCommon::TRGB<double> Col;
        UCommon::TRGB<double> DCol;
    } ListCover;


    //Party and Non Party specific Statics and Texts
    AThemeStatic StaticParty;
    AThemeText TextParty;

    AThemeStatic StaticNonParty;
    AThemeText TextNonParty;

    TThemeText InfoMessageText;
    TThemeStatic InfoMessageBG;

    //Screen Song Scores
    TThemeText TextScore;

    TThemeText TextMaxScore;
    TThemeText TextMediaScore;

    TThemeText TextMaxScore2;
    TThemeText TextMediaScore2;

    TThemeText TextScoreUser;
    TThemeText TextScoreUserLocal;

    TThemeText TextMaxScoreLocal;
    TThemeText TextMediaScoreLocal;

    //Party Mode
    std::array<std::array<TThemeStatic, 5>, UIni::IMaxTeamCount> StaticTeamJoker;

    TThemeText TextPartyTime;

    std::array<TThemeStatic, 2> Static2PlayersDuetSinger;
    std::array<TThemeStatic, 3> Static3PlayersDuetSinger;

    std::array<TThemeStatic, 2> Static4PlayersDuetSinger;
    std::array<TThemeStatic, 3> Static6PlayersDuetSinger;

    std::array<TThemeText, 2> Text2PlayersDuetSinger;
    std::array<TThemeText, 3> Text3PlayersDuetSinger;
};

struct SingMode
{
    TThemeStatic Static;
    TThemeStatic StaticAvatar;
    TThemeStatic StaticScoreBG;
    TThemeText Text;
    TThemeText TextScore;
    TThemeStatic StaticSingBar;
    TThemePosition SingOscilloscope;
};

struct TThemeSing : public TThemeBasic
{
    TThemeStatic StaticLyricsBar;
    TThemeStatic StaticLyricsBarDuet;

    //TimeBar mod
    TThemeStatic StaticTimeBar;
    TThemeStatic StaticTimeProgress;
    TThemeText TextTimeLabelText;
    TThemeText TextTimeText;
    //eoa TimeBar mod

    SingMode P1; //added for ps3 skin
    SingMode P1TwoP; //game in 2/4 player modi
    SingMode P1ThreeP; //game in 3/6 player modi

    SingMode P2R;
    SingMode P2M;
    SingMode P3R;

    SingMode DuetP1ThreeP;
    SingMode DuetP2M;
    SingMode DuetP3R;

    //game in 4/6 player modi in 1 Screen
    std::array<SingMode, 4> FourP;
    std::array<SingMode, 6> SixP;

    // duet 4/6 players in one screen
    std::array<SingMode, 4> DuetFourP;
    std::array<SingMode, 6> DuetSixP;

    TThemeStatic StaticSongName;
    TThemeText TextSongName;

    //Linebonus Translations
    std::array<std::string, 9> LineBonusText;

    //Pause Popup
    TThemeStatic PausePopUp;

    TThemeText InfoMessageText;
    TThemeStatic InfoMessageBG;
};

struct TThemeJukebox : public TThemeBasic
{
    TThemeStatic StaticTimeProgress;
    TThemeStatic StaticTimeBackground;
    TThemeStatic StaticSongBackground;
    TThemeStatic StaticSongListBackground;
    TThemeText TextTimeText;
    TThemeText TextSongText;
    TThemeButton SongDescription;
    TThemeButton FindSong;
    TThemeButton RepeatSongList;
    TThemeButton SongListOrder;
    TThemeButton RandomSongList;
    TThemeButton Lyric;
    TThemeButton Options;
    TThemeButton SongListClose;
    TThemeButton SongListFixPin;
    TThemeText TextListText;
    TThemeText TextCountText;
    TThemeStatic SongCover;
    TThemeButton SongListPlayPause;

    AThemeStatic StaticActualSongStatics;
    TThemeStatic StaticActualSongCover;
    TThemeText TextActualSongArtist;
    TThemeText TextActualSongTitle;

    TThemeButton SongListUp;
    TThemeButton SongListDown;

    //Jukebox SongMenu
    TThemeStatic StaticSongMenuBackground;
    TThemeButton SongMenuPlayPause;
    TThemeStatic StaticSongMenuTimeProgress;
    TThemeStatic StaticSongMenuTimeBackground;
    TThemeButton SongMenuNext;
    TThemeButton SongMenuPrevious;
    TThemeButton SongMenuPlaylist;
    TThemeText SongMenuTextTime;
    TThemeButton SongMenuOptions;

    //Jukebox SongOptions
    TThemeText SongOptionsTextSaved;
    TThemeStatic StaticSongOptionsBackground;
    TThemeButton SongOptionsClose;
    TThemeButton SongOptionsSave;
    TThemeButton SongOptionsDefault;
    TThemeText SongOptionsVideoText;
    TThemeText SongOptionsLyricText;
    TThemeSelectSlide SongOptionsVideoAspectSlide;
    TThemeSelectSlide SongOptionsVideoWidthSlide;
    TThemeSelectSlide SongOptionsVideoHeightSlide;
    TThemeSelectSlide SongOptionsLyricSizeSlide;
    TThemeSelectSlide SongOptionsLyricPositionSlide;
    TThemeSelectSlide SongOptionsLyricColorSlide;
    TThemeSelectSlide SongOptionsLyricLineSlide;
    TThemeSelectSlide SongOptionsLyricPropertySlide;
    TThemeSelectSlide SongOptionsLyricAlphaSlide;
    TThemeColor<TThemeSelectSlide> Select;
    TThemeColor<TThemeStatic> Tex;
    TThemeColor<TThemeStatic> Pointer;
};

struct TThemeJukeboxPlaylist : public TThemeBasic
{
    TThemeSelectSlide SelectPlayList;
    TThemeSelectSlide SelectPlayList2;
};

struct TThemeLyricBar
{
    int IndicatorYOffset, UpperX, UpperW, UpperY, UpperH,
        LowerX, LowerW, LowerY, LowerH;
};

struct TThemePlayerScore
{
    AThemeStatic PlayerStatic;
    AThemeText PlayerTexts;

    TThemeText TextName;
    TThemeText TextScore;

    TThemeStatic AvatarStatic;

    TThemeText TextNotes;
    TThemeText TextNotesScore;
    TThemeText TextLineBonus;
    TThemeText TextLineBonusScore;
    TThemeText TextGoldenNotes;
    TThemeText TextGoldenNotesScore;
    TThemeText TextTotal;
    TThemeText TextTotalScore;

    TThemeStatic StaticBoxLightest;
    TThemeStatic StaticBoxLight;
    TThemeStatic StaticBoxDark;

    TThemeStatic StaticRatings;

    TThemeStatic StaticBackLevel;
    TThemeStatic StaticBackLevelRound;
    TThemeStatic StaticLevel;
    TThemeStatic StaticLevelRound;

    TThemeButton ButtonSend;
};

struct TThemeScore : public TThemeBasic
{
    TThemeText TextArtist;
    TThemeText TextTitle;

    TThemeText TextArtistTitle;

    std::array<TThemePlayerScore, UIni::IMaxPlayerCount> PlayerScore;

    TThemeStatic StaticNavigate;
    TThemeText TextNavigate;
};

struct TThemeTop5 : public TThemeBasic
{
    TThemeText TextLevel;
    TThemeText TextArtistTitle;

    AThemeStatic StaticNumber;
    AThemeText TextNumber;
    AThemeText TextName;
    AThemeText TextScore;
    AThemeText TextDate;
};

struct TThemeOptions : public TThemeBasic
{
    TThemeButton ButtonGame;
    TThemeButton ButtonGraphics;
    TThemeButton ButtonSound;
    TThemeButton ButtonInput;
    TThemeButton ButtonLyrics;
    TThemeButton ButtonThemes;
    TThemeButton ButtonRecord;
    TThemeButton ButtonAdvanced;
    TThemeButton ButtonNetwork;
    TThemeButton ButtonWebcam;
    TThemeButton ButtonJukebox;
    TThemeButton ButtonExit;

    TThemeText TextDescription;
    std::array<std::string, 12> Description;
};

struct TThemeOptionsGame : public TThemeBasic
{
    TThemeSelectSlide SelectLanguage;
    TThemeSelectSlide SelectSongMenu;
    TThemeSelectSlide SelectSorting;
    TThemeSelectSlide SelectTabs;
    TThemeSelectSlide SelectShowScores;
    TThemeSelectSlide SelectDebug;
    TThemeSelectSlide SelectAVDelay;
    TThemeSelectSlide SelectMicDelay;
    TThemeButton ButtonExit;
};

struct TThemeOptionsGraphics : public TThemeBasic
{
    TThemeSelectSlide SelectFullscreen;
	TThemeSelectSlide SelectResolution;
	TThemeSelectSlide SelectDepth;
	TThemeSelectSlide SelectVisualizer;
	TThemeSelectSlide SelectOscilloscope;
	TThemeSelectSlide SelectLineBonus;
	TThemeSelectSlide SelectMovieSize;
	TThemeButton ButtonExit;
};

struct TThemeOptionsSound : public TThemeBasic
{
    TThemeSelectSlide SelectBackgroundMusic;
    TThemeSelectSlide SelectClickAssist;
    TThemeSelectSlide SelectBeatClick;
    TThemeSelectSlide SelectSlidePreviewVolume;
    TThemeSelectSlide SelectSlidePreviewFading;
    TThemeSelectSlide SelectSlideVoicePassthrough;
    TThemeSelectSlide SelectSlideMusicAutoGain;
    TThemeButton ButtonExit;
};

struct TThemeOptionsInput : public TThemeBasic
{
    TThemeSelectSlide SelectMouse;
    TThemeSelectSlide SelectJoypad;
    TThemeButton ButtonExit;
};

struct TThemeOptionsLyrics : public TThemeBasic
{
    TThemeSelectSlide SelectLyricsFont;
    TThemeSelectSlide SelectLyricsStyle;
    TThemeSelectSlide SelectLyricsEffect;
    //    TThemeSelectSlide SelectSolmization;
    TThemeSelectSlide SelectNoteLines;
    TThemeButton ButtonExit;
    int UpperX, UpperW, UpperY, UpperH,
        LowerX, LowerW, LowerY, LowerH;
};

struct TThemeOptionsThemes : public TThemeBasic
{
    TThemeSelectSlide SelectTheme;
    TThemeSelectSlide SelectSkin;
    TThemeSelectSlide SelectColor;
    TThemeButton ButtonExit;
};

struct TThemeOptionsRecord : public TThemeBasic
{
    TThemeSelectSlide SelectSlideCard;
    TThemeSelectSlide SelectSlideInput;
    TThemeSelectSlide SelectChannel;
    TThemeSelectSlide SelectAssignee;
    TThemeSelectSlide SelectThreshold;
    TThemeSelectSlide SelectMicBoost;
    TThemeButton ButtonExit;
};

struct TThemeOptionsAdvanced : public TThemeBasic
{
    TThemeSelectSlide SelectLoadAnimation;
    TThemeSelectSlide SelectEffectSing;
    TThemeSelectSlide SelectScreenFade;
    TThemeSelectSlide SelectLineBonus;
    TThemeSelectSlide SelectAskbeforeDel;
    TThemeSelectSlide SelectOnSongClick;
    TThemeSelectSlide SelectPartyPopup;
    TThemeSelectSlide SelectSingScores;
    TThemeSelectSlide SelectTopScores;
    TThemeButton ButtonExit;
};

struct TThemeOptionsNetwork : public TThemeBasic
{
    TThemeSelectSlide SelectWebsite;
    TThemeSelectSlide SelectUsername;
    TThemeSelectSlide SelectSendName;
    TThemeSelectSlide SelectAutoMode;
    TThemeSelectSlide SelectAutoPlayer;

    std::array<TThemeSelectSlide, static_cast<size_t>(UIni::Difficulty::Size)> SelectAutoScoreDifficulty;
    TThemeText TextInsertUser;
    TThemeButton ButtonInsert;
    TThemeButton ButtonExit;
};

struct TThemeOptionsWebcam : public TThemeBasic
{
    TThemeSelectSlide SelectWebcam;
    TThemeSelectSlide SelectResolution;
    TThemeSelectSlide SelectFPS;
    TThemeSelectSlide SelectFlip;
    TThemeSelectSlide SelectBrightness;
    TThemeSelectSlide SelectSaturation;
    TThemeSelectSlide SelectHue;
    TThemeSelectSlide SelectEffect;

    TThemeButton ButtonPreVisualization;
    TThemeButton ButtonExit;
};

struct TThemeOptionsJukebox : public TThemeBasic
{
    TThemeSelectSlide SelectLyricsFont;
    TThemeSelectSlide SelectLyricsStyle;
    TThemeSelectSlide SelectLyricsEffect;
    TThemeSelectSlide SelectLyricsAlpha;
    TThemeSelectSlide SelectLine;
    TThemeSelectSlide SelectProperty;
    TThemeSelectSlide SelectColor;
    TThemeColor<TThemeSelectSlide> Select;
    TThemeColor<TThemeStatic> Tex;
    TThemeStatic TexColor;
    TThemeColor<TThemeStatic> Pointer;
    TThemeButton ButtonExit;
    int UpperX, UpperW, UpperY, UpperH,
        LowerX, LowerW, LowerY, LowerH;
};

struct TThemeEdit : public TThemeBasic
{
    TThemeButton ButtonConvert;
    TThemeButton ButtonExit;

    TThemeText TextDescription;
    TThemeText TextDescriptionLong;
	std::array<std::string, 6> Description;
	std::array<std::string, 6> DescriptionLong;
};

struct TThemeEditConvert : public TThemeBasic
{
    TThemeButton ButtonOpen;
    TThemeButton ButtonPlay;
    TThemeButton ButtonPlaySelected;
    TThemeButton ButtonStop;
    TThemeButton ButtonSave;

    TThemeText TextDescription;
    TThemeText TextDescriptionLong;
	std::array<std::string, 6> Description;
	std::array<std::string, 6> DescriptionLong;
};

struct TThemeEditOpen: public TThemeBasic
{
    TThemeButton ButtonFileName;
    TThemeButton ButtonLoad;
    TThemeButton ButtonBack;

    TThemeText TextDescription;
    TThemeText TextDescriptionLong;
	std::array<std::string, 6> Description;
	std::array<std::string, 6> DescriptionLong;
};

struct TThemeEditSub: public TThemeBasic
{
    // statics
    TThemeStatic BackgroundImage;
    TThemeStatic HeaderBackground;
    TThemeStatic CurrentNoteInfoBackground;
    TThemeStatic VolumeSliderBackground;
    TThemeStatic NotesBackground;
    TThemeStatic P1InfoBarBackground;
    TThemeStatic P2InfoBarBackground;
    TThemeStatic SentenceBackground;

    // buttons
    TThemeButton ButtonCurrentLine;
    TThemeButton ButtonCurrentNote;
    TThemeButton PlayOnly;
    TThemeButton PlayWithNote;
    TThemeButton PlayNote;
    TThemeButton previousseq;
    TThemeButton nextseq;
    TThemeButton undo;
    TThemeButton gold;
    TThemeButton freestyle;

    // sliders
    TThemeSelectSlide SlideTitle;
    TThemeSelectSlide SlideArtist;
    TThemeSelectSlide SlideLanguage;
    TThemeSelectSlide SlideEdition;
    TThemeSelectSlide SlideGenre;
    TThemeSelectSlide SlideYear;
    TThemeSelectSlide SlideCreator;
    TThemeSelectSlide SlideMP3;
    TThemeSelectSlide SlideCover;
    TThemeSelectSlide SlideBackground;
    TThemeSelectSlide SlideVideo;
    TThemeSelectSlide SlideBPM;
    TThemeSelectSlide SlideGAP;
    TThemeSelectSlide SlideStartTag;
    TThemeSelectSlide SlideEndTag;
    TThemeSelectSlide SlideMedleyStart;
    TThemeSelectSlide SlideMedleyEnd;
    TThemeSelectSlide SlidePreviewStart;
    TThemeSelectSlide SlideRelative;
    TThemeSelectSlide SlideStart;
    TThemeSelectSlide SlideDuration;
    TThemeSelectSlide SlideTone;
    TThemeSelectSlide SlideLyric;
    TThemeSelectSlide SelectVolAudio;
    TThemeSelectSlide SelectVolMidi;
    TThemeSelectSlide SelectVolClick;
    TThemeSelectSlide SlideVideoGap;

    // texts
    TThemeText TextInfo;
    TThemeText TextSentence;
    TThemeText TextCurrentTone;
};

  //Error- and Check-Popup
struct TThemeError: public TThemeBasic
{
    TThemeButton Button1;
    TThemeText TextError;
};

struct TThemeCheck: public TThemeBasic
{
    TThemeButton Button1;
    TThemeButton Button2;
    TThemeText TextCheck;
};

  //Help-Popup
struct TThemeHelp: public TThemeBasic
{
    TThemeButton Button1;
};

struct TThemeInsertUser: public TThemeBasic
{
    TThemeText TextInsertUser;
    TThemeButton ButtonUsername;
    TThemeButton ButtonPassword;
    TThemeButton Button1;
    TThemeButton Button2;
};

struct TThemeSendScore: public TThemeBasic
{
    TThemeSelectSlide SelectSlide1;
    TThemeSelectSlide SelectSlide2;
    TThemeSelectSlide SelectSlide3;
    TThemeButton ButtonUsername;
    TThemeButton ButtonPassword;
    TThemeButton Button1;
    TThemeButton Button2;
};

struct TThemeScoreDownload: public TThemeBasic
{
    TThemeButton Button1;
    TThemeText TextSongScoreDownload;
    TThemeText TextWebScoreDownload;
    TThemeStatic DownloadProgressSong;
    TThemeStatic DownloadProgressWeb;
};

  //ScreenSong Menu
struct TThemeSongMenu: public TThemeBasic
{
    TThemeButton Button1;
    TThemeButton Button2;
    TThemeButton Button3;
    TThemeButton Button4;
    TThemeButton Button5;

    TThemeSelectSlide SelectSlide1;
    TThemeSelectSlide SelectSlide2;
    TThemeSelectSlide SelectSlide3;

    TThemeText TextMenu;
};

struct TThemeSongJumpTo: public TThemeBasic
{
    TThemeButton ButtonSearchText;
    TThemeSelectSlide SelectSlideType;
    TThemeText TextFound;

    //Translated Texts
    std::string Songsfound;
    std::string NoSongsfound;
    std::string CatText;
	std::array<std::string, 8> IType;
};

struct PartyRound
{
    TThemeText TextRound;
    TThemeText TextWinner;
    TThemeStatic StaticRound;
};
/*
struct PartyTeam
{
    TThemeText TextScoreTeam1;
    TThemeText TextNameTeam1;
    TThemeStatic StaticTeam1;
};

struct PartyPlayer
{
    TThemeText TextNextPlayer1;
    TThemeText TextTeam1Players;
    TThemeStatic StaticNextPlayer1;
};
*/

struct PartyEntity
{
    TThemeText TextNextPlayer;
    TThemeText TextScoreTeam;
    TThemeText TextNameTeam;
    TThemeText TextTeamPlayers;
    TThemeStatic StaticTeam;
    TThemeStatic StaticNextPlayer;
};

  //Party Screens
struct TThemePartyNewRound: public TThemeBasic
{
    std::array<PartyRound, 7> Rounds;
    std::array<PartyEntity, UIni::IMaxTeamCount> Entity;

    TThemeText TextNextRound;
    TThemeText TextNextRoundNo;
};

struct TeamPartyScore
{
    TThemeText TextScore;
    TThemeText TextName;
    TThemeStatic Static;
    TThemeStatic StaticBG;
    TThemeStatic StaticDeco;
};

struct TThemePartyScore: public TThemeBasic
{
    std::array<TeamPartyScore, UIni::IMaxTeamCount> Team;

    struct 
    {
        bool ChangeTextures;

        std::string FirstTexture;
        UTexture::TTextureType FirstTyp;
        std::string FirstColor;

        std::string SecondTexture;
        UTexture::TTextureType SecondTyp;
        std::string SecondColor;

        std::string ThirdTexture;
        UTexture::TTextureType ThirdTyp;
        std::string ThirdColor;
    } DecoTextures;
	TThemeText TextWinner;
};

struct TThemePartyWin: public TThemeBasic
{
    std::array<TThemeText, UIni::IMaxTeamCount> TextScoreTeam;
    std::array<TThemeText, UIni::IMaxTeamCount> TextNameTeam;
    std::array<TThemeStatic, UIni::IMaxTeamCount> StaticTeam;
    std::array<TThemeStatic, UIni::IMaxTeamCount> StaticTeamBG;
    std::array<TThemeStatic, UIni::IMaxTeamCount> StaticTeamDeco;

    TThemeText TextWinner;
};

struct TThemePartyOptions: public TThemeBasic
{
    TThemeSelectSlide SelectMode;
    TThemeSelectSlide SelectLevel;
    TThemeSelectSlide SelectPlayList;
    TThemeSelectSlide SelectPlayList2;

    /*{ButtonNext: TThemeButton;
    ButtonPrev: TThemeButton;}*/
};

struct TThemePartyPlayer: public TThemeBasic
{
    std::array<TThemeButton, UIni::IMaxPlayerCount> PlayerName;
    std::array<TThemeSelectSlide, UIni::IMaxTeamCount> SelectPlayers;

    TThemeSelectSlide SelectTeams;

    TThemeButton Team1Name;
    TThemeButton Team2Name;
    TThemeButton Team3Name;

    /*TThemeButton{ ButtonNext;
    TThemeButton ButtonPrev; }*/
};

struct TThemePartyRounds: public TThemeBasic
{
    TThemeSelectSlide SelectRoundCount;
	std::array<TThemeSelectSlide, 7> SelectRound;
};

struct TThemePartyTournamentPlayer: public TThemeBasic
{
    TThemeSelectSlide SelectPlayers;

    std::array<TThemeButton, UIni::IMaxPlayerCount> PlayerName;
};

struct TThemePartyTournamentOptions: public TThemeBasic
{
    TThemeSelectSlide SelectRoundsFinal;
    TThemeSelectSlide SelectRounds2Final;
    TThemeSelectSlide SelectRounds4Final;
    TThemeSelectSlide SelectRounds8Final;
};

struct TThemePartyTournamentRounds: public TThemeBasic
{
	std::array<std::array<TThemeButton, 8>, 2> TextNamePlayer;
    TThemeText TextWinner;
    TThemeText TextResult;
    TThemeText NextPlayers;
};

struct PlayerPartyTournamentWin
{
    TThemeText TextScore;
    TThemeText TextName;
    TThemeStatic StaticBGP;
};

struct TThemePartyTournamentWin: public TThemeBasic
{
    std::array<PlayerPartyTournamentWin, 2> Player;
};

  //About
struct TThemeAboutMain: public TThemeBasic
{
    TThemeButton ButtonCredits;
    TThemeButton ButtonExit;

    TThemeText TextOverview;
};

  //Stats Screens
struct TThemeStatMain: public TThemeBasic
{
    TThemeButton ButtonScores;
    TThemeButton ButtonSingers;
    TThemeButton ButtonSongs;
    TThemeButton ButtonBands;
    TThemeButton ButtonExit;

    TThemeText TextOverview;
};

struct TThemeStatDetail: public TThemeBasic
{
    TThemeButton ButtonNext;
    TThemeButton ButtonPrev;
    TThemeButton ButtonReverse;
    TThemeButton ButtonExit;

    TThemeText TextDescription;
    TThemeText TextPage;
    AThemeText TextList;

    std::array<std::string, 4> Description;
    std::array<std::string, 4> DescriptionR;
    std::array<std::string, 4> FormatStr;
    std::string PageStr;
};

  //Playlist Translations
struct TThemePlaylist
{
	std::string CatText;
};

struct TThemeEntry
{
    std::string Name;
    std::filesystem::path Filename;
    int DefaultSkin;
    std::string Creator;
};

class TTheme
{
private:
#ifdef THEMESAVE
    TIniFile ThemeIni;
#else
    TMemIniFile ThemeIni;
#endif

    TThemeBasic LastThemeBasic;
    void CreateThemeObjects();
    void LoadHeader(std::filesystem::path FileName);
public:
    std::vector<TThemeEntry> Themes;
    TThemeLoading Loading;
    TThemeMain Main;
    TThemeName Name;
    TThemeLevel Level;
    TThemeSong Song;
    TThemeSing Sing;
    TThemeLyricBar LyricBar;
    TThemeLyricBar LyricBarDuetP1;
    TThemeLyricBar LyricBarDuetP2;
    TThemeLyricBar LyricBarJukebox;
    TThemeJukebox Jukebox;
    TThemeJukeboxPlaylist JukeboxPlaylist;
    TThemeScore Score;
    TThemeTop5 Top5;
    TThemeOptions Options;
    TThemeOptionsGame OptionsGame;
    TThemeOptionsGraphics OptionsGraphics;
    TThemeOptionsSound OptionsSound;
    TThemeOptionsInput OptionsInput;
    TThemeOptionsLyrics OptionsLyrics;
    TThemeOptionsThemes OptionsThemes;
    TThemeOptionsRecord OptionsRecord;
    TThemeOptionsAdvanced OptionsAdvanced;
    TThemeOptionsNetwork OptionsNetwork;
    TThemeOptionsWebcam OptionsWebcam;
    TThemeOptionsJukebox OptionsJukebox;
    //edit
    TThemeEdit Edit;
    TThemeEditConvert EditConvert;
    TThemeEditOpen EditOpen;
    TThemeEditSub EditSub;
    //error and check popup
    TThemeError ErrorPopup;
    TThemeCheck CheckPopup;
    TThemeInsertUser InsertUserPopup;
    TThemeSendScore SendScorePopup;
    TThemeScoreDownload ScoreDownloadPopup;
    //help popup
    TThemeHelp HelpPopup;
    //ScreenSong extensions
    TThemeSongMenu SongMenu;
    TThemeSongJumpTo SongJumpto;
    //Party Screens
    TThemePartyNewRound PartyNewRound;
    TThemePartyScore PartyScore;
    TThemePartyWin PartyWin;
    TThemePartyOptions PartyOptions;
    TThemePartyPlayer PartyPlayer;
    TThemePartyRounds PartyRounds;

    //Tournament
    TThemePartyTournamentPlayer PartyTournamentPlayer;
    TThemePartyTournamentOptions PartyTournamentOptions;
    TThemePartyTournamentRounds PartyTournamentRounds;
    TThemePartyTournamentWin PartyTournamentWin;

    // About
    TThemeAboutMain AboutMain;

    //Stats Screens
    TThemeStatMain StatMain;
    TThemeStatDetail StatDetail;

    TThemePlaylist Playlist;

    std::array<std::string, static_cast<size_t>(UIni::Difficulty::Size)> ILevel;
    std::array<std::string, static_cast<size_t>(UIni::PartyMode::Size)> IMode;

    TTheme();

    void LoadList();

    bool LoadTheme(int ThemeNum, int sColor); // Load some theme settings from file

    void LoadColors();

    void ThemeLoadBasic(TThemeBasic& Theme, const std::string Name);
    void ThemeLoadBackground(TThemeBackground& ThemeBackground, const std::string Name);
    void ThemeLoadText(TThemeText& ThemeText, const std::string Name);
    void ThemeLoadTexts(AThemeText& ThemeText, const std::string Name);
    void ThemeLoadStatic(TThemeStatic& ThemeStatic, const std::string Name);
    void ThemeLoadStatics(AThemeStatic& ThemeStatic, const std::string Name);
    void ThemeLoadButton(TThemeButton& ThemeButton, const std::string Name, PAThemeButtonCollection Collections = nullptr);
    void ThemeLoadButtonCollection(TThemeButtonCollection& Collection, const std::string Name);
    void ThemeLoadButtonCollections(AThemeButtonCollection& Collections, const std::string Name);
    void ThemeLoadSelectSlide(TThemeSelectSlide& ThemeSelectS, const std::string Name);
    void ThemeLoadEqualizer(TThemeEqualizer& ThemeEqualizer, const std::string Name);
    void ThemeLoadPosition(TThemePosition& ThemePosition, const std::string Name);

    void ThemeSave(const std::string FileName);
    void ThemeSaveBasic(const TThemeBasic& Theme, const std::string Name);
    void ThemeSaveBackground(const TThemeBackground& ThemeBackground, const std::string Name);
    void ThemeSaveStatic(const TThemeStatic& ThemeStatic, const std::string Name);
    void ThemeSaveStatics(const AThemeStatic& ThemeStatic, const std::string Name);
    void ThemeSaveText(const TThemeText& ThemeText, const std::string Name);
    void ThemeSaveTexts(const AThemeText& ThemeText, const std::string Name);
    void ThemeSaveButton(const TThemeButton& ThemeButton, const std::string Name);

    void ThemeScoreLoad();
    void ThemePartyLoad();
    void ThemeSongLoad();
};

struct TColor
{
	std::string Name;
	UCommon::TRGB<double> RGB;
};

void glColorRGB(UCommon::TRGB<double> Color);
void glColorRGB(UCommon::TRGB<double> Color, double Alpha);
void glColorRGB(UCommon::TRGBA Color);
void glColorRGB(UCommon::TRGBA Color, double Alpha);

int ColorExists(std::string Name);
void LoadColor(double& R, double& G, double& B, std::string ColorName);
void LoadColor(UCommon::TRGB<double>& Col, std::string ColorName)
{
    LoadColor(Col.R, Col.G, Col.B, ColorName);
}
UCommon::TRGB<double> GetSystemColor(int Color);
//UCommon::TRGB<double> ColorSqrt(UCommon::TRGB<double> RGB);

UCommon::TRGB<double> GetJukeboxLyricOtherColor(int Line);
UCommon::TRGB<double> GetJukeboxLyricOtherOutlineColor(int Line);
UCommon::TRGB<double> GetLyricColor(int Color);
UCommon::TRGB<double> GetLyricGrayColor(int Color);
UCommon::TRGB<double> GetLyricOutlineColor(int Color);
UCommon::TRGB<double> GetLyricBarColor(int Color);

UCommon::TRGB<double> GetPlayerColor(int Color);
UCommon::TRGB<double> GetPlayerLightColor(int Color);
UCommon::TRGB<double> GetPlayerLightColorV2(int Color);
void LoadPlayersColors();
void LoadTeamsColors();

//inline TSkin Skin;
inline TTheme Theme;
inline std::vector<TColor> Color;
inline int LastC;

/*uses
  ULanguage,
  USkins,
  UPathUtils,
  UFileSystem,
  TextGL,
  dglOpenGL,
  math,
  StrUtils;*/

//-----------
//Helper procs to use TRGB in Opengl ...maybe this should be somewhere else
//-----------
void glColorRGB(UCommon::TRGB<double> Color)
{
  glColor3f(Color.R, Color.G, Color.B);
}

void glColorRGB(UCommon::TRGB<double> Color, double Alpha)
{
  glColor4f(Color.R, Color.G, Color.B, Alpha);
}

void glColorRGB(UCommon::TRGBA Color)
{
  glColor4f(Color.R, Color.G, Color.B, Color.A);
}

void glColorRGB(UCommon::TRGBA Color, double Alpha)
{
  glColor4f(Color.R, Color.G, Color.B, std::min(Color.A, Alpha));
}

TTheme::TTheme()
{
    /*
  Loading = TThemeLoading.Create;
  Main = TThemeMain.Create;
  Name = TThemeName.Create;
  Level = TThemeLevel.Create;
  Song = TThemeSong.Create;
  Sing = TThemeSing.Create;
  Score = TThemeScore.Create;
  Top5 = TThemeTop5.Create;
  Options = TThemeOptions.Create;
  OptionsGame = TThemeOptionsGame.Create;
  OptionsGraphics = TThemeOptionsGraphics.Create;
  OptionsSound = TThemeOptionsSound.Create;
  OptionsInput = TThemeOptionsInput.Create;
  OptionsLyrics = TThemeOptionsLyrics.Create;
  OptionsThemes = TThemeOptionsThemes.Create;
  OptionsRecord = TThemeOptionsRecord.Create;
  OptionsAdvanced = TThemeOptionsAdvanced.Create;
  OptionsNetwork = TThemeOptionsNetwork.Create;
  OptionsWebcam = TThemeOptionsWebcam.Create;
  OptionsJukebox = TThemeOptionsJukebox.Create;

  Edit = TThemeEdit.Create;
  EditConvert = TThemeEditConvert.Create;
  EditOpen = TThemeEditOpen.Create;
  EditSub = TThemeEditSub.Create;

  ErrorPopup = TThemeError.Create;
  CheckPopup = TThemeCheck.Create;
  InsertUserPopup = TThemeInsertUser.Create;
  SendScorePopup = TThemeSendScore.Create;
  ScoreDownloadPopup = TThemeScoreDownload.Create;

  HelpPopup = TThemeHelp.Create;

  SongMenu = TThemeSongMenu.Create;
  SongJumpto = TThemeSongJumpTo.Create;
  //Party Screens
  PartyNewRound = TThemePartyNewRound.Create;
  PartyWin = TThemePartyWin.Create;
  PartyScore = TThemePartyScore.Create;
  PartyOptions = TThemePartyOptions.Create;
  PartyPlayer = TThemePartyPlayer.Create;
  PartyRounds = TThemePartyRounds.Create;

  // Tournament
  PartyTournamentPlayer = TThemePartyTournamentPlayer.Create;
  PartyTournamentOptions = TThemePartyTournamentOptions.Create;
  PartyTournamentRounds = TThemePartyTournamentRounds.Create;
  PartyTournamentWin = TThemePartyTournamentWin.Create;

  // About
  AboutMain =   TThemeAboutMain.Create;

  //Stats Screens
  StatMain =   TThemeStatMain.Create;
  StatDetail = TThemeStatDetail.Create;

  JukeboxPlaylist = TThemeJukeboxPlaylist.Create;
  */
  //LoadTheme(FileName, Color);
  LoadList();
};

void TTheme::LoadHeader(std::filesystem::path FileName)
/*var
  TThemeEntry Entry;
  TMemIniFile Ini;
  string SkinName;
  boolean SkinsFound;
  string ThemeVersion;
  int I;
  int Len;
  TUTF8StringDynArray Skins;*/
{
    TThemeEntry Entry;
    Entry.Filename = UPathUtils::ThemePath / FileName;
    //read info from theme header
    auto Ini = TMemIniFile(Entry.Filename);

    Entry.Name = Ini.ReadString("Theme", "Name", FileName.filename().string());
    auto ThemeVersion = boost::algorithm::trim_copy(boost::algorithm::to_upper_copy(Ini.ReadString("Theme", "US_Version", "no version tag")));
    Entry.Creator = Ini.ReadString("Theme", "Creator", "Unknown");
    auto SkinName = Ini.ReadString("Theme", "DefaultSkin", FileName.filename().string());


    // don"t load theme with wrong version tag
    if (ThemeVersion != "USD 110")
    {
        ULog::Log.LogWarn("Wrong Version (" + ThemeVersion + ") in Theme : " + Entry.Name, "Theme.LoadHeader");
    }
    else
    {
        //Search for Skins for this Theme
        bool SkinsFound = false;
        for (I = Low(Skin.Skin) to High(Skin.Skin))
        {
            if (CompareText(Skin.Skin[I].Theme, Entry.Name) == 0)
            {
                SkinsFound = true;
                break;
            }
        }

        if (SkinsFound)
        {
            //{ found a valid Theme }
            // set correct default skin
            Skin.GetSkinsByTheme(Entry.Name, Skins);
            Entry.DefaultSkin = max(0, GetArrayIndex(Skins, SkinName, true));

            Len = Length(Themes);
            SetLength(Themes, Len + 1);
            SetLength(ITheme, Len + 1);
            Themes[Len] = Entry;
            ITheme[Len] = Entry.Name;
        }
    }
}

void TTheme::LoadList()
  /*var
    IFileIterator Iter;
    TFileInfo FileInfo;*/
{
  ULog::Log.LogStatus("Searching for Theme : " + UPathUtils::ThemePath.string() + "*.ini", "Theme.LoadList");

  Iter = FileSystem.FileFind(ThemePath.Append("*.ini"), 0);
  while (Iter.HasNext)
  {
    FileInfo = Iter.Next;
    ULog::Log.LogStatus("Found " + FileInfo.Name.string(), "Theme.LoadList" Theme);
    LoadHeader(Fileinfo.Name);
  }
};

bool TTheme::LoadTheme(int ThemeNum, int sColor)
/*var
  I, int J;*/
{
  Result = false;

  CreateThemeObjects();

  ULog::Log.LogStatus("+ Themes[ThemeNum].FileName.string(), "TTheme::LoadTheme" "Loading);

  if (!is_regular_file(Themes[ThemeNum].Filename))
  {
    ULog::Log.LogError("Theme does not exist ("+ Themes[ThemeNum].Filename.string() +")", "TTheme::LoadTheme");
  }
  else
  {
    Result = true;

    #ifdef THEMESAVE
    ThemeIni = TIniFile.Create(Themes[ThemeNum].FileName.string());
    #else
    ThemeIni = TMemIniFile(Themes[ThemeNum].FileName.string());
    #endif

    if (!ThemeIni.ReadString("Theme", "Name", "").empty())
    {

      /*{Skin.SkinName = ThemeIni.ReadString("Theme", "Name", "Singstar");
      Skin.SkinPath = "Skins\" + Skin.SkinName + "\";
      Skin.SkinReg = false; }*/
      Skin.Color = sColor;

      Skin.LoadSkin(ISkin[Ini.SkinNo], Themes[ThemeNum].Name);

      LoadColors();

//      ThemeIni.Free;
//      ThemeIni = TIniFile.Create("Themes\Singstar\Main.ini");

      // Loading
      ThemeLoadBasic(Loading, "Loading");
      ThemeLoadText(Loading.TextLoading, "LoadingTextLoading");
      ThemeLoadStatic(Loading.StaticAnimation, "LoadingStaticAnimation");

      // Main
      ThemeLoadBasic(Main, "Main");

      ThemeLoadText(Main.TextDescription, "MainTextDescription");
      ThemeLoadText(Main.TextDescriptionLong, "MainTextDescriptionLong");
      ThemeLoadButton(Main.ButtonSolo, "MainButtonSolo");
      ThemeLoadButton(Main.ButtonMulti, "MainButtonMulti");
      ThemeLoadButton(Main.ButtonJukebox, "MainButtonJukebox");
      ThemeLoadButton(Main.ButtonStat, "MainButtonStats");
      ThemeLoadButton(Main.ButtonEditor, "MainButtonEditor");
      ThemeLoadButton(Main.ButtonOptions, "MainButtonOptions");
      ThemeLoadButton(Main.ButtonAbout, "MainButtonAbout");
      ThemeLoadButton(Main.ButtonExit, "MainButtonExit");

      //Main Desc Text Translation Start

      Main.Description[0] = ULanguage::Language.Translate("SING_SING");
      Main.DescriptionLong[0] = ULanguage::Language.Translate("SING_SING_DESC");
      Main.Description[1] = ULanguage::Language.Translate("SING_MULTI");
      Main.DescriptionLong[1] = ULanguage::Language.Translate("SING_MULTI_DESC");
      Main.Description[2] = ULanguage::Language.Translate("SING_JUKEBOX");
      Main.DescriptionLong[2] = ULanguage::Language.Translate("SING_JUKEBOX_DESC");
      Main.Description[3] = ULanguage::Language.Translate("SING_STATS");
      Main.DescriptionLong[3] = ULanguage::Language.Translate("SING_STATS_DESC");
      Main.Description[4] = ULanguage::Language.Translate("SING_EDITOR");
      Main.DescriptionLong[4] = ULanguage::Language.Translate("SING_EDITOR_DESC");
      Main.Description[5] = ULanguage::Language.Translate("SING_GAME_OPTIONS");
      Main.DescriptionLong[5] = ULanguage::Language.Translate("SING_GAME_OPTIONS_DESC");
      Main.Description[6] = ULanguage::Language.Translate("SING_ABOUT");
      Main.DescriptionLong[6] = ULanguage::Language.Translate("SING_ABOUT_DESC");
      Main.Description[7] = ULanguage::Language.Translate("SING_EXIT");
      Main.DescriptionLong[7] = ULanguage::Language.Translate("SING_EXIT_DESC");

      //Main Desc Text Translation End

      Main.TextDescription.Text = Main.Description[0];
      Main.TextDescriptionLong.Text = Main.DescriptionLong[0];

      // Name
      ThemeLoadBasic(Name, "Name");

      ThemeLoadButton(Name.PlayerButtonName, "NamePlayerButtonName");
      ThemeLoadButton(Name.PlayerButtonAvatar, "NamePlayerButtonAvatar");

      Name.PlayerScrollAvatar.NumAvatars = ThemeIni.ReadInteger("NamePlayerScrollAvatar", "Count", 5);
      Name.PlayerScrollAvatar.DistanceAvatars = ThemeIni.ReadInteger("NamePlayerScrollAvatar", "Distance", 40);

      ThemeLoadButton(Name.PlayerAvatar, "NamePlayerAvatar");

      ThemeLoadSelectSlide(Name.SelectPlayersCount, "NameSelectPlayerCount");
      ThemeLoadSelectSlide(Name.SelectPlayerColor, "NameSelectPlayerColor");
      ThemeLoadSelectSlide(Name.SelectPlayerLevel, "NameSelectPlayerLevel");

      for (size_t I = 0; I < UIni::IMaxPlayerCount; ++I)
      {
        ThemeLoadStatic(Name.PlayerSelect[I], "NamePlayerSelectStatic" + std::to_string((I + 1)));
        ThemeLoadText(Name.PlayerSelectText[I], "NamePlayerSelectStatic" + std::to_string((I + 1)) + "Text");
        ThemeLoadStatic(Name.PlayerSelectAvatar[I], "NamePlayerSelectStatic" + std::to_string((I + 1)) + "Avatar");
      }

      ThemeLoadButton(Name.PlayerSelectCurrent, "NamePlayerSelectCurrent");

      // Level
      ThemeLoadBasic(Level, "Level");

      ThemeLoadButton(Level.ButtonEasy, "LevelButtonEasy");
      ThemeLoadButton(Level.ButtonMedium, "LevelButtonMedium");
      ThemeLoadButton(Level.ButtonHard, "LevelButtonHard");

      //Song
      ThemeSongLoad();

      //LyricBar
      LyricBar.UpperX = ThemeIni.ReadInteger("SingLyricsUpperBar", "X", 0);
      LyricBar.UpperW = ThemeIni.ReadInteger("SingLyricsUpperBar", "W", 0);
      LyricBar.UpperY = ThemeIni.ReadInteger("SingLyricsUpperBar", "Y", 0);
      LyricBar.UpperH = ThemeIni.ReadInteger("SingLyricsUpperBar", "H", 0);
      LyricBar.IndicatorYOffset = ThemeIni.ReadInteger("SingLyricsUpperBar", "IndicatorYOffset", 0);
      LyricBar.LowerX = ThemeIni.ReadInteger("SingLyricsLowerBar", "X", 0);
      LyricBar.LowerW = ThemeIni.ReadInteger("SingLyricsLowerBar", "W", 0);
      LyricBar.LowerY = ThemeIni.ReadInteger("SingLyricsLowerBar", "Y", 0);
      LyricBar.LowerH = ThemeIni.ReadInteger("SingLyricsLowerBar", "H", 0);

      //LyricBarDuet
      LyricBarDuetP1.UpperX = ThemeIni.ReadInteger("SingLyricsDuetP1UpperBar", "X", 0);
      LyricBarDuetP1.UpperW = ThemeIni.ReadInteger("SingLyricsDuetP1UpperBar", "W", 0);
      LyricBarDuetP1.UpperY = ThemeIni.ReadInteger("SingLyricsDuetP1UpperBar", "Y", 0);
      LyricBarDuetP1.UpperH = ThemeIni.ReadInteger("SingLyricsDuetP1UpperBar", "H", 0);
      LyricBarDuetP1.IndicatorYOffset = ThemeIni.ReadInteger("SingLyricsDuetP1UpperBar", "IndicatorYOffset", 0);
      LyricBarDuetP1.LowerX = ThemeIni.ReadInteger("SingLyricsDuetP1LowerBar", "X", 0);
      LyricBarDuetP1.LowerW = ThemeIni.ReadInteger("SingLyricsDuetP1LowerBar", "W", 0);
      LyricBarDuetP1.LowerY = ThemeIni.ReadInteger("SingLyricsDuetP1LowerBar", "Y", 0);
      LyricBarDuetP1.LowerH = ThemeIni.ReadInteger("SingLyricsDuetP1LowerBar", "H", 0);

      LyricBarDuetP2.UpperX = ThemeIni.ReadInteger("SingLyricsDuetP2UpperBar", "X", 0);
      LyricBarDuetP2.UpperW = ThemeIni.ReadInteger("SingLyricsDuetP2UpperBar", "W", 0);
      LyricBarDuetP2.UpperY = ThemeIni.ReadInteger("SingLyricsDuetP2UpperBar", "Y", 0);
      LyricBarDuetP2.UpperH = ThemeIni.ReadInteger("SingLyricsDuetP2UpperBar", "H", 0);
      LyricBarDuetP2.IndicatorYOffset = ThemeIni.ReadInteger("SingLyricsDuetP2UpperBar", "IndicatorYOffset", 0);
      LyricBarDuetP2.LowerX = ThemeIni.ReadInteger("SingLyricsDuetP2LowerBar", "X", 0);
      LyricBarDuetP2.LowerW = ThemeIni.ReadInteger("SingLyricsDuetP2LowerBar", "W", 0);
      LyricBarDuetP2.LowerY = ThemeIni.ReadInteger("SingLyricsDuetP2LowerBar", "Y", 0);
      LyricBarDuetP2.LowerH = ThemeIni.ReadInteger("SingLyricsDuetP2LowerBar", "H", 0);

      // Lyric Jukebox
      /*{ Need to change calculation in SongOptions
      LyricBarJukebox.UpperX = ThemeIni.ReadInteger("JukeboxLyricsUpperBar", "X", 0);
      LyricBarJukebox.UpperW = ThemeIni.ReadInteger("JukeboxLyricsUpperBar", "W", 0);
      LyricBarJukebox.UpperY = ThemeIni.ReadInteger("JukeboxLyricsUpperBar", "Y", 0);
      LyricBarJukebox.UpperH = ThemeIni.ReadInteger("JukeboxLyricsUpperBar", "H", 0);
      LyricBarJukebox.LowerX = ThemeIni.ReadInteger("JukeboxLyricsLowerBar", "X", 0);
      LyricBarJukebox.LowerW = ThemeIni.ReadInteger("JukeboxLyricsLowerBar", "W", 0);
      LyricBarJukebox.LowerY = ThemeIni.ReadInteger("JukeboxLyricsLowerBar", "Y", 0);
      LyricBarJukebox.LowerH = ThemeIni.ReadInteger("JukeboxLyricsLowerBar", "H", 0);
      LyricBarJukebox.IndicatorYOffset = ThemeIni.ReadInteger("JukeboxLyricsUpperBar", "IndicatorYOffset", 0);
      }*/

      LyricBarJukebox.UpperX = 40;
      LyricBarJukebox.UpperW = 720;
      LyricBarJukebox.UpperY = 490;
      LyricBarJukebox.UpperH = 52;
      LyricBarJukebox.LowerX = 40;
      LyricBarJukebox.LowerW = 720;
      LyricBarJukebox.LowerY = 540;
      LyricBarJukebox.LowerH = 52;
      LyricBarJukebox.IndicatorYOffset = 8;

      // Jukebox
      ThemeLoadStatic(Jukebox.StaticTimeProgress, "JukeboxTimeProgress");
      ThemeLoadStatic(Jukebox.StaticTimeBackground, "JukeboxTimeBackground");
      ThemeLoadStatic(Jukebox.StaticSongBackground, "JukeboxSongBackground");
      ThemeLoadStatic(Jukebox.StaticSongListBackground, "JukeboxSongListBackground");
      //ThemeLoadText(Jukebox.TextTimeText, "JukeboxTimeText");
      //ThemeLoadText(Jukebox.TextTimeDesc, "JukeboxTimeDesc");
      //ThemeLoadText(Jukebox.TextSongText, "JukeboxTextSong");
      ThemeLoadButton(Jukebox.SongDescription, "JukeboxSongDescription");
      ThemeLoadButton(Jukebox.FindSong, "JukeboxFind");
      ThemeLoadButton(Jukebox.RepeatSongList, "JukeboxRepeat");
      ThemeLoadButton(Jukebox.SongListPlayPause, "JukeboxPlayPause");
      ThemeLoadButton(Jukebox.SongListOrder, "JukeboxSort");
      ThemeLoadButton(Jukebox.RandomSongList, "JukeboxRandom");
      ThemeLoadButton(Jukebox.Lyric, "JukeboxLyric");
      ThemeLoadButton(Jukebox.SongListClose, "JukeboxSongListClose");
      ThemeLoadButton(Jukebox.Options, "JukeboxOptions");
      ThemeLoadText(Jukebox.TextListText, "JukeboxListText");
      ThemeLoadText(Jukebox.TextCountText, "JukeboxCountText");
      ThemeLoadStatic(Jukebox.SongCover, "JukeboxSongCover");

      ThemeLoadStatics(Jukebox.StaticActualSongStatics, "JukeboxStaticActualSong");
      ThemeLoadStatic(Jukebox.StaticActualSongCover, "JukeboxStaticActualSongCover");
      ThemeLoadText(Jukebox.TextActualSongArtist, "JukeboxTextActualSongArtist");
      ThemeLoadText(Jukebox.TextActualSongTitle, "JukeboxTextActualSongTitle");

      ThemeLoadButton(Jukebox.SongListUp, "JukeboxSongListUp");
      ThemeLoadButton(Jukebox.SongListDown, "JukeboxSongListDown");

      // Jukebox SongMenu
      ThemeLoadStatic(Jukebox.StaticSongMenuTimeProgress, "JukeboxSongMenuTimeProgress");
      ThemeLoadStatic(Jukebox.StaticSongMenuTimeBackground, "JukeboxSongMenuTimeBackground");
      ThemeLoadText(Jukebox.SongMenuTextTime, "JukeboxSongMenuTextTime");

      ThemeLoadStatic(Jukebox.StaticSongMenuBackground, "JukeboxSongMenuBackground");
      ThemeLoadButton(Jukebox.SongMenuPlayPause, "JukeboxSongMenuPlayPause");

      ThemeLoadButton(Jukebox.SongMenuNext, "JukeboxSongMenuNext");
      ThemeLoadButton(Jukebox.SongMenuPrevious, "JukeboxSongMenuPrevious");
      ThemeLoadButton(Jukebox.SongMenuPlaylist, "JukeboxSongMenuPlaylist");
      ThemeLoadButton(Jukebox.SongMenuOptions, "JukeboxSongMenuOptions");

      // Jukebox SongOptions
      ThemeLoadText(Jukebox.SongOptionsTextSaved, "JukeboxSongOptionsTextSaved");
      ThemeLoadStatic(Jukebox.StaticSongOptionsBackground, "JukeboxSongOptionsBackground");
      ThemeLoadButton(Jukebox.SongOptionsClose, "JukeboxSongOptionsClose");
      ThemeLoadButton(Jukebox.SongOptionsDefault, "JukeboxSongOptionsDefault");
      ThemeLoadButton(Jukebox.SongOptionsSave, "JukeboxSongOptionsSave");
      ThemeLoadButton(Jukebox.SongListFixPin, "JukeboxSongListFixPin");
      ThemeLoadText(Jukebox.SongOptionsVideoText, "JukeboxSongOptionsVideoText");
      ThemeLoadText(Jukebox.SongOptionsLyricText, "JukeboxSongOptionsLyricText");
      ThemeLoadSelectSlide(Jukebox.SongOptionsVideoAspectSlide, "JukeboxSongOptionsVideoAspectSlide");
      ThemeLoadSelectSlide(Jukebox.SongOptionsVideoWidthSlide, "JukeboxSongOptionsVideoWidthSlide");
      ThemeLoadSelectSlide(Jukebox.SongOptionsVideoHeightSlide, "JukeboxSongOptionsVideoHeightSlide");
      ThemeLoadSelectSlide(Jukebox.SongOptionsLyricSizeSlide, "JukeboxSongOptionsLyricSizeSlide");
      ThemeLoadSelectSlide(Jukebox.SongOptionsLyricPositionSlide, "JukeboxSongOptionsLyricPositionSlide");
      ThemeLoadSelectSlide(Jukebox.SongOptionsLyricAlphaSlide, "JukeboxSongOptionsLyricAlphaSlide");
      ThemeLoadSelectSlide(Jukebox.SongOptionsLyricColorSlide, "JukeboxSongOptionsLyricColorSlide");
      ThemeLoadSelectSlide(Jukebox.SongOptionsLyricLineSlide, "JukeboxSongOptionsLyricLineSlide");
      ThemeLoadSelectSlide(Jukebox.SongOptionsLyricPropertySlide, "JukeboxSongOptionsLyricPropertySlide");
      ThemeLoadSelectSlide(Jukebox.SelectR,    "JukeboxSongOptionsLyricSelectR");
      ThemeLoadSelectSlide(Jukebox.SelectG,    "JukeboxSongOptionsLyricSelectG");
      ThemeLoadSelectSlide(Jukebox.SelectB,    "JukeboxSongOptionsLyricSelectB");
      ThemeLoadStatic(Jukebox.PointerR,        "JukeboxSongOptionsLyricPointerR");
      ThemeLoadStatic(Jukebox.PointerG,        "JukeboxSongOptionsLyricPointerG");
      ThemeLoadStatic(Jukebox.PointerB,        "JukeboxSongOptionsLyricPointerB");
      ThemeLoadStatic(Jukebox.TexR,            "JukeboxSongOptionsLyricRed");
      ThemeLoadStatic(Jukebox.TexG,            "JukeboxSongOptionsLyricGreen");
      ThemeLoadStatic(Jukebox.TexB,            "JukeboxSongOptionsLyricBlue");

      // JukeboxPlaylist
      ThemeLoadBasic(JukeboxPlaylist, "JukeboxPlaylist");
      ThemeLoadSelectSlide(JukeboxPlaylist.SelectPlayList, "JukeboxPlaylistSelectPlayList");
      ThemeLoadSelectSlide(JukeboxPlaylist.SelectPlayList2, "JukeboxPlaylistSelectPlayList2");

      // Sing
      ThemeLoadBasic(Sing, "Sing");

      //ThemeLoadStatics(Sing.StaticDuet, "SingStaticDuet");

      // lyrics bar
      ThemeLoadStatic(Sing.StaticLyricsBar, "SingLyricsBar");
      ThemeLoadStatic(Sing.StaticLyricsBarDuet, "SingLyricsBarDuet");

      //TimeBar mod
      ThemeLoadStatic(Sing.StaticTimeBar, "SingTimeBar");
      ThemeLoadStatic(Sing.StaticTimeProgress, "SingTimeProgress");
      ThemeLoadText(Sing.TextTimeLabelText, "SingTimeLabelText");
      ThemeLoadText(Sing.TextTimeText, "SingTimeText");
      //eoa TimeBar mod

      ThemeLoadText (Sing.InfoMessageText, "SingInfoMessageText");
      ThemeLoadStatic (Sing.InfoMessageBG, "SingInfoMessageBG");

      //moveable singbar mod
      ThemeLoadStatic(Sing.StaticP1SingBar, "SingP1SingBar");
      ThemeLoadStatic(Sing.StaticP1TwoPSingBar, "SingP1TwoPSingBar");
      ThemeLoadStatic(Sing.StaticP1ThreePSingBar, "SingP1ThreePSingBar");
      ThemeLoadStatic(Sing.StaticP2RSingBar, "SingP2RSingBar");
      ThemeLoadStatic(Sing.StaticP2MSingBar, "SingP2MSingBar");
      ThemeLoadStatic(Sing.StaticP3SingBar, "SingP3SingBar");
    //eoa moveable singbar

      ThemeLoadStatic(Sing.StaticP1, "SingP1Static");
      ThemeLoadText(Sing.TextP1, "SingP1Text");
      ThemeLoadStatic(Sing.StaticP1ScoreBG, "SingP1Static2");
      ThemeLoadText(Sing.TextP1Score, "SingP1TextScore");
      ThemeLoadStatic(Sing.StaticP1Avatar, "SingP1Avatar");


  //Added for ps3 skin
  //This one is shown in 2/4P mode
  //if it exists, otherwise the one Player equivaltents are used
      if (ThemeIni.SectionExists("SingP1TwoPTextScore"))
      {
        ThemeLoadStatic(Sing.StaticP1TwoP, "SingP1TwoPStatic");
        ThemeLoadStatic(Sing.StaticP1TwoPAvatar, "SingP1TwoPAvatar");
        ThemeLoadText(Sing.TextP1TwoP, "SingP1TwoPText");
        ThemeLoadStatic(Sing.StaticP1TwoPScoreBG, "SingP1TwoPStatic2");
        ThemeLoadText(Sing.TextP1TwoPScore, "SingP1TwoPTextScore");
      }
      else
      {
        Sing.StaticP1TwoP = Sing.StaticP1;
        Sing.StaticP1TwoPAvatar = Sing.StaticP1Avatar;
        Sing.TextP1TwoP = Sing.TextP1;
        Sing.StaticP1TwoPScoreBG = Sing.StaticP1ScoreBG;
        Sing.TextP1TwoPScore = Sing.TextP1Score;
      }

  //This one is shown in 3/6P mode
  //if it exists, otherwise the one Player equivaltents are used
      if (ThemeIni.SectionExists("SingP1TwoPTextScore"))
      {
        ThemeLoadStatic(Sing.StaticP1ThreeP, "SingP1ThreePStatic");
        ThemeLoadStatic(Sing.StaticP1ThreePAvatar, "SingP1ThreePAvatar");
        ThemeLoadText(Sing.TextP1ThreeP, "SingP1ThreePText");
        ThemeLoadStatic(Sing.StaticP1ThreePScoreBG, "SingP1ThreePStatic2");
        ThemeLoadText(Sing.TextP1ThreePScore, "SingP1ThreePTextScore");
      }
      else
      {
        Sing.StaticP1ThreeP = Sing.StaticP1;
        Sing.StaticP1ThreePAvatar = Sing.StaticP1Avatar;
        Sing.TextP1ThreeP = Sing.TextP1;
        Sing.StaticP1ThreePScoreBG = Sing.StaticP1ScoreBG;
        Sing.TextP1ThreePScore = Sing.TextP1Score;
      };
  //eoa
      ThemeLoadStatic(Sing.StaticP2R, "SingP2RStatic");
      ThemeLoadText(Sing.TextP2R, "SingP2RText");
      ThemeLoadStatic(Sing.StaticP2RScoreBG, "SingP2RStatic2");
      ThemeLoadText(Sing.TextP2RScore, "SingP2RTextScore");
      ThemeLoadStatic(Sing.StaticP2RAvatar, "SingP2RAvatar");

      ThemeLoadStatic(Sing.StaticP2M, "SingP2MStatic");
      ThemeLoadText(Sing.TextP2M, "SingP2MText");
      ThemeLoadStatic(Sing.StaticP2MScoreBG, "SingP2MStatic2");
      ThemeLoadText(Sing.TextP2MScore, "SingP2MTextScore");
      ThemeLoadStatic(Sing.StaticP2MAvatar, "SingP2MAvatar");

      ThemeLoadStatic(Sing.StaticP3R, "SingP3RStatic");
      ThemeLoadText(Sing.TextP3R, "SingP3RText");
      ThemeLoadStatic(Sing.StaticP3RScoreBG, "SingP3RStatic2");
      ThemeLoadText(Sing.TextP3RScore, "SingP3RTextScore");
      ThemeLoadStatic(Sing.StaticP3RAvatar, "SingP3RAvatar");

      ThemeLoadStatic(Sing.StaticSongName, "SingSongNameStatic");
      ThemeLoadText(Sing.TextSongName, "SingSongNameText");

      // 3/6 players duet
      ThemeLoadStatic(Sing.StaticDuetP1ThreeP, "SingDuetP1ThreePStatic");
      ThemeLoadText(Sing.TextDuetP1ThreeP, "SingDuetP1ThreePText");
      ThemeLoadStatic(Sing.StaticDuetP1ThreePScoreBG, "SingDuetP1ThreePStatic2");
      ThemeLoadText(Sing.TextDuetP1ThreePScore, "SingDuetP1ThreePTextScore");
      ThemeLoadStatic(Sing.StaticDuetP1ThreePAvatar, "SingDuetP1ThreePAvatar");

      ThemeLoadStatic(Sing.StaticDuetP2M, "SingDuetP2MStatic");
      ThemeLoadText(Sing.TextDuetP2M, "SingDuetP2MText");
      ThemeLoadStatic(Sing.StaticDuetP2MScoreBG, "SingDuetP2MStatic2");
      ThemeLoadText(Sing.TextDuetP2MScore, "SingDuetP2MTextScore");
      ThemeLoadStatic(Sing.StaticDuetP2MAvatar, "SingDuetP2MAvatar");

      ThemeLoadStatic(Sing.StaticDuetP3R, "SingDuetP3RStatic");
      ThemeLoadText(Sing.TextDuetP3R, "SingDuetP3RText");
      ThemeLoadStatic(Sing.StaticDuetP3RScoreBG, "SingDuetP3RStatic2");
      ThemeLoadText(Sing.TextDuetP3RScore, "SingDuetP3RTextScore");
      ThemeLoadStatic(Sing.StaticDuetP3RAvatar, "SingDuetP3RAvatar");

      ThemeLoadStatic(Sing.StaticDuetP1ThreePSingBar, "SingDuetP1ThreePSingBar");
      ThemeLoadStatic(Sing.StaticDuetP2MSingBar, "SingDuetP2MSingBar");
      ThemeLoadStatic(Sing.StaticDuetP3RSingBar, "SingDuetP3RSingBar");

      //4P/6P mode in 1 Screen
      ThemeLoadStatic(Sing.StaticP1FourPSingBar, "SingP1FourPSingBar");
      ThemeLoadStatic(Sing.StaticP1FourP, "SingP1FourPStatic");
      ThemeLoadText(Sing.TextP1FourP, "SingP1FourPText");
      ThemeLoadStatic(Sing.StaticP1FourPScoreBG, "SingP1FourPStatic2");
      ThemeLoadText(Sing.TextP1FourPScore, "SingP1FourPTextScore");
      ThemeLoadStatic(Sing.StaticP1FourPAvatar, "SingP1FourPAvatar");

      ThemeLoadStatic(Sing.StaticP2FourPSingBar, "SingP2FourPSingBar");
      ThemeLoadStatic(Sing.StaticP2FourP, "SingP2FourPStatic");
      ThemeLoadText(Sing.TextP2FourP, "SingP2FourPText");
      ThemeLoadStatic(Sing.StaticP2FourPScoreBG, "SingP2FourPStatic2");
      ThemeLoadText(Sing.TextP2FourPScore, "SingP2FourPTextScore");
      ThemeLoadStatic(Sing.StaticP2FourPAvatar, "SingP2FourPAvatar");

      ThemeLoadStatic(Sing.StaticP3FourPSingBar, "SingP3FourPSingBar");
      ThemeLoadStatic(Sing.StaticP3FourP, "SingP3FourPStatic");
      ThemeLoadText(Sing.TextP3FourP, "SingP3FourPText");
      ThemeLoadStatic(Sing.StaticP3FourPScoreBG, "SingP3FourPStatic2");
      ThemeLoadText(Sing.TextP3FourPScore, "SingP3FourPTextScore");
      ThemeLoadStatic(Sing.StaticP3FourPAvatar, "SingP3FourPAvatar");

      ThemeLoadStatic(Sing.StaticP4FourPSingBar, "SingP4FourPSingBar");
      ThemeLoadStatic(Sing.StaticP4FourP, "SingP4FourPStatic");
      ThemeLoadText(Sing.TextP4FourP, "SingP4FourPText");
      ThemeLoadStatic(Sing.StaticP4FourPScoreBG, "SingP4FourPStatic2");
      ThemeLoadText(Sing.TextP4FourPScore, "SingP4FourPTextScore");
      ThemeLoadStatic(Sing.StaticP4FourPAvatar, "SingP4FourPAvatar");

      ThemeLoadStatic(Sing.StaticP1SixPSingBar, "SingP1SixPSingBar");
      ThemeLoadStatic(Sing.StaticP1SixP, "SingP1SixPStatic");
      ThemeLoadText(Sing.TextP1SixP, "SingP1SixPText");
      ThemeLoadStatic(Sing.StaticP1SixPScoreBG, "SingP1SixPStatic2");
      ThemeLoadText(Sing.TextP1SixPScore, "SingP1SixPTextScore");
      ThemeLoadStatic(Sing.StaticP1SixPAvatar, "SingP1SixPAvatar");

      ThemeLoadStatic(Sing.StaticP2SixPSingBar, "SingP2SixPSingBar");
      ThemeLoadStatic(Sing.StaticP2SixP, "SingP2SixPStatic");
      ThemeLoadText(Sing.TextP2SixP, "SingP2SixPText");
      ThemeLoadStatic(Sing.StaticP2SixPScoreBG, "SingP2SixPStatic2");
      ThemeLoadText(Sing.TextP2SixPScore, "SingP2SixPTextScore");
      ThemeLoadStatic(Sing.StaticP2SixPAvatar, "SingP2SixPAvatar");

      ThemeLoadStatic(Sing.StaticP3SixPSingBar, "SingP3SixPSingBar");
      ThemeLoadStatic(Sing.StaticP3SixP, "SingP3SixPStatic");
      ThemeLoadText(Sing.TextP3SixP, "SingP3SixPText");
      ThemeLoadStatic(Sing.StaticP3SixPScoreBG, "SingP3SixPStatic2");
      ThemeLoadText(Sing.TextP3SixPScore, "SingP3SixPTextScore");
      ThemeLoadStatic(Sing.StaticP3SixPAvatar, "SingP3SixPAvatar");

      ThemeLoadStatic(Sing.StaticP4SixPSingBar, "SingP4SixPSingBar");
      ThemeLoadStatic(Sing.StaticP4SixP, "SingP4SixPStatic");
      ThemeLoadText(Sing.TextP4SixP, "SingP4SixPText");
      ThemeLoadStatic(Sing.StaticP4SixPScoreBG, "SingP4SixPStatic2");
      ThemeLoadText(Sing.TextP4SixPScore, "SingP4SixPTextScore");
      ThemeLoadStatic(Sing.StaticP4SixPAvatar, "SingP4SixPAvatar");

      ThemeLoadStatic(Sing.StaticP5SixPSingBar, "SingP5SixPSingBar");
      ThemeLoadStatic(Sing.StaticP5SixP, "SingP5SixPStatic");
      ThemeLoadText(Sing.TextP5SixP, "SingP5SixPText");
      ThemeLoadStatic(Sing.StaticP5SixPScoreBG, "SingP5SixPStatic2");
      ThemeLoadText(Sing.TextP5SixPScore, "SingP5SixPTextScore");
      ThemeLoadStatic(Sing.StaticP5SixPAvatar, "SingP5SixPAvatar");

      ThemeLoadStatic(Sing.StaticP6SixPSingBar, "SingP6SixPSingBar");
      ThemeLoadStatic(Sing.StaticP6SixP, "SingP6SixPStatic");
      ThemeLoadText(Sing.TextP6SixP, "SingP6SixPText");
      ThemeLoadStatic(Sing.StaticP6SixPScoreBG, "SingP6SixPStatic2");
      ThemeLoadText(Sing.TextP6SixPScore, "SingP6SixPTextScore");
      ThemeLoadStatic(Sing.StaticP6SixPAvatar, "SingP6SixPAvatar");

      // duet 4/6 players in one screen
      ThemeLoadStatic(Sing.StaticP1DuetFourPSingBar, "SingP1DuetFourPSingBar");
      ThemeLoadStatic(Sing.StaticP1DuetFourP, "SingP1DuetFourPStatic");
      ThemeLoadText(Sing.TextP1DuetFourP, "SingP1DuetFourPText");
      ThemeLoadStatic(Sing.StaticP1DuetFourPScoreBG, "SingP1DuetFourPStatic2");
      ThemeLoadText(Sing.TextP1DuetFourPScore, "SingP1DuetFourPTextScore");
      ThemeLoadStatic(Sing.StaticP1DuetFourPAvatar, "SingP1DuetFourPAvatar");

      ThemeLoadStatic(Sing.StaticP2DuetFourPSingBar, "SingP2DuetFourPSingBar");
      ThemeLoadStatic(Sing.StaticP2DuetFourP, "SingP2DuetFourPStatic");
      ThemeLoadText(Sing.TextP2DuetFourP, "SingP2DuetFourPText");
      ThemeLoadStatic(Sing.StaticP2DuetFourPScoreBG, "SingP2DuetFourPStatic2");
      ThemeLoadText(Sing.TextP2DuetFourPScore, "SingP2DuetFourPTextScore");
      ThemeLoadStatic(Sing.StaticP2DuetFourPAvatar, "SingP2DuetFourPAvatar");

      ThemeLoadStatic(Sing.StaticP3DuetFourPSingBar, "SingP3DuetFourPSingBar");
      ThemeLoadStatic(Sing.StaticP3DuetFourP, "SingP3DuetFourPStatic");
      ThemeLoadText(Sing.TextP3DuetFourP, "SingP3DuetFourPText");
      ThemeLoadStatic(Sing.StaticP3DuetFourPScoreBG, "SingP3DuetFourPStatic2");
      ThemeLoadText(Sing.TextP3DuetFourPScore, "SingP3DuetFourPTextScore");
      ThemeLoadStatic(Sing.StaticP3DuetFourPAvatar, "SingP3DuetFourPAvatar");

      ThemeLoadStatic(Sing.StaticP4DuetFourPSingBar, "SingP4DuetFourPSingBar");
      ThemeLoadStatic(Sing.StaticP4DuetFourP, "SingP4DuetFourPStatic");
      ThemeLoadText(Sing.TextP4DuetFourP, "SingP4DuetFourPText");
      ThemeLoadStatic(Sing.StaticP4DuetFourPScoreBG, "SingP4DuetFourPStatic2");
      ThemeLoadText(Sing.TextP4DuetFourPScore, "SingP4DuetFourPTextScore");
      ThemeLoadStatic(Sing.StaticP4DuetFourPAvatar, "SingP4DuetFourPAvatar");


      ThemeLoadStatic(Sing.StaticP1DuetSixPSingBar, "SingP1DuetSixPSingBar");
      ThemeLoadStatic(Sing.StaticP1DuetSixP, "SingP1DuetSixPStatic");
      ThemeLoadText(Sing.TextP1DuetSixP, "SingP1DuetSixPText");
      ThemeLoadStatic(Sing.StaticP1DuetSixPScoreBG, "SingP1DuetSixPStatic2");
      ThemeLoadText(Sing.TextP1DuetSixPScore, "SingP1DuetSixPTextScore");
      ThemeLoadStatic(Sing.StaticP1DuetSixPAvatar, "SingP1DuetSixPAvatar");

      ThemeLoadStatic(Sing.StaticP2DuetSixPSingBar, "SingP2DuetSixPSingBar");
      ThemeLoadStatic(Sing.StaticP2DuetSixP, "SingP2DuetSixPStatic");
      ThemeLoadText(Sing.TextP2DuetSixP, "SingP2DuetSixPText");
      ThemeLoadStatic(Sing.StaticP2DuetSixPScoreBG, "SingP2DuetSixPStatic2");
      ThemeLoadText(Sing.TextP2DuetSixPScore, "SingP2DuetSixPTextScore");
      ThemeLoadStatic(Sing.StaticP2DuetSixPAvatar, "SingP2DuetSixPAvatar");

      ThemeLoadStatic(Sing.StaticP3DuetSixPSingBar, "SingP3DuetSixPSingBar");
      ThemeLoadStatic(Sing.StaticP3DuetSixP, "SingP3DuetSixPStatic");
      ThemeLoadText(Sing.TextP3DuetSixP, "SingP3DuetSixPText");
      ThemeLoadStatic(Sing.StaticP3DuetSixPScoreBG, "SingP3DuetSixPStatic2");
      ThemeLoadText(Sing.TextP3DuetSixPScore, "SingP3DuetSixPTextScore");
      ThemeLoadStatic(Sing.StaticP3DuetSixPAvatar, "SingP3DuetSixPAvatar");

      ThemeLoadStatic(Sing.StaticP4DuetSixPSingBar, "SingP4DuetSixPSingBar");
      ThemeLoadStatic(Sing.StaticP4DuetSixP, "SingP4DuetSixPStatic");
      ThemeLoadText(Sing.TextP4DuetSixP, "SingP4DuetSixPText");
      ThemeLoadStatic(Sing.StaticP4DuetSixPScoreBG, "SingP4DuetSixPStatic2");
      ThemeLoadText(Sing.TextP4DuetSixPScore, "SingP4DuetSixPTextScore");
      ThemeLoadStatic(Sing.StaticP4DuetSixPAvatar, "SingP4DuetSixPAvatar");

      ThemeLoadStatic(Sing.StaticP5DuetSixPSingBar, "SingP5DuetSixPSingBar");
      ThemeLoadStatic(Sing.StaticP5DuetSixP, "SingP5DuetSixPStatic");
      ThemeLoadText(Sing.TextP5DuetSixP, "SingP5DuetSixPText");
      ThemeLoadStatic(Sing.StaticP5DuetSixPScoreBG, "SingP5DuetSixPStatic2");
      ThemeLoadText(Sing.TextP5DuetSixPScore, "SingP5DuetSixPTextScore");
      ThemeLoadStatic(Sing.StaticP5DuetSixPAvatar, "SingP5DuetSixPAvatar");

      ThemeLoadStatic(Sing.StaticP6DuetSixPSingBar, "SingP6DuetSixPSingBar");
      ThemeLoadStatic(Sing.StaticP6DuetSixP, "SingP6DuetSixPStatic");
      ThemeLoadText(Sing.TextP6DuetSixP, "SingP6DuetSixPText");
      ThemeLoadStatic(Sing.StaticP6DuetSixPScoreBG, "SingP6DuetSixPStatic2");
      ThemeLoadText(Sing.TextP6DuetSixPScore, "SingP6DuetSixPTextScore");
      ThemeLoadStatic(Sing.StaticP6DuetSixPAvatar, "SingP6DuetSixPAvatar");

      // Oscilloscope Position
      ThemeLoadPosition(Sing.SingP1Oscilloscope, "SingP1Oscilloscope");
      ThemeLoadPosition(Sing.SingP1TwoPOscilloscope, "SingP1TwoPOscilloscope");
      ThemeLoadPosition(Sing.SingP2ROscilloscope, "SingP2ROscilloscope");
      ThemeLoadPosition(Sing.SingP1ThreePOscilloscope, "SingP1ThreePOscilloscope");
      ThemeLoadPosition(Sing.SingP2MOscilloscope, "SingP2MOscilloscope");
      ThemeLoadPosition(Sing.SingP3ROscilloscope, "SingP3ROscilloscope");
      ThemeLoadPosition(Sing.SingDuetP1ThreePOscilloscope, "SingDuetP1ThreePOscilloscope");
      ThemeLoadPosition(Sing.SingDuetP2MOscilloscope, "SingDuetP2MOscilloscope");
      ThemeLoadPosition(Sing.SingDuetP3ROscilloscope, "SingDuetP3ROscilloscope");
      ThemeLoadPosition(Sing.SingP1FourPOscilloscope, "SingP1FourPOscilloscope");
      ThemeLoadPosition(Sing.SingP2FourPOscilloscope, "SingP2FourPOscilloscope");
      ThemeLoadPosition(Sing.SingP3FourPOscilloscope, "SingP3FourPOscilloscope");
      ThemeLoadPosition(Sing.SingP4FourPOscilloscope, "SingP4FourPOscilloscope");
      ThemeLoadPosition(Sing.SingP1SixPOscilloscope, "SingP1SixPOscilloscope");
      ThemeLoadPosition(Sing.SingP2SixPOscilloscope, "SingP2SixPOscilloscope");
      ThemeLoadPosition(Sing.SingP3SixPOscilloscope, "SingP3SixPOscilloscope");
      ThemeLoadPosition(Sing.SingP4SixPOscilloscope, "SingP4SixPOscilloscope");
      ThemeLoadPosition(Sing.SingP5SixPOscilloscope, "SingP5SixPOscilloscope");
      ThemeLoadPosition(Sing.SingP6SixPOscilloscope, "SingP6SixPOscilloscope");
      ThemeLoadPosition(Sing.SingP1DuetFourPOscilloscope, "SingP1DuetFourPOscilloscope");
      ThemeLoadPosition(Sing.SingP2DuetFourPOscilloscope, "SingP2DuetFourPOscilloscope");
      ThemeLoadPosition(Sing.SingP3DuetFourPOscilloscope, "SingP3DuetFourPOscilloscope");
      ThemeLoadPosition(Sing.SingP4DuetFourPOscilloscope, "SingP4DuetFourPOscilloscope");
      ThemeLoadPosition(Sing.SingP1DuetSixPOscilloscope, "SingP1DuetSixPOscilloscope");
      ThemeLoadPosition(Sing.SingP2DuetSixPOscilloscope, "SingP2DuetSixPOscilloscope");
      ThemeLoadPosition(Sing.SingP3DuetSixPOscilloscope, "SingP3DuetSixPOscilloscope");
      ThemeLoadPosition(Sing.SingP4DuetSixPOscilloscope, "SingP4DuetSixPOscilloscope");
      ThemeLoadPosition(Sing.SingP5DuetSixPOscilloscope, "SingP5DuetSixPOscilloscope");
      ThemeLoadPosition(Sing.SingP6DuetSixPOscilloscope, "SingP6DuetSixPOscilloscope");

      //Line Bonus Texts
      Sing.LineBonusText[0] = ULanguage::Language.Translate("POPUP_AWFUL");
      Sing.LineBonusText[1] = Sing.LineBonusText[0];
      Sing.LineBonusText[2] = ULanguage::Language.Translate("POPUP_POOR");
      Sing.LineBonusText[3] = ULanguage::Language.Translate("POPUP_BAD");
      Sing.LineBonusText[4] = ULanguage::Language.Translate("POPUP_NOTBAD");
      Sing.LineBonusText[5] = ULanguage::Language.Translate("POPUP_GOOD");
      Sing.LineBonusText[6] = ULanguage::Language.Translate("POPUP_GREAT");
      Sing.LineBonusText[7] = ULanguage::Language.Translate("POPUP_AWESOME");
      Sing.LineBonusText[8] = ULanguage::Language.Translate("POPUP_PERFECT");

      //PausePopup
      ThemeLoadStatic(Sing.PausePopUp, "PausePopUpStatic");

      // Score
      ThemeLoadBasic(Score, "Score");

      ThemeLoadText(Score.TextArtist, "ScoreTextArtist");
      ThemeLoadText(Score.TextTitle, "ScoreTextTitle");
      ThemeLoadText(Score.TextArtistTitle, "ScoreTextArtistTitle");

      // Send Button"s
		for (int I = 1; I <= 3; ++I)
         ThemeLoadButton(Score.ButtonSend[I], "ScoreButtonSend" + std::to_string(I));

      ThemeLoadStatic(Score.StaticNavigate, "ScoreStaticNavigate");
      ThemeLoadText(Score.TextNavigate, "ScoreTextNavigate");

      // Top5
      ThemeLoadBasic(Top5, "Top5");

      ThemeLoadText(Top5.TextLevel,       "Top5TextLevel");
      ThemeLoadText(Top5.TextArtistTitle, "Top5TextArtistTitle");
      ThemeLoadStatics(Top5.StaticNumber, "Top5StaticNumber");
      ThemeLoadTexts(Top5.TextNumber,     "Top5TextNumber");
      ThemeLoadTexts(Top5.TextName,       "Top5TextName");
      ThemeLoadTexts(Top5.TextScore,      "Top5TextScore");
      ThemeLoadTexts(Top5.TextDate,       "Top5TextDate");

      // Options
      ThemeLoadBasic(Options, "Options");

      ThemeLoadButton(Options.ButtonGame,     "OptionsButtonGame");
      ThemeLoadButton(Options.ButtonGraphics, "OptionsButtonGraphics");
      ThemeLoadButton(Options.ButtonSound,    "OptionsButtonSound");
      ThemeLoadButton(Options.ButtonInput,    "OptionsButtonInput");
      ThemeLoadButton(Options.ButtonLyrics,   "OptionsButtonLyrics");
      ThemeLoadButton(Options.ButtonThemes,   "OptionsButtonThemes");
      ThemeLoadButton(Options.ButtonRecord,   "OptionsButtonRecord");
      ThemeLoadButton(Options.ButtonAdvanced, "OptionsButtonAdvanced");
      ThemeLoadButton(Options.ButtonNetwork,  "OptionsButtonNetwork");
      ThemeLoadButton(Options.ButtonWebcam,   "OptionsButtonWebcam");
      ThemeLoadButton(Options.ButtonJukebox,  "OptionsButtonJukebox");
      ThemeLoadButton(Options.ButtonExit,     "OptionsButtonExit");

      // always update the indexes constant on top of this unit when changing the order  Note(see OPTIONS_DESC_INDEX_*)
      Options.Description[OPTIONS_DESC_INDEX_BACK] = ULanguage::Language.Translate("SING_OPTIONS_EXIT");
      Options.Description[OPTIONS_DESC_INDEX_GAME] = ULanguage::Language.Translate("SING_OPTIONS_GAME_DESC");
      Options.Description[OPTIONS_DESC_INDEX_GRAPHICS] = ULanguage::Language.Translate("SING_OPTIONS_GRAPHICS_DESC");
      Options.Description[OPTIONS_DESC_INDEX_SOUND] = ULanguage::Language.Translate("SING_OPTIONS_SOUND_DESC");
      Options.Description[OPTIONS_DESC_INDEX_INPUT] = ULanguage::Language.Translate("SING_OPTIONS_INPUT_DESC");
      Options.Description[OPTIONS_DESC_INDEX_LYRICS] = ULanguage::Language.Translate("SING_OPTIONS_LYRICS_DESC");
      Options.Description[OPTIONS_DESC_INDEX_THEMES] = ULanguage::Language.Translate("SING_OPTIONS_THEMES_DESC");
      Options.Description[OPTIONS_DESC_INDEX_RECORD] = ULanguage::Language.Translate("SING_OPTIONS_RECORD_DESC");
      Options.Description[OPTIONS_DESC_INDEX_ADVANCED] = ULanguage::Language.Translate("SING_OPTIONS_ADVANCED_DESC");
      Options.Description[OPTIONS_DESC_INDEX_NETWORK] = ULanguage::Language.Translate("SING_OPTIONS_NETWORK_DESC");
      Options.Description[OPTIONS_DESC_INDEX_WEBCAM] = ULanguage::Language.Translate("SING_OPTIONS_WEBCAM_DESC");
      Options.Description[OPTIONS_DESC_INDEX_JUKEBOX] = ULanguage::Language.Translate("SING_OPTIONS_JUKEBOX_DESC");

      ThemeLoadText(Options.TextDescription, "OptionsTextDescription");
      Options.TextDescription.Text = Options.Description[OPTIONS_DESC_INDEX_GAME]; // Select default first menu button "Game"

      // Options Game
      ThemeLoadBasic(OptionsGame, "OptionsGame");

      ThemeLoadSelectSlide(OptionsGame.SelectLanguage,   "OptionsGameSelectSlideLanguage");
      ThemeLoadSelectSlide(OptionsGame.SelectSongMenu,   "OptionsGameSelectSongMenu");
      ThemeLoadSelectSlide(OptionsGame.SelectSorting,    "OptionsGameSelectSlideSorting");
      ThemeLoadSelectSlide(OptionsGame.SelectTabs,       "OptionsGameSelectTabs");
      ThemeLoadSelectSlide(OptionsGame.SelectShowScores, "OptionsGameSelectShowScores");
      ThemeLoadSelectSlide(OptionsGame.SelectDebug,      "OptionsGameSelectDebug");
      ThemeLoadSelectSlide(OptionsGame.SelectAVDelay,    "OptionsGameSelectAVDelay");
      ThemeLoadSelectSlide(OptionsGame.SelectMicDelay,   "OptionsGameSelectMicDelay");
      ThemeLoadButton(OptionsGame.ButtonExit,            "OptionsGameButtonExit");

      // Options Graphics
      ThemeLoadBasic(OptionsGraphics, "OptionsGraphics");

      ThemeLoadSelectSlide(OptionsGraphics.SelectFullscreen,   "OptionsGraphicsSelectFullscreen");
      ThemeLoadSelectSlide(OptionsGraphics.SelectResolution,   "OptionsGraphicsSelectSlideResolution");
      ThemeLoadSelectSlide(OptionsGraphics.SelectDepth,        "OptionsGraphicsSelectDepth");
      ThemeLoadSelectSlide(OptionsGraphics.SelectVisualizer,   "OptionsGraphicsSelectVisualizer");
      ThemeLoadSelectSlide(OptionsGraphics.SelectOscilloscope, "OptionsGraphicsSelectOscilloscope");
      ThemeLoadSelectSlide(OptionsGraphics.SelectLineBonus,    "OptionsGraphicsSelectLineBonus");
      ThemeLoadSelectSlide(OptionsGraphics.SelectMovieSize,    "OptionsGraphicsSelectMovieSize");
      ThemeLoadButton(OptionsGraphics.ButtonExit,              "OptionsGraphicsButtonExit");

      // Options Sound
      ThemeLoadBasic(OptionsSound, "OptionsSound");

      ThemeLoadSelectSlide(OptionsSound.SelectBackgroundMusic,       "OptionsSoundSelectBackgroundMusic");
      ThemeLoadSelectSlide(OptionsSound.SelectClickAssist,           "OptionsSoundSelectClickAssist");
      ThemeLoadSelectSlide(OptionsSound.SelectBeatClick,             "OptionsSoundSelectBeatClick");
      //Song Preview
      ThemeLoadSelectSlide(OptionsSound.SelectSlidePreviewVolume,    "OptionsSoundSelectSlidePreviewVolume");
      ThemeLoadSelectSlide(OptionsSound.SelectSlidePreviewFading,    "OptionsSoundSelectSlidePreviewFading");
      ThemeLoadSelectSlide(OptionsSound.SelectSlideVoicePassthrough, "OptionsSoundSelectVoicePassthrough");
      ThemeLoadSelectSlide(OptionsSound.SelectSlideMusicAutoGain,    "OptionsSoundSelectSlideMusicAutoGain");

      ThemeLoadButton(OptionsSound.ButtonExit, "OptionsSoundButtonExit");

      // Options Input
      ThemeLoadBasic(OptionsInput, "OptionsInput");

      ThemeLoadSelectSlide(OptionsInput.SelectMouse,     "OptionsInputSelectMouse");
      ThemeLoadSelectSlide(OptionsInput.SelectJoypad,    "OptionsInputSelectJoypad");
      ThemeLoadButton(OptionsInput.ButtonExit, "OptionsInputButtonExit");

      // Options Lyrics
      ThemeLoadBasic(OptionsLyrics, "OptionsLyrics");

      ThemeLoadSelectSlide(OptionsLyrics.SelectLyricsFont,   "OptionsLyricsSelectLyricsFont");
      ThemeLoadSelectSlide(OptionsLyrics.SelectLyricsStyle,  "OptionsLyricsSelectLyricsStyle");
      ThemeLoadSelectSlide(OptionsLyrics.SelectLyricsEffect, "OptionsLyricsSelectLyricsEffect");
      //ThemeLoadSelectSlide(OptionsLyrics.SelectSolmization,  "OptionsLyricsSelectSolmization");
      ThemeLoadSelectSlide(OptionsLyrics.SelectNoteLines,    "OptionsLyricsSelectNoteLines");

      OptionsLyrics.UpperX = ThemeIni.ReadInteger("OptionsLyricsUpperBar", "X", 0);
      OptionsLyrics.UpperW = ThemeIni.ReadInteger("OptionsLyricsUpperBar", "W", 0);
      OptionsLyrics.UpperY = ThemeIni.ReadInteger("OptionsLyricsUpperBar", "Y", 0);
      OptionsLyrics.UpperH = ThemeIni.ReadInteger("OptionsLyricsUpperBar", "H", 0);
      OptionsLyrics.LowerX = ThemeIni.ReadInteger("OptionsLyricsLowerBar", "X", 0);
      OptionsLyrics.LowerW = ThemeIni.ReadInteger("OptionsLyricsLowerBar", "W", 0);
      OptionsLyrics.LowerY = ThemeIni.ReadInteger("OptionsLyricsLowerBar", "Y", 0);
      OptionsLyrics.LowerH = ThemeIni.ReadInteger("OptionsLyricsLowerBar", "H", 0);

      ThemeLoadButton(OptionsLyrics.ButtonExit,              "OptionsLyricsButtonExit");

      // Options Themes
      ThemeLoadBasic(OptionsThemes, "OptionsThemes");

      ThemeLoadSelectSlide(OptionsThemes.SelectTheme, "OptionsThemesSelectTheme");
      ThemeLoadSelectSlide(OptionsThemes.SelectSkin,  "OptionsThemesSelectSkin");
      ThemeLoadSelectSlide(OptionsThemes.SelectColor, "OptionsThemesSelectColor");
      ThemeLoadButton(OptionsThemes.ButtonExit,       "OptionsThemesButtonExit");

      // Options Record
      ThemeLoadBasic(OptionsRecord, "OptionsRecord");

      ThemeLoadSelectSlide(OptionsRecord.SelectSlideCard,     "OptionsRecordSelectSlideCard");
      ThemeLoadSelectSlide(OptionsRecord.SelectSlideInput,    "OptionsRecordSelectSlideInput");
      ThemeLoadSelectSlide(OptionsRecord.SelectChannel,       "OptionsRecordSelectChannel");
      ThemeLoadSelectSlide(OptionsRecord.SelectAssignee,      "OptionsRecordSelectAssignee");
      ThemeLoadSelectSlide(OptionsRecord.SelectThreshold,     "OptionsRecordSelectThreshold"); //basisbit TODO
      ThemeLoadSelectSlide(OptionsRecord.SelectMicBoost,      "OptionsRecordSelectMicBoost");
      ThemeLoadButton(OptionsRecord.ButtonExit,               "OptionsRecordButtonExit");

      //Options Advanced
      ThemeLoadBasic(OptionsAdvanced, "OptionsAdvanced");

      ThemeLoadSelectSlide(OptionsAdvanced.SelectLoadAnimation, "OptionsAdvancedSelectLoadAnimation");
      ThemeLoadSelectSlide(OptionsAdvanced.SelectScreenFade,    "OptionsAdvancedSelectScreenFade");
      ThemeLoadSelectSlide(OptionsAdvanced.SelectEffectSing,    "OptionsAdvancedSelectEffectSing");
      ThemeLoadSelectSlide(OptionsAdvanced.SelectLineBonus,     "OptionsAdvancedSelectLineBonus");
      ThemeLoadSelectSlide(OptionsAdvanced.SelectOnSongClick,   "OptionsAdvancedSelectSlideOnSongClick");
      ThemeLoadSelectSlide(OptionsAdvanced.SelectAskbeforeDel,  "OptionsAdvancedSelectAskbeforeDel");
      ThemeLoadSelectSlide(OptionsAdvanced.SelectPartyPopup,    "OptionsAdvancedSelectPartyPopup");
      ThemeLoadSelectSlide(OptionsAdvanced.SelectSingScores,    "OptionsAdvancedSelectSingScores");
      ThemeLoadSelectSlide(OptionsAdvanced.SelectTopScores,     "OptionsAdvancedSelectTopScores");
      ThemeLoadButton     (OptionsAdvanced.ButtonExit,          "OptionsAdvancedButtonExit");

      //Options Network
      ThemeLoadBasic(OptionsNetwork, "OptionsNetwork");

      ThemeLoadSelectSlide(OptionsNetwork.SelectWebsite,       "OptionsNetworkSelectWebsite");
      ThemeLoadSelectSlide(OptionsNetwork.SelectUsername,      "OptionsNetworkSelectUsername");
      ThemeLoadSelectSlide(OptionsNetwork.SelectSendName,      "OptionsNetworkSelectSendSaveName");
      ThemeLoadSelectSlide(OptionsNetwork.SelectAutoMode,      "OptionsNetworkSelectAutoMode");
      ThemeLoadSelectSlide(OptionsNetwork.SelectAutoPlayer,      "OptionsNetworkSelectAutoPlayer");
      ThemeLoadSelectSlide(OptionsNetwork.SelectAutoScoreEasy,   "OptionsNetworkSelectAutoScoreEasy");
      ThemeLoadSelectSlide(OptionsNetwork.SelectAutoScoreMedium, "OptionsNetworkSelectAutoScoreMedium");
      ThemeLoadSelectSlide(OptionsNetwork.SelectAutoScoreHard,   "OptionsNetworkSelectAutoScoreHard");
      ThemeLoadText(OptionsNetwork.TextInsertUser, "OptionsNetworkTextInsertUser");

      ThemeLoadButton(OptionsNetwork.ButtonInsert,          "OptionsNetworkButtonInsert");
      ThemeLoadButton(OptionsNetwork.ButtonExit,          "OptionsNetworkButtonExit");

      //Options Webcam
      ThemeLoadBasic(OptionsWebcam, "OptionsWebcam");

      ThemeLoadSelectSlide(OptionsWebcam.SelectWebcam,     "OptionsWebcamSelectWebcam");
      ThemeLoadSelectSlide(OptionsWebcam.SelectResolution, "OptionsWebcamSelectResolution");
      ThemeLoadSelectSlide(OptionsWebcam.SelectFPS,        "OptionsWebcamSelectFPS");
      ThemeLoadSelectSlide(OptionsWebcam.SelectFlip,       "OptionsWebcamSelectFlip");
      ThemeLoadSelectSlide(OptionsWebcam.SelectBrightness, "OptionsWebcamSelectBrightness");
      ThemeLoadSelectSlide(OptionsWebcam.SelectSaturation, "OptionsWebcamSelectSaturation");
      ThemeLoadSelectSlide(OptionsWebcam.SelectHue,        "OptionsWebcamSelectHue");
      ThemeLoadSelectSlide(OptionsWebcam.SelectEffect,     "OptionsWebcamSelectEffect");

      ThemeLoadButton(OptionsWebcam.ButtonPreVisualization,          "OptionsWebcamButtonPreVisualization");
      ThemeLoadButton(OptionsWebcam.ButtonExit,          "OptionsWebcamButtonExit");

      // Options Jukebox
      ThemeLoadBasic(OptionsJukebox, "OptionsJukebox");

      ThemeLoadSelectSlide(OptionsJukebox.SelectLyricsFont,   "OptionsJukeboxSelectLyricsFont");
      ThemeLoadSelectSlide(OptionsJukebox.SelectLyricsStyle,   "OptionsJukeboxSelectLyricsStyle");
      ThemeLoadSelectSlide(OptionsJukebox.SelectLyricsEffect, "OptionsJukeboxSelectLyricsEffect");
      ThemeLoadSelectSlide(OptionsJukebox.SelectLyricsAlpha,   "OptionsJukeboxSelectLyricsAlpha");
      ThemeLoadSelectSlide(OptionsJukebox.SelectLine,         "OptionsJukeboxSelectLine");
      ThemeLoadSelectSlide(OptionsJukebox.SelectProperty,    "OptionsJukeboxSelectProperty");
      ThemeLoadSelectSlide(OptionsJukebox.SelectColor,        "OptionsJukeboxSelectColor");
      ThemeLoadSelectSlide(OptionsJukebox.SelectR,    "OptionsJukeboxSelectR");
      ThemeLoadSelectSlide(OptionsJukebox.SelectG,    "OptionsJukeboxSelectG");
      ThemeLoadSelectSlide(OptionsJukebox.SelectB,    "OptionsJukeboxSelectB");
      ThemeLoadStatic(OptionsJukebox.PointerR,        "OptionsJukeboxPointerR");
      ThemeLoadStatic(OptionsJukebox.PointerG,        "OptionsJukeboxPointerG");
      ThemeLoadStatic(OptionsJukebox.PointerB,        "OptionsJukeboxPointerB");
      ThemeLoadStatic(OptionsJukebox.TexR,            "OptionsJukeboxRed");
      ThemeLoadStatic(OptionsJukebox.TexG,            "OptionsJukeboxGreen");
      ThemeLoadStatic(OptionsJukebox.TexB,            "OptionsJukeboxBlue");
      ThemeLoadStatic(OptionsJukebox.TexColor,        "OptionsJukeboxColor");

      OptionsJukebox.UpperX = ThemeIni.ReadInteger("OptionsJukeboxUpperBar", "X", 0);
      OptionsJukebox.UpperW = ThemeIni.ReadInteger("OptionsJukeboxUpperBar", "W", 0);
      OptionsJukebox.UpperY = ThemeIni.ReadInteger("OptionsJukeboxUpperBar", "Y", 0);
      OptionsJukebox.UpperH = ThemeIni.ReadInteger("OptionsJukeboxUpperBar", "H", 0);
      OptionsJukebox.LowerX = ThemeIni.ReadInteger("OptionsJukeboxLowerBar", "X", 0);
      OptionsJukebox.LowerW = ThemeIni.ReadInteger("OptionsJukeboxLowerBar", "W", 0);
      OptionsJukebox.LowerY = ThemeIni.ReadInteger("OptionsJukeboxLowerBar", "Y", 0);
      OptionsJukebox.LowerH = ThemeIni.ReadInteger("OptionsJukeboxLowerBar", "H", 0);

      ThemeLoadButton(OptionsJukebox.ButtonExit,              "OptionsJukeboxButtonExit");

      //Edit Menu
      ThemeLoadBasic (Edit,               "Edit");

      ThemeLoadButton(Edit.ButtonConvert, "EditButtonConvert");
      ThemeLoadButton(Edit.ButtonExit,    "EditButtonExit");

      Edit.Description[0] = ULanguage::Language.Translate("SING_EDIT_BUTTON_DESCRIPTION_CONVERT");
      Edit.Description[1] = ULanguage::Language.Translate("SING_EDIT_BUTTON_DESCRIPTION_EXIT");

      ThemeLoadText(Edit.TextDescription, "EditTextDescription");
      Edit.TextDescription.Text = Edit.Description[0];

      //Edit Convert Menu
      ThemeLoadBasic (EditConvert,        "EditConvert");

      ThemeLoadButton(EditConvert.ButtonOpen,         "EditConvertButtonOpen");
      ThemeLoadButton(EditConvert.ButtonPlay,         "EditConvertButtonPlay");
      ThemeLoadButton(EditConvert.ButtonPlaySelected, "EditConvertButtonPlaySelected");
      ThemeLoadButton(EditConvert.ButtonStop,         "EditConvertButtonStop");
      ThemeLoadButton(EditConvert.ButtonSave,         "EditConvertButtonSave");

      EditConvert.Description[0] = ULanguage::Language.Translate("SING_EDIT_CONVERT_BUTTON_DESCRIPTION_OPEN");
      EditConvert.Description[1] = ULanguage::Language.Translate("SING_EDIT_CONVERT_BUTTON_DESCRIPTION_PLAY");
      EditConvert.Description[2] = ULanguage::Language.Translate("SING_EDIT_CONVERT_BUTTON_DESCRIPTION_PLAYSELECTED");
      EditConvert.Description[3] = ULanguage::Language.Translate("SING_EDIT_CONVERT_BUTTON_DESCRIPTION_STOP");
      EditConvert.Description[4] = ULanguage::Language.Translate("SING_EDIT_CONVERT_BUTTON_DESCRIPTION_SAVE");

      ThemeLoadText(EditConvert.TextDescription, "EditConvertTextDescription");
      EditConvert.TextDescription.Text = EditConvert.Description[0];

      //Edit Open Menu
      ThemeLoadBasic (EditOpen,           "EditOpen");

      ThemeLoadButton(EditOpen.ButtonFileName, "EditOpenButtonFileName");
      ThemeLoadButton(EditOpen.ButtonLoad,     "EditOpenButtonLoad");
      ThemeLoadButton(EditOpen.ButtonBack,     "EditOpenButtonBack");

      //EditOpen.Description[0] = ULanguage::Language.Translate("SING_EDIT_OPEN_BUTTON_DESCRIPTION_LOAD");
      //EditOpen.Description[1] = ULanguage::Language.Translate("SING_EDIT_OPEN_BUTTON_DESCRIPTION_BACK");

      ThemeLoadText(EditOpen.TextDescription, "EditOpenTextDescription");
      EditOpen.TextDescription.Text = EditOpen.Description[0];

      // editor
      ThemeLoadBasic (EditSub,               "EditSub");
      ThemeLoadStatic(EditSub.BackgroundImage, "EditSubBackgroundImage");
      ThemeLoadStatic(EditSub.HeaderBackground, "EditSubHeaderBackground");
      ThemeLoadStatic(EditSub.CurrentNoteInfoBackground, "EditSubCurrentNoteInfoBackground");
      ThemeLoadStatic(EditSub.VolumeSliderBackground, "EditSubVolumeSliderBackground");
      ThemeLoadStatic(EditSub.NotesBackground, "EditSubNotesBackground");
      ThemeLoadStatic(EditSub.P1InfoBarBackground, "EditSubP1InfoBarBackground");
      ThemeLoadStatic(EditSub.P2InfoBarBackground, "EditSubP2InfoBarBackground");
      ThemeLoadStatic(EditSub.SentenceBackground, "EditSubSentenceBackground");
      // current position in editor
      ThemeLoadButton(EditSub.ButtonCurrentLine, "EditSubButtonCurrentLine");
      ThemeLoadButton(EditSub.ButtonCurrentNote, "EditSubButtonCurrentNote");
      ThemeLoadButton(EditSub.PlayOnly,    "EditSubBarStatic1");
      ThemeLoadButton(EditSub.PlayWithNote,    "EditSubBarStatic2");
      ThemeLoadButton(EditSub.PlayNote,    "EditSubBarStatic3");
      ThemeLoadButton(EditSub.previousseq,    "EditSubBarStatic4");
      ThemeLoadButton(EditSub.nextseq,    "EditSubBarStatic5");
      ThemeLoadButton(EditSub.gold,    "EditSubBarStatic6");
      ThemeLoadButton(EditSub.freestyle,    "EditSubBarStatic7");
      ThemeLoadButton(EditSub.undo,    "EditSubBarStatic8");
      ThemeLoadSelectSlide(EditSub.SlideTitle, "EditSubTitle");
      ThemeLoadSelectSlide(EditSub.SlideArtist, "EditSubArtist");
      ThemeLoadSelectSlide(EditSub.SlideLanguage, "EditSubLanguage");
      ThemeLoadSelectSlide(EditSub.SlideEdition, "EditSubEdition");
      ThemeLoadSelectSlide(EditSub.SlideGenre, "EditSubGenre");
      ThemeLoadSelectSlide(EditSub.SlideYear, "EditSubYear");
      ThemeLoadSelectSlide(EditSub.SlideCreator, "EditSubCreator");
      ThemeLoadSelectSlide(EditSub.SlideMP3, "EditSubMP3");
      ThemeLoadSelectSlide(EditSub.SlideCover, "EditSubSlideCover");
      ThemeLoadSelectSlide(EditSub.SlideBackground, "EditSubSlideBackground");
      ThemeLoadSelectSlide(EditSub.SlideVideo, "EditSubSlideVideo");
      ThemeLoadSelectSlide(EditSub.SlideBPM, "EditSubBPM");
      ThemeLoadSelectSlide(EditSub.SlideGAP, "EditSubGAP");
      ThemeLoadSelectSlide(EditSub.SlideStartTag, "EditSubStartTag");
      ThemeLoadSelectSlide(EditSub.SlideEndTag, "EditSubEndTag");
      ThemeLoadSelectSlide(EditSub.SlideMedleyStart, "EditSubMedleyStart");
      ThemeLoadSelectSlide(EditSub.SlideMedleyEnd, "EditSubMedleyEnd");
      ThemeLoadSelectSlide(EditSub.SlidePreviewStart, "EditSubPreviewStart");
      ThemeLoadSelectSlide(EditSub.SlideRelative, "EditSubRelative");
      ThemeLoadSelectSlide(EditSub.SlideStart, "EditSubStart");
      ThemeLoadSelectSlide(EditSub.SlideDuration, "EditSubDuration");
      ThemeLoadSelectSlide(EditSub.SlideTone, "EditSubTone");
      ThemeLoadSelectSlide(EditSub.SlideLyric, "EditSubLyric");
      ThemeLoadSelectSlide(EditSub.SelectVolAudio, "EditSubSelectVolAudio");
      ThemeLoadSelectSlide(EditSub.SelectVolMidi, "EditSubSelectVolMidi");
      ThemeLoadSelectSlide(EditSub.SelectVolClick, "EditSubSelectVolClick");
      ThemeLoadSelectSlide(EditSub.SlideVideoGap, "EditSubVideoGap");
      ThemeLoadText(EditSub.TextInfo, "EditSubTextInfo");
      ThemeLoadText(EditSub.TextSentence, "EditSubTextSentence");
      ThemeLoadText(EditSub.TextCurrentTone, "EditSubTextCurrentTone");

      //error popup
      ThemeLoadBasic (ErrorPopup, "ErrorPopup");
      ThemeLoadButton(ErrorPopup.Button1, "ErrorPopupButton1");
      ThemeLoadText  (ErrorPopup.TextError,"ErrorPopupText");

      //check popup
      ThemeLoadBasic (CheckPopup, "CheckPopup");
      ThemeLoadButton(CheckPopup.Button1, "CheckPopupButton1");
      ThemeLoadButton(CheckPopup.Button2, "CheckPopupButton2");
      ThemeLoadText(CheckPopup.TextCheck , "CheckPopupText");

      // insert user popup
      ThemeLoadBasic (InsertUserPopup, "InsertUserPopup");
      ThemeLoadText  (InsertUserPopup.TextInsertUser , "InsertUserPopupText");
      ThemeLoadButton(InsertUserPopup.ButtonUsername, "InsertUserPopupButtonUsername");
      ThemeLoadButton(InsertUserPopup.ButtonPassword, "InsertUserPopupButtonPassword");
      ThemeLoadButton(InsertUserPopup.Button1, "InsertUserPopupButton1");
      ThemeLoadButton(InsertUserPopup.Button2, "InsertUserPopupButton2");

      // send score popup
      ThemeLoadBasic (SendScorePopup, "SendScorePopup");
      ThemeLoadSelectSlide(SendScorePopup.SelectSlide1, "SendScorePopupSelectSlide1");
      ThemeLoadSelectSlide(SendScorePopup.SelectSlide2, "SendScorePopupSelectSlide2");
      ThemeLoadSelectSlide(SendScorePopup.SelectSlide3, "SendScorePopupSelectSlide3");
      ThemeLoadButton(SendScorePopup.ButtonUsername, "SendScorePopupButtonUsername");
      ThemeLoadButton(SendScorePopup.ButtonPassword, "SendScorePopupButtonPassword");
      ThemeLoadButton(SendScorePopup.Button1, "SendScorePopupButton1");
      ThemeLoadButton(SendScorePopup.Button2, "SendScorePopupButton2");

      // download score popup
      ThemeLoadBasic (ScoreDownloadPopup, "ScoreDownloadPopup");
      ThemeLoadButton(ScoreDownloadPopup.Button1, "ScoreDownloadPopupButton1");
      ThemeLoadText(ScoreDownloadPopup.TextSongScoreDownload , "ScoreDownloadPopupSongText");
      ThemeLoadText(ScoreDownloadPopup.TextWebScoreDownload , "ScoreDownloadPopupWebText");
      ThemeLoadStatic(ScoreDownloadPopup.DownloadProgressSong, "ScoreDownloadPopupProgressSong");
      ThemeLoadStatic(ScoreDownloadPopup.DownloadProgressWeb, "ScoreDownloadPopupProgressWeb");

      //Song Menu
      ThemeLoadBasic (SongMenu, "SongMenu");
      ThemeLoadButton(SongMenu.Button1, "SongMenuButton1");
      ThemeLoadButton(SongMenu.Button2, "SongMenuButton2");
      ThemeLoadButton(SongMenu.Button3, "SongMenuButton3");
      ThemeLoadButton(SongMenu.Button4, "SongMenuButton4");
      ThemeLoadButton(SongMenu.Button5, "SongMenuButton5");
      ThemeLoadSelectSlide(SongMenu.SelectSlide1, "SongMenuSelectSlide1");
      ThemeLoadSelectSlide(SongMenu.SelectSlide2, "SongMenuSelectSlide2");
      ThemeLoadSelectSlide(SongMenu.SelectSlide3, "SongMenuSelectSlide3");

      ThemeLoadText(SongMenu.TextMenu, "SongMenuTextMenu");

      //Song Jumpto
      ThemeLoadBasic (SongJumpto, "SongJumpto");
      ThemeLoadButton(SongJumpto.ButtonSearchText, "SongJumptoButtonSearchText");

      ThemeLoadSelectSlide(SongJumpto.SelectSlideType, "SongJumptoSelectSlideType");
      ThemeLoadText(SongJumpto.TextFound, "SongJumptoTextFound");
      //Translations
      SongJumpto.IType[0] = ULanguage::Language.Translate("SONG_JUMPTO_TYPE1");
      SongJumpto.IType[1] = ULanguage::Language.Translate("SONG_JUMPTO_TYPE2");
      SongJumpto.IType[2] = ULanguage::Language.Translate("SONG_JUMPTO_TYPE3");
      SongJumpto.IType[3] = ULanguage::Language.Translate("SONG_JUMPTO_TYPE4");
      SongJumpto.IType[4] = ULanguage::Language.Translate("SONG_JUMPTO_TYPE5");
      SongJumpto.IType[5] = ULanguage::Language.Translate("SONG_JUMPTO_TYPE6");
      SongJumpto.IType[6] = ULanguage::Language.Translate("SONG_JUMPTO_TYPE7");
      SongJumpto.IType[7] = ULanguage::Language.Translate("SONG_JUMPTO_TYPE8");
      SongJumpto.Songsfound = ULanguage::Language.Translate("SONG_JUMPTO_SONGSFOUND");
      SongJumpto.NoSongsfound = ULanguage::Language.Translate("SONG_JUMPTO_NOSONGSFOUND");
      SongJumpto.CatText = ULanguage::Language.Translate("SONG_JUMPTO_CATTEXT");

      //Party Options
      ThemeLoadBasic(PartyOptions, "PartyOptions");
      ThemeLoadSelectSlide(PartyOptions.SelectMode, "PartyOptionsSelectMode");
      ThemeLoadSelectSlide(PartyOptions.SelectLevel, "PartyOptionsSelectLevel");
      ThemeLoadSelectSlide(PartyOptions.SelectPlayList, "PartyOptionsSelectPlayList");
      ThemeLoadSelectSlide(PartyOptions.SelectPlayList2, "PartyOptionsSelectPlayList2");
      /*{ThemeLoadButton (ButtonNext, "ButtonNext");
      ThemeLoadButton (ButtonPrev, "ButtonPrev");}*/

      //Party Player
      ThemeLoadBasic(PartyPlayer, "PartyPlayer");

      ThemeLoadSelectSlide(PartyPlayer.SelectTeams, "PartyPlayerSelectTeams");
      ThemeLoadSelectSlide(PartyPlayer.SelectPlayers1, "PartyPlayerSelectPlayers1");
      ThemeLoadSelectSlide(PartyPlayer.SelectPlayers2, "PartyPlayerSelectPlayers2");
      ThemeLoadSelectSlide(PartyPlayer.SelectPlayers3, "PartyPlayerSelectPlayers3");

      ThemeLoadButton(PartyPlayer.Team1Name, "PartyPlayerTeam1Name");
      ThemeLoadButton(PartyPlayer.Player1Name, "PartyPlayerPlayer1Name");
      ThemeLoadButton(PartyPlayer.Player2Name, "PartyPlayerPlayer2Name");
      ThemeLoadButton(PartyPlayer.Player3Name, "PartyPlayerPlayer3Name");
      ThemeLoadButton(PartyPlayer.Player4Name, "PartyPlayerPlayer4Name");

      ThemeLoadButton(PartyPlayer.Team2Name, "PartyPlayerTeam2Name");
      ThemeLoadButton(PartyPlayer.Player5Name, "PartyPlayerPlayer5Name");
      ThemeLoadButton(PartyPlayer.Player6Name, "PartyPlayerPlayer6Name");
      ThemeLoadButton(PartyPlayer.Player7Name, "PartyPlayerPlayer7Name");
      ThemeLoadButton(PartyPlayer.Player8Name, "PartyPlayerPlayer8Name");

      ThemeLoadButton(PartyPlayer.Team3Name, "PartyPlayerTeam3Name");
      ThemeLoadButton(PartyPlayer.Player9Name, "PartyPlayerPlayer9Name");
      ThemeLoadButton(PartyPlayer.Player10Name, "PartyPlayerPlayer10Name");
      ThemeLoadButton(PartyPlayer.Player11Name, "PartyPlayerPlayer11Name");
      ThemeLoadButton(PartyPlayer.Player12Name, "PartyPlayerPlayer12Name");

      // Party Rounds
      ThemeLoadBasic(PartyRounds, "PartyRounds");

      ThemeLoadSelectSlide(PartyRounds.SelectRoundCount, "PartyRoundsSelectRoundCount");

      for (size_t I = 0; I < PartyRounds.SelectRound.size(); ++I)
        ThemeLoadSelectSlide(PartyRounds.SelectRound[I], "PartyRoundsSelectRound" + std::to_string(I + 1));

      /*{ThemeLoadButton(ButtonNext, "PartyPlayerButtonNext");
      ThemeLoadButton(ButtonPrev, "PartyPlayerButtonPrev");}*/

      //Party Tournament Player
      ThemeLoadBasic(PartyTournamentPlayer, "PartyTournamentPlayer");

      ThemeLoadSelectSlide(PartyTournamentPlayer.SelectPlayers, "PartyTournamentPlayerSelectPlayers");

      ThemeLoadButton(PartyTournamentPlayer.Player1Name, "PartyTournamentPlayerPlayer1Name");
      ThemeLoadButton(PartyTournamentPlayer.Player2Name, "PartyTournamentPlayerPlayer2Name");
      ThemeLoadButton(PartyTournamentPlayer.Player3Name, "PartyTournamentPlayerPlayer3Name");
      ThemeLoadButton(PartyTournamentPlayer.Player4Name, "PartyTournamentPlayerPlayer4Name");
      ThemeLoadButton(PartyTournamentPlayer.Player5Name, "PartyTournamentPlayerPlayer5Name");
      ThemeLoadButton(PartyTournamentPlayer.Player6Name, "PartyTournamentPlayerPlayer6Name");
      ThemeLoadButton(PartyTournamentPlayer.Player7Name, "PartyTournamentPlayerPlayer7Name");
      ThemeLoadButton(PartyTournamentPlayer.Player8Name, "PartyTournamentPlayerPlayer8Name");
      ThemeLoadButton(PartyTournamentPlayer.Player9Name, "PartyTournamentPlayerPlayer9Name");
      ThemeLoadButton(PartyTournamentPlayer.Player10Name, "PartyTournamentPlayerPlayer10Name");
      ThemeLoadButton(PartyTournamentPlayer.Player11Name, "PartyTournamentPlayerPlayer11Name");
      ThemeLoadButton(PartyTournamentPlayer.Player12Name, "PartyTournamentPlayerPlayer12Name");
      ThemeLoadButton(PartyTournamentPlayer.Player13Name, "PartyTournamentPlayerPlayer13Name");
      ThemeLoadButton(PartyTournamentPlayer.Player14Name, "PartyTournamentPlayerPlayer14Name");
      ThemeLoadButton(PartyTournamentPlayer.Player15Name, "PartyTournamentPlayerPlayer15Name");
      ThemeLoadButton(PartyTournamentPlayer.Player16Name, "PartyTournamentPlayerPlayer16Name");

      //Party Tournament Options
      ThemeLoadBasic(PartyTournamentOptions, "PartyTournamentOptions");
      ThemeLoadSelectSlide(PartyTournamentOptions.SelectRoundsFinal, "PartyTournamentOptionsSelectRoundsFinal");
      ThemeLoadSelectSlide(PartyTournamentOptions.SelectRounds2Final, "PartyTournamentOptionsSelectRounds2Final");
      ThemeLoadSelectSlide(PartyTournamentOptions.SelectRounds4Final, "PartyTournamentOptionsSelectRounds4Final");
      ThemeLoadSelectSlide(PartyTournamentOptions.SelectRounds8Final, "PartyTournamentOptionsSelectRounds8Final");

      //Party Tournament Rounds

      ThemeLoadBasic(PartyTournamentRounds, "PartyTournamentRounds");

      for (size_t I = 0; I <= 1; ++I)
      {
        for (size_t J = 0; J <= 7; ++J)
        {
          ThemeLoadButton (PartyTournamentRounds.TextNamePlayer[I][J], "PartyTournamentRoundsTextNameBlock" + std::to_string(I + 1) + "Player" + std::to_string(J + 1));
        }
      }

      ThemeLoadText(PartyTournamentRounds.TextWinner, "PartyTournamentRoundsWinner");
      ThemeLoadText(PartyTournamentRounds.TextResult, "PartyTournamentRoundsResult");

      ThemeLoadText(PartyTournamentRounds.NextPlayers, "PartyTournamentRoundsNextPlayers");

      //Party Tournament Win
      ThemeLoadBasic(PartyTournamentWin, "PartyTournamentWin");

      ThemeLoadText (PartyTournamentWin.TextScorePlayer1,    "PartyTournamentWinTextScorePlayer1");
      ThemeLoadText (PartyTournamentWin.TextScorePlayer2,    "PartyTournamentWinTextScorePlayer2");

      ThemeLoadText (PartyTournamentWin.TextNamePlayer1,     "PartyTournamentWinTextNamePlayer1");
      ThemeLoadText (PartyTournamentWin.TextNamePlayer2,     "PartyTournamentWinTextNamePlayer2");

      ThemeLoadStatic (PartyTournamentWin.StaticBGPlayer1,   "PartyTournamentWinStaticBGPlayer1");
      ThemeLoadStatic (PartyTournamentWin.StaticBGPlayer2,   "PartyTournamentWinStaticBGPlayer2");

      // About
      ThemeLoadBasic(AboutMain, "AboutMain");
      ThemeLoadButton(AboutMain.ButtonCredits, "AboutMainButtonCredits");
      ThemeLoadButton(AboutMain.ButtonExit, "AboutMainButtonExit");
      ThemeLoadText (AboutMain.TextOverview, "AboutMainTextOverview");

      // Stats
      ThemeLoadBasic(StatMain, "StatMain");

      ThemeLoadButton(StatMain.ButtonScores, "StatMainButtonScores");
      ThemeLoadButton(StatMain.ButtonSingers, "StatMainButtonSingers");
      ThemeLoadButton(StatMain.ButtonSongs, "StatMainButtonSongs");
      ThemeLoadButton(StatMain.ButtonBands, "StatMainButtonBands");
      ThemeLoadButton(StatMain.ButtonExit, "StatMainButtonExit");

      ThemeLoadText (StatMain.TextOverview, "StatMainTextOverview");


      ThemeLoadBasic(StatDetail, "StatDetail");

      ThemeLoadButton(StatDetail.ButtonNext, "StatDetailButtonNext");
      ThemeLoadButton(StatDetail.ButtonPrev, "StatDetailButtonPrev");
      ThemeLoadButton(StatDetail.ButtonReverse, "StatDetailButtonReverse");
      ThemeLoadButton(StatDetail.ButtonExit, "StatDetailButtonExit");

      ThemeLoadText (StatDetail.TextDescription, "StatDetailTextDescription");
      ThemeLoadText (StatDetail.TextPage, "StatDetailTextPage");
      ThemeLoadTexts(StatDetail.TextList, "StatDetailTextList");

      //Translate Texts
      StatDetail.Description[0] = ULanguage::Language.Translate("STAT_DESC_SCORES");
      StatDetail.Description[1] = ULanguage::Language.Translate("STAT_DESC_SINGERS");
      StatDetail.Description[2] = ULanguage::Language.Translate("STAT_DESC_SONGS");
      StatDetail.Description[3] = ULanguage::Language.Translate("STAT_DESC_BANDS");

      StatDetail.DescriptionR[0] = ULanguage::Language.Translate("STAT_DESC_SCORES_REVERSED");
      StatDetail.DescriptionR[1] = ULanguage::Language.Translate("STAT_DESC_SINGERS_REVERSED");
      StatDetail.DescriptionR[2] = ULanguage::Language.Translate("STAT_DESC_SONGS_REVERSED");
      StatDetail.DescriptionR[3] = ULanguage::Language.Translate("STAT_DESC_BANDS_REVERSED");

      StatDetail.FormatStr[0] = ULanguage::Language.Translate("STAT_FORMAT_SCORES");
      StatDetail.FormatStr[1] = ULanguage::Language.Translate("STAT_FORMAT_SINGERS");
      StatDetail.FormatStr[2] = ULanguage::Language.Translate("STAT_FORMAT_SONGS");
      StatDetail.FormatStr[3] = ULanguage::Language.Translate("STAT_FORMAT_BANDS");

      StatDetail.PageStr = ULanguage::Language.Translate("STAT_PAGE");

      //Playlist Translations
      Playlist.CatText = ULanguage::Language.Translate("PLAYLIST_CATTEXT");

      //Level Translations
      //Fill ILevel
      ILevel[0] = ULanguage::Language.Translate("SING_EASY");
      ILevel[1] = ULanguage::Language.Translate("SING_MEDIUM");
      ILevel[2] = ULanguage::Language.Translate("SING_HARD");

      //Mode Translations
      //Fill IMode
      IMode[0] = ULanguage::Language.Translate("PARTY_MODE_CLASSIC");
      IMode[1] = ULanguage::Language.Translate("PARTY_MODE_CLASSIC_FREE");
      IMode[2] = ULanguage::Language.Translate("PARTY_MODE_CHALLENGE");
      IMode[3] = ULanguage::Language.Translate("PARTY_MODE_TOURNAMENT");
    }
  }
}

void TTheme::ThemeLoadBasic(TThemeBasic Theme, const string Name)
{
  ThemeLoadBackground(Theme.Background, Name);
  ThemeLoadTexts(Theme.Text, Name + "Text");
  ThemeLoadStatics(Theme.Statics, Name + "Static");
  ThemeLoadButtonCollections(Theme.ButtonCollection, Name + "ButtonCollection");

  LastThemeBasic = Theme;
}

void TTheme::ThemeLoadBackground(TThemeBackground& ThemeBackground, const std::string Name)
/*var
  string BGType;
  TBackgroundType I;*/
{
  BGType = LowerCase(ThemeIni.ReadString(Name + "Background", "Type", "auto"));

  ThemeBackground.BGType = bgtAuto;
  for I = Low(BGT_Names) to High(BGT_Names) do
  {
    if (BGT_Names[I] == BGType) then
    {
      ThemeBackground.BGType = I;
      Break;
    };
  };

  ThemeBackground.Tex     = ThemeIni.ReadString(Name + "Background", "Tex", "");
  ThemeBackground.Color.R = ThemeIni.ReadFloat(Name + "Background", "ColR", 1);
  ThemeBackground.Color.G = ThemeIni.ReadFloat(Name + "Background", "ColG", 1);
  ThemeBackground.Color.B = ThemeIni.ReadFloat(Name + "Background", "ColB", 1);
  ThemeBackground.Alpha   = ThemeIni.ReadFloat(Name + "Background", "Alpha", 1);
};

void TTheme::ThemeLoadText(TThemeText& ThemeText, const std::string Name)
/*var
  int C;*/
{
  ThemeText.X     = ThemeIni.ReadInteger(Name, "X", 0);
  ThemeText.Y     = ThemeIni.ReadInteger(Name, "Y", 0);
  ThemeText.W     = ThemeIni.ReadInteger(Name, "W", 0);
  ThemeText.H     = ThemeIni.ReadInteger(Name, "H", 0);

  ThemeText.Z     = ThemeIni.ReadFloat(Name, "Z", 0);

  ThemeText.ColR  = ThemeIni.ReadFloat(Name, "ColR", 0);
  ThemeText.ColG  = ThemeIni.ReadFloat(Name, "ColG", 0);
  ThemeText.ColB  = ThemeIni.ReadFloat(Name, "ColB", 0);

  ThemeText.Font  = ThemeIni.ReadInteger(Name, "Font", 0);
  ThemeText.Style = ThemeIni.ReadInteger(Name, "Style", TextGL::ftRegular);
  ThemeText.Size  = ThemeIni.ReadInteger(Name, "Size", 0);
  ThemeText.Align = ThemeIni.ReadInteger(Name, "Align", 0);

  ThemeText.Text  = ULanguage::Language.Translate(ThemeIni.ReadString(Name, "Text", ""));
  ThemeText.Color = ThemeIni.ReadString(Name, "Color", "");
  ThemeText.DColor = ThemeIni.ReadString(Name, "DColor", "");

  //Reflection
  ThemeText.Reflection         = (ThemeIni.ReadInteger(Name, "Reflection", 0)) == 1;
  ThemeText.Reflectionspacing  = ThemeIni.ReadFloat(Name, "ReflectionSpacing", 15);

  C = ColorExists(ThemeText.Color);
  if C >= 0 then
  {
    ThemeText.ColR = Color[C].RGB.R;
    ThemeText.ColG = Color[C].RGB.G;
    ThemeText.ColB = Color[C].RGB.B;
  };

  C = ColorExists(ThemeText.DColor);
  if C >= 0 then
  {
    ThemeText.DColR = Color[C].RGB.R;
    ThemeText.DColG = Color[C].RGB.G;
    ThemeText.DColB = Color[C].RGB.B;
  };

};

void TTheme::ThemeLoadTexts(AThemeText& ThemeText, const std::string Name)
/*var
  int T;*/
{
  T = 1;
  while ThemeIni.SectionExists(Name + std::to_string(T)) do
  {
    SetLength(ThemeText, T);
    ThemeLoadText(ThemeText[T-1], Name + std::to_string(T));
    Inc(T);
  };
};

void TTheme::ThemeLoadStatic(TThemeStatic& ThemeStatic, const std::string Name)
/*var
  int C;*/
{
  ThemeStatic.Tex = ThemeIni.ReadString(Name, "Tex", "");

  ThemeStatic.X = ThemeIni.ReadInteger(Name, "X", 0);
  ThemeStatic.Y = ThemeIni.ReadInteger(Name, "Y", 0);
  ThemeStatic.Z = ThemeIni.ReadFloat  (Name, "Z", 0);
  ThemeStatic.W = ThemeIni.ReadInteger(Name, "W", 0);
  ThemeStatic.H = ThemeIni.ReadInteger(Name, "H", 0);
  ThemeStatic.Alpha = ThemeIni.ReadFloat(Name, "Alpha", 1);
  if ThemeIni.ReadString(Name, "Type", "") == "" then ULog::Log.LogError("no texture type for " + Name + " found.", "TTheme::ThemeLoadStatic");
  ThemeStatic.Typ   = ParseTextureType(ThemeIni.ReadString(Name, "Type", ""), TEXTURE_TYPE_PLAIN);
  ThemeStatic.Color = ThemeIni.ReadString(Name, "Color", "");

  C = ColorExists(ThemeStatic.Color);
  if C >= 0 then
  {
    ThemeStatic.ColR = Color[C].RGB.R;
    ThemeStatic.ColG = Color[C].RGB.G;
    ThemeStatic.ColB = Color[C].RGB.B;
  };

  ThemeStatic.TexX1 = ThemeIni.ReadFloat(Name, "TexX1", 0);
  ThemeStatic.TexY1 = ThemeIni.ReadFloat(Name, "TexY1", 0);
  ThemeStatic.TexX2 = ThemeIni.ReadFloat(Name, "TexX2", 1);
  ThemeStatic.TexY2 = ThemeIni.ReadFloat(Name, "TexY2", 1);

  //Reflection Mod
  ThemeStatic.Reflection        = (ThemeIni.ReadInteger(Name, "Reflection", 0) == 1);
  ThemeStatic.ReflectionSpacing = ThemeIni.ReadFloat(Name, "ReflectionSpacing", 15);
};

void TTheme::ThemeLoadStatics(AThemeStatic& ThemeStatic, const std::string Name)
/*var
  int S;*/
{
  S = 1;
  while ThemeIni.SectionExists(Name + std::to_string(S)) do
  {
    SetLength(ThemeStatic, S);
    ThemeLoadStatic(ThemeStatic[S-1], Name + std::to_string(S));
    Inc(S);
  };
};

//Button Collection Mod
void TTheme::ThemeLoadButtonCollection(TThemeButtonCollection& Collection, const std::string Name)
//var int T;
{
  //Load Collection Style
  ThemeLoadButton(Collection.Style, Name);

  //Load Other Attributes
  T = ThemeIni.ReadInteger (Name, "FirstChild", 0);
  if (T > 0) And (T < 256) then
    Collection.FirstChild = T
  else
    Collection.FirstChild = 0;
};

void TTheme::ThemeLoadButtonCollections(AThemeButtonCollection& Collections, const std::string Name)
//var int I;
{
  I = 1;
  while ThemeIni.SectionExists(Name + std::to_string(I)) do
  {
    SetLength(Collections, I);
    ThemeLoadButtonCollection(Collections[I-1], Name + std::to_string(I));
    Inc(I);
  };
};
//End Button Collection Mod

void TTheme::ThemeLoadButton(TThemeButton& ThemeButton, const std::string Name, PAThemeButtonCollection Collections)
/*var
  int C;
  int TLen;
  int T;
  PAThemeButtonCollection Collections2;*/
{
    if (!ThemeIni.SectionExists(Name))
    {
        ThemeButton.Visible = False;
        return;
    };
    ThemeButton.Tex = ThemeIni.ReadString(Name, "Tex", "");
    ThemeButton.X = ThemeIni.ReadInteger(Name, "X", 0);
    ThemeButton.Y = ThemeIni.ReadInteger(Name, "Y", 0);
    ThemeButton.Z = ThemeIni.ReadFloat(Name, "Z", 0);
    ThemeButton.W = ThemeIni.ReadInteger(Name, "W", 0);
    ThemeButton.H = ThemeIni.ReadInteger(Name, "H", 0);
    ThemeButton.Typ = ParseTextureType(ThemeIni.ReadString(Name, "Type", ""), UTexture::TEXTURE_TYPE_PLAIN);

    //Reflection Mod
    ThemeButton.Reflection = (ThemeIni.ReadInteger(Name, "Reflection", 0) == 1);
    ThemeButton.ReflectionSpacing = ThemeIni.ReadFloat(Name, "ReflectionSpacing", 15);

    ThemeButton.ColR = ThemeIni.ReadFloat(Name, "ColR", 1);
    ThemeButton.ColG = ThemeIni.ReadFloat(Name, "ColG", 1);
    ThemeButton.ColB = ThemeIni.ReadFloat(Name, "ColB", 1);
    ThemeButton.Int = ThemeIni.ReadFloat(Name, "Int", 1);
    ThemeButton.DColR = ThemeIni.ReadFloat(Name, "DColR", 1);
    ThemeButton.DColG = ThemeIni.ReadFloat(Name, "DColG", 1);
    ThemeButton.DColB = ThemeIni.ReadFloat(Name, "DColB", 1);
    ThemeButton.DInt = ThemeIni.ReadFloat(Name, "DInt", 1);

    ThemeButton.Color = ThemeIni.ReadString(Name, "Color", "");
    C = ColorExists(ThemeButton.Color);
    if (C >= 0)
    {
      ThemeButton.ColR = Color[C].RGB.R;
      ThemeButton.ColG = Color[C].RGB.G;
      ThemeButton.ColB = Color[C].RGB.B;
    };

    ThemeButton.DColor = ThemeIni.ReadString(Name, "DColor", "");
    C = ColorExists(ThemeButton.DColor);
    if (C >= 0)
    {
      ThemeButton.DColR = Color[C].RGB.R;
      ThemeButton.DColG = Color[C].RGB.G;
      ThemeButton.DColB = Color[C].RGB.B;
    };

    ThemeButton.Visible = (ThemeIni.ReadInteger(Name, "Visible", 1) == 1);

    //Fade Mod
    ThemeButton.SelectH = ThemeIni.ReadInteger(Name, "SelectH", ThemeButton.H);
    ThemeButton.SelectW = ThemeIni.ReadInteger(Name, "SelectW", ThemeButton.W);

    ThemeButton.DeSelectReflectionspacing = ThemeIni.ReadFloat(Name, "DeSelectReflectionSpacing", ThemeButton.Reflectionspacing);

    ThemeButton.Fade = (ThemeIni.ReadInteger(Name, "Fade", 0) == 1);
    ThemeButton.FadeText = (ThemeIni.ReadInteger(Name, "FadeText", 0) == 1);


    ThemeButton.FadeTex = ThemeIni.ReadString(Name, "FadeTex", "");
    ThemeButton.FadeTexPos = ThemeIni.ReadInteger(Name, "FadeTexPos", 0);
    if (ThemeButton.FadeTexPos > 4) Or(ThemeButton.FadeTexPos < 0) then
        ThemeButton.FadeTexPos = 0;

    //Button Collection Mod
    T = ThemeIni.ReadInteger(Name, "Parent", 0);

    //Set Collections to Last Basic Collections if no valid Value
    if (Collections == nil) then
        Collections2 = @LastThemeBasic.ButtonCollection
    else
        Collections2 = Collections;
    //Test for valid Value
    if (Collections2 != nil && T > 0 && T <= Length(Collections2^))
    {
        Inc(Collections2 ^ [T - 1].ChildCount);
        ThemeButton.Parent = T;
    }
    else
        ThemeButton.Parent = 0;

    //Read ButtonTexts
    TLen = ThemeIni.ReadInteger(Name, "Texts", 0);
    SetLength(ThemeButton.Text, TLen);
    for T = 1 to TLen do
        ThemeLoadText(ThemeButton.Text[T - 1], Name + "Text" + std::to_string(T));
}

void TTheme::ThemeLoadSelectSlide(TThemeSelectSlide& ThemeSelectS, const std::string Name)
{
  ThemeSelectS.Text = ULanguage::Language.Translate(ThemeIni.ReadString(Name, "Text", ""));

  ThemeSelectS.Tex = {Skin.SkinPath + }ThemeIni.ReadString(Name, "Tex", "");
  ThemeSelectS.Typ = ParseTextureType(ThemeIni.ReadString(Name, "Type", ""), UTexture::TEXTURE_TYPE_PLAIN);
  ThemeSelectS.TexSBG = {Skin.SkinPath + }ThemeIni.ReadString(Name, "TexSBG", "");
  ThemeSelectS.TypSBG = ParseTextureType(ThemeIni.ReadString(Name, "TypeSBG", ""), UTexture::TEXTURE_TYPE_PLAIN);

  ThemeSelectS.X = ThemeIni.ReadInteger(Name, "X", 0);
  ThemeSelectS.Y = ThemeIni.ReadInteger(Name, "Y", 0);
  ThemeSelectS.W = ThemeIni.ReadInteger(Name, "W", 0);
  ThemeSelectS.H = ThemeIni.ReadInteger(Name, "H", 0);

  ThemeSelectS.Z = ThemeIni.ReadFloat(Name, "Z", 0);

  ThemeSelectS.TextSize = ThemeIni.ReadInteger(Name, "TextSize", 30);

  ThemeSelectS.SkipX = ThemeIni.ReadInteger(Name, "SkipX", 0);

  ThemeSelectS.SBGW = ThemeIni.ReadInteger(Name, "SBGW", 400);

  LoadColor(ThemeSelectS.ColR, ThemeSelectS.ColG,  ThemeSelectS.ColB, ThemeIni.ReadString(Name, "Color", ""));
  ThemeSelectS.Int =  ThemeIni.ReadFloat(Name, "Int", 1);
  LoadColor(ThemeSelectS.DColR, ThemeSelectS.DColG,  ThemeSelectS.DColB, ThemeIni.ReadString(Name, "DColor", ""));
  ThemeSelectS.DInt =  ThemeIni.ReadFloat(Name, "DInt", 1);

  LoadColor(ThemeSelectS.TColR, ThemeSelectS.TColG,  ThemeSelectS.TColB, ThemeIni.ReadString(Name, "TColor", ""));
  ThemeSelectS.TInt =  ThemeIni.ReadFloat(Name, "TInt", 1);
  LoadColor(ThemeSelectS.TDColR, ThemeSelectS.TDColG,  ThemeSelectS.TDColB, ThemeIni.ReadString(Name, "TDColor", ""));
  ThemeSelectS.TDInt =  ThemeIni.ReadFloat(Name, "TDInt", 1);

  LoadColor(ThemeSelectS.SBGColR, ThemeSelectS.SBGColG,  ThemeSelectS.SBGColB, ThemeIni.ReadString(Name, "SBGColor", ""));
  ThemeSelectS.SBGInt =  ThemeIni.ReadFloat(Name, "SBGInt", 1);
  LoadColor(ThemeSelectS.SBGDColR, ThemeSelectS.SBGDColG,  ThemeSelectS.SBGDColB, ThemeIni.ReadString(Name, "SBGDColor", ""));
  ThemeSelectS.SBGDInt =  ThemeIni.ReadFloat(Name, "SBGDInt", 1);

  LoadColor(ThemeSelectS.STColR, ThemeSelectS.STColG,  ThemeSelectS.STColB, ThemeIni.ReadString(Name, "STColor", ""));
  ThemeSelectS.STInt =  ThemeIni.ReadFloat(Name, "STInt", 1);
  LoadColor(ThemeSelectS.STDColR, ThemeSelectS.STDColG,  ThemeSelectS.STDColB, ThemeIni.ReadString(Name, "STDColor", ""));
  ThemeSelectS.STDInt =  ThemeIni.ReadFloat(Name, "STDInt", 1);

  ThemeSelectS.showArrows = (ThemeIni.ReadInteger(Name, "ShowArrows", 0) == 1);
  ThemeSelectS.oneItemOnly = (ThemeIni.ReadInteger(Name, "OneItemOnly", 0) == 1);
};

void TTheme::ThemeLoadEqualizer(TThemeEqualizer& ThemeEqualizer, const std::string Name)
//var int I;
{
  ThemeEqualizer.Visible = (ThemeIni.ReadInteger(Name, "Visible", 0) == 1);
  ThemeEqualizer.Direction = (ThemeIni.ReadInteger(Name, "Direction", 0) == 1);
  ThemeEqualizer.Alpha = ThemeIni.ReadInteger(Name, "Alpha", 1);
  ThemeEqualizer.Space = ThemeIni.ReadInteger(Name, "Space", 1);
  ThemeEqualizer.X = ThemeIni.ReadInteger(Name, "X", 0);
  ThemeEqualizer.Y = ThemeIni.ReadInteger(Name, "Y", 0);
  ThemeEqualizer.Z = ThemeIni.ReadInteger(Name, "Z", 1);
  ThemeEqualizer.W = ThemeIni.ReadInteger(Name, "PieceW", 8);
  ThemeEqualizer.H = ThemeIni.ReadInteger(Name, "PieceH", 8);
  ThemeEqualizer.Bands = ThemeIni.ReadInteger(Name, "Bands", 5);
  ThemeEqualizer.Length = ThemeIni.ReadInteger(Name, "Length", 12);
  ThemeEqualizer.Reflection = (ThemeIni.ReadInteger(Name, "Reflection", 0) == 1);
  ThemeEqualizer.ReflectionSpacing = ThemeIni.ReadFloat(Name, "ReflectionSpacing", 15);

  //Color
  I = ColorExists(ThemeIni.ReadString(Name, "Color", "Black"));
  if I >= 0 then
  {
    ThemeEqualizer.ColR = Color[I].RGB.R;
    ThemeEqualizer.ColG = Color[I].RGB.G;
    ThemeEqualizer.ColB = Color[I].RGB.B;
  }
  else
  {
    ThemeEqualizer.ColR = 0;
    ThemeEqualizer.ColG = 0;
    ThemeEqualizer.ColB = 0;
  };
};

void TTheme::ThemeLoadPosition(TThemePosition& ThemePosition, const std::string Name)
{
  ThemePosition.X = ThemeIni.ReadInteger(Name, "X", 0);
  ThemePosition.Y = ThemeIni.ReadInteger(Name, "Y", 0);
  ThemePosition.H = ThemeIni.ReadInteger(Name, "H", 0);
  ThemePosition.W = ThemeIni.ReadInteger(Name, "W", 0);
};

void TTheme::LoadColors();
var
  TStringList SL;
  int C;
  string S;
{
  SL = TStringList.Create;
  ThemeIni.ReadSection("Colors", SL);

  // normal colors
  SetLength(Color, SL.Count);
  for C = 0 to SL.Count-1 do
  {
    Color[C].Name = SL.Strings[C];

    S = ThemeIni.ReadString("Colors", SL.Strings[C], "");

    Color[C].RGB.R = StrToInt(Copy(S, 1, Pos(" " , S)-1))/255;
    Delete(S, 1, Pos(" ", S));

    Color[C].RGB.G = StrToInt(Copy(S, 1, Pos(" " , S)-1))/255;
    Delete(S, 1, Pos(" ", S));

    Color[C].RGB.B = StrToInt(S)/255;
  };

  // skin color
  SetLength(Color, SL.Count + 3);
  C = SL.Count;
  Color[C].Name = "ColorDark";
  Color[C].RGB = GetSystemColor(Skin.Color);  //Ini.Color);

  C = C+1;
  Color[C].Name = "ColorLight";
  Color[C].RGB = Color[C-1].RGB.sqrt();

  C = C+1;
  Color[C].Name = "ColorLightest";
  Color[C].RGB = Color[C-1].RGB.sqrt();

  LastC = C;

  // players colors
  SetLength(Color, Length(Color)+18);

  LoadPlayersColors;

  SL.Free;

};

void LoadPlayersColors();
var
  int C;
{

  C = LastC;

  // P1
  C = C+1;
  Color[C].Name = "P1Dark";
  Color[C].RGB = GetPlayerColor(Ini.PlayerColor[0]);

  C = C+1;
  Color[C].Name = "P1Light";
  Color[C].RGB = GetPlayerLightColor(Ini.PlayerColor[0]);

  C = C+1;
  Color[C].Name = "P1Lightest";
  Color[C].RGB = ColorSqrt(Color[C-1].RGB);

  // P2
  C = C+1;
  Color[C].Name = "P2Dark";
  Color[C].RGB = GetPlayerColor(Ini.PlayerColor[1]);

  C = C+1;
  Color[C].Name = "P2Light";
  Color[C].RGB = GetPlayerLightColor(Ini.PlayerColor[1]);

  C = C+1;
  Color[C].Name = "P2Lightest";
  Color[C].RGB = ColorSqrt(Color[C-1].RGB);

  // P3
  C = C+1;
  Color[C].Name = "P3Dark";
  Color[C].RGB = GetPlayerColor(Ini.PlayerColor[2]);

  C = C+1;
  Color[C].Name = "P3Light";
  Color[C].RGB = GetPlayerLightColor(Ini.PlayerColor[2]);

  C = C+1;
  Color[C].Name = "P3Lightest";
  Color[C].RGB = ColorSqrt(Color[C-1].RGB);

  // P4
  C = C+1;
  Color[C].Name = "P4Dark";
  Color[C].RGB = GetPlayerColor(Ini.PlayerColor[3]);

  C = C+1;
  Color[C].Name = "P4Light";
  Color[C].RGB = GetPlayerLightColor(Ini.PlayerColor[3]);

  C = C+1;
  Color[C].Name = "P4Lightest";
  Color[C].RGB = ColorSqrt(Color[C-1].RGB);

  // P5
  C = C+1;
  Color[C].Name = "P5Dark";
  Color[C].RGB = GetPlayerColor(Ini.PlayerColor[4]);

  C = C+1;
  Color[C].Name = "P5Light";
  Color[C].RGB = GetPlayerLightColor(Ini.PlayerColor[4]);

  C = C+1;
  Color[C].Name = "P5Lightest";
  Color[C].RGB = ColorSqrt(Color[C-1].RGB);

  // P6
  C = C+1;
  Color[C].Name = "P6Dark";
  Color[C].RGB = GetPlayerColor(Ini.PlayerColor[5]);

  C = C+1;
  Color[C].Name = "P6Light";
  Color[C].RGB = GetPlayerLightColor(Ini.PlayerColor[5]);

  C = C+1;
  Color[C].Name = "P6Lightest";
  Color[C].RGB = ColorSqrt(Color[C-1].RGB);

};

void LoadTeamsColors();
var
  int C;
{

  C = LastC;

  // P1
  C = C+1;
  Color[C].Name = "P1Dark";
  Color[C].RGB = GetPlayerColor(Ini.TeamColor[0]);

  C = C+1;
  Color[C].Name = "P1Light";
  Color[C].RGB = GetPlayerLightColor(Ini.TeamColor[0]);

  C = C+1;
  Color[C].Name = "P1Lightest";
  Color[C].RGB = ColorSqrt(Color[C-1].RGB);

  // P2
  C = C+1;
  Color[C].Name = "P2Dark";
  Color[C].RGB = GetPlayerColor(Ini.TeamColor[1]);

  C = C+1;
  Color[C].Name = "P2Light";
  Color[C].RGB = GetPlayerLightColor(Ini.TeamColor[1]);

  C = C+1;
  Color[C].Name = "P2Lightest";
  Color[C].RGB = ColorSqrt(Color[C-1].RGB);

  // P3
  C = C+1;
  Color[C].Name = "P3Dark";
  Color[C].RGB = GetPlayerColor(Ini.TeamColor[2]);

  C = C+1;
  Color[C].Name = "P3Light";
  Color[C].RGB = GetPlayerLightColor(Ini.TeamColor[2]);

  C = C+1;
  Color[C].Name = "P3Lightest";
  Color[C].RGB = ColorSqrt(Color[C-1].RGB);

};

function ColorExists(string Name): int;
var
  int C;
{
  Result = -1;
  for C = 0 to High(Color) do
    if Color[C].Name == Name then
      Result = C;
};

void LoadColor(var R, G, real B; string ColorName);
var
  int C;
{
  C = ColorExists(ColorName);
  if C >= 0 then
  {
    R = Color[C].RGB.R;
    G = Color[C].RGB.G;
    B = Color[C].RGB.B;
  };
};

UCommon::TRGB<double> GetSystemColor(int Color)
{
    switch (Color)
    {
    case 0: {
        // blue
        return (UCommon::TRGB<double> { 71, 175, 247 }) / 255.0;
    }
    case 1:
    {          // green
        return (UCommon::TRGB<double> { 63, 191, 63 }) / 255.0;
    }
    case 2:
    {          // pink
        return (UCommon::TRGB<double> { 255, 175, 247 }) / 255.0;

        /*{          Result.G = 63/255;
                  Result.B = 192/255;}*/
    }
    case 3:
    {          // red
        return (UCommon::TRGB<double> { 247, 71, 71 }) / 255.0;
    }
    //"Violet", "Orange", "Yellow", "Brown", "Black"
    //New Theme-Color Patch
    case 4:
    {          // violet
        return (UCommon::TRGB<double> { 212, 71, 247 }) / 255.0;
    }
    case 5:
    {          // orange
        return (UCommon::TRGB<double> { 247, 144, 71 }) / 255.0;
    }
    case 6:
    {          // yellow
        return (UCommon::TRGB<double> { 230, 230, 95 }) / 255.0;
    }
    case 7:
    {          // brown
        return (UCommon::TRGB<double> { 192, 127, 31 }) / 255.0;
    }
    case 8:
    {          // black
        return (UCommon::TRGB<double> { 0, 0, 0 });
    }
    default:
    {
        // blue
        return (UCommon::TRGB<double> { 71, 175, 247 }) / 255.0;
        }
        //New Theme-Color Patch End
    }
}

UCommon::TRGB<double> GetPlayerColor(int Color)
{
    switch (Color)
    {
        //blue
    case 1:
    {
        return (UCommon::TRGB<double> { 5, 153, 204 }) / 255.0;
    }
    //red
    case 2:
    {
        return (UCommon::TRGB<double> { 230, 0, 0 }) / 255.0;
    }
    //green
    case 3:
    {
        return (UCommon::TRGB<double> { 0, 190, 0 }) / 255.0;
    }
    //yellow
    case 4:
    {
        return (UCommon::TRGB<double> { 255, 255, 0 }) / 255.0;
    }
    //orange
    case 5:
    {
        return (UCommon::TRGB<double> { 255, 127, 0 }) / 255.0;
    }
    //pink
    case 6:
    {
        return (UCommon::TRGB<double> { 255, 110, 180 }) / 255.0;
    }
    //purple
    case 7:
    {
        return (UCommon::TRGB<double> { 175, 0, 210 }) / 255.0;
    }
    //gold
    case 8:
    {
        return (UCommon::TRGB<double> { 218, 165, 32 }) / 255.0;
    }
    //gray
    case 9:
    {
        return (UCommon::TRGB<double> { 150, 150, 150 }) / 255.0;
    }
    //dark blue
    case 10:
    {
        return (UCommon::TRGB<double> { 0, 0, 220 }) / 255.0;
    }
    //sky
    case 11:
    {
        return (UCommon::TRGB<double> { 0, 110, 210 }) / 255.0;
    }
    //cyan
    case 12:
    {
        return (UCommon::TRGB<double> { 0, 215, 215 }) / 255.0;
    }
    //flame
    case 13:
    {
        return (UCommon::TRGB<double> { 210, 70, 0 }) / 255.0;
    }
    //orchid
    case 14:
    {
        return (UCommon::TRGB<double> { 210, 0, 210 }) / 255.0;
    }
    //harlequin
    case 15:
    {
        return (UCommon::TRGB<double> { 110, 210, 0 }) / 255.0;
    }
    //lime
    case 16:
    {
        return (UCommon::TRGB<double> { 160, 210, 0 }) / 255.0;
    }
    default:
    {
        return (UCommon::TRGB<double> { 5, 153, 204 }) / 255.0;
    }
    }
}

UCommon::TRGB<double> GetPlayerLightColor(int Color)
{
    switch (Color)
    {
        //blue
    case 1:
    {
        return (UCommon::TRGB<double> { 145, 215, 240 }) / 255.0;
    }
    //red
    case 2:
    {
        return (UCommon::TRGB<double> { 245, 162, 162 }) / 255.0;
    }
    //green
    case 3:
    {
        return (UCommon::TRGB<double> { 152, 250, 153 }) / 255.0;
    }
    //yellow
    case 4:
    {
        return (UCommon::TRGB<double> { 255, 246, 143 }) / 255.0;
    }
    //orange
    case 5:
    {
        return (UCommon::TRGB<double> { 255, 204, 156 }) / 255.0;
    }
    //pink
    case 6:
    {
        return (UCommon::TRGB<double> { 255, 192, 205 }) / 255.0;
    }
    //purple
    case 7:
    {
        return (UCommon::TRGB<double> { 240, 170, 255 }) / 255.0;
    }
    //gold
    case 8:
    {
        return (UCommon::TRGB<double> { 255, 214, 118 }) / 255.0;
    }
    //gray
    case 9:
    {
        return (UCommon::TRGB<double> { 220, 220, 220 }) / 255.0;
    }
    default:
    {
        return (UCommon::TRGB<double> { 145, 215, 240 }) / 255.0;
    }
    }
}

UCommon::TRGB<double> GetPlayerLightColorV2(int Color)
{
    switch (Color)
    {
        //blue
    case 1:
    {
        return (UCommon::TRGB<double> { 145, 215, 240 }) / 255.0;
    }
    //red
    case 2:
    {
        return (UCommon::TRGB<double> { 245, 162, 162 }) / 255.0;
    }
    //green
    case 3:
    {
        return (UCommon::TRGB<double> { 152, 250, 153 }) / 255.0;
    }
    //yellow
    case 4:
    {
        return (UCommon::TRGB<double> { 255, 246, 143 }) / 255.0;
    }
    //orange
    case 5:
    {
        return (UCommon::TRGB<double> { 255, 204, 156 }) / 255.0;
    }
    //pink
    case 6:
    {
        return (UCommon::TRGB<double> { 255, 192, 205 }) / 255.0;
    }
    //violet
    case 7:
    {
        return (UCommon::TRGB<double> { 240, 170, 255 }) / 255.0;
    }
    //gold
    case 8:
    {
        return (UCommon::TRGB<double> { 255, 214, 118 }) / 255.0;
    }
    //gray
    case 9:
    {
        return (UCommon::TRGB<double> { 220, 220, 220 }) / 255.0;
    }
    //dark blue
    case 10:
    {
        return (UCommon::TRGB<double> { 90, 90, 255 }) / 255.0;
    }
    //sky
    case 11:
    {
        return (UCommon::TRGB<double> { 80, 160, 235 }) / 255.0;
    }
    //cyan
    case 12:
    {
        return (UCommon::TRGB<double> { 150, 230, 230 }) / 255.0;
    }
    //flame
    case 13:
    {
        return (UCommon::TRGB<double> { 230, 130, 80 }) / 255.0;
    }
    //orchid
    case 14:
    {
        return (UCommon::TRGB<double> { 230, 100, 230 }) / 255.0;
    }
    //harlequin
    case 15:
    {
        return (UCommon::TRGB<double> { 160, 230, 90 }) / 255.0;
    }
    //lime
    case 16:
    {
        return (UCommon::TRGB<double> { 190, 230, 100 }) / 255.0;
    }
    default:
    {
        return (UCommon::TRGB<double> { 145, 215, 240 }) / 255.0;
    }
    }
}

UCommon::TRGB<double> GetJukeboxLyricOtherColor(int Line)
{
    switch (Line)
    {
    case 0:
    {
        Result.R = Ini.JukeboxSingLineOtherColorR / 255;
        Result.G = Ini.JukeboxSingLineOtherColorG / 255;
        Result.B = Ini.JukeboxSingLineOtherColorB / 255;
    }
    case 1:
    {
        Result.R = Ini.JukeboxActualLineOtherColorR / 255;
        Result.G = Ini.JukeboxActualLineOtherColorG / 255;
        Result.B = Ini.JukeboxActualLineOtherColorB / 255;
    }
    case 2:
    {
        Result.R = Ini.JukeboxNextLineOtherColorR / 255;
        Result.G = Ini.JukeboxNextLineOtherColorG / 255;
        Result.B = Ini.JukeboxNextLineOtherColorB / 255;
    }
    default:
    {
        Result.R = Ini.JukeboxSingLineOtherColorR / 255;
        Result.G = Ini.JukeboxSingLineOtherColorG / 255;
        Result.B = Ini.JukeboxSingLineOtherColorB / 255;
    }
    }
}

UCommon::TRGB<double> GetJukeboxLyricOtherOutlineColor(int Line)
{
    auto& jkCol = UIni::Ini.juke_box_fill_colors;
    switch (Line)
    {
    case 0:
    {
        return (UCommon::TRGB<double> { 0, 150, 255 }) / 255.0;
    }
    case 1:
    {
        return (UCommon::TRGB<double> { 0, 150, 255 }) / 255.0;
    }
    case 2:
    {
        return (UCommon::TRGB<double> { 0, 150, 255 }) / 255.0;
    }
    default:
    {
        return (UCommon::TRGB<double> { 0, 150, 255 }) / 255.0;
    }
    }
}

UCommon::TRGB<double> GetLyricColor(int Color)
{
    switch (Color)
    {
    case 0:
    {          // blue
        return (UCommon::TRGB<double> { 0, 150, 255 }) / 255.0;
    }
    case 1:
    {          // green
        return (UCommon::TRGB<double> { 63, 191, 63 }) / 255.0;
    }
    case 2:
    {          // pink
        return (UCommon::TRGB<double> { 255, 63, 192 }) / 255.0;
        /*{
      Result.G = 175/255;
      Result.B = 247/255; }*/
    }
    case 3:
    {          // red
        return (UCommon::TRGB<double> { 220, 0, 0 }) / 255.0;
    }
    //"Violet", "Orange", "Yellow", "Brown", "Black"
    //New Theme-Color Patch
    case 4:
    {          // violet
        return (UCommon::TRGB<double> { 180, 63, 230 }) / 255.0;
    }
    case 5:
    {          // orange
        return (UCommon::TRGB<double> { 255, 144, 0 }) / 255.0;
    }
    case 6:
    {          // yellow
        return (UCommon::TRGB<double> { 255, 255, 0 }) / 255.0;
    }
    case 7:
    {          // brown
        return (UCommon::TRGB<double> { 192, 127, 31 }) / 255.0;
    }
    case 8:
    {          // black
        return (UCommon::TRGB<double> { 0, 0, 0 });
    }
    //New Theme-Color Patch End
    // daniel20 colors
    //Turquoise
    case 9:
    {
        return (UCommon::TRGB<double> { 0, 255, 230 }) / 255.0;
    }
    //Salmon
    case 10:
    {
        return (UCommon::TRGB<double> { 255, 127, 102 }) / 255.0;
    }
    //GreenYellow
    case 11:
    {
        return (UCommon::TRGB<double> { 153, 255, 102 }) / 255.0;
    }
    //Lavender
    case 12:
    {
        return (UCommon::TRGB<double> { 204, 204, 255 }) / 255.0;
    }
    //Beige
    case 13:
    {
        return (UCommon::TRGB<double> { 255, 230, 204 }) / 255.0;
    }
    //Teal
    case 14:
    {
        return (UCommon::TRGB<double> { 51, 153, 153 }) / 255.0;
    }
    //Orchid
    case 15:
    {
        return (UCommon::TRGB<double> { 153, 0, 204 }) / 255.0;
    }
    //SteelBlue
    case 16:
    {
        return (UCommon::TRGB<double> { 51, 102, 153 }) / 255.0;
    }
    //Plum
    case 17:
    {
        return (UCommon::TRGB<double> { 255, 153, 255 }) / 255.0;
    }
    //Chocolate
    case 18:
    {
        return (UCommon::TRGB<double> { 138, 92, 46 }) / 255.0;
    }
    //Gold
    case 19:
    {
        return (UCommon::TRGB<double> { 255, 204, 51 }) / 255.0;
    }
    default:
    {
        // blue
        return (UCommon::TRGB<double> { 0, 150, 255 }) / 255.0;
    }
    }
}

UCommon::TRGB<double> GetLyricGrayColor(int Color)
{
    switch (Color)
    {
    case 0:
    {          // black
        return (UCommon::TRGB<double> { 0, 0, 0 }) / 255.0;
    }
    case 1:
    {          // gray +3
        return (UCommon::TRGB<double> { 32, 32, 32 }) / 255.0;
    }
    case 2:
    {          // gray +2
        return (UCommon::TRGB<double> { 64, 64, 64 }) / 255.0;
    }
    case 3:
    {          // gray +1
        return (UCommon::TRGB<double> { 96, 96, 96 }) / 255.0;
    }
    case 4:
    {          // gray
        return (UCommon::TRGB<double> { 128, 128, 128 }) / 255.0;
    }
    case 5:
    {          // gray -1
        return (UCommon::TRGB<double> { 160, 160, 160 }) / 255.0;
    }
    case 6:
    {          // gray -2
        return (UCommon::TRGB<double> { 192, 192, 192 }) / 255.0;
    }
    case 7:
    {          // gray -3
        return (UCommon::TRGB<double> { 214, 214, 214 }) / 255.0;
    }
    case 8:
    {          // white
        return (UCommon::TRGB<double> { 1, 1, 1 }) / 255.0;
    }
    default:
    {
        // black
        return (UCommon::TRGB<double> { 0, 0, 0 }) / 255.0;
    }
    }
}

UCommon::TRGB<double> GetLyricOutlineColor(int Color)
{
    switch (Color)
    {
    case 0:
    {          // black
        return (UCommon::TRGB<double> { 0, 0, 0 });
    }
    case 1:
    {          // white
        return (UCommon::TRGB<double> { 1, 1, 1 });
    }
    }
}

UCommon::TRGB<double> GetLyricBarColor(int Color)
{
    return GetPlayerColor(Color);
}

void TTheme::ThemeSave(const std::string FileName)
/*var
  int I;*/
{
  #ifdef THEMESAVE
  ThemeIni = TIniFile(FileName);
  #else
  ThemeIni = TMemIniFile(FileName);
  #endif

  ThemeSaveBasic(Loading, "Loading");

  ThemeSaveBasic(Main, "Main");
  ThemeSaveText(Main.TextDescription, "MainTextDescription");
  ThemeSaveText(Main.TextDescriptionLong, "MainTextDescriptionLong");
  ThemeSaveButton(Main.ButtonSolo, "MainButtonSolo");

  ThemeSaveButton(Main.ButtonEditor, "MainButtonEditor");
  ThemeSaveButton(Main.ButtonOptions, "MainButtonOptions");
  ThemeSaveButton(Main.ButtonExit, "MainButtonExit");

  ThemeSaveBasic(Name, "Name");

  //ThemeSaveButton(Name.PlayerName, "NameButtonPlayer");

  ThemeSaveBasic(Level, "Level");
  ThemeSaveButton(Level.ButtonEasy, "LevelButtonEasy");
  ThemeSaveButton(Level.ButtonMedium, "LevelButtonMedium");
  ThemeSaveButton(Level.ButtonHard, "LevelButtonHard");

  ThemeSaveBasic(Song, "Song");
  ThemeSaveText(Song.TextArtist, "SongTextArtist");
  ThemeSaveText(Song.TextTitle, "SongTextTitle");
  ThemeSaveText(Song.TextNumber, "SongTextNumber");

  //Show CAt in Top Left Mod
  ThemeSaveText(Song.TextCat, "SongTextCat");
  ThemeSaveStatic(Song.StaticCat, "SongStaticCat");

  ThemeSaveBasic(Sing, "Sing");

  // lyrics bar
  ThemeSaveStatic(Sing.StaticLyricsBar, "SingLyricsBar");
  ThemeSaveStatic(Sing.StaticLyricsBarDuet, "SingLyricsBarDuet");

  //TimeBar mod
  ThemeSaveStatic(Sing.StaticTimeBar, "SingTimeBar");
  ThemeSaveStatic(Sing.StaticTimeProgress, "SingTimeProgress");
  ThemeSaveText(Sing.TextTimeLabelText, "SingTimeLabelText");
  ThemeSaveText(Sing.TextTimeText, "SingTimeText");
  //eoa TimeBar mod

  ThemeSaveStatic(Sing.StaticP1, "SingP1Static");
  ThemeSaveText(Sing.TextP1, "SingP1Text");
  ThemeSaveStatic(Sing.StaticP1ScoreBG, "SingP1Static2");
  ThemeSaveText(Sing.TextP1Score, "SingP1TextScore");

  //moveable singbar mod
  ThemeSaveStatic(Sing.StaticP1SingBar, "SingP1SingBar");
  ThemeSaveStatic(Sing.StaticP1TwoPSingBar, "SingP1TwoPSingBar");
  ThemeSaveStatic(Sing.StaticP1ThreePSingBar, "SingP1ThreePSingBar");
  ThemeSaveStatic(Sing.StaticP2RSingBar, "SingP2RSingBar");
  ThemeSaveStatic(Sing.StaticP2MSingBar, "SingP2MSingBar");
  ThemeSaveStatic(Sing.StaticP3SingBar, "SingP3SingBar");
  //eoa moveable singbar

  //Added for ps3 skin
  //This one is shown in 2/4P mode
  ThemeSaveStatic(Sing.StaticP1TwoP, "SingP1TwoPStatic");
  ThemeSaveText(Sing.TextP1TwoP, "SingP1TwoPText");
  ThemeSaveStatic(Sing.StaticP1TwoPScoreBG, "SingP1TwoPStatic2");
  ThemeSaveText(Sing.TextP1TwoPScore, "SingP1TwoPTextScore");

  //This one is shown in 3/6P mode
  ThemeSaveStatic(Sing.StaticP1ThreeP, "SingP1ThreePStatic");
  ThemeSaveText(Sing.TextP1ThreeP, "SingP1ThreePText");
  ThemeSaveStatic(Sing.StaticP1ThreePScoreBG, "SingP1ThreePStatic2");
  ThemeSaveText(Sing.TextP1ThreePScore, "SingP1ThreePTextScore");
  //eoa

  ThemeSaveStatic(Sing.StaticP2R, "SingP2RStatic");
  ThemeSaveText(Sing.TextP2R, "SingP2RText");
  ThemeSaveStatic(Sing.StaticP2RScoreBG, "SingP2RStatic2");
  ThemeSaveText(Sing.TextP2RScore, "SingP2RTextScore");

  ThemeSaveStatic(Sing.StaticP2M, "SingP2MStatic");
  ThemeSaveText(Sing.TextP2M, "SingP2MText");
  ThemeSaveStatic(Sing.StaticP2MScoreBG, "SingP2MStatic2");
  ThemeSaveText(Sing.TextP2MScore, "SingP2MTextScore");

  ThemeSaveStatic(Sing.StaticP3R, "SingP3RStatic");
  ThemeSaveText(Sing.TextP3R, "SingP3RText");
  ThemeSaveStatic(Sing.StaticP3RScoreBG, "SingP3RStatic2");
  ThemeSaveText(Sing.TextP3RScore, "SingP3RTextScore");

  ThemeSaveBasic(Score, "Score");
  ThemeSaveText(Score.TextArtist, "ScoreTextArtist");
  ThemeSaveText(Score.TextTitle, "ScoreTextTitle");

  ThemeSaveBasic(Top5, "Top5");
  ThemeSaveText(Top5.TextLevel, "Top5TextLevel");
  ThemeSaveText(Top5.TextArtistTitle, "Top5TextArtistTitle");
  ThemeSaveStatics(Top5.StaticNumber, "Top5StaticNumber");
  ThemeSaveTexts(Top5.TextNumber, "Top5TextNumber");
  ThemeSaveTexts(Top5.TextName, "Top5TextName");
  ThemeSaveTexts(Top5.TextScore, "Top5TextScore");

  ThemeIni.Free;
};

void TTheme::ThemeSaveBasic(TThemeBasic Theme, const std::string Name)
{
  ThemeIni.WriteInteger(Name, "Texts", Length(Theme.Text));

  ThemeSaveBackground(Theme.Background, Name + "Background");
  ThemeSaveStatics(Theme.Statics, Name + "Static");
  ThemeSaveTexts(Theme.Text, Name + "Text");
};

void TTheme::ThemeSaveBackground(TThemeBackground ThemeBackground, const std::string Name)
{
  if ThemeBackground.Tex != "" then
    ThemeIni.WriteString(Name, "Tex", ThemeBackground.Tex)
  else
  {
    ThemeIni.EraseSection(Name);
  };
};

void TTheme::ThemeSaveStatic(TThemeStatic ThemeStatic, const std::string Name)
{
  ThemeIni.WriteInteger(Name, "X", ThemeStatic.X);
  ThemeIni.WriteInteger(Name, "Y", ThemeStatic.Y);
  ThemeIni.WriteInteger(Name, "W", ThemeStatic.W);
  ThemeIni.WriteInteger(Name, "H", ThemeStatic.H);

  ThemeIni.WriteString(Name, "Tex", ThemeStatic.Tex);
  ThemeIni.WriteString(Name, "Type", TextureTypeToStr(ThemeStatic.Typ));
  ThemeIni.WriteString(Name, "Color", ThemeStatic.Color);

  ThemeIni.WriteFloat(Name, "TexX1", ThemeStatic.TexX1);
  ThemeIni.WriteFloat(Name, "TexY1", ThemeStatic.TexY1);
  ThemeIni.WriteFloat(Name, "TexX2", ThemeStatic.TexX2);
  ThemeIni.WriteFloat(Name, "TexY2", ThemeStatic.TexY2);
};

void TTheme::ThemeSaveStatics(AThemeStatic ThemeStatic, const std::string Name)
var
  int S;
{
  for S = 0 to High(ThemeStatic) do
    ThemeSaveStatic(ThemeStatic[S], Name + {"Static" +} std::to_string(S+1));

  ThemeIni.EraseSection(Name + {"Static" + }std::to_string(S+1));
};

void TTheme::ThemeSaveText(TThemeText ThemeText; const std::string Name)
{
  ThemeIni.WriteInteger(Name, "X", ThemeText.X);
  ThemeIni.WriteInteger(Name, "Y", ThemeText.Y);

  ThemeIni.WriteInteger(Name, "Font", ThemeText.Font);
  ThemeIni.WriteInteger(Name, "Style", ThemeText.Style);
  ThemeIni.WriteInteger(Name, "Size", ThemeText.Size);
  ThemeIni.WriteInteger(Name, "Align", ThemeText.Align);

  ThemeIni.WriteString(Name, "Text", ThemeText.Text);
  ThemeIni.WriteString(Name, "Color", ThemeText.Color);

  ThemeIni.WriteBool(Name, "Reflection", ThemeText.Reflection);
  ThemeIni.WriteFloat(Name, "ReflectionSpacing", ThemeText.ReflectionSpacing);
};

void TTheme::ThemeSaveTexts(AThemeText ThemeText, const std::string Name)
/*var
  int T;*/
{
  for T = 0 to Length(ThemeText)-1 do
    ThemeSaveText(ThemeText[T], Name + {"Text" + }std::to_string(T+1));

  ThemeIni.EraseSection(Name + {"Text" + }std::to_string(T+1));
};

void TTheme::ThemeSaveButton(TThemeButton ThemeButton, const std::string Name)
/*var
  int T;*/
{
  ThemeIni.WriteString(Name, "Tex", ThemeButton.Tex);
  ThemeIni.WriteInteger(Name, "X", ThemeButton.X);
  ThemeIni.WriteInteger(Name, "Y", ThemeButton.Y);
  ThemeIni.WriteInteger(Name, "W", ThemeButton.W);
  ThemeIni.WriteInteger(Name, "H", ThemeButton.H);
  ThemeIni.WriteString(Name, "Type", TextureTypeToStr(ThemeButton.Typ));
  ThemeIni.WriteInteger(Name, "Texts", Length(ThemeButton.Text));

  ThemeIni.WriteString(Name, "Color", ThemeButton.Color);

{  ThemeButton.ColR = ThemeIni.ReadFloat(Name, "ColR", 1);
  ThemeButton.ColG = ThemeIni.ReadFloat(Name, "ColG", 1);
  ThemeButton.ColB = ThemeIni.ReadFloat(Name, "ColB", 1);
  ThemeButton.Int =  ThemeIni.ReadFloat(Name, "Int", 1);
  ThemeButton.DColR = ThemeIni.ReadFloat(Name, "DColR", 1);
  ThemeButton.DColG = ThemeIni.ReadFloat(Name, "DColG", 1);
  ThemeButton.DColB = ThemeIni.ReadFloat(Name, "DColB", 1);
  ThemeButton.DInt =  ThemeIni.ReadFloat(Name, "DInt", 1);}

{  C = ColorExists(ThemeIni.ReadString(Name, "Color", ""));
  if C >= 0 then
  {
    ThemeButton.ColR = Color[C].RGB.R;
    ThemeButton.ColG = Color[C].RGB.G;
    ThemeButton.ColB = Color[C].RGB.B;
  };

  C = ColorExists(ThemeIni.ReadString(Name, "DColor", ""));
  if C >= 0 then
  {
    ThemeButton.DColR = Color[C].RGB.R;
    ThemeButton.DColG = Color[C].RGB.G;
    ThemeButton.DColB = Color[C].RGB.B;
  };}

  for T = 0 to High(ThemeButton.Text) do
    ThemeSaveText(ThemeButton.Text[T], Name + "Text" + std::to_string(T+1));
};

void TTheme::ThemePartyLoad()
{

  ThemeIni = TMemIniFile.Create(Themes[Ini.Theme].FileName.string());

  //Party 
 Screens  //Party NewRound
  ThemeLoadBasic(PartyNewRound, "PartyNewRound");

  ThemeLoadText (PartyNewRound.TextRound1, "PartyNewRoundTextRound1");
  ThemeLoadText (PartyNewRound.TextRound2, "PartyNewRoundTextRound2");
  ThemeLoadText (PartyNewRound.TextRound3, "PartyNewRoundTextRound3");
  ThemeLoadText (PartyNewRound.TextRound4, "PartyNewRoundTextRound4");
  ThemeLoadText (PartyNewRound.TextRound5, "PartyNewRoundTextRound5");
  ThemeLoadText (PartyNewRound.TextRound6, "PartyNewRoundTextRound6");
  ThemeLoadText (PartyNewRound.TextRound7, "PartyNewRoundTextRound7");
  ThemeLoadText (PartyNewRound.TextWinner1, "PartyNewRoundTextWinner1");
  ThemeLoadText (PartyNewRound.TextWinner2, "PartyNewRoundTextWinner2");
  ThemeLoadText (PartyNewRound.TextWinner3, "PartyNewRoundTextWinner3");
  ThemeLoadText (PartyNewRound.TextWinner4, "PartyNewRoundTextWinner4");
  ThemeLoadText (PartyNewRound.TextWinner5, "PartyNewRoundTextWinner5");
  ThemeLoadText (PartyNewRound.TextWinner6, "PartyNewRoundTextWinner6");
  ThemeLoadText (PartyNewRound.TextWinner7, "PartyNewRoundTextWinner7");
  ThemeLoadText (PartyNewRound.TextNextRound, "PartyNewRoundTextNextRound");
  ThemeLoadText (PartyNewRound.TextNextRoundNo, "PartyNewRoundTextNextRoundNo");
  ThemeLoadText (PartyNewRound.TextNextPlayer1, "PartyNewRoundTextNextPlayer1");
  ThemeLoadText (PartyNewRound.TextNextPlayer2, "PartyNewRoundTextNextPlayer2");
  ThemeLoadText (PartyNewRound.TextNextPlayer3, "PartyNewRoundTextNextPlayer3");

  ThemeLoadStatic (PartyNewRound.StaticRound1, "PartyNewRoundStaticRound1");
  ThemeLoadStatic (PartyNewRound.StaticRound2, "PartyNewRoundStaticRound2");
  ThemeLoadStatic (PartyNewRound.StaticRound3, "PartyNewRoundStaticRound3");
  ThemeLoadStatic (PartyNewRound.StaticRound4, "PartyNewRoundStaticRound4");
  ThemeLoadStatic (PartyNewRound.StaticRound5, "PartyNewRoundStaticRound5");
  ThemeLoadStatic (PartyNewRound.StaticRound6, "PartyNewRoundStaticRound6");
  ThemeLoadStatic (PartyNewRound.StaticRound7, "PartyNewRoundStaticRound7");

  ThemeLoadText (PartyNewRound.TextScoreTeam1, "PartyNewRoundTextScoreTeam1");
  ThemeLoadText (PartyNewRound.TextScoreTeam2, "PartyNewRoundTextScoreTeam2");
  ThemeLoadText (PartyNewRound.TextScoreTeam3, "PartyNewRoundTextScoreTeam3");
  ThemeLoadText (PartyNewRound.TextNameTeam1, "PartyNewRoundTextNameTeam1");
  ThemeLoadText (PartyNewRound.TextNameTeam2, "PartyNewRoundTextNameTeam2");
  ThemeLoadText (PartyNewRound.TextNameTeam3, "PartyNewRoundTextNameTeam3");

  ThemeLoadText (PartyNewRound.TextTeam1Players, "PartyNewRoundTextTeam1Players");
  ThemeLoadText (PartyNewRound.TextTeam2Players, "PartyNewRoundTextTeam2Players");
  ThemeLoadText (PartyNewRound.TextTeam3Players, "PartyNewRoundTextTeam3Players");

  ThemeLoadStatic (PartyNewRound.StaticTeam1, "PartyNewRoundStaticTeam1");
  ThemeLoadStatic (PartyNewRound.StaticTeam2, "PartyNewRoundStaticTeam2");
  ThemeLoadStatic (PartyNewRound.StaticTeam3, "PartyNewRoundStaticTeam3");
  ThemeLoadStatic (PartyNewRound.StaticNextPlayer1, "PartyNewRoundStaticNextPlayer1");
  ThemeLoadStatic (PartyNewRound.StaticNextPlayer2, "PartyNewRoundStaticNextPlayer2");
  ThemeLoadStatic (PartyNewRound.StaticNextPlayer3, "PartyNewRoundStaticNextPlayer3");

  //Party Score
  ThemeLoadBasic(PartyScore, "PartyScore");

  ThemeLoadText (PartyScore.TextScoreTeam1, "PartyScoreTextScoreTeam1");
  ThemeLoadText (PartyScore.TextScoreTeam2, "PartyScoreTextScoreTeam2");
  ThemeLoadText (PartyScore.TextScoreTeam3, "PartyScoreTextScoreTeam3");
  ThemeLoadText (PartyScore.TextNameTeam1, "PartyScoreTextNameTeam1");
  ThemeLoadText (PartyScore.TextNameTeam2, "PartyScoreTextNameTeam2");
  ThemeLoadText (PartyScore.TextNameTeam3, "PartyScoreTextNameTeam3");

  ThemeLoadStatic (PartyScore.StaticTeam1, "PartyScoreStaticTeam1");
  ThemeLoadStatic (PartyScore.StaticTeam1BG, "PartyScoreStaticTeam1BG");
  ThemeLoadStatic (PartyScore.StaticTeam1Deco, "PartyScoreStaticTeam1Deco");
  ThemeLoadStatic (PartyScore.StaticTeam2, "PartyScoreStaticTeam2");
  ThemeLoadStatic (PartyScore.StaticTeam2BG, "PartyScoreStaticTeam2BG");
  ThemeLoadStatic (PartyScore.StaticTeam2Deco, "PartyScoreStaticTeam2Deco");
  ThemeLoadStatic (PartyScore.StaticTeam3, "PartyScoreStaticTeam3");
  ThemeLoadStatic (PartyScore.StaticTeam3BG, "PartyScoreStaticTeam3BG");
  ThemeLoadStatic (PartyScore.StaticTeam3Deco, "PartyScoreStaticTeam3Deco");

  //Load Party Score DecoTextures Object
  PartyScore.DecoTextures.ChangeTextures = (ThemeIni.ReadInteger("PartyScoreDecoTextures", "ChangeTextures", 0) == 1);
  PartyScore.DecoTextures.FirstTexture   =  ThemeIni.ReadString("PartyScoreDecoTextures",  "FirstTexture", "");
  PartyScore.DecoTextures.FirstTyp       =  ParseTextureType(ThemeIni.ReadString("PartyScoreDecoTextures", "FirstTyp", ""), TEXTURE_TYPE_COLORIZED);
  PartyScore.DecoTextures.FirstColor     =  ThemeIni.ReadString("PartyScoreDecoTextures",  "FirstColor", "Black");

  PartyScore.DecoTextures.SecondTexture  =  ThemeIni.ReadString("PartyScoreDecoTextures",  "SecondTexture", "");
  PartyScore.DecoTextures.SecondTyp      =  ParseTextureType(ThemeIni.ReadString("PartyScoreDecoTextures", "SecondTyp", ""), TEXTURE_TYPE_COLORIZED);
  PartyScore.DecoTextures.SecondColor    =  ThemeIni.ReadString("PartyScoreDecoTextures",  "SecondColor", "Black");

  PartyScore.DecoTextures.ThirdTexture   =  ThemeIni.ReadString("PartyScoreDecoTextures",  "ThirdTexture", "");
  PartyScore.DecoTextures.ThirdTyp       =  ParseTextureType(ThemeIni.ReadString("PartyScoreDecoTextures", "ThirdTyp", ""), TEXTURE_TYPE_COLORIZED);
  PartyScore.DecoTextures.ThirdColor     =  ThemeIni.ReadString("PartyScoreDecoTextures",  "ThirdColor", "Black");

  ThemeLoadText (PartyScore.TextWinner, "PartyScoreTextWinner");

  //Party Win
  ThemeLoadBasic(PartyWin, "PartyWin");

  ThemeLoadText (PartyWin.TextScoreTeam1,    "PartyWinTextScoreTeam1");
  ThemeLoadText (PartyWin.TextScoreTeam2,    "PartyWinTextScoreTeam2");
  ThemeLoadText (PartyWin.TextScoreTeam3,    "PartyWinTextScoreTeam3");
  ThemeLoadText (PartyWin.TextNameTeam1,     "PartyWinTextNameTeam1");
  ThemeLoadText (PartyWin.TextNameTeam2,     "PartyWinTextNameTeam2");
  ThemeLoadText (PartyWin.TextNameTeam3,     "PartyWinTextNameTeam3");

  ThemeLoadStatic (PartyWin.StaticTeam1,     "PartyWinStaticTeam1");
  ThemeLoadStatic (PartyWin.StaticTeam1BG,   "PartyWinStaticTeam1BG");
  ThemeLoadStatic (PartyWin.StaticTeam1Deco, "PartyWinStaticTeam1Deco");
  ThemeLoadStatic (PartyWin.StaticTeam2,     "PartyWinStaticTeam2");
  ThemeLoadStatic (PartyWin.StaticTeam2BG,   "PartyWinStaticTeam2BG");
  ThemeLoadStatic (PartyWin.StaticTeam2Deco, "PartyWinStaticTeam2Deco");
  ThemeLoadStatic (PartyWin.StaticTeam3,     "PartyWinStaticTeam3");
  ThemeLoadStatic (PartyWin.StaticTeam3BG,   "PartyWinStaticTeam3BG");
  ThemeLoadStatic (PartyWin.StaticTeam3Deco, "PartyWinStaticTeam3Deco");

  ThemeLoadText (PartyWin.TextWinner,        "PartyWinTextWinner");


  ThemeIni.Free;

};

void TTheme::ThemeScoreLoad()
/*var
  int I;
  string prefix;*/
{

  ThemeIni = TMemIniFile.Create(Themes[Ini.Theme].FileName.string());

  // Score
  ThemeLoadBasic(Score, "Score");

  ThemeLoadText(Score.TextArtist, "ScoreTextArtist");
  ThemeLoadText(Score.TextTitle, "ScoreTextTitle");
  ThemeLoadText(Score.TextArtistTitle, "ScoreTextArtistTitle");

  if (Ini.Players < 3) or (Ini.Screens == 1) then
    prefix = ""
  else
  {
    // 4 players 1 screen
    if (Ini.Players == 3) then
      prefix = "FourP";

    // 6 players 1 screen
    if (Ini.Players == 4) then
      prefix = "SixP";
  };

  for I = 1 to 6 do
  {
    ThemeLoadStatics(Score.PlayerStatic[I],        "Score" + prefix + "Player" + std::to_string(I) + "Static");
    ThemeLoadTexts(Score.PlayerTexts[I],           "Score" + prefix + "Player" + std::to_string(I) + "Text");
    ThemeLoadStatic(Score.AvatarStatic[I],         "Score" + prefix + "Player" + std::to_string(I) + "Avatar");

    ThemeLoadText(Score.TextName[I],               "Score" + prefix + "TextName"             + std::to_string(I));
    ThemeLoadText(Score.TextScore[I],              "Score" + prefix + "TextScore"            + std::to_string(I));
    ThemeLoadText(Score.TextNotes[I],              "Score" + prefix + "TextNotes"            + std::to_string(I));
    ThemeLoadText(Score.TextNotesScore[I],         "Score" + prefix + "TextNotesScore"       + std::to_string(I));
    ThemeLoadText(Score.TextLineBonus[I],          "Score" + prefix + "TextLineBonus"        + std::to_string(I));
    ThemeLoadText(Score.TextLineBonusScore[I],     "Score" + prefix + "TextLineBonusScore"   + std::to_string(I));
    ThemeLoadText(Score.TextGoldenNotes[I],        "Score" + prefix + "TextGoldenNotes"      + std::to_string(I));
    ThemeLoadText(Score.TextGoldenNotesScore[I],   "Score" + prefix + "TextGoldenNotesScore" + std::to_string(I));
    ThemeLoadText(Score.TextTotal[I],              "Score" + prefix + "TextTotal"            + std::to_string(I));
    ThemeLoadText(Score.TextTotalScore[I],         "Score" + prefix + "TextTotalScore"       + std::to_string(I));

    ThemeLoadStatic(Score.StaticBoxLightest[I],    "Score" + prefix + "StaticBoxLightest"    + std::to_string(I));
    ThemeLoadStatic(Score.StaticBoxLight[I],       "Score" + prefix + "StaticBoxLight"       + std::to_string(I));
    ThemeLoadStatic(Score.StaticBoxDark[I],        "Score" + prefix + "StaticBoxDark"        + std::to_string(I));

    ThemeLoadStatic(Score.StaticBackLevel[I],      "Score" + prefix + "StaticBackLevel"      + std::to_string(I));
    ThemeLoadStatic(Score.StaticBackLevelRound[I], "Score" + prefix + "StaticBackLevelRound" + std::to_string(I));
    ThemeLoadStatic(Score.StaticLevel[I],          "Score" + prefix + "StaticLevel"          + std::to_string(I));
    ThemeLoadStatic(Score.StaticLevelRound[I],     "Score" + prefix + "StaticLevelRound"     + std::to_string(I));
    ThemeLoadStatic(Score.StaticRatings[I],        "Score" + prefix + "StaticRatingPicture"  + std::to_string(I));
  };

  ThemeIni.Free;
};

void TTheme::ThemeSongLoad()
/*var
  I, int C;
  string prefix;*/
{
  case (TSongMenuMode(Ini.SongMenu)) of
    {
    prefix = "Roulette" smRoulette;
    prefix = "Chessboard" smChessboard;
    prefix = "Carousel" smCarousel;
    prefix = "SlotMachine" smSlotMachine;
    prefix = "Slide" smSlide;
    prefix = "List" smList;
    prefix = "Mosaic" smMosaic;
  };

  ThemeIni = TMemIniFile.Create(Themes[Ini.Theme].FileName.string());

  // Song
  ThemeLoadBasic(Song, "Song" + prefix);

  ThemeLoadText(Song.TextArtist, "Song" + prefix + "TextArtist");
  ThemeLoadText(Song.TextTitle, "Song" + prefix + "TextTitle");
  ThemeLoadText(Song.TextNumber, "Song" + prefix + "TextNumber");
  ThemeLoadText(Song.TextYear, "Song" + prefix + "TextYear");

  // medley playlist
  Song.TextMedleyMax = ThemeIni.ReadInteger("Song" + prefix + "TextMedleyMax", "N", 4);

  SetLength(Song.TextArtistMedley, Song.TextMedleyMax);
  SetLength(Song.TextTitleMedley, Song.TextMedleyMax);
  SetLength(Song.TextNumberMedley, Song.TextMedleyMax);
  SetLength(Song.StaticMedley, Song.TextMedleyMax);

  for I = 0 to Song.TextMedleyMax - 1 do
  {
    ThemeLoadText(Song.TextArtistMedley[I], "Song" + prefix + "TextMedleyArtist" + std::to_string(I + 1));
    ThemeLoadText(Song.TextTitleMedley[I], "Song" + prefix + "TextMedleyTitle" + std::to_string(I + 1));
    ThemeLoadText(Song.TextNumberMedley[I], "Song" + prefix + "TextMedleyNumber" + std::to_string(I + 1));
    ThemeLoadStatic(Song.StaticMedley[I], "Song" + prefix + "StaticMedley" + std::to_string(I + 1));
  };

  //Video Icon Mod
  ThemeLoadStatic(Song.VideoIcon, "Song" + prefix + "VideoIcon");

  //Medley Icons
  ThemeLoadStatic(Song.MedleyIcon, "Song" + prefix + "MedleyIcon");
  ThemeLoadStatic(Song.CalculatedMedleyIcon, "Song" + prefix + "CalculatedMedleyIcon");

  //Duet Icon
  ThemeLoadStatic(Song.DuetIcon, "Song" + prefix + "DuetIcon");

  //Rap Icon
  ThemeLoadStatic(Song.RapIcon, "Song" + prefix + "RapIcon");

  //Show Cat in TopLeft Mod
  ThemeLoadStatic(Song.StaticCat, "Song" + prefix + "StaticCat");
  ThemeLoadText(Song.TextCat, "Song" + prefix + "TextCat");

  //Load Cover Pos and Size from Theme Mod
  Song.Cover.X = ThemeIni.ReadInteger("Song" + prefix + "Cover", "X", 300);
  Song.Cover.Y = ThemeIni.ReadInteger("Song" + prefix + "Cover", "Y", 190);
  Song.Cover.W = ThemeIni.ReadInteger("Song" + prefix + "Cover", "W", 300);
  Song.Cover.H = ThemeIni.ReadInteger("Song" + prefix + "Cover", "H", 200);

  // 0 - roulette
  // 1 - chessboard
  // 2 - carousel
  // 3 - slotmachine
  // 4 - slide
  // 5 - list
  // 6 - mosaic
  
  if (TSongMenuMode(Ini.SongMenu) in [smChessboard, smMosaic]) then
  {
    Song.Cover.Rows = ThemeIni.ReadInteger("Song" + prefix + "Cover", "Rows", 4);
    Song.Cover.Cols = ThemeIni.ReadInteger("Song" + prefix + "Cover", "Cols", 4);
    Song.Cover.Padding = ThemeIni.ReadInteger("Song" + prefix + "Cover", "Padding", 0);
    Song.Cover.SelectX = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectX", 300);
    Song.Cover.SelectY = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectY", 120);
    Song.Cover.SelectW = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectW", 325);
    Song.Cover.SelectH = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectH", 200);
    Song.Cover.SelectReflection = ThemeIni.ReadBool("Song" + prefix + "Cover", "SelectReflection", false);
    Song.Cover.SelectReflectionSpacing = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectReflectionSpacing", 0);
    Song.Cover.ZoomThumbW = ThemeIni.ReadInteger("Song" + prefix + "Cover", "ZoomThumbW", 120);
    Song.Cover.ZoomThumbH = ThemeIni.ReadInteger("Song" + prefix + "Cover", "ZoomThumbH", 120);
    Song.Cover.ZoomThumbStyle = ThemeIni.ReadInteger("Song" + prefix + "Cover", "ZoomThumbStyle", 0);
    Song.Cover.Tex = ThemeIni.ReadString("Song" + prefix + "Cover",  "Text", "");
  };

  if (TSongMenuMode(Ini.SongMenu) in [smCarousel, smSlide]) then
  {
    Song.Cover.Padding = ThemeIni.ReadInteger("Song" + prefix + "Cover", "Padding", 60);
  };

  if (TSongMenuMode(Ini.SongMenu) == smList) then
  {
    Song.Cover.SelectX = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectX", 300);
    Song.Cover.SelectY = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectY", 120);
    Song.Cover.SelectW = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectW", 325);
    Song.Cover.SelectH = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectH", 200);
    Song.Cover.SelectReflection = ThemeIni.ReadBool("Song" + prefix + "Cover", "SelectReflection", false);
    Song.Cover.SelectReflectionSpacing = ThemeIni.ReadInteger("Song" + prefix + "Cover", "SelectReflectionSpacing", 0);
    Song.Cover.Padding = ThemeIni.ReadInteger("Song" + prefix + "Cover", "Padding", 4);

    Song.ListCover.Rows = ThemeIni.ReadInteger("Song" + prefix + "SelectSong", "Rows", 5);
    Song.ListCover.X = ThemeIni.ReadInteger("Song" + prefix + "SelectSong", "X", 300);
    Song.ListCover.Y = ThemeIni.ReadInteger("Song" + prefix + "SelectSong", "Y", 120);
    Song.ListCover.W = ThemeIni.ReadInteger("Song" + prefix + "SelectSong", "W", 325);
    Song.ListCover.H = ThemeIni.ReadInteger("Song" + prefix + "SelectSong", "H", 200);
    Song.ListCover.Z = ThemeIni.ReadInteger("Song" + prefix + "SelectSong", "Z", 1);
    Song.ListCover.Reflection = ThemeIni.ReadBool("Song" + prefix + "SelectSong", "Reflection", false);
    Song.ListCover.ReflectionSpacing = ThemeIni.ReadInteger("Song" + prefix + "SelectSong", "ReflectionSpacing", 0);
    Song.ListCover.Padding = ThemeIni.ReadInteger("Song" + prefix + "SelectSong", "Padding", 4);

    Song.ListCover.Typ   = ParseTextureType(ThemeIni.ReadString("Song" + prefix + "SelectSong", "Type", ""), TEXTURE_TYPE_PLAIN);
    Song.ListCover.Tex = ThemeIni.ReadString("Song" + prefix + "SelectSong", "Tex", "");
    Song.ListCover.DTex = ThemeIni.ReadString("Song" + prefix + "SelectSong", "DTex", "");

    Song.ListCover.Color = ThemeIni.ReadString("Song" + prefix + "SelectSong", "Color", "");

    C = ColorExists(Song.ListCover.Color);
    if C >= 0 then
    {
      Song.ListCover.ColR = Color[C].RGB.R;
      Song.ListCover.ColG = Color[C].RGB.G;
      Song.ListCover.ColB = Color[C].RGB.B;
    };

    Song.ListCover.DColor = ThemeIni.ReadString("Song" + prefix + "SelectSong", "DColor", "");

    C = ColorExists(Song.ListCover.DColor);
    if C >= 0 then
    {
      Song.ListCover.DColR = Color[C].RGB.R;
      Song.ListCover.DColG = Color[C].RGB.G;
      Song.ListCover.DColB = Color[C].RGB.B;
    };

  };

  //  Song.Cover.Style = ThemeIni.ReadInteger("SongCover", "Style", 4);
  Song.Cover.Reflections = (ThemeIni.ReadInteger("Song" + prefix + "Cover", "Reflections", 0) == 1);
  //Load Cover Pos and Size from Theme Mod End

  ThemeLoadEqualizer(Song.Equalizer, "Song" + prefix + "Equalizer");

  //Screen Song Scores
  ThemeLoadText(Song.TextScore, "Song" + prefix + "TextScore");
  ThemeLoadText(Song.TextMaxScore, "Song" + prefix + "TextMaxScore");
  ThemeLoadText(Song.TextMediaScore, "Song" + prefix + "TextMediaScore");
  ThemeLoadText(Song.TextMaxScore2, "Song" + prefix + "TextMaxScore2");
  ThemeLoadText(Song.TextMediaScore2, "Song" + prefix + "TextMediaScore2");
  ThemeLoadText(Song.TextScoreUser, "Song" + prefix + "TextScoreUser");
  ThemeLoadText(Song.TextMaxScoreLocal, "Song" + prefix + "TextMaxScoreLocal");
  ThemeLoadText(Song.TextMediaScoreLocal, "Song" + prefix + "TextMediaScoreLocal");
  ThemeLoadText(Song.TextScoreUserLocal, "Song" + prefix + "TextScoreUserLocal");

  //Party and Non Party specific Statics and Texts
  ThemeLoadStatics (Song.StaticParty, "Song" + prefix + "StaticParty");
  ThemeLoadTexts (Song.TextParty, "Song" + prefix + "TextParty");

  ThemeLoadStatics (Song.StaticNonParty, "Song" + prefix + "StaticNonParty");
  ThemeLoadTexts (Song.TextNonParty, "Song" + prefix + "TextNonParty");

  // Duet Singers
  ThemeLoadStatic (Song.Static2PlayersDuetSingerP1, "Song" + prefix + "Static2PlayersDuetSingerP1");
  ThemeLoadStatic (Song.Static2PlayersDuetSingerP2, "Song" + prefix + "Static2PlayersDuetSingerP2");
  ThemeLoadText (Song.Text2PlayersDuetSingerP1, "Song" + prefix + "Text2PlayersDuetSingerP1");
  ThemeLoadText (Song.Text2PlayersDuetSingerP2, "Song" + prefix + "Text2PlayersDuetSingerP2");

  ThemeLoadStatic (Song.Static3PlayersDuetSingerP1, "Song" + prefix + "Static3PlayersDuetSingerP1");
  ThemeLoadStatic (Song.Static3PlayersDuetSingerP2, "Song" + prefix + "Static3PlayersDuetSingerP2");
  ThemeLoadStatic (Song.Static3PlayersDuetSingerP3, "Song" + prefix + "Static3PlayersDuetSingerP3");
  ThemeLoadText (Song.Text3PlayersDuetSingerP1, "Song" + prefix + "Text3PlayersDuetSingerP1");
  ThemeLoadText (Song.Text3PlayersDuetSingerP2, "Song" + prefix + "Text3PlayersDuetSingerP2");
  ThemeLoadText (Song.Text3PlayersDuetSingerP3, "Song" + prefix + "Text3PlayersDuetSingerP3");

  // 4/6 players 1 screen
  ThemeLoadStatic (Song.Static4PlayersDuetSingerP3, "Song" + prefix + "Static4PlayersDuetSingerP3");
  ThemeLoadStatic (Song.Static4PlayersDuetSingerP4, "Song" + prefix + "Static4PlayersDuetSingerP4");

  ThemeLoadStatic (Song.Static6PlayersDuetSingerP4, "Song" + prefix + "Static6PlayersDuetSingerP4");
  ThemeLoadStatic (Song.Static6PlayersDuetSingerP5, "Song" + prefix + "Static6PlayersDuetSingerP5");
  ThemeLoadStatic (Song.Static6PlayersDuetSingerP6, "Song" + prefix + "Static6PlayersDuetSingerP6");

  //Party Mode
  ThemeLoadStatic(Song.StaticTeam1Joker1, "Song" + prefix + "StaticTeam1Joker1");
  ThemeLoadStatic(Song.StaticTeam1Joker2, "Song" + prefix + "StaticTeam1Joker2");
  ThemeLoadStatic(Song.StaticTeam1Joker3, "Song" + prefix + "StaticTeam1Joker3");
  ThemeLoadStatic(Song.StaticTeam1Joker4, "Song" + prefix + "StaticTeam1Joker4");
  ThemeLoadStatic(Song.StaticTeam1Joker5, "Song" + prefix + "StaticTeam1Joker5");

  ThemeLoadStatic(Song.StaticTeam2Joker1, "Song" + prefix + "StaticTeam2Joker1");
  ThemeLoadStatic(Song.StaticTeam2Joker2, "Song" + prefix + "StaticTeam2Joker2");
  ThemeLoadStatic(Song.StaticTeam2Joker3, "Song" + prefix + "StaticTeam2Joker3");
  ThemeLoadStatic(Song.StaticTeam2Joker4, "Song" + prefix + "StaticTeam2Joker4");
  ThemeLoadStatic(Song.StaticTeam2Joker5, "Song" + prefix + "StaticTeam2Joker5");

  ThemeLoadStatic(Song.StaticTeam3Joker1, "Song" + prefix + "StaticTeam3Joker1");
  ThemeLoadStatic(Song.StaticTeam3Joker2, "Song" + prefix + "StaticTeam3Joker2");
  ThemeLoadStatic(Song.StaticTeam3Joker3, "Song" + prefix + "StaticTeam3Joker3");
  ThemeLoadStatic(Song.StaticTeam3Joker4, "Song" + prefix + "StaticTeam3Joker4");
  ThemeLoadStatic(Song.StaticTeam3Joker5, "Song" + prefix + "StaticTeam3Joker5");

  ThemeLoadText (Song.TextPartyTime, "Song" + prefix + "TextPartyTime");

  ThemeLoadText (Song.InfoMessageText, "Song" + prefix + "InfoMessageText");
  ThemeLoadStatic (Song.InfoMessageBG, "Song" + prefix + "InfoMessageBG");
};

void TTheme::CreateThemeObjects()
{
  freeandnil(Loading);
  Loading = TThemeLoading.Create;

  freeandnil(Main);
  Main = TThemeMain.Create;

  freeandnil(Name);
  Name = TThemeName.Create;

  freeandnil(Level);
  Level = TThemeLevel.Create;

  freeandnil(Song);
  Song = TThemeSong.Create;

  freeandnil(Sing);
  Sing = TThemeSing.Create;

  freeandnil(Jukebox);
  Jukebox = TThemeJukebox.Create;

  freeandnil(JukeboxPlaylist);
  JukeboxPlaylist = TThemeJukeboxPlaylist.Create;

  freeandnil(AboutMain);
  AboutMain = TThemeAboutMain.Create;

  freeandnil(Score);
  Score = TThemeScore.Create;

  freeandnil(Top5);
  Top5 = TThemeTop5.Create;

  freeandnil(Options);
  Options = TThemeOptions.Create;

  freeandnil(OptionsGame);
  OptionsGame = TThemeOptionsGame.Create;

  freeandnil(OptionsGraphics);
  OptionsGraphics = TThemeOptionsGraphics.Create;

  freeandnil(OptionsSound);
  OptionsSound = TThemeOptionsSound.Create;

  freeandnil(OptionsInput);
  OptionsInput = TThemeOptionsInput.Create;

  freeandnil(OptionsLyrics);
  OptionsLyrics = TThemeOptionsLyrics.Create;

  freeandnil(OptionsThemes);
  OptionsThemes = TThemeOptionsThemes.Create;

  freeandnil(OptionsRecord);
  OptionsRecord = TThemeOptionsRecord.Create;

  freeandnil(OptionsAdvanced);
  OptionsAdvanced = TThemeOptionsAdvanced.Create;

  freeandnil(OptionsNetwork);
  OptionsNetwork = TThemeOptionsNetwork.Create;

  freeandnil(OptionsWebcam);
  OptionsWebcam = TThemeOptionsWebcam.Create;

  freeandnil(OptionsJukebox);
  OptionsJukebox = TThemeOptionsJukebox.Create;

  freeandnil(Edit);
  Edit = TThemeEdit.Create;

  freeandnil(EditSub);
  EditSub = TThemeEditSub.Create;

  freeandnil(ErrorPopup);
  ErrorPopup = TThemeError.Create;

  freeandnil(CheckPopup);
  CheckPopup = TThemeCheck.Create;

  freeandnil(HelpPopup);
  HelpPopup = TThemeHelp.Create;

  freeandnil(InsertUserPopup);
  InsertUserPopup = TThemeInsertUser.Create;

  freeandnil(SendScorePopup);
  SendScorePopup = TThemeSendScore.Create;

  freeandnil(ScoreDownloadPopup);
  ScoreDownloadPopup = TThemeScoreDownload.Create;

  freeandnil(SongMenu);
  SongMenu = TThemeSongMenu.Create;

  freeandnil(SongJumpto);
  SongJumpto = TThemeSongJumpTo.Create;

  //Party Screens
  freeandnil(PartyNewRound);
  PartyNewRound = TThemePartyNewRound.Create;

  freeandnil(PartyWin);
  PartyWin = TThemePartyWin.Create;

  freeandnil(PartyScore);
  PartyScore = TThemePartyScore.Create;

  freeandnil(PartyOptions);
  PartyOptions = TThemePartyOptions.Create;

  freeandnil(PartyPlayer);
  PartyPlayer = TThemePartyPlayer.Create;

  //Stats 
 Screens  freeandnil(StatMain);
  StatMain = TThemeStatMain.Create;

  freeandnil(StatDetail);
  StatDetail = TThemeStatDetail.Create;

 };
}