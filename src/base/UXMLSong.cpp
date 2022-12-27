#include "UXMLSong.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace UXMLSong
{
    TParser::TParser()
    {
        //DefaultFormatSettings.DecimalSeparator = ".";
    }

bool TParser::ParseSong(const std::filesystem::path Filename)
/*var
  I: Integer;
  FileStream: TBinaryFileStream;*/
{
    bool Result = false;
    if (std::filesystem::is_regular_file(Filename))
    {
        ErrorMessage = "Can't open melody.xml file";

        SSFile = TStringList.Create;
        FileStream = TBinaryFileStream.Create(Filename, fmOpenRead);
        try
        {
            SSFile.LoadFromStream(FileStream);

            ErrorMessage = "";
            Result = true;

            int i = 0;

            SongInfo.CountSentences = 0;
            CurDuetSinger  = DS_Both; //Both is Singstar Standard
            CurPosInSong   = 0; //Start at Pos 0
            BindLyrics     = true; //Dont start with Space
            FirstNote      = true; //First Note found should be the First Note ;)

            SongInfo.Header.Language = "";
            SongInfo.Header.Edition = Edition;
            SongInfo.DualChannel = false;

            ParserState    = PS_None;

            SongInfo.Sentences.resize(0);

            while (Result && (i < SSFile.size()))
            {
                Result = ParseLine(SSFile.Strings[i]);

                ++i;
            }
        }
        catch (...)
        {
            SSFile.Free;
            FileStream.Free;
        }
    }
}

bool TParser::ParseSongHeader(const std::filesystem::path Filename)
/*var
  I: Integer;
  Stream: TBinaryFileStream;*/
{
  bool Result = false;

  if (std::filesystem::is_regular_file(Filename))
  {
    SSFile = TStringList.Create;
    Stream = TBinaryFileStream.Create(Filename, fmOpenRead);
    try
    {
      SSFile.LoadFromStream(Stream);

      if (SSFile.Count > 0)
      {
        Result = true;
        int i = 0;

        SongInfo.CountSentences = 0;
        CurDuetSinger = DS_Both; // Both is Singstar Standard
        CurPosInSong  = 0;       // Start at Pos 0
        BindLyrics    = true;    // Don"t start with Space
        FirstNote     = true;    // First Note found should be the First Note ;)

        SongInfo.ID = 0;
        SongInfo.Header.Language = "";
        SongInfo.Header.Edition = Edition;
        SongInfo.DualChannel = false;
        ParserState    = PS_None;

        while ((SongInfo.ID < 4) && Result && (i < SSFile.Count))
        {
          Result = ParseLine(SSFile.Strings[i]);

          ++i;
        }
      }
      else
        ErrorMessage = "Can""t open melody.xml file";
    }
    catch (...)
    {
      SSFile.Free;
      Stream.Free;
    }
  }
  else
    ErrorMessage = "Can""t find melody.xml file";
}

bool TParser::ParseLine(std::string Line)
/*var
  Tag:    String;
  Values: String;
  AValues: Array of Record
    Name: String;
    Value: String;
  }
  I, J, K:   Integer;
  Duration, Tone: Integer;
  Lyric: String;
  NoteType: Byte;
  IsRap: Boolean;
*/
{
  std::string Values;
  struct Value 
  {
    std::string Name;
    std::string Value;
  };
  std::vector<Value> AValues;

  auto MakeValuesArray = [&Values, &AValues]() 
  {
    //var Len, Pos, State, StateChange: Integer;
    int StateChange;
    int Len = 0;
    AValues.resize(0);

    int Pos = 0;
    int State = 0;
    while (Pos < Values.size() && Pos != std::string::npos)
    {
        switch(State)
        {
            case 0:
            { //Search for ValueName
                if (Values[Pos] != ' ' && Values[Pos] != '=')
                {
                    //Found Something
                    State = 1; //State search for "="
                    StateChange = Pos; //Save Pos of Change
                    Pos = Values.find('=', Pos + 1);
                }
                else 
                    ++Pos; //When nothing found then go to next char
                break;
            }
            case 1: 
            {   //Search for Equal Mark
                //Add New Value
                ++Len;
                AValues.resize(Len);
                AValues[Len - 1].Name = boost::algorithm::to_upper_copy(Values.substr(StateChange - 1, Pos - StateChange));


                State = 2; //Now Search for starting "\""
                StateChange = Pos; //Save Pos of Change
                Pos = Values.find('\"', Pos + 1);
                break;
            }
            case 2: 
            { //Search for starting """ or " " <- End if there was no "
                if (Values[Pos] == '\"')
                { //Found starting """
                    State = 3; //Now Search for ending "\""
                    StateChange = Pos; //Save Pos of Change
                    Pos = Values.find('\"', Pos + 1);
                }
                else if (Values[Pos] == ' ') //Found ending Space
                {
                    //Save Value to Array
                    AValues[Len - 1].Value = Values.substr(StateChange, Pos - StateChange - 1);

                    //Search for next Valuename
                    State = 0;
                    StateChange = Pos;
                    ++Pos;
                }
                break;
            }
            case 3: 
            {   //Search for ending "\""
                //Save Value to Array
                AValues[Len - 1].Value = Values.substr(StateChange, Pos - StateChange - 1);

                //Search for next Valuename
                State = 0;
                StateChange = Pos;
                ++Pos;
                break;
            }
        }

        if (State >= 2)
        { //Save Last Value
            AValues[Len - 1].Value = Values.substr(StateChange, Values.size() - StateChange);
        }
    }
  };

  bool Result = true;

  boost::algorithm::trim(Line);
  if (!Line.empty())
  {
    int I = Line.find('<');
    int J = Line.find(' ', I + 1);
    int K = Line.find('>', I + 1);

    if (J = 0)
        J = K;
    else if (K < J && K != 0)
        J = K; //Use nearest Tagname End indicator
    std::string Tag = boost::algorithm::to_upper_copy(Line.substr(I, J - I - 1));
    Values = Line.substr(J, K - J - 1);

    switch(ParserState)
    {
      case PS_None: 
      {//Search for Melody Tag
        if (Tag == "MELODY")
        {
          ++SongInfo.ID; //Inc SongID when header Information is added
          MakeValuesArray();
          for (auto& avalue : AValues)
          {
            if (avalue.Name == "TEMPO")
            {
              try 
              {
                SongInfo.Header.BPM = std::stof(avalue.Value);
              }
              catch (...)
              {
                SongInfo.Header.BPM = 0.f;
              }
              if (SongInfo.Header.BPM <= 0.f)
              {
                Result = false;
                ErrorMessage = "Can't read BPM from Song";
              }
            }
            else if (avalue.Name == "RESOLUTION")
            {
              boost::algorithm::to_upper(avalue.Value);
              //Ultrastar Resolution is "how often a Beat is split / 4"
              if (avalue.Value == "HEMIDEMISEMIQUAVER")
                SongInfo.Header.Resolution = 64 / 4;
              else if (avalue.Value == "DEMISEMIQUAVER")
                SongInfo.Header.Resolution = 32 / 4;
              else if (avalue.Value == "SEMIQUAVER")
                SongInfo.Header.Resolution = 16 / 4;
              else if (avalue.Value == "QUAVER")
                SongInfo.Header.Resolution = 8 / 4;
              else if (avalue.Value == "CROTCHET")
                SongInfo.Header.Resolution = 4 / 4;
              else
              { //Can"t understand teh Resolution :/
                Result = false;
                ErrorMessage = "Can""t read Resolution from Song";
              }
            }
            else if (avalue.Name == "GENRE")
            {
              SongInfo.Header.Genre = avalue.Value;
            }
            else if (avalue.Name == "YEAR")
            {
              SongInfo.Header.Year = avalue.Value;
            }
            else if (avalue.Name == "VERSION")
            {
              SingstarVersion = avalue.Value;
            }
          }
          ParserState = PS_Melody; //In Melody Tag
        }
        break;
      }
      case PS_Melody: 
      { //Search for Sentence, Artist/Title Info or eo Melody
        if (Tag == "SENTENCE")
        {
          ParserState = PS_Sentence; //Parse in a Sentence Tag now
          
          //Increase SentenceCount
          ++SongInfo.CountSentences;

          BindLyrics = true; //Don"t let Txts { w/ Space
          
          //Search for duet Singer Info
          MakeValuesArray();
          for (int I = 0; I < AValues.size(); ++I)
            if (AValues[I].Name == "SINGER")
            {
              boost::algorithm::to_upper(AValues[I].Value);
              if (AValues[I].Value == "SOLO 1")
                CurDuetSinger = DS_Player1;
              else if (AValues[I].Value == "SOLO 2")
                CurDuetSinger = DS_Player2;
              else
                CurDuetSinger = DS_Both; //In case of "Group" or anything that is not identified use Both
            }
        }
        else if (Tag == "!--")
        { //Comment, this may be Artist or Title Info
          I = Values.find(':'); //Search for Delimiter

          if (I != 0) //if Found check for Title or Artist
          {
            //Copy Title or Artist Tag to Tag String
            Tag = boost::algorithm::to_upper_copy(boost::algorithm::trim_copy(Values.substr(0, I - 1)));

            if (Tag == "ARTIST")
            {
              SongInfo.Header.Artist = boost::algorithm::trim_copy(Values.substr(I, Values.size() - I - 2));
              ++SongInfo.ID; //Inc SongID when header Information is added
            }
            else if (Tag == "TITLE")
            {
              SongInfo.Header.Title = boost::algorithm::trim_copy(Values.substr(I, Values.size() - I - 2));
              ++SongInfo.ID; //Inc SongID when header Information is added
            }
          }
        }
        //Parsing for weird "Die toten Hosen" Tags
        else if (Tag == "!--ARTIST:" || Tag == "!--ARTIST")
        { //Comment, with Artist Info
          I = Values.find(':'); //Search for Delimiter

          ++SongInfo.ID; //Inc SongID when header Information is added

          SongInfo.Header.Artist = boost::algorithm::trim_copy(Values.substr(I, Values.size() - I - 2));
        }
        else if (Tag == "!--TITLE:" || Tag == "!--TITLE")
        { //Comment, with Artist Info
          I = Values.find(':'); //Search for Delimiter

          ++SongInfo.ID; //Inc SongID when header Information is added

          SongInfo.Header.Title = boost::algorithm::trim_copy(Values.substr(I, Values.size() - I - 2));
        }
        else if (Tag == "/MELODY")
        {
          ParserState = PS_None;
          return; //Stop Parsing, Melody iTag ended
        }
        break;
      }
      case PS_Sentence: 
      { //Search for Notes or eo Sentence
        if (Tag == "NOTE")
        { //Found Note
          //Get Values
          MakeValuesArray();

          uint8_t NoteType = NT_Normal;
          bool IsRap = false;
          int Tone;
          uint32_t Duration;
          std::string Lyric;
          for (const auto& avalue : AValues)
          {
            if (avalue.Name == "DURATION")
            {
              try
              {
                Duration = std::stoi(avalue.Value);
              }
              catch (...)
              {
                Duration = -1;
              }
              if (Duration < 0)
              {
                Result = false;
                ErrorMessage = "Can't read duration from Note in Line: \"" + Line + "\"";
                return;
              }
            }
            else if (avalue.Name == "MIDINOTE")
            {
              try
              {
                Tone = std::stoi(avalue.Value);
              }
              catch (...)
              {
                Tone = 0;
              }
            }
            else if (avalue.Name == "BONUS" && boost::algorithm::to_upper_copy(avalue.Value) == "YES")
            {
              NoteType = NT_Golden;
            }
            else if (avalue.Name == "FREESTYLE" && boost::algorithm::to_upper_copy(avalue.Value) == "YES")
            {
              NoteType = NT_Freestyle;
            }
            else if (avalue.Name == "RAP" && boost::algorithm::to_upper_copy(avalue.Value) == "YES")
            {
              IsRap = true;
            }
            else if (avalue.Name == "LYRIC")
            {
              Lyric = avalue.Value;

              if (!Lyric.empty())
              {
                if (Lyric == "-")
                  Lyric[0] = Settings.DashReplacement;

                if (!BindLyrics)
                  Lyric = " " + Lyric;


                if (Lyric.size() > 2 && Lyric[Lyric.size() - 2] == ' ' && Lyric.back() == '-')
                { //Between this && the next Lyric should be no space
                  BindLyrics = true;
                  Lyric.resize(Lyric.size() - 2);
                }
                else
                  BindLyrics = false; //There should be a Space
              }
            }
          }

          //Set rap
          if (IsRap && NoteType == NT_Freestyle)
          {
            NoteType = NT_Rap;
          }
          if (IsRap && NoteType == NT_Golden)
          {
            NoteType = NT_RapGolden;
          }

          //Add Note
          I = SongInfo.CountSentences - 1;

          if (!Lyric.empty())
          { //Real note, no rest
            //First Note of Sentence
            if (SongInfo.Sentences.size() < SongInfo.CountSentences)
            {
              SongInfo.Sentences.resize(SongInfo.CountSentences);
              SongInfo.Sentences[I].Notes.resize(0);
            }

            //First Note of Song -> Generate Gap
            if (FirstNote)
            {
              //Calculate Gap
              if (SongInfo.Header.Resolution != 0 && SongInfo.Header.BPM != 0)
                SongInfo.Header.Gap = std::round(CurPosInSong / (SongInfo.Header.BPM * SongInfo.Header.Resolution) * 60000);
              else
              {
                Result = false;
                ErrorMessage = "Can't calculate Gap, no Resolution or BPM present.";
                return;
              }

              CurPosInSong = 0; //Start at 0, because Gap goes until here
              ++SongInfo.ID; //Add Header Value therefore Inc
              FirstNote = false;
            }

            SongInfo.Sentences[I].Notes.emplace_back(
                TNote {
                    CurPosInSong,
                    Duration,
                    Tone,
                    NoteType,
                    Lyric
                }
            );
            //Inc Pos in Song
            CurPosInSong += Duration;
          }
          else
          {
            //just change pos in Song
            CurPosInSong += Duration;
          }
        }
        else if (Tag == "/SENTENCE")
        { //End of Sentence Tag
          ParserState = PS_Melody;

          //Delete Sentence if no Note is Added
          SongInfo.CountSentences = SongInfo.Sentences.size();
        }
        break;
      }
    }
  }
  else  //Empty Line -> parsed succesful ;)
    Result = true;
}

std::string TParser::ParseConfigForEdition(const std::filesystem::path Filename)
/*var
  txt: TStringlist;
  Stream: TBinaryFileStream;
  I: Integer;
  J, K: Integer;
  S: String;*/
{
  std::string Result;

  Stream = TBinaryFileStream.Create(Filename, fmOpenRead);
  try
  {
    std::list<std::string> txt;
    txt.LoadFromStream(Stream);
    for (int I = 0; I < txt.size(); ++I)
    {
      std::string S = boost::algorithm::trim_copy(txt.Strings[I]);
      int J = S.find("<PRODUCT_NAME>");

      if (J != std::string::npos)
      {
        J += 14;
        int K = S.find("</PRODUCT_NAME>");
        if (K < J)
            K = S.size() + 1;

        Result = S.substr(J, K - J);
        break;
      }
    }
    Edition = Result;
  }
  catch (...)
  {
    txt.Free;
    Stream.Free;
  }
}
}