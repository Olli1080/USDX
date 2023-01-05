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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/UPlatformLinux.pas $
 * $Id: UPlatformLinux.pas 1939 2009-11-09 00:27:55Z s_alexander $
 */
#include "../switches.h"

#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <filesystem>

#include "UPlatformBase.h"


namespace UPlatformLinux
{
    /*
    uses
        Classes,
        UPlatform,
        UConfig,
        UPath;
        */

    class TPlatformLinux : private UPlatformBase::TPlatformBase
    {
    private:

        bool UseLocalDirs;

        void DetectLocalExecution();
        std::filesystem::path GetHomeDir();

    public:

        TPlatformLinux();

        [[nodiscard]] std::filesystem::path GetLogPath() const;
        [[nodiscard]] std::filesystem::path GetGameSharedPath() const;
        [[nodiscard]] std::filesystem::path GetGameUserPath() const;
    };

    /*
     *TODO:: restore?!?
     *const
    {$I paths.inc}
    */

    TPlatformLinux::TPlatformLinux()
    {
        DetectLocalExecution();
    }

    /**
        *Detects whether the game was executed locally or globally.
        * -It is local if it was not installed and directly executed from
        * within the game folder.In this case resources(themes, language - files)
        * reside in the directory of the executable.
        * -It is global if the game was installed(e.g.to / usr / bin) and
        *the resources are in a separate folder(e.g. / usr / share / ultrastardx)
        * which name is stored in the INSTALL_DATADIR constant in paths.inc.
        *
        * Sets UseLocalDirs to true if the game is executed locally, false otherwise.
        */
    void TPlatformLinux::DetectLocalExecution()
    {
        // we just check if the 'languages' folder exists in the
        // directory of the executable. If so -> local execution.
        UseLocalDirs = is_directory(std::filesystem::current_path() / "languages");// and not LocalDir.IsReadonly;
    }

    std::filesystem::path TPlatformLinux::GetLogPath()
    {
        std::filesystem::path Result;
        if (UseLocalDirs)
            Result = std::filesystem::current_path();
        else
            Result = GetGameUserPath() / "logs";

        // create non-existing directories
        create_directories(Result);
        return Result;
    }

    std::filesystem::path TPlatformLinux::GetGameSharedPath()
    {
        std::filesystem::path Result;
        if (UseLocalDirs)
            Result = std::filesystem::current_path();
        else
        {
            Result = std::filesystem::path(INSTALL_DATADIR);
			//if (!Result.IsAbsolute)
				//Result = GetExecutionDir.Append(Result);
		}
        return Result;
    }

    std::filesystem::path TPlatformLinux::GetGameUserPath()
    {
        std::filesystem::path Result;
        if (UseLocalDirs)
            Result = std::filesystem::current_path();
        else
            Result = GetHomeDir() / ".ultrastardx";
    }

    /**
        *Returns the user's home directory terminated by a path delimiter
        */
    std::filesystem::path TPlatformLinux::GetHomeDir()
    {
        std::filesystem::path Result;

        // try to retrieve the info from passwd
        struct passwd* pw = getpwuid(getuid());
        if (pw)
            Result = std::filesystem::path(pw->pw_dir);
        // fallback if passwd does not contain the path
        if (!Result.empty())
            return Result;

        size_t required;
        getenv_s(&required, nullptr, 0, "HOME");

        if (required > 0)
        {
            std::string str;
            str.resize(required);

            getenv_s(&required, str.data(), required, "HOME");

            Result = std::filesystem::path(str);
        }
        return Result;

        // GetUserDir() is another function that returns a user path.
        // It uses env-var HOME or a fallback to a temp-dir.
        //Result = GetUserDir();
    }
}
