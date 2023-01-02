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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UCatCovers.pas $
 * $Id: UCatCovers.pas 2276 2010-04-22 11:07:24Z whiteshark0 $
 */

#include "../switches.h"

#include <string>
#include <vector>
#include <array>
#include <filesystem>

#include "UIni.h"

namespace UCatCovers
{
    class TCatCovers
    {
    protected:
        std::array<std::vector<std::string>, UIni::TSortingType::Size> cNames;
        std::array<std::vector<std::filesystem::path>, UIni::TSortingType::Size> cFiles;
    public:
        TCatCovers();
        void Load(); //Load Cover aus Cover.ini and Cover Folder
        void LoadPath(const std::filesystem::path CoversPath);
        void Add(UIni::TSortingType Sorting, const std::string Name, const std::filesystem::path Filename); //Add a Cover
        bool CoverExists(UIni::TSortingType Sorting, const std::string Name); //Returns True when a cover with the given Name exists
        std::filesystem::path GetCover(UIni::TSortingType Sorting, const std::string Name); //Returns the Filename of a Cover
    };

    inline TCatCovers CatCovers;
}