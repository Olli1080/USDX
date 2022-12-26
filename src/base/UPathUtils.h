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
 * $URL$
 * $Id$
 */
//#include "UPath.h"
#include <filesystem>

#include "switches.h"

namespace UPathUtils
{
/*
uses
  SysUtils,
  Classes,
  UPath;
*/
// Absolute Paths
std::filesystem::path GamePath;
std::filesystem::path SoundPath;
std::vector<std::filesystem::path> SongPaths;
std::filesystem::path LogPath;
std::filesystem::path ThemePath;
std::filesystem::path SkinsPath;
std::filesystem::path ScreenshotsPath;
std::vector<std::filesystem::path> CoverPaths;
std::filesystem::path LanguagesPath;
std::filesystem::path PluginPath;
std::filesystem::path FontPath;
std::filesystem::path ResourcesPath;
std::filesystem::path PlaylistPath;
std::filesystem::path WebsitePath;
std::filesystem::path WebScoresPath;
std::filesystem::path SoundFontsPath;
std::filesystem::path AvatarsPath;

bool FindPath(std::filesystem::path& PathResult, const std::filesystem::path RequestedPath, bool NeedsWritePermission);
bool IsWritable(const std::filesystem::path& path);
void InitializePaths();
void AddSongPath(const std::filesystem::path Path);

/*
uses
  StrUtils,
  UPlatform,
  UCommandLine,
  ULog;
*/
void AddSpecialPath(std::vector<std::filesystem::path>& PathList, const std::filesystem::path Path);

void AddCoverPath(const Path: IPath);
begin
  AddSpecialPath(CoverPaths, Path);
end;

/**
 * Function sets all absolute paths e.g. song path and makes sure the directorys exist
 */
void InitializePaths;
var
  SharedPath, UserPath: IPath;
begin
  // Log directory (must be writable)
  if (not FindPath(LogPath, Platform.GetLogPath, true)) then
  begin
    Log.FileOutputEnabled := false;
    Log.LogWarn('Log directory "'+ Platform.GetLogPath.ToNative +'" not available', 'InitializePaths');
  end;

  SharedPath := Platform.GetGameSharedPath;
  UserPath := Platform.GetGameUserPath;

  FindPath(SoundPath,     SharedPath.Append('sounds'),    false);
  FindPath(ThemePath,     SharedPath.Append('themes'),    false);
  FindPath(SkinsPath,     SharedPath.Append('themes'),    false);
  FindPath(LanguagesPath, SharedPath.Append('languages'), false);
  FindPath(PluginPath,    SharedPath.Append('plugins'),   false);
  FindPath(FontPath,      SharedPath.Append('fonts'),     false);
  FindPath(ResourcesPath, SharedPath.Append('resources'), false);
  FindPath(WebsitePath,   SharedPath.Append('webs'), false);
  FindPath(SoundFontsPath, SharedPath.Append('soundfonts'), false);
  FindPath(AvatarsPath, SharedPath.Append('avatars'), false);

  // Playlists are not shared as we need one directory to write too
  FindPath(PlaylistPath, UserPath.Append('playlists'), true);

  // Screenshot directory (must be writable)
  if (not FindPath(ScreenshotsPath, UserPath.Append('screenshots'), true)) then
  begin
    Log.LogWarn('Screenshot directory "'+ UserPath.ToNative +'" not available', 'InitializePaths');
  end;

  // Add song paths
  AddSongPath(Params.SongPath);
{$IF Defined(DARWIN)}
  AddSongPath(Platform.GetMusicPath);
  AddSongPath(UserPath.Append('songs'));
{$ELSE}
  AddSongPath(SharedPath.Append('songs'));
  AddSongPath(UserPath.Append('songs'));
{$IFEND}

  // Add category cover paths
  AddCoverPath(SharedPath.Append('covers'));
  AddCoverPath(UserPath.Append('covers'));
end;

end.
};