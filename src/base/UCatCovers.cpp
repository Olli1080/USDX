#include "UCatCovers.h"

#include <boost/algorithm/string/case_conv.hpp>

#include "UPathUtils.h"

namespace UCatCovers
{

TCatCovers::TCatCovers()
{
    Load();
}

void TCatCovers::Load()
{
    for (const auto& path : UPathUtils::CoverPaths)
        LoadPath(path);
}

/**
 * Load Cover from Cover.ini and Cover Folder
 */
void TCatCovers::LoadPath(const std::filesystem::path CoversPath)
{
    try
    {
        auto Ini = TMemIniFile(CoversPath / "covers.ini");
        std::list<std::string> List;

        //Add every Cover in Covers Ini for Every Sorting option
        for (int SortType = 0; SortType < UIni::TSortingType::Size; ++SortType)
        {
            Ini.ReadSection(UIni::ISorting[SortType], List);

            for (const auto& li : List)
            {
                auto CatCover = std::filesystem::path(Ini.ReadString(UIni::ISorting[SortType], li, 'NoCover.jpg'));
                Add((UIni::TSortingType)SortType, li, CoversPath / CatCover);
            }
        }
    }
    catch (...)
    {
    }

    //Add Covers from Folder
    for (const auto& entry : std::filesystem::directory_iterator(CoversPath))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".jpg")
            continue;

        //Add Cover if it doesn't exist for every Section
        for (int SortType = 0; SortType < UIni::TSortingType::Size; ++SortType)
        {
            auto TmpName = entry.path().filename().string();
            if (SortType == UIni::sTitle)
            {
                const std::string tag = "Title";
                const size_t pos = TmpName.find(tag);
                if (pos != std::string::npos)
                    TmpName.erase(pos, tag.size());
            }
            else if (SortType == UIni::sArtist)
            {
                const std::string tag = "Artist";
                const size_t pos = TmpName.find(tag);
                if (pos != std::string::npos)
                    TmpName.erase(pos, tag.size());
            }

            if (!CoverExists((UIni::TSortingType)SortType, TmpName))
                Add((UIni::TSortingType)SortType, TmpName, entry.path());
        }
    }
}

//Add a Cover
void TCatCovers::Add(UIni::TSortingType Sorting, const std::string Name, const std::filesystem::path Filename)
{
    if (is_regular_file(Filename)) //If Exists -> Add
    {
        cNames[Sorting].emplace_back(boost::algorithm::to_upper_copy(Name));
        cFiles[Sorting].emplace_back(Filename);
    }
}

//Returns True when a cover with the given Name exists
bool TCatCovers::CoverExists(UIni::TSortingType Sorting, const std::string Name)
{
    const auto UpperName = boost::algorithm::to_upper_copy(Name); //Case Insensitiv
    return std::ranges::find(cNames[Sorting], UpperName) != cNames[Sorting].end();
}

//Returns the Filename of a Cover
std::filesystem::path TCatCovers::GetCover(UIni::TSortingType Sorting, const std::string Name)
{
    auto UpperName = boost::algorithm::to_upper_copy(Name);

    auto it = std::ranges::find(cNames[Sorting], UpperName);
    if (it != cNames[Sorting].end())
        return *it;

    //No Cover
    for (const auto& cp : UPathUtils::CoverPaths)
    {
        auto NoCoverPath = cp / "NoCover.jpg";
        if (is_regular_file(NoCoverPath))
        {
            return NoCoverPath;
        }
    }
    return "";
}
}