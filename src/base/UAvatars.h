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
 * $URL: $
 * $Id:  $
 */
#include <filesystem>
#include <exception>
#include <cstdint>
#include <memory>

#include <sqlite_orm/sqlite_orm.h>

#include "UIni.h"
#include "UDataBaseHelper.hpp"
#include "../switches.h"

namespace UAvatars
{
    const std::string AVATARDB_FILENAME = "avatar.db";
    const int AVATARDB_VERSION = 01; // 0.1
    const std::string AVATAR_TBL = "Avatar";
    const std::string AVATAR_THUMBNAIL_TBL = "AvatarThumbnail";
    const std::string AVATAR_IDX = "Avatar_Filename_IDX";

    class ECoverDBException : public std::exception
    {};

    struct AvatarData
    {
        int64_t ID;
        std::filesystem::path Filename;
        std::chrono::system_clock::time_point Date;
        int Width;
        int Height;

        static auto getTable()
        {
            using namespace sqlite_orm;
            return make_table
            (
                AVATAR_TBL,
                make_column("ID", &AvatarData::ID, autoincrement(), primary_key()),
                make_column("Filename", &AvatarData::Filename, unique()),
                make_column("Date", &AvatarData::Date),
                make_column("Width", &AvatarData::Width),
                make_column("Height", &AvatarData::Height)
            );
        }
        static auto getFilenameIndex()
        {
            using namespace sqlite_orm;
            return make_unique_index
            (
                AVATAR_IDX,
                indexed_column(&AvatarData::Filename).asc()
            );
        }
    };

    struct ThumbnailData
    {
        int64_t ID;
        int Format;
        int Width;
        int Height;
        std::shared_ptr<std::vector<uint8_t>> Data;

        static auto getTable()
        {
            using namespace sqlite_orm;
            return make_table
            (
                AVATAR_TBL,
                make_column("ID", &ThumbnailData::ID, primary_key()),
                make_column("Format", &ThumbnailData::Format),
                make_column("Width", &ThumbnailData::Width),
                make_column("Height", &ThumbnailData::Height),
                make_column("Data", &ThumbnailData::Data)
            );
        }
    };

    class TAvatar
    {
    private:

        int64_t ID;
        std::filesystem::path Filename;

    public:

        TAvatar(int64_t ID, std::filesystem::path Filename);
        [[nodiscard]] UTexture::PTexture GetPreviewTexture() const;
        [[nodiscard]] UTexture::PTexture GetTexture() const;
    };

    struct TAvatarThumbnailInfo
    {
        int AvatarWidth;         // Original width of avatar
        int AvatarHeight;        // Original height of avatar
        UImage::TImagePixelFmt PixelFormat; // Pixel-format of avatar thumbnail
    };

    struct Thumbnail : TAvatarThumbnailInfo
    {
        USDX::SDL_Surface_SPtr surface;
    };

    static auto createDatabase(const std::filesystem::path& path)
    {
        using namespace sqlite_orm;
        return make_storage
        (
            path.string(),
            AvatarData::getFilenameIndex(),
            AvatarData::getTable(),
            ThumbnailData::getTable()
        );
    }

    class TAvatarDatabase
    {
    private:

        
        typedef std::invoke_result_t<decltype(&createDatabase), std::filesystem::path> DataBase;

        std::unique_ptr<DataBase> DB;
        
        std::optional<Thumbnail> CreateAvatarThumbnail(const std::filesystem::path Filename);
        void DeleteAvatar(int64_t AvatarID);
        [[nodiscard]] std::optional<int64_t> FindAvatarIntern(const std::filesystem::path Filename) const;
        void Open();
        [[nodiscard]] int GetVersion() const;
        void SetVersion(int Version);

    public:

        TAvatarDatabase();

        std::optional<TAvatar> AddAvatar(const std::filesystem::path Filename);
        [[nodiscard]] std::optional<TAvatar> FindAvatar(const std::filesystem::path Filename) const;
        [[nodiscard]] UTexture::PTexture LoadAvatar(int64_t AvatarID) const;
        [[nodiscard]] bool AvatarExists(const std::filesystem::path Filename) const;
        int GetMaxAvatarSize();
        void SetMaxAvatarSize(int Size);

    	static void LoadAvatars();
    };

    inline TAvatarDatabase Avatars;
    inline std::vector<std::filesystem::path> AvatarsList;
    inline std::array<UTexture::PTexture, UIni::IMaxPlayerCount> NoAvatarTexture;
    inline std::array<UTexture::PTexture, UIni::IMaxPlayerCount> AvatarPlayerTextures;
}