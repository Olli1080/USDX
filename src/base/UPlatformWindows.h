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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/UPlatformWindows.pas $
 * $Id: UPlatformWindows.pas 2344 2010-05-08 15:02:05Z tobigun $
 */

#include "UPlatformBase.h"
#include "../switches.h"

#include <fstream>
#include <filesystem>

#include <winrt/windows.storage.h>

namespace UPlatformWindows
{
    class TPlatformWindows : private UPlatformBase::TPlatformBase
    {
    private:

        bool UseLocalDirs;

        //std::filesystem::path GetSpecialPath(int CSIDL);
        void DetectLocalExecution();

    public:

        TPlatformWindows();

        std::filesystem::path GetLogPath();
        std::filesystem::path GetGameSharedPath();
        std::filesystem::path GetGameUserPath();
    };

    //function GetConsoleWindow : THandle; stdcall; external kernel32 name 'GetConsoleWindow';
    //function HasConsole : bool;
  
    TPlatformWindows::TPlatformWindows()
    {
        DetectLocalExecution();
    }

    /**
        *Detects whether the was executed locally or globally.
        * -Local mode :
    *-Condition :
        *-config.ini is writable or creatable in the directory of the executable.
        * -Examples :
        *-The USDX zip - archive has been unpacked to a directory with write.
        * permissions
        * -XP : USDX was installed to % ProgramFiles % andthe user is an admin.
        * -USDX is started from an external HD - or flash - drive
        * -Behavior :
        *Config files like config.ini or score db reside in the directory of the
        * executable.This is useful to enable windows users to have a portable
        * installation e.g.on an external hdd.
        * This is also the default behaviour of usdx prior to version 1.1
        * -Global mode :
    *-Condition :
        *-config.ini is not writable.
        * -Examples :
        *-Vista / 7 : USDX was installed to % ProgramFiles % .
        * -XP : USDX was installed to % ProgramFiles % andthe user is not an admin.
        * -USDX is started from CD
        * -Behavior :
        *-The config files are in a separate folder(e.g. % APPDATA % \ultrastardx)
        *
        *On windows, resources(themes, language - files)
        * reside in the directory of the executable in any case
        *
        * Sets UseLocalDirs to true if the game is executed locally, false otherwise.
        */
    void TPlatformWindows::DetectLocalExecution()
    {
        auto ConfigIni = std::filesystem::current_path() / "config.yaml";

        // check if config.yaml is writable or creatable, if so use local dirs
        UseLocalDirs = false;
        
        std::ofstream testFile{ ConfigIni };
        if (testFile.is_open())
        {
            UseLocalDirs = true;
        }
    }

    std::filesystem::path TPlatformWindows::GetLogPath()
    {
        return GetGameUserPath();
    }

    std::filesystem::path TPlatformWindows::GetGameSharedPath()
    {
        return std::filesystem::current_path();
    }

    std::filesystem::path TPlatformWindows::GetGameUserPath()
    {
        if (UseLocalDirs)
            return std::filesystem::current_path();

        const auto appPaths = winrt::Windows::Storage::AppDataPaths::GetDefault();
        return std::filesystem::path(to_string(appPaths.LocalAppData())) / "ultrastardx";
    }
    /*
    bool HasConsole()
    {
        return GetConsoleWindow != 0;
    }*/
}