#pragma once
/** UltraStar Deluxe - Karaoke Game
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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/USongs.pas $
 * $Id: USongs.pas 3103 2014-11-22 23:21:19Z k-m_schindler $
 **/

#include "../switches.h"

#include <string>
#include <list>
#include <filesystem>

#include "UCatCovers.h"
#include "ULog.h"
#include "UPathUtils.h"
#include "USong.h"
#include "UIni.h"

namespace USongs
{
    /*$I switches.inc*/

    /*$IFDEF DARWIN*/
    /*$IFDEF DEBUG*/
    /*$DEFINE USE_PSEUDO_THREAD*/
    /*$}IF*/
    /*$}IF*/

    /*uses
        SysUtils,
        Classes,
        // $IFDEF MSWINDOWS
        Windows,
        LazUTF8Classes,
        // $ELSE
        //$IFNDEF DARWIN
        syscall,
        // $}IF
        baseunix,
        UnixType,
        // $}IF
        UPlatform,
        ULog,
        UTexture,
        UCommon,
        // $IFDEF USE_PSEUDO_THREAD
        PseudoThread,
        // $}IF
        UPath,
        UPlaylist,
        USong,
        UIni,
        UCatCovers;*/

    enum TSongFilter
    {
        fltAll,
        fltTitle,
        fltArtist,
        fltLanguage,
        fltEdition,
        fltGenre,
        fltYear,
        fltCreator
    };

    struct TBPM
    {
        double BPM;
        double StartBeat;
    };

    struct TScore
    {
        std::string Name;
        int Score;
        std::string Length;
    };

    typedef std::vector<std::filesystem::path> TPathDynArray;

    class TSongs
    {
    private:
        long int fNotify, fWatch;
        bool fParseSongDirectory;
        bool fProcessing;
        void int_LoadSongList();
        void DoDirChanged(TObject Sender);
    protected:
        void operator()();
    public:
        std::list<USong::TSong> SongList;            // array of songs

        int Selected;        // selected song index
        TSongs();
        ~TSongs();

        void LoadSongList();     // load all songs

        template<bool Recursive>
        void FindFilesByExtension(const std::filesystem::path Dir, const std::filesystem::path Ext, TPathDynArray& Files);
        void BrowseDir(std::filesystem::path Dir); // should return number of songs in the future
        void BrowseTXTFiles(std::filesystem::path Dir);
        void BrowseXMLFiles(std::filesystem::path Dir);
        void Sort(UIni::TSortingType Order);
        bool Processing() const { return fProcessing; }
    };

    class TCatSongs
    {
        TCatSongs()
        {
            Refresh();
        }

        std::vector<USong::TSong> Song; // array of categories with songs
        std::vector<USong::TSong> SongSort;

        int Selected; // selected song index
        int Order; // order type (0=title)
        int CatNumShow; // Category Number being seen
        int CatCount; // Number of Categories
        int LastVisChecked; // The double index of the last song that had its VisibilityIndex updated
        int LastVisIndex; // The VisibilityIndex of the last song that had this value updated

        void SortSongs();
        void Refresh();                                      // refreshes arrays by recreating them from Songs array
        void ShowCategory(int Index);                 // expands all songs in category
        void HideCategory(int Index);                 // hides all songs in category
        void ClickCategoryButton(int Index);          // uses ShowCategory and HideCategory when needed
        void ShowCategoryList();                             // Hides all Songs And Show the List of all Categorys
        int FindNextVisible(int SearchFrom); // Find Next visible Song
        int FindPreviousVisible(int SearchFrom); // Find Previous visible Song
        int VisibleSongs();                         // returns number of visible songs (for tabs)
        int VisibleIndex(int Index);         // returns visible song index (skips invisible)

        uint32_t SetFilter(std::string FilterStr, TSongFilter Filter);
    };

    inline TSongs Songs;    // all songs
    inline TCatSongs CatSongs; // categorized songs

    constexpr int IN_ACCESS = 0x00000001; //* File was accessed */
    constexpr int IN_MODIFY = 0x00000002; //* File was modified */
    constexpr int IN_ATTRIB = 0x00000004; //* Metadata changed */
    constexpr int IN_CLOSE_WRITE = 0x00000008; //* Writtable file was closed */
    constexpr int IN_CLOSE_NOWRITE = 0x00000010; //* Unwrittable file closed */
    constexpr int IN_OPEN = 0x00000020; //* File was opened */
    constexpr int IN_MOVED_FROM = 0x00000040; //* File was moved from X */
    constexpr int IN_MOVED_TO = 0x00000080; //* File was moved to Y */
    constexpr int IN_CREATE = 0x00000100; //* Subfile was created */
    constexpr int IN_DELETE = 0x00000200; //* Subfile was deleted */
    constexpr int IN_DELETE_SELF = 0x00000400; //* Self was deleted */

