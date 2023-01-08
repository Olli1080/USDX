#include "UPathUtils.h"

#include "UCommandLine.h"
#include "ULog.h"
#include "UPlatform.hpp"

#if defined(_WIN32)
#include <io.h>
#elif defined(__unix__)
#include <unistd.h>
//#else//TODO::mac?
//#error "No implementation defined"
#endif

namespace UPathUtils
{
    void AddSpecialPath(std::vector<std::filesystem::path>& PathList, const std::filesystem::path Path)
    {        
        if (Path.empty() || !create_directories(Path))
            return;

        auto PathAbs = absolute(Path);

        // check if path or a part of the path was already added
        for (int i = 0; i < PathList.size(); ++i)
        {
            auto& OldPath = PathList[i];
            auto OldPathAbs = absolute(OldPath);

            // check if the new directory is a sub-directory of a previously added one.
            // This is also true, if both paths point to the same directories.
            if (OldPathAbs == PathAbs || IsChild(OldPathAbs, PathAbs))
            {
                // ignore the new path
                return;
            }

            // check if a previously added directory is a sub-directory of the new one.
            if (IsChild(PathAbs, OldPathAbs))
            {
                // replace the old with the new one.
                PathList[i] = PathAbs;
                return;
            }
        }
        PathList.emplace_back(PathAbs);
    }

    void AddSongPath(const std::filesystem::path Path)
    {
        AddSpecialPath(SongPaths, Path);
    }

    bool IsChild(const std::filesystem::path& potChild, const std::filesystem::path& base)
    {
        const auto mismatch_pair = std::mismatch(potChild.begin(), potChild.end(), base.begin(), base.end());
        return mismatch_pair.second == base.end();
    }

    /**
     * Initialize a path variable
     * After setting paths, make sure that paths exist
     */
    bool FindPath(std::filesystem::path& PathResult, const std::filesystem::path RequestedPath, bool NeedsWritePermission)
    {
        /*if (RequestedPath.Equals(PATH_NONE)) then
            Exit;*/

        // Make sure the directory exists
        if (!create_directories(RequestedPath))
        {
            PathResult = "";
            return false;
        }

        //PathResult = RequestedPath.AppendPathDelim();

        if (NeedsWritePermission && IsWritable(RequestedPath))
            return false;

        return true;
    }

    bool IsWritable(const std::filesystem::path& path)
    {
        auto cStrPath = path.string().c_str();

        #if defined(_WIN32)
            return _access_s(cStrPath, 2) == 0;
        #elif defined(__unix__)
            return access(cStrPath, W_OK) == 0;
        #else 
            #error "No access check implemented!"
        #endif
    }

    void AddCoverPath(const std::filesystem::path Path)
    {
        AddSpecialPath(CoverPaths, Path);
    }

    void InitializePaths()
    {
        // Log directory (must be writable)
        if (!FindPath(LogPath, UPlatform::Platform.GetLogPath(), true))
        {
            ULog::Log.FileOutputEnabled = false;
            ULog::Log.LogWarnLocation("Log directory \"" + UPlatform::Platform.GetLogPath().string() + "\" not available");
        }

        auto SharedPath = UPlatform::Platform.GetGameSharedPath();
        auto UserPath = UPlatform::Platform.GetGameUserPath();

        FindPath(SoundPath, SharedPath / "sounds", false);
        FindPath(ThemePath, SharedPath / "themes", false);
        FindPath(SkinsPath, SharedPath / "themes", false);
        FindPath(LanguagesPath, SharedPath / "languages", false);
        FindPath(PluginPath, SharedPath / "plugins", false);
        FindPath(FontPath, SharedPath / "fonts", false);
        FindPath(ResourcesPath, SharedPath / "resources", false);
        FindPath(WebsitePath, SharedPath / "webs", false);
        FindPath(SoundFontsPath, SharedPath / "soundfonts", false);
        FindPath(AvatarsPath, SharedPath / "avatars", false);

        // Playlists are not shared as we need one directory to write too
        FindPath(PlaylistPath, UserPath / "playlists", true);

        // Screenshot directory (must be writable)
        if (!FindPath(ScreenshotsPath, UserPath / "screenshots", true))
        {
            ULog::Log.LogWarn("Screenshot directory \"" + UserPath.string() + "\" not available");
        }

        // Add song paths
        if (UCommandLine::Params.SongPath)
            AddSongPath(UCommandLine::Params.SongPath.value());

#ifdef (__APPLE__)
        AddSongPath(UPlatform::Platform.GetMusicPath);
#else
        AddSongPath(SharedPath / "songs");
#endif
        AddSongPath(UserPath / "songs");

        // Add category cover paths
        AddCoverPath(SharedPath / "covers");
        AddCoverPath(UserPath / "covers");
    }
}