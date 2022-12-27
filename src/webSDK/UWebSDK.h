#pragma once

#include <string>
#include <functional>
#include <array>
#include <cstdint>

namespace UWebSDK
{
  // Website
  struct TWebsiteInfo
  {
    std::string Name;
    int ID;
  };

  struct TSendInfo
  {
    std::string Username;   // Username & name of the player
    std::string Password;   // Password
    int ScoreInt;  // Player's Score Int
    int ScoreLineInt;  // Player's Score Line
    int ScoreGoldenInt;  // Player's Score Golden
    std::string MD5Song;   // Song Hash
    uint8_t Level;         // Level (0- Easy, 1- Medium, 2- Hard)
  };

  struct TLoginInfo
  {
    std::string Username;   // Username
    std::string Password;   // Password
  };

  typedef std::function<void(TWebsiteInfo& Info)> pModi_WebsiteInfo;
  typedef std::function<int(TSendInfo SendInfo)> fModi_SendScore;
  typedef std::function<std::wstring(TSendInfo SendInfo)> fModi_EncryptScore;
  typedef std::function<uint8_t(TLoginInfo LoginInfo)> fModi_Login;
  typedef std::function<std::wstring(TLoginInfo LoginInfo)> fModi_EncryptPassword;
  typedef std::function<std::wstring(std::wstring ListMD5Song, uint8_t Level)> fModi_DownloadScore;
  typedef std::function<std::wstring(std::wstring ListMD5Song)> fModi_VerifySong;
}