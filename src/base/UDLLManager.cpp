#include "UDLLManager.h"

#include "UPathUtils.h"
#include "ULog.h"

namespace UDLLManager
{
    
TDLLMan::TDLLMan()
{
  GetWebsiteList();
}

void TDLLMan::GetWebsiteList()
{
  for (const auto& FileInfo : std::filesystem::recursive_directory_iterator(UPathUtils::WebsitePath))
  {
    if (FileInfo.path().extension() == DLLExt)
    {
      if (auto info = LoadWebsiteInfo(FileInfo); info)
      {
        Websites.emplace_back(info.value());
        WebsitePaths.emplace_back(FileInfo.path().filename());
      }
    }
  }
}

std::optional<UWebSDK::TWebsiteInfo> TDLLMan::LoadWebsiteInfo(const std::filesystem::path Filename)
{
  UWebSDK::TWebsiteInfo out;

  // load libary
  auto hLibg = boost::dll::shared_library(boost::filesystem::path(UPathUtils::WebsitePath / Filename));
  // if loaded
  if (hLibg.is_loaded())
  {
    try
    {
      // load info void
      UWebSDK::pModi_WebsiteInfo Info = hLibg.get<UWebSDK::pModi_WebsiteInfo>("WebsiteInfo");
      // load website info
      Info(out);
      return out;
    }
    catch (...)
    {
      ULog::Log.LogError("Could not load website \"" + Filename.string() + "\": Info void not found");
    }
  }
  else
    ULog::Log.LogError("Could not load website \"" + Filename.string() + "\": Libary not loaded");
  return std::nullopt;
}

bool TDLLMan::LoadWebsite(uint32_t No)
{
  bool Result = true;
  //std::string functions_error;
  // load libary
  hLibW = boost::dll::shared_library(boost::filesystem::path(UPathUtils::WebsitePath / WebsitePaths[No]));
  // if loaded
  if (hLibW.is_loaded())
  {
    try
    {
      // load info void
      P_Login = hLibW.get<UWebSDK::fModi_Login>("Login");
      P_SendScore = hLibW.get<UWebSDK::fModi_SendScore>("SendScore");
      P_EncryptScore = hLibW.get<UWebSDK::fModi_EncryptScore>("EncryptScore");
      P_EncryptPassword = hLibW.get<UWebSDK::fModi_EncryptPassword>("EncryptPassword");
      P_VerifySong = hLibW.get<UWebSDK::fModi_VerifySong>("VerifySong");
      P_DownloadScore = hLibW.get<UWebSDK::fModi_DownloadScore>("DownloadScore");

      SelectedW = &Websites[No];
      Result = true;
    }
    catch (...)
    {
      ULog::Log.LogError("Could not load website \"" + WebsitePaths[No].string() + "\": Basic voids not found");
    }
  }
  else
    ULog::Log.LogError("Could not load website \"" + WebsitePaths[No].string() + "\": Libary not loaded");
  return Result;
}

void TDLLMan::UnLoadWebsite()
{
  if (hLibW.is_loaded())
    hLibW.unload();

  SelectedW = nullptr;
  P_SendScore       = nullptr;
  P_Login           = nullptr;
  P_EncryptScore    = nullptr;
  P_EncryptPassword = nullptr;
  P_DownloadScore   = nullptr;
  P_VerifySong      = nullptr;
}

uint8_t TDLLMan::WebsiteSendScore(UWebSDK::TSendInfo& SendInfo)
{
  if (P_SendScore)
    return P_SendScore(SendInfo);
  else
    return 0;
}

std::wstring TDLLMan::WebsiteEncryptScore(UWebSDK::TSendInfo& SendInfo)
{
  if (P_EncryptScore)
    return P_EncryptScore(SendInfo);
  else
    return L"";
}

uint8_t TDLLMan::WebsiteLogin(UWebSDK::TLoginInfo& LoginInfo)
{
  if (P_Login)
    return P_Login(LoginInfo);
  else
    return 0;
}

std::wstring TDLLMan::WebsiteEncryptPassword(UWebSDK::TLoginInfo& LoginInfo)
{
  if (P_EncryptPassword)
  {
    return P_EncryptPassword(LoginInfo);
  }
  else
    return L"";
}

std::wstring TDLLMan::WebsiteDownloadScore(std::wstring List_MD5Song, uint8_t Level)
{
  if (P_DownloadScore)
    return P_DownloadScore(List_MD5Song, Level);
  else
    return L"";
}

std::wstring TDLLMan::WebsiteVerifySong(std::wstring MD5Song)
{
  if (P_VerifySong)
    return P_VerifySong(MD5Song);
  else
    return L"";
}
}