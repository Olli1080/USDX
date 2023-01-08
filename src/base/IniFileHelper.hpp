#pragma once

#include <string>
#include <list>
#include <filesystem>

class TCustomIniFile
{
public:
    std::string ReadString(std::string section, std::string key, std::string fallback) const
    {
        return "";
    }

    void ReadSection(std::string, std::list<std::string>& out) const
    {

    };

    bool ValueExists(std::string, std::string) const
    {
        return false;
    };

    int ReadInteger(std::string, std::string, int) const
    {

    };

    void WriteInteger(std::string, std::string, int)
    {

    };

    void WriteString(std::string, std::string, std::string)
    {

    }

    void DeleteKey(std::string, std::string)
    {

    };

    void ReadSections(std::list<std::string>&)
    {};

    bool SectionExists(std::string)
    {};

    float ReadFloat(std::string, std::string, float def)
    {}

    void ReadSectionValues(std::string, std::list<std::string>&)
    {}
};

class TIniFile : public TCustomIniFile
{
public:
    TIniFile(std::filesystem::path p)
    {};
};

class TMemIniFile : public TIniFile
{

};

class TUnicodeMemIniFile : public TMemIniFile
{
public:
    TUnicodeMemIniFile(std::filesystem::path p) : TMemIniFile()
    {};
};
/*
class TEncoding
{

};
*/