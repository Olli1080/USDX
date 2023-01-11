#include "USkins.h"

#include "IniFileHelper.hpp"
#include "UIni.h"

#include "UPathUtils.h"

namespace USkins
{
    TSkin::TSkin()
    {
        LoadList();
        //  LoadSkin("...");
        //  SkinColor := Color;
    }

    void TSkin::LoadList()
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(UPathUtils::SkinsPath))
        {
            if (!entry.is_regular_file())
                continue;

            if (entry.path().extension() != ".yaml")
                continue;

            LoadHeader(entry.path());
        }
    }

    void TSkin::LoadHeader(const std::filesystem::path& FileName)
    {
        auto SkinIni = TMemIniFile(FileName);

        Skin.emplace_back
        (
            SkinIni.ReadString("Skin", "Theme", ""),
            SkinIni.ReadString("Skin", "Name", ""),
            FileName.parent_path(),
            FileName.filename(),
            std::max(0, GetArrayIndex(UIni::IColor, SkinIni.ReadString("Skin", "Color", ""), true)),
            SkinIni.ReadString("Skin", "Creator", "")
        );
    }

    void TSkin::LoadSkin(const std::string& Name, const std::string& Theme)
    {
		size_t S = GetSkinNumber(Name, Theme);

		auto SkinIni = TMemIniFile(Skin[S].Path / Skin[S].FileName);

        std::list<std::string> SL;
        SkinIni.ReadSection("Textures", SL);

        SkinTexture.resize(0);
        SkinTexture.reserve(SL.size());
        for (const auto& s : SL)
        {
            SkinTexture.emplace_back
            (
                s,
                std::filesystem::path(SkinIni.ReadString("Textures", s, ""))
            );
        }
    }

    std::filesystem::path TSkin::GetTextureFileName(const std::string& TextureName)
    {
        if (TextureName.empty())
            return "";

        for (const auto& skT : SkinTexture)
        {
            if (skT.Name == TextureName &&
                !skT.FileName.empty())
                return SkinPath / skT.FileName;
        }
        return "";
        //Log.LogError("", "-----------------------------------------");
        //Log.LogError("Was not able to retrieve Texture for " + TextureName + " - " + Result.ToNative, "TSkin::GetTextureFileName");
    }

    int TSkin::GetSkinNumber(const std::string& Name, const std::string& Theme)
    {
        for (size_t S = 0; S < Skin.size(); ++S)
            if (Skin[S].Name == Name && Skin[S].Theme == Theme)
                return S;
        return 0; // set default to the first available skin
    }

    void TSkin::GetSkinsByTheme(const std::string& Theme, std::vector<std::string>& Skins)
    {
        Skins.clear();
        for (const auto& sk : Skin)
        {
            if (Theme != sk.Theme)
                continue;

            Skins.emplace_back(sk.Name);
        }
    }

    /* returns number of default color for skin with
        index SkinNo in ISkin(not in the actual skin array) */
    int TSkin::GetDefaultColor(int SkinNo)
    {
        //TODO:: does SkinNo do anything?
        for (const auto& sk : Skin)
            if (UIni::ITheme[UIni::Ini.Theme] == sk.Theme)
            {
                if (SkinNo > 0)
                    --SkinNo;
                else
                {
					return sk.DefaultColor;
                }
            }
        return 0;
    }

    void TSkin::onThemeChange()
    {
	    UIni::Ini.SkinNo = 0;
        GetSkinsByTheme(UIni::ITheme[UIni::Ini.Theme], UIni::ISkin);
    }
}