    /*uses
    StrUtils,
    UCovers,
    UFiles,
    UGraphic,
    UMain,
    UPathUtils,
    UNote,
    UFilesystem,
    UUnicodeUtils;*/

    TSongs::TSongs()
    {
        // do not start thread BEFORE initialization (suspended = true)
        //inherited Create(true);
        //Self.FreeOnTerminate := true;

        //SongList: = TList.Create()

        // until it is fixed, simply load the song-list
        int_LoadSongList();
    };

    TSongs::~TSongs()
    {
        //FreeAndNil(SongList);
        //inherited;
    };

    void TSongs::DoDirChanged(TObject Sender)
    {
        LoadSongList();
    };

    void TSongs::operator()()
        /*var
            fChangeNotify : THandle;*/
    {
        fParseSongDirectory = true;

        while (!terminated)
        {
            if (fParseSongDirectory)
            {
                ULog::Log.LogStatus("Calling int_LoadSongList", "TSongs::Execute");
                int_LoadSongList();
            }
            Suspend();
        }
    }

    void TSongs::int_LoadSongList()
    {
        try
        {
            fProcessing = true;

            ULog::Log.LogStatus("Searching For Songs", "SongList");

            // browse directories
            for (const auto& SongPath : UPathUtils::SongPaths)
                BrowseDir(SongPath);

            /*if (CatSongs)
                CatSongs.Refresh();*/

            /*if (UCatCovers::CatCovers)
	            UCatCovers::CatCovers.Load();*/

            //if assigned(Covers) then
            //  Covers.Load;

            if (ScreenSong)
            {
                ScreenSong.GenerateThumbnails();
                ScreenSong.OnShow(); // refresh ScreenSong
            }
        }
        catch (...)
        {
            ULog::Log.LogStatus("Search Complete", "SongList");
        }

        fParseSongDirectory = false;
        fProcessing = false;
    }


    void TSongs::LoadSongList()
    {
        fParseSongDirectory = true;
        Resume();
    }

    void TSongs::BrowseDir(std::filesystem::path Dir)
    {
        BrowseTXTFiles(Dir);
        BrowseXMLFiles(Dir);
    }

    template<>
    void TSongs::FindFilesByExtension<true>(const std::filesystem::path Dir, const std::filesystem::path Ext, TPathDynArray& Files)
    {
        // search for all files and directories
		for (const auto entry : std::filesystem::recursive_directory_iterator(Dir))
		{
            if (!entry.is_regular_file())
                continue;

            const auto& path = entry.path();
            // do not load files which either have wrong extension or start with a point
            if (Ext.extension() == path.extension() && !path.filename().string().starts_with('.'))
                Files.emplace_back(path);
		}
    }

    template<>
    void TSongs::FindFilesByExtension<false>(const std::filesystem::path Dir, const std::filesystem::path Ext, TPathDynArray& Files)
    {
        // search for all files and directories
        for (const auto entry : std::filesystem::directory_iterator(Dir))
        {
            if (!entry.is_regular_file())
                continue;

            const auto& path = entry.path();
            // do not load files which either have wrong extension or start with a point
            if (Ext.extension() == path.extension() && !path.filename().string().starts_with('.'))
                Files.emplace_back(path);
        }
    }

    void TSongs::BrowseTXTFiles(std::filesystem::path Dir)
    {
		auto Extension = std::filesystem::path(".txt");

        TPathDynArray Files;
        FindFilesByExtension<true>(Dir, Extension, Files);

        for (const auto& file : Files)
        {
            auto Song = USong::TSong(file);
            if (Song.Analyse())
                SongList.emplace_back(Song);
            else
            {
                ULog::Log.LogError("AnalyseFile failed for \"" + file.string() + "\".");
                //FreeAndNil(Song);
            }
        }
    }

    void TSongs::BrowseXMLFiles(std::filesystem::path Dir)
    {
		auto Extension = std::filesystem::path(".xml");

        TPathDynArray Files;
		FindFilesByExtension<true>(Dir, Extension, Files);

        for (const auto& file : Files)
	    {
            auto Song = USong::TSong(file);
            if (Song.Analyse())
                SongList.emplace_back(Song);
            else
            {
                ULog::Log.LogError("AnalyseFile failed for \"" + file.string() + "\".");
                //FreeAndNil(Song);
            }
	    }
    }

		/*
        *Comparison functions for sorting
        */

    int CompareByEdition(USong::TSong& Song1, USong::TSong& Song2)
    {
        return Song1.Edition.compare(Song2.Edition);
    }

    int CompareByGenre(USong::TSong& Song1, USong::TSong& Song2)
    {
        return Song1.Genre.compare(Song2.Genre);
    }

    int CompareByTitle(USong::TSong& Song1, USong::TSong& Song2)
    {
        return Song1.Title.compare(Song2.Title);
    }

    int CompareByArtist(USong::TSong& Song1, USong::TSong& Song2)
    {
        return Song1.Artist.compare(Song2.Artist);
    }

    int CompareByFolder(USong::TSong& Song1, USong::TSong& Song2)
    {
        return Song1.Folder.compare(Song2.Folder);
    }

    int CompareByLanguage(USong::TSong& Song1, USong::TSong& Song2)
    {
        return Song1.Language.compare(Song2.Language);
    }

    bool CompareByYear(USong::TSong& Song1, USong::TSong& Song2)
    {
        return Song1.Year > Song2.Year;
    }

    bool CompareByYearReversed(USong::TSong& Song1, USong::TSong& Song2)
    {
        return CompareByYear(Song2, Song1);
    }

    void TSongs::Sort(UIni::TSortingType Order)
    {
        // FIXME: what is the difference between artist and artist2, etc.?
        switch (Order)
        {
        case UIni::sEdition: // by edition
			SongList.sort(CompareByEdition);
            return;
        case UIni::sGenre: // by genre
			SongList.sort(CompareByGenre);
            return;
        case UIni::sTitle: // by title  
			SongList.sort(CompareByTitle);
            return;
        case UIni::sArtist: // by artist
			SongList.sort(CompareByArtist);
            return;
        case UIni::sFolder: // by folder    
			SongList.sort(CompareByFolder);
            return;
        case UIni::sArtist2: // by artist2
            SongList.sort(CompareByArtist);
            return;
        case UIni::sLanguage: // by Language
            SongList.sort(CompareByLanguage);
            return;
        case UIni::sYear: // by Year
			SongList.sort(CompareByYear);
            return;
        case UIni::sYearReversed: //by year reversed
			SongList.sort(CompareByYearReversed);
            return;
        case UIni::sDecade: // by Decade
			SongList.sort(CompareByYear);
            return;
        default:
            ULog::Log.LogCritical("Unsupported comparison", "TSongs::Sort");
        }
    }

    void TCaTSongs::SortSongs()
    {
        switch (UIni::Ini.Sorting)
        {
        case UIni::sEdition:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist);
            Songs->Sort(UIni::sEdition);
            return;
        }
        case UIni::sGenre:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist);
            Songs->Sort(UIni::sGenre);
            return;
        }
        case UIni::sLanguage:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist);
            Songs->Sort(UIni::sLanguage);
            return;
        }
        case UIni::sFolder:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist);
            Songs->Sort(UIni::sFolder);
            return;
        }
        case UIni::sTitle:
        {
            Songs->Sort(UIni::sTitle);
            return;
        }
        case UIni::sArtist:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist);
            return;
        }
        case UIni::sArtist2:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist2);
            return;
        }
        case UIni::sYear:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist);
            Songs->Sort(UIni::sYear);
            return;
        }
        case UIni::sYearReversed:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist);
            Songs->Sort(UIni::sYearReversed);
            return;
        }
        case UIni::sDecade:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist);
            Songs->Sort(UIni::sYear);
            return;
        }
        case UIni::sPlaylist:
        {
            Songs->Sort(UIni::sTitle);
            Songs->Sort(UIni::sArtist);
            return;
        }
        } // case
    }

void TCatSongs::Refresh()
/*var
SongIndex : int;
CurSong:     TSong;
CatIndex:    int;    // index of current song in Song
Letter:      UCS4Char;   // current letter for sorting using letter
CurCategory: UTF8String; // current edition for sorting using edition, genre etc.
Order:       int;    // number used for ordernum
LetterTmp:   UCS4Char;
CatNumber:   int;    // Number of Song in Category
tmpCategory: UTF8String; //
I, J:        int;
StringIndex: int;
MainArtist:  UTF8String;
*/
{
    int Order;
    int CatIndex, CatNumber;

    auto AddCategoryButton = [&Order, &CatIndex, &CatNumber, this](const std::string CategoryName) -> void
    /*var
        PrevCatBtnIndex : int;*/
    {
        ++Order;
		CatIndex = Song.size();

        auto&& tmp = USong::TSong();
        tmp.Artist = '[' + CategoryName + ']';
        tmp.Main = true;
        tmp.OrderTyp = 0;
        tmp.OrderNum = Order;
        tmp.Cover = UCatCovers::CatCovers.GetCover((UIni::TSortingType)UIni::Ini.Sorting, CategoryName);
        tmp.Visible = true;

        Song.emplace_back(std::move(tmp));

        // set number of songs in previous category
		const int PrevCatBtnIndex = CatIndex - CatNumber - 1;
        if ((PrevCatBtnIndex >= 0) and Song[PrevCatBtnIndex].Main)
            Song[PrevCatBtnIndex].CatNumber = CatNumber;

    	CatNumber = 0;
    };
    
	CatNumShow = -1;

	SortSongs();

	std::string CurCategory;
	Order = 0;
	CatNumber = 0;

	// Note: do NOT set Letter to ' ', otherwise no category-button will be
	// created for songs {ning with ' ' if songs of this category exist.
	// TODO: trim song-properties so ' ' will not occur as first chararcter.
	char Letter = 0;

	// clear song-list
	for (SongIndex : = 0 to Songs.SongList.Count - 1)
	{
		// free category buttons
		// Note: do NOT delete songs, they are just references to Songs.SongList entries
		CurSong : = TSong(Songs.SongList[SongIndex]);
		if (CurSong.Main) then
		CurSong.Free;
	}
	SetLength(Song, 0);

	for (SongIndex : = 0 to Songs.SongList.Count - 1)
	{
		CurSong : = TSong(Songs.SongList[SongIndex]);
		// if tabs are on, add section buttons for each new section
		if (UIni::Ini.Tabs = 1) then
		{
		case (TSortingType(Ini.Sorting)) of
        sEdition : 
        {
			if (CompareText(CurCategory, CurSong.Edition) != 0)
			{
			CurCategory : = CurSong.Edition;

			// add Category Button
			AddCategoryButton(CurCategory);
			}
		}
		sGenre: 
        {
			if (CompareText(CurCategory, CurSong.Genre) != 0)
			{
				CurCategory : = CurSong.Genre;
				// add Genre Button
				AddCategoryButton(CurCategory);
			}
		}
		sLanguage: {
		if (CompareText(CurCategory, CurSong.Language) < > 0) then
		{
		CurCategory : = CurSong.Language;
		// add Language Button
		AddCategoryButton(CurCategory);
		}
		};

		sTitle: {
		if (Length(CurSong.Title) >= 1) then
		{
		LetterTmp : = UCS4UpperCase(UTF8ToUCS4String(CurSong.Title)[0]);
		/* all numbersand some punctuation chars are put into a
		category named '#'
		we can't put the other punctuation chars into this category
		because they are not in order, so there will be two different
		categories named '#' */
		if (LetterTmp in[Ord('!') ..Ord('?')]) then
		LetterTmp : = Ord('#')
		else
		LetterTmp : = UCS4UpperCase(LetterTmp);
		if (Letter <> LetterTmp) then
		{
		Letter : = LetterTmp;
		// add a letter Category Button
		AddCategoryButton(UCS4ToUTF8String(Letter));
		};
		};
		};

		sArtist: {
		if (Length(CurSong.Artist) >= 1) then
		{
		LetterTmp : = UCS4UpperCase(UTF8ToUCS4String(CurSong.Artist)[0]);
		/* all numbersand some punctuation chars are put into a
		category named '#'
		we can't put the other punctuation chars into this category
		because they are not in order, so there will be two different
		categories named '#' */
		if (LetterTmp in[Ord('!') ..Ord('?')]) then
		LetterTmp : = Ord('#')
		else
		LetterTmp : = UCS4UpperCase(LetterTmp);

		if (Letter <> LetterTmp) then
		{
		Letter : = LetterTmp;
		// add a letter Category Button
		AddCategoryButton(UCS4ToUTF8String(Letter));
		};
		};
		};

		sFolder: {
		if (UTF8CompareText(CurCategory, CurSong.Folder) < > 0) then
		{
		CurCategory : = CurSong.Folder;
		// add folder tab
		AddCategoryButton(CurCategory);
		};
		};

		sArtist2: {
		/* this new sorting puts all songs by the same artist into
		  a single category */
		  //
		    if (UTF8ContainsText(CurSong.Artist, ' feat.')) then
		        {
		        StringIndex : = UTF8Pos(' feat', CurSong.Artist);
		MainArtist: = TrimRight(UTF8Copy(CurSong.Artist, 1, StringIndex - 1));
		}
		    else
		        MainArtist : = CurSong.Artist;
		//
		if (UTF8CompareText(CurCategory, MainArtist) < > 0) then
		{
		CurCategory : = MainArtist;
		// add folder tab
		AddCategoryButton(CurCategory);
		};
		};

		sYear: {
		if (CurSong.Year < > 0) then
		tmpCategory : = IntToStr(CurSong.Year)
		else
		tmpCategory : = 'Unknown';

		if (tmpCategory <> CurCategory) then
		{
		CurCategory : = tmpCategory;

		// add Category Button
		AddCategoryButton(CurCategory);
		};
		};

		sYearReversed: {
		if (CurSong.Year < > 0) then
		tmpCategory : = IntToStr(CurSong.Year)
		else
		tmpCategory : = 'Unknown';

		if (tmpCategory <> CurCategory) then
		{
		CurCategory : = tmpCategory;

		// add Category Button
		AddCategoryButton(CurCategory);
		};
		};

		sDecade: {
		if (CurSong.Year < > 0) then
		tmpCategory : = IntToStr(Trunc(CurSong.Year / 10) * 10) + '-' + IntToStr(Trunc(CurSong.Year / 10) * 10 + 9)
		else
		tmpCategory : = 'Unknown';

		if (tmpCategory <> CurCategory) then
		{
		CurCategory : = tmpCategory;

		// add Category Button
		AddCategoryButton(CurCategory);
		};
		};
}; // case (Ini.Sorting)
}; // if (Ini.Tabs = 1)

CatIndex: = Length(Song);
SetLength(Song, CatIndex + 1);

Inc(CatNumber); // increase number of songs in category

// copy reference to current song
Song[CatIndex] : = CurSong;

// set song's category info
CurSong.OrderNum : = Order; // assigns category
CurSong.CatNumber : = CatNumber;

if (Ini.Tabs = 0) then
{
CurSong.Visible : = true;
}
else if (Ini.Tabs = 1) then
{
CurSong.Visible : = false;
};
/*
    if (Ini.Tabs = 1) and (Order = 1) then
        {
        //open first tab
        CurSong.Visible : = true;
    };
    CurSong.Visible : = true;
*/
};
LastVisChecked: = 0;
LastVisIndex: = 0;

// set CatNumber of last category
if (UIni::Ini.TabsAtStartup == 1 && High(Song) >= 1)
{
// set number of songs in previous category
SongIndex : = CatIndex - CatNumber;
if ((SongIndex >= 0) and Song[SongIndex].Main) then
Song[SongIndex].CatNumber : = CatNumber;
};

// update number of categories
CatCount: = Order;
};

void TCaTSongs::ShowCategory(Index: int);
var
S : int; // song
{
CatNumShow : = Index;
for S : = 0 to high(CaTSongs::Song) do
{
/*
    if (CaTSongs::Song[S].OrderNum = Index) and (not CaTSongs::Song[S].Main) then
      CaTSongs::Song[S].Visible : = true
    else
      CaTSongs::Song[S].Visible : = false;
*/
//  KMS: This should be the same, but who knows :-)
CaTSongs::Song[S].Visible : = ((CaTSongs::Song[S].OrderNum = Index) and (not CaTSongs::Song[S].Main));
};
LastVisChecked: = 0;
LastVisIndex: = 0;
};

void TCaTSongs::HideCategory(Index: int); // hides all songs in category
var
S : int; // song
{
for S : = 0 to high(CaTSongs::Song) do
{
if not CaTSongs::Song[S].Main then
CaTSongs::Song[S].Visible : = false // hides all at now
};
LastVisChecked: = 0;
LastVisIndex: = 0;
};

void TCaTSongs::ClickCategoryButton(Index: int);
var
Num : int;
{
Num : = CaTSongs::Song[Index].OrderNum;
if Num <> CatNumShow then
{
ShowCategory(Num);
}
else
{
ShowCategoryList;
};
};

//Hide Categorys when in Category Hack
void TCaTSongs::ShowCategoryList;
var
S : int;
{
// Hide All Songs Show All Cats
for S : = 0 to high(CaTSongs::Song) do
CaTSongs::Song[S].Visible : = CaTSongs::Song[S].Main;
CaTSongs::Selected : = CatNumShow; //Show last shown Category
CatNumShow: = -1;
LastVisChecked: = 0;
LastVisIndex: = 0;
};
//Hide Categorys when in Category Hack }

// Wrong song selected when tabs on bug
function TCaTSongs::FindNextVisible(SearchFrom:int) : int;// Find next Visible Song
var
I : int;
{
Result : = -1;
I: = SearchFrom;
while (Result = -1) do
{
Inc(I);

if (I > High(CaTSongs::Song)) then
I : = Low(CaTSongs::Song);

if (I = SearchFrom) then // Make One Round and no song found->quit
Break;

if (CaTSongs::Song[I].Visible) then
Result : = I;
};
};

function TCaTSongs::FindPreviousVisible(SearchFrom:int) : int;// Find previous Visible Song
var
I : int;
{
Result : = -1;
I: = SearchFrom;
while (Result = -1) do
{
Dec(I);

if (I < Low(CaTSongs::Song)) then
    I : = High(CaTSongs::Song);

if (I = SearchFrom) then // Make One Round and no song found->quit
Break;

if (CaTSongs::Song[I].Visible) then
Result : = I;
};
};

// Wrong song selected when tabs on bug }

(**
    *Returns the number of visible songs.
    *)
    function TCaTSongs::VisibleSongs: int;
{
Result : = VisibleIndex(High(Song));
if Song[High(Song)].Visible then
Inc(Result);
};

(**
    *Returns the index of a song in the subset of all visible songs.
    * If all songs are visible, the result will be equal to the Index parameter.
    *)
    function TCaTSongs::VisibleIndex(Index: int) : int;
{
while LastVisChecked < Index do
    {
    if Song[LastVisChecked].Visible then
        Inc(LastVisIndex);
Inc(LastVisChecked);
Song[LastVisChecked].VisibleIndex : = LastVisIndex;
};
Result: = Song[Index].VisibleIndex;
};

function TCaTSongs::SetFilter(FilterStr: UTF8String; Filter: TSongFilter) : uint32_t;
var
I, J:      int;
TmpString: UTF8String;
WordArray: array of UTF8String;
{

FilterStr : = Trim(LowerCase(FilterStr));
FilterStr: = GetStringWithNoAccents(FilterStr);

if (FilterStr < > '') then
{
Result : = 0;

// initialize word array
SetLength(WordArray, 1);

// Copy words to SearchStr
I: = Pos(' ', FilterStr);
while (I < > 0) do
{
WordArray[High(WordArray)] : = Copy(FilterStr, 1, I - 1);
SetLength(WordArray, Length(WordArray) + 1);

FilterStr: = TrimLeft(Copy(FilterStr, I + 1, Length(FilterStr) - I));
I: = Pos(' ', FilterStr);
};

// Copy last word
WordArray[High(WordArray)] : = FilterStr;

for I : = 0 to High(Song) do
{
if not Song[i].Main then
{
case Filter of
fltAll :
TmpString: = Song[I].ArtistNoAccent + ' ' + Song[i].TitleNoAccent + ' ' + Song[i].LanguageNoAccent + ' ' + Song[i].EditionNoAccent + ' ' + Song[i].GenreNoAccent + ' ' + IntToStr(Song[i].Year) + ' ' + Song[i].CreatorNoAccent; //+ ' ' + Song[i].Folder;
fltTitle:
TmpString: = Song[I].TitleNoAccent;
fltArtist:
TmpString: = Song[I].ArtistNoAccent;
fltLanguage:
TmpString: = Song[I].LanguageNoAccent;
fltEdition:
TmpString: = Song[I].EditionNoAccent;
fltGenre:
TmpString: = Song[I].GenreNoAccent;
fltYear:
TmpString: = IntToStr(Song[I].Year);
fltCreator:
TmpString: = Song[I].CreatorNoAccent;
};
Song[i].Visible : = true;
// Look for every searched word
for J : = 0 to High(WordArray) do
{
Song[i].Visible : = Song[i].Visible and
UTF8ContainsStr(TmpString, WordArray[J])
};
if Song[i].Visible then
Inc(Result);
}
else
Song[i].Visible : = false;
};
CatNumShow: = -2;
}
else
{
for i : = 0 to High(Song) do
{
Song[i].Visible : = (Ini.Tabs = 1) = Song[i].Main;
CatNumShow: = -1;
};
Result: = 0;
};
LastVisChecked: = 0;
LastVisIndex: = 0;
};

// -----------------------------------------------------------------------------
}