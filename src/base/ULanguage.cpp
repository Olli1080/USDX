#include "ULanguage.h"

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
    Log.CriticalError("Could not load any language file");

  UIni::ILanguageTranslated.resize(UIni::ILanguage.size());

  //Load each translation and store the native language name
  for (int i = 0; i < List.size(); ++i)
  {
    ChangeLanguage(List[i].Name);
    UIni::ILanguageTranslated[i] = Translate("LANGUAGE");

    //If currently loaded translation is English, use this translation as default (for incomplete translation files)
    if (Uppercase(List[i].Name) == "ENGLISH")
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
    Log.LogStatus("Load Default Language English", "Initialization");
  else
    Log.LogError("English language file missing! No default translation loaded");
}

/**
 * Parse the Language Dir searching Translations
 */
void TLanguage::LoadList()
{
  IFileIterator Iter;
  TFileInfo IniInfo;

  std::filesystem::directory_iterator it();
  std::string LangName;
  
  List.resize(0);
  UIni::ILanguage.resize(0);

  Iter = FileSystem.FileFind(LanguagesPath.Append("*.ini"), 0);
  while(Iter.HasNext)
  {
    IniInfo = Iter.Next;

    LangName = IniInfo.Name.SetExtension("").ToUTF8;

    SetLength(List, Length(List)+1);
    List[High(List)].Name = LangName;

    SetLength(ILanguage, Length(ILanguage)+1);
    ILanguage[High(ILanguage)] = LangName;
  }
}

}