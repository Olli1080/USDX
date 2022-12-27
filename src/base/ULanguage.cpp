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
  IniFile = TUnicodeMemIniFile.Create(LanguagesPath.Append(Language + ".ini"));
  S = TStringList.Create;

  IniFile.ReadSectionValues("Text", S);
  SetLength(Entry, S.Count);

  for (int E = 0; E < Entry.size(); ++E)
  {
    if (S.Names[E] = "IMPLODE_GLUE1")
      Implode_Glue1 = S.ValueFromIndex[E]+ " ";
    else if (S.Names[E] = "IMPLODE_GLUE2")
      Implode_Glue2 = " " + S.ValueFromIndex[E] + " ";

    Entry[E].ID = S.Names[E];
    Entry[E].Text = S.ValueFromIndex[E];
  }

  S.Free;
  IniFile.Free;
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
  std::string ID = UpperCase(Text);

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
    if (EntryConst[I].ID = ID)
    {
      EntryConst[I].Text = Text;
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