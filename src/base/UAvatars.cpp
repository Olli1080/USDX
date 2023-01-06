#include "UAvatars.h"

#include "UPathUtils.h"
#include "UPlatform.hpp"

namespace UAvatars
{
    //  TCover
    TAvatar::TAvatar(int64_t ID, std::filesystem::path Filename)
        :
        ID(ID), Filename(std::move(Filename))
    {}

    UTexture::PTexture TAvatar::GetPreviewTexture() const
    {
        return Avatars.LoadAvatar(ID);
    }

    UTexture::PTexture TAvatar::GetTexture() const
    {
        return UTexture::Texture.LoadTexture(Filename);
    }

    //  TCoverDatabase 
    TAvatarDatabase::TAvatarDatabase()
    {
        //TODO:: remove thumbs for files that do not longer exist
        //TODO:: check hashes on startup?
        Open();
        LoadAvatars();
        //InitAvatarDatabase();
    }

    int TAvatarDatabase::GetVersion() const
    {
        return DB->pragma.user_version();
    }

    void TAvatarDatabase::SetVersion(int Version)
    {
        DB->pragma.user_version(Version);
    }

    int TAvatarDatabase::GetMaxAvatarSize()
    {
        return UIni::ITextureSizeVals[UIni::Ini.TextureSize];
    }

    void TAvatarDatabase::SetMaxAvatarSize(int Size)
    {
        // search for first valid avatar-size > Size
        for (size_t I = 0; I < UIni::ITextureSizeVals.size(); ++I)
        {
            if (Size <= UIni::ITextureSizeVals[I])
            {
                UIni::Ini.TextureSize = I;
                return;
            }
        }
        // fall-back to highest size
        UIni::Ini.TextureSize = UIni::ITextureSizeVals.size() - 1;
    }

    void TAvatarDatabase::Open()
        /*var
            int Version;
        std::filesystem::path Filename;*/
    {
        auto Filename = UPlatform::Platform.GetGameUserPath() / AVATARDB_FILENAME;

        DB = std::make_unique<DataBase>(createDatabase(Filename));
        //auto Version = GetVersion();

        DB->sync_schema();

        // check version, if version is too old/new, delete database file
        /*if (Version != 0 && Version != AVATARDB_VERSION)
        {
            ULog::Log.LogInfo("Outdated avatar-database file found", "TAvatarDatabase::Open");
            // close and delete outdated file
            
            if (!Filename.DeleteFile())
                throw ECoverDBException("Could not delete " + Filename.ToNative);
            // reopen
            DB = TSQLiteDatabase.Create(Filename.ToUTF8());
            Version = 0;
        }

        // set version number after creation
        if (Version == 0)
            SetVersion(AVATARDB_VERSION);

        // speed-up disk-writing. The default FULL-synchronous mode is too slow.
        // With this option disk-writing is approx. 4 times faster but the database
        // might be corrupted if the OS crashes, although this is very unlikely.
        */
        SetVersion(AVATARDB_VERSION);
        DB->pragma.synchronous(0);

        // the next line rather gives a slow-down instead of a speed-up, so we do not use it
        //DB.ExecSQL("PRAGMA temp_store = MEMORY;");
    }

    std::optional<int64_t> TAvatarDatabase::FindAvatarIntern(const std::filesystem::path Filename) const
    {
        using namespace sqlite_orm;
        auto temp = DB->select(&AvatarData::ID, where(c(&AvatarData::Filename) == Filename));
        if (temp.empty())
            return std::nullopt;
        return temp.front();
    }

    std::optional<TAvatar> TAvatarDatabase::FindAvatar(const std::filesystem::path Filename) const
    {
        try
        {
	        if (const auto AvatarID = FindAvatarIntern(Filename); AvatarID)
                return TAvatar(AvatarID.value(), Filename);
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogErrorLocation(e.what());
        }
        return std::nullopt;
    }

    bool TAvatarDatabase::AvatarExists(const std::filesystem::path Filename) const
    {
        try
        {
            return FindAvatarIntern(Filename).has_value();
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogErrorLocation(e.what());
        }
        return false;
    }

    std::optional<TAvatar> TAvatarDatabase::AddAvatar(const std::filesystem::path Filename)
    {
        //if (not FileExists(Filename)) then
        //  Exit;

        // TODO: replace "\" with "/" in filename
        const auto FileDate = std::chrono::system_clock::now(); //FileDateToDateTime(FileAge(Filename));

        const auto ThumbnailOpt = CreateAvatarThumbnail(Filename);
        if (!ThumbnailOpt)
            return std::nullopt;

        /*AvatarData = TBlobWrapper::Create;
        AvatarData.Write(Thumbnail ^ .pixels, Thumbnail ^ .h * Thumbnail ^ .pitch);*/
        const auto& Thumbnail = ThumbnailOpt.value();
        auto& ThumbSurface = Thumbnail.surface;

        try
        {
            // Note: use a transaction to speed-up file-writing.
            // Without data written by the first INSERT might be moved at the second INSERT.
            auto guard = DB->transaction_guard();

            // add general cover info && get auto-generated avatar ID
            const auto AvatarID = DB->insert(
                AvatarData{ -1,
                    Filename, FileDate,
                    Thumbnail.AvatarWidth, Thumbnail.AvatarHeight
                });

            auto pixels = static_cast<uint8_t*>(ThumbSurface->pixels);
            // add thumbnail info
            DB->insert(
                ThumbnailData{
                    AvatarID, Thumbnail.PixelFormat,
                    ThumbSurface->w, ThumbSurface->h,
                   std::make_shared<std::vector<uint8_t>>
	                (
	                    pixels,
	                    pixels + ThumbSurface->h * ThumbSurface->pitch
	                )
                });
            guard.commit();
            return TAvatar(AvatarID, Filename);
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogErrorLocation(e.what());
        }
        return std::nullopt;
    }

    UTexture::PTexture TAvatarDatabase::LoadAvatar(int64_t AvatarID) const
    {
        using namespace sqlite_orm;
        try
        {
            auto Tab = DB->select(
                columns(&AvatarData::Filename, &ThumbnailData::Format, &ThumbnailData::Width, &ThumbnailData::Height, &ThumbnailData::Data),
                inner_join<ThumbnailData>(using_(&ThumbnailData::ID)),
                where(c(&ThumbnailData::ID) == AvatarID)
            );
            if (Tab.empty())
                return nullptr;
            auto [Filename, PixelFmt, Width, Height, Data] = Tab.front();
            
            if (Data && PixelFmt == UImage::ipfRGB)
                return UTexture::Texture.CreateTexture(*Data, Filename, Width, Height);
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogErrorLocation(e.what());
        }
        return nullptr;
    }

    void TAvatarDatabase::DeleteAvatar(int64_t AvatarID)
    {
        DB->remove<AvatarData>(AvatarID);
        DB->remove<ThumbnailData>(AvatarID);
    }

    /**
        *Returns a pointer to an array of bytes containing the texture data in the
        * requested size
        */
    std::optional<Thumbnail> TAvatarDatabase::CreateAvatarThumbnail(const std::filesystem::path Filename)
            
    {
        //TargetAspect, SourceAspect: double;
        //TargetWidth, TargetHeight: int;
        const int MaxSize = GetMaxAvatarSize();

        const auto ThumbnailSurface = UImage::ImageLoad(Filename);
        if (!ThumbnailSurface)
        {
            ULog::Log.LogErrorLocation("Could not load avatar: \"" + Filename.string() + "\"");
            return std::nullopt;
        }

        // Convert pixel format as needed
        AdjustPixelFormat(*ThumbnailSurface, UTexture::TEXTURE_TYPE_PLAIN);

        Thumbnail out;
        out.AvatarWidth = ThumbnailSurface->w;
        out.AvatarHeight = ThumbnailSurface->h;
        out.PixelFormat = UImage::ipfRGB;

        /*TODO: keep aspect ratio
            TargetAspect = Width / Height;
        SourceAspect = TexSurface.w / TexSurface.h;

        // Scale texture to covers dimensions (keep aspect)
        if (SourceAspect >= TargetAspect) then
            {
            TargetWidth = Width;
        TargetHeight = Trunc(Width / SourceAspect);
        }
        else
            {
            TargetHeight = Height;
        TargetWidth = Trunc(Height * SourceAspect);
        }
        */

        // TODO: do not scale if image is smaller
        UImage::ScaleImage(*ThumbnailSurface, MaxSize, MaxSize);
        out.surface = ThumbnailSurface;

        return out;
    }

    void TAvatarDatabase::LoadAvatars()
    {
        // first position for no-avatar TODO::why?
        //SetLength(AvatarsList, 1);

        for (const auto& entry : std::filesystem::directory_iterator(UPathUtils::AvatarsPath))
        {
            if (!entry.is_regular_file())
                continue;

            //TODO:: check with macro or std::set or sth else if format is supported
            const auto ext = entry.path().extension();
            switch (ext)
            {
            case ".jpg": [[fallthrough]]
            case ".png": 
                AvatarsList.emplace_back(entry.path());
                break;
            default:
                break;
            }
        }
    }
}
