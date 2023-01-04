#include "ULanguage.h"

#include <boost/algorithm/string/case_conv.hpp>

#include "ULog.h"
#include "UPathUtils.h"

namespace ULanguage
{
/**
 * LoadList, set default language, set standard implode glues
 */
TLanguage::TLanguage()
{
    bool DefaultLanguageFound = false;

    //Load list of available translations (*.ini files in LanguagesPath)
    LoadList();

    //Set Implode Glues for Backward Compatibility
    Implode_Glue1 = ", ";
    Implode_Glue2 = " and ";

    //If no translations are found, abort loading
    if (List.empty())
        ULog::Log.CriticalError("Could not load any language file");

    UIni::ILanguageTranslated.resize(UIni::ILanguage.size());

    //Load each translation and store the native language name
    auto it = List.begin();
    for (int i = 0; i < List.size(); ++i, ++it)
    {
        ChangeLanguage(it->Name);
        UIni::ILanguageTranslated[i] = Translate("LANGUAGE");

        //If currently loaded translation is English, use this translation as default (for incomplete translation files)
        if (boost::algorithm::to_upper_copy(it->Name) == "ENGLISH")
        {
            EntryDefault.resize(Entry.size());
            for (int j = 0; j < Entry.size(); ++j)
                EntryDefault[j] = Entry[j];

            Entry.resize(0);
            DefaultLanguageFound = true;
        }
    }

    //Log an error if English language file was not found and therefore no default translation is available
    if (DefaultLanguageFound)
        ULog::Log.LogStatus("Load Default Language English", "Initialization");
    else
        ULog::Log.LogError("English language file missing! No default translation loaded");
}

/**
* Parse the Language Dir searching Translations
*/
void TLanguage::LoadList()
{
    std::filesystem::directory_iterator it();
    std::string LangName;

    List.resize(0);
    UIni::ILanguage.resize(0);

    for (const auto& entry : std::filesystem::directory_iterator(UPathUtils::LanguagesPath))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".ini")
            continue;

        auto filename = entry.path().filename().string();

        List.emplace_back(filename);
        UIni::ILanguage.emplace_back(filename);
    }
}

/**
 * Load the specified LanguageFile
 */
void TLanguage::ChangeLanguage(const std::string Language)
{
  /*
  var
  IniFile:    TUnicodeMemIniFile;
  E:          integer; // entry
  S:          TStringList;
  */
  Entry.resize(0);
  auto IniFile = TUnicodeMemIniFile((UPathUtils::LanguagesPath / Language).concat(".ini"));
  std::list<std::string> S;

  IniFile.ReadSectionValues("Text", S);
  for (int E = 0; E < Entry.size(); ++E)
  {
    if (S.Names[E] == "IMPLODE_GLUE1")
      Implode_Glue1 = S.ValueFromIndex[E]+ " ";
    else if (S.Names[E] = "IMPLODE_GLUE2")
      Implode_Glue2 = " " + S.ValueFromIndex[E] + " ";

    Entry[E].ID = S.Names[E];
    Entry[E].Text = S.ValueFromIndex[E];
  }
}

/**
 * Find the index of ID an array of language entries.
 * @returns the index on success, -1 otherwise.
 */
int TLanguage::FindID(const std::string ID, const std::vector<TLanguageEntry> EntryList)
{
  for (int i = 0; i < EntryList.size(); ++i)
  {
    if (ID == EntryList[i].ID)
    {
      return i;
    }
  }
  return -1;
}

/**
 * Translate the Text.
 * If Text is an ID, text will be translated according to the current language
 * setting. If Text is not a known ID, it will be returned as is. 
 * @param Text either an ID or an UTF-8 encoded string 
 */
std::string TLanguage::Translate(const std::string Text)
{
  // normalize ID case
  std::string ID = boost::algorithm::to_upper_copy(Text);

  // Check if ID exists

  //Const Mod
  int EntryIndex = FindID(ID, EntryConst);
  if (EntryIndex >= 0)
    return EntryConst[EntryIndex].Text;

  EntryIndex = FindID(ID, Entry);
  if (EntryIndex >= 0)
    return Entry[EntryIndex].Text;

  //Standard Language (If a Language File is Incomplete)
  //Then use Standard Language
  EntryIndex = FindID(ID, EntryDefault);
  if (EntryIndex >= 0)
    return EntryDefault[EntryIndex].Text;

  // fallback result in case Text is not a known ID
  return Text;
}

/**
 * Add a Constant ID that will be Translated but not Loaded from the LanguageFile
 */
void TLanguage::AddConst(const std::string ID, const std::string Text)
{
  EntryConst.emplace_back(TLanguageEntry{ ID, Text });
}

/**
 * Change a Constant Value by ID
 */
void TLanguage::ChangeConst(const std::string ID, const std::string Text)
{
  for (int i = 0; i < EntryConst.size(); ++i)
  {
    if (EntryConst[i].ID == ID)
    {
      EntryConst[i].Text = Text;
      return;
    }
  }
}

/**
 * Connect an array of strings with ' and ' or ', ' to one string
 */
std::string TLanguage::Implode(const std::vector<std::string> Pieces)
{
  std::string Result = "";
  //Go through Pieces

  for (int i = 0; i < Pieces.size(); ++i)
  {
    Result += Pieces[i];

    //Add Glue
    if (i < Pieces.size() - 1)
      Result += Implode_Glue1;
    else if (i < Pieces.size())
      Result += Implode_Glue2;
  }
}
}
