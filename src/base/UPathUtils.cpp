#include "UPathUtils.h"

#if defined(_WIN32)
#include <io.h>
#elif defined(__unix__)
#include <unistd.h>
//#else
//#error "No implementation defined"
#endif

namespace UPathUtils
{
    void AddSpecialPath(std::vector<std::filesystem::path>& PathList, const std::filesystem::path Path)
    {
        std::filesystem::path PathAbs, PathTmp;
        std::filesystem::path OldPath, OldPathAbs, OldPathTmp;
        
        if (Path.empty() || !std::filesystem::create_directory(Path))
            return;

        PathAbs = std::filesystem::absolute(Path);

        // check if path or a part of the path was already added
        for (int i = 0; i < PathList.size(); ++i)
        {
            auto& OldPath = PathList[i];
            OldPathAbs = std::filesystem::absolute(OldPath);

            // check if the new directory is a sub-directory of a previously added one.
            // This is also true, if both paths point to the same directories.
            if (OldPathAbs.IsChildOf(PathAbs, false) || OldPathAbs == PathAbs)
            {
                // ignore the new path
                return;
            }

            // check if a previously added directory is a sub-directory of the new one.
            if (PathAbs.IsChildOf(OldPathAbs, false))
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


    /**
     * Initialize a path variable
     * After setting paths, make sure that paths exist
     */
    bool FindPath(std::filesystem::path& PathResult, const std::filesystem::path RequestedPath, bool NeedsWritePermission)
    {
        /*if (RequestedPath.Equals(PATH_NONE)) then
            Exit;*/

        // Make sure the directory exists
        if (!std::filesystem::create_directory(RequestedPath))
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
}