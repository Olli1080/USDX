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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/UPlatformMacOSX.pas $
 * $Id: UPlatformMacOSX.pas 3018 2013-12-06 21:48:55Z k-m_schindler $
 */

#include "UPlatformBase.h"
#include "../switches.h"

#include <filesystem>

#include <CoreFoundation/CFBundle.h>

namespace UPlatformMacOSX
{
    /**
     *@abstract(Provides Mac OS X specific details.)
     * @lastmod(August 1, 2008)
     * The UPlatformMacOSX unit takes care of setting paths to resource folders.
     *
     *(Note for non - Maccies: "folder" is the Mac name for directory.)
     *
     * Note on the resource folders :
     *1. Installation of an application on the mac works as follows : Extract and
     *copy an application and if you don't like or need the application
     * anymore you move the folder to the trash - and you're done.
     * 2. The use of folders in the user's home directory is against Apple's
     * guidelines and strange to an average user.
     * 3. Even worse is using / usr / local / ... since all lowercase folders in / are
     * not visible to an average user in the Finder, at least not without some
     * "tricks".
     *
     *The best way would be to store everything within the application bundle.
     * However, this requires USDX to offer the handling of the resources.Until
     * this is implemented, the second best solution is as follows :
     *
     *According to Aple guidelines handling of resourcesand folders should follow
     * these lines :
     *
     *Acceptable places for files are folders named UltraStarDeluxe either in
     * / Library / Application Support /
     *or
     *~/ Library / Application Support /
     *
     *So
     * GetGameSharedPath could return
     * / Library / Application Support / UltraStarDeluxe1.3 / .
     * GetGameUserPath could return
     *~/ Library / Application Support / UltraStarDeluxe1.3 / .
     *
     *Right now, only $HOME / Library / Application Support / UltraStarDeluxe1.3
     * is used.So every user needs the complete set of files and folders.
     * Future versions may also use shared resources in
     * / Library / Application Support / UltraStarDeluxe1.3.However, this is
     * not treated yet in the code outside this unit.
     *
     * USDX checks, whether GetGameUserPath exists.If not, USDX creates it.
     * The existence of needed files is then checkedand if a file is missing
     * it is copied to there from within the folder Contents in the Application
     * bundle, which contains the default files.USDX should not delete files or
     *folders in Application Support / UltraStarDeluxe1.3 automatically or without
     * user confirmation.
     *
     *The log and benchmark files are stored in
     * $HOME / Library / Log / UltraStar Deluxe /
     *
     *Music should go into ~/ Music / UltraStar Deluxe /
     *
     *~/ Library / Application Support / UltraStarDeluxe1.3 / songs is also used.
     * The idea is to remove this at some time.
     *
     */
    class TPlatformMacOSX : private UPlatformBase::TPlatformBase
    {
    private:
        /**
        *GetBundlePath returns the path to the application bundle
        * UltraStarDeluxe.app.
        */
        std::filesystem::path GetBundlePath();

        /**
        *GetApplicationSupportPath returns the path to
        * $HOME / Library / Application Support / UltraStarDeluxe1.3.
        */
        std::filesystem::path GetApplicationSupportPath();

        /**
        *see the description of @link(Init).
        */
        void CreateUserFolders();

        /**
        *GetHomeDir returns the path to $HOME.
        */
        std::filesystem::path GetHomeDir();

    public:
        /**
            *Init simply calls @link(CreateUserFolders), which in turn scans the
            * folder UltraStarDeluxe.app / Contents for all filesand
            * folders.$HOME / Library / Application Support / UltraStarDeluxe1.3
            * is then checked for their presenceand missing ones are copied.
            */
        TPlatformMacOSX();

        /**
            *GetLogPath returns the path for log messages.Currently it is set to
            * $HOME / Library / Logs / UltraStar Deluxe / .
            */
        std::filesystem::path GetLogPath();

        /**
            *GetMusicPath returns the path for music.Currently it is set to
            * $HOME / Music / UltraStar Deluxe / .
            */
        std::filesystem::path GetMusicPath();

        /**
        *GetGameSharedPath returns the path for shared resources.Currently it
        * is also set to $HOME / Library / Application Support / UltraStarDeluxe1.3.
        * However it is not used.
        */
        std::filesystem::path GetGameSharedPath();

        /**
        *GetGameUserPath returns the path for user resources.Currently it is
        * set to $HOME / Library / Application Support / UltraStarDeluxe1.3.
        * This is where a user can add themes, ....
        */
        std::filesystem::path GetGameUserPath();
    };

    enum TLogSwitch
    {
        On, Off
    };
     const TLogSwitch LogSwitch = Off;

     TPlatformMacOSX::TPlatformMacOSX()
     {
         CreateUserFolders();
     }

     void TPlatformMacOSX::CreateUserFolders()
         /*var
             RelativePath : IPath;
         // BaseDir contains the path to the folder, where a search is performed.
         // It is set to the entries in @link(DirectoryList) one after the other.
     BaseDir: IPath;
         // OldBaseDir contains the path to the folder, where the search started.
         // It is used to return to it, when the search is completed in all folders.
     OldBaseDir: IPath;
     Iter:       IFileIterator;
     FileInfo:   TFileInfo;
     CurPath:    IPath;
         // These two lists contain all folder and file names found
         // within the folder @link(BaseDir).
         DirectoryList, FileList: IInterfaceList;
         // DirectoryIsFinished contains the index of the folder in @link(DirectoryList),
         // which is the last one completely searched. Later folders are still to be
         // searched for additional files and folders.
     DirectoryIsFinished: longint;
     I: longint;
         // These three are for creating directories, due to possible symlinks
     CreatedDirectory: boolean;
     FileAttrs:        integer;
     DirectoryPath:    IPath;
     UserPath:         IPath;
         SrcFile, TgtFile: IPath;
     mainBundle:       CFBundleRef;
     resourcesURL:     CFURLRef;
     bundlePath:       AnsiString;
     success:          boolean;
     Position:         integer;*/
     {
         constexpr int PATH_MAX = 500;
         // Get the current folder and save it in OldBaseDir for returning to it, when
         // finished.
         auto OldBaseDir = std::filesystem::current_path();
         if constexpr (LogSwitch == On)
             std::cout << "Old base directory : " + OldBaseDir.string() << std::endl;

         // UltraStarDeluxe.app/Contents contains all the default files and folders.
         CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
         std::string bundlePath;
         bundlePath.resize(PATH_MAX);

         bool success = CFURLGetFileSystemRepresentation(resourcesURL, true, bundlePath.data(), PATH_MAX);
         if (!success)
             std::cout << "CreateUserFolders:CFURLGetFileSystemRepresentation unexpectedly failed." << std::endl;
         CFRelease(resourcesURL);
         if constexpr (LogSwitch == On)
             std::cout << "BundlePath: " << bundlePath << std::endl;

         if (success)
         {
             const std::string appString = "UltraStarDeluxe.app";
             auto Position = bundlePath.find(appString);
             auto fsPath = std::filesystem::path(bundlePath.substr(0, Position + appString.size()));
             current_path(fsPath);
         }
         auto BaseDir = std::filesystem::current_path() / "Contents";
         current_path(BaseDir);

         // Right now, only $HOME/Library/Application Support/UltraStarDeluxe1.3 is used.
         auto UserPath = GetGameUserPath();
         if constexpr (LogSwitch == On)
             std::cout << "User path : " << UserPath.string() << std::endl;
         
         std::set<std::filesystem::path> FileList;
         std::set<std::filesystem::path> DirectoryList;
         

         // create the folder and file lists
         auto& it = std::filesystem::recursive_directory_iterator(std::filesystem::current_path());
         for (const auto& entry : it)
         {
	         if (entry.is_directory())
	         {
                 if (canonical(entry.path()).filename() == "MacOS")
                 {
                     it.pop();
                     continue;
                 }
                 DirectoryList.emplace(entry.path()).second;
	         }
             else
             {
                 FileList.emplace(entry.path());
             }
         }
        
         // create missing folders
         create_directories(UserPath); // should not be necessary since (UserPathName+'/.') is created.
         for (const auto& CurPath : DirectoryList)
         {
             if constexpr (LogSwitch == On)
                 std::cout << "Current path : " << CurPath.string() << std::endl;
             auto DirectoryPath = UserPath / CurPath; //TODO:: Really?
             if constexpr (LogSwitch == On)
                 std::cout << "Directory path: " << DirectoryPath.string() << std::endl;
             //FileAttrs = DirectoryPath.GetAttr();
             // Maybe analyse the target of the link with FpReadlink().
             // Let's assume the symlink is pointing to an existing directory.
             if (!create_directories(DirectoryPath))//TODO:: && (FileAttrs && faSymLink > 0))
                 std::cout << "Failed to create the folder \"" << DirectoryPath.string() + "\" in PlatformMacOSX.CreateUserFolders" << std::endl;
         }

         // copy missing files
         for (const auto& CurPath : Filelist)
         {
             if constexpr (LogSwitch == On)
                 std::cout << "Current path : " << CurPath.string() << std::endl;
             auto SrcFile = BaseDir / CurPath;
             auto TgtFile = UserPath / CurPath;
             std::filesystem::copy_file(SrcFile, TgtFile, std::filesystem::copy_options::recursive);
         }

         // go back to the initial folder
         FileSystem.SetCurrentDir(OldBaseDir);
     }

     std::filesystem::path TPlatformMacOSX::GetBundlePath()
     {
         // Mac applications are packaged in folders.
         // Cutting the last two folders yields the application folder.
         auto Result = std::filesystem::current_path().parent_path().parent_path();
         if constexpr (LogSwitch == On)
             std::cout << "Bundle path : " + Result.string() << std::endl;
     }

     std::filesystem::path TPlatformMacOSX::GetHomeDir()
     {
         size_t required;
         getenv_s(&required, nullptr, 0, "HOME");
         std::filesystem::path Result;

         if (required > 0)
         {
             std::string str;
             str.resize(required);

             getenv_s(&required, str.data(), required, "HOME");

             Result = std::filesystem::path(str);
         }
         if constexpr (LogSwitch == On)
             std::cout << "Home path : " << Result.string() << std::endl;

         return Result;
     }

     std::filesystem::path TPlatformMacOSX::GetApplicationSupportPath()
     {
         // append the version for conflict resolution
         return GetHomeDir() / "Library/Application Support/UltraStarDeluxe1.3";
     }

     std::filesystem::path TPlatformMacOSX::GetLogPath()
     {
         return GetHomeDir() / "Library/Logs/UltraStar Deluxe";
     }

     std::filesystem::path TPlatformMacOSX::GetMusicPath()
     {
         auto Result = GetHomeDir() / "Music/UltraStar Deluxe";
     	if constexpr (LogSwitch == On)
			 std::cout << "Music path : " + Result.string() << std::endl;
     }

     std::filesystem::path TPlatformMacOSX::GetGameSharedPath()
     {
         return GetApplicationSupportPath();
     }

     std::filesystem::path TPlatformMacOSX::GetGameUserPath()
     {
         return GetApplicationSupportPath();
     }
}