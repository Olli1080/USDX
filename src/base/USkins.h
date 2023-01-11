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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/USkins.pas $
 * $Id: USkins.pas 3137 2015-09-14 00:11:19Z basisbit $
 */
#include "../switches.h"

#include <string>
#include <vector>
#include <filesystem>

namespace USkins
{
    struct TSkinTexture
    {
		std::string Name;
		std::filesystem::path FileName;
    };

    struct TSkinEntry
    {
		std::string Theme;
        std::string Name;
	 	std::filesystem::path Path;
        std::filesystem::path FileName;

		int DefaultColor;
		std::string Creator; // not used yet
    };

    class TSkin
    {
    public:

		std::vector<TSkinEntry> Skin;
		std::vector<TSkinTexture> SkinTexture;
		std::filesystem::path SkinPath;
        int Color;

        TSkin();

        void LoadList();
        void LoadHeader(const std::filesystem::path& FileName);
        void LoadSkin(const std::string& Name, const std::string& Theme);
        std::filesystem::path GetTextureFileName(const std::string& TextureName);
        int GetSkinNumber(const std::string& Name, const std::string& Theme);
        int GetDefaultColor(int SkinNo);

        void GetSkinsByTheme(const std::string& Theme, std::vector<std::string>& Skins);

        void onThemeChange();
    };

    inline TSkin Skin;

}