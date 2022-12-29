#pragma once
/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it &&/or
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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UDLLManager.pas $
 * $Id: UDLLManager.pas 2259 2009-11-09 00:27:55Z zup3r_vock $
 */

#include "switches.h"

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <stdint.h>

#include <boost/dll.hpp>

#include "../webSDK/UWebSDK.h"

namespace UDLLManager
{

class TDLLMan
{
private:

  boost::dll::shared_library hLibW;

  UWebSDK::fModi_SendScore P_SendScore;
  UWebSDK::fModi_EncryptScore P_EncryptScore;
  UWebSDK::fModi_Login P_Login;
  UWebSDK::fModi_EncryptPassword P_EncryptPassword;
  UWebSDK::fModi_DownloadScore P_DownloadScore;
  UWebSDK::fModi_VerifySong P_VerifySong;

public:

  std::vector<UWebSDK::TWebsiteInfo> Websites;
  std::vector<std::filesystem::path> WebsitePaths;
  UWebSDK::TWebsiteInfo* SelectedW = nullptr;

  TDLLMan();

  void GetWebsiteList();

  std::optional<UWebSDK::TWebsiteInfo> LoadWebsiteInfo(const std::filesystem::path Filename);

  bool LoadWebsite(uint32_t No);
  void UnLoadWebsite();

  uint8_t WebsiteSendScore(UWebSDK::TSendInfo& SendInfo);
  std::wstring WebsiteEncryptScore(UWebSDK::TSendInfo& SendInfo);
  uint8_t WebsiteLogin(UWebSDK::TLoginInfo& LoginInfo);
  std::wstring WebsiteEncryptPassword(UWebSDK::TLoginInfo& LoginInfo);
  std::wstring WebsiteDownloadScore(std::wstring List_MD5Song, uint8_t Level);
  std::wstring WebsiteVerifySong(std::wstring MD5Song);
};

inline TDLLMan DLLMan;

const std::string DLLExt =
#if defined(_WIN32)
   ".dll";
#elif defined(__APPLE__)
  ".dylib";
#elif defined(__unix__)
  ".so";
#else
  ""
  #error "Undefined dll extension"
#endif
}