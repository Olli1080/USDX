#include "UFiles.h"

#include "UNote.h"
#include "USongs.h"
#include "UMusic.h"

#include <filesystem>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/locale.hpp>

namespace UFiles
{
    /*
    void ResetSingTemp()
       
    {
        SetLength(Tracks, Length(Player));
        for (size_t Count = 0 to High(Player))
        {
            SetLength(Tracks[Count].Lines, 1);
            SetLength(Tracks[Count].Lines[0].Notes, 0);
            Tracks[Count].Lines[0].Lyric = "";
            Player[Count].Score = 0;
            Player[Count].LengthNote = 0;
            Player[Count].HighNote = -1;
        }
    }*/

    void WriteCustomTags(const std::vector<TCustomHeaderTag>& CustomTags, std::ofstream& SongFile)
    {
        for (const auto& tag : CustomTags)
        {
            std::string Line;
            if (!tag.Tag.empty())
                Line = tag.Tag + ":";
            Line += tag.Content;

            SongFile << "#" << Line << std::endl;
        }
    }

    std::optional<std::string> fileHash(const std::filesystem::path& path)
    {
        if (!is_regular_file(path))
            return std::nullopt;

        auto file = std::ifstream(path);
        if (!file.is_open())
            return std::nullopt;

        MD5 hash;
        std::string buffer;
        while (std::getline(file, buffer))
            hash.add(buffer.data(), buffer.size() * sizeof(char));

        return hash.getHash();
    }

    TSaveSongResult SongIO::SaveSongTXT(const SongData& SongData, const UMusic::TrackVec& Tracks, const std::filesystem::path Name, bool Relative)
    {
        //  Relative := true; // override (idea - use shift+S to save with relative)
        try
        {
            std::ofstream SongFile{ Name, std::fstream::trunc };
            if (!SongFile.is_open())
                return ssrFileError;

            const auto& song = SongData.payload.Song;

            // do not save "auto" encoding tag
            if (Song.Encoding.empty())
                SongFile << "#ENCODING:" << Song.Encoding << std::endl;
            SongFile << "#TITLE:" << Song.Title << std::endl;
            SongFile << "#ARTIST:" << Song.Artist << std::endl;

            if (Song.Language != "Unknown") SongFile << "#LANGUAGE:" + Song.Language << std::endl;
            if (Song.Edition != "Unknown") SongFile << "#EDITION:" + Song.Edition << std::endl;
            if (Song.Genre != "Unknown") SongFile << "#GENRE:" + Song.Genre << std::endl;
            if (Song.Year != 0) SongFile << "#YEAR:" << Song.Year << std::endl;
            if (!Song.Creator.empty()) SongFile << "#CREATOR:" + Song.Creator << std::endl;

            SongFile << "#MP3:" << Song.Mp3 << std::endl;
            if (!Song.Cover.empty()) SongFile << "#COVER:" << Song.Cover << std::endl;
            if (!Song.Background.empty()) SongFile << "#BACKGROUND:" << Song.Background << std::endl;
            if (!Song.Video.empty()) SongFile << "#VIDEO:" << Song.Video << std::endl;

            if (Song.VideoGAP != 0.0) SongFile << "#VIDEOGAP:" << Song.VideoGAP << std::endl;
            if (Song.Resolution != 4) SongFile << "#RESOLUTION:" << Song.Resolution << std::endl;
            if (Song.NotesGAP != 0) SongFile << "#NOTESGAP:" << Song.NotesGAP << std::endl;
            if (Song.Start != decltype(Song.Start)::zero()) SongFile << "#START:" << Song.Start << std::endl;
            if (Song.Finish != decltype(Song.Finish)::zero()) SongFile << "#END:" << Song.Finish << std::endl;
            if (!Song.isDuet && Relative) SongFile << "#RELATIVE:yes" << std::endl;

            if (Song.HasPreview && Song.PreviewStart >= decltype(Song.PreviewStart)::zero()) // also allow writing 0.0 preview if set
                SongFile << "#PREVIEWSTART:" << Song.PreviewStart << std::endl;

            if (Song.Medley.Source == USong::msTag && !Relative && Song.Medley.EndBeat - Song.Medley.StartBeat > 0)
            {
                SongFile << "#MEDLEYSTARTBEAT:" << Song.Medley.StartBeat << std::endl;
                SongFile << "#MEDLEYENDBEAT:" << Song.Medley.EndBeat << std::endl;
            }

            auto BPM = Song.BPM.begin();
            SongFile << "#BPM:" << BPM->BPM / 4 << std::endl;
            SongFile << "#GAP:" << Song.GAP << std::endl;

            if (Song.isDuet)
            {
                SongFile << "#P1:" << Song.DuetNames[0] << std::endl;
                SongFile << "#P2:" << Song.DuetNames[1] << std::endl;
            }

            // write custom header tags
            WriteCustomTags();

            int RelativeSubTime = 0;
            for (auto B = ++BPM; B != Song.BPM.end(); ++B)
                SongFile << "B " << B->StartBeat << " " << B->BPM / 4 << std::endl;

            for (size_t CurrentTrack = 0; CurrentTrack < Tracks.size(); ++CurrentTrack)
            {
                if (Song.isDuet)
                    SongFile << "P" << CurrentTrack + 1 << std::endl;

                const auto& track = Tracks[CurrentTrack];
                for (size_t CurrentLine = 0; CurrentLine < track.Lines.size(); ++CurrentLine)
                {
                    const auto& line = track.Lines[CurrentLine];
                    for (size_t CurrentNote = 0; CurrentNote <= line.HighNote; ++CurrentNote)

                    {
                        const auto& Note = line.Notes[CurrentNote];
                        char NoteState;
                        //Golden + Freestyle Note Patch
                        switch (Note.NoteType)
                        {
                        case UMusic::ntFreestyle:
                            NoteState = 'F';
                            break;
                        case UMusic::ntNormal:
                            NoteState = ':';
                            break;
                        case UMusic::ntGolden:
                            NoteState = '*';
                            break;
                        case UMusic::ntRap:
                            NoteState = 'R';
                            break;
                        case UMusic::ntRapGolden:
                            NoteState = 'G';
                            break;
                        case UMusic::Size: [[fallthrough]]
                        default:
                            break;
                        } // case
                        SongFile << NoteState << " " << Note.StartBeat - RelativeSubTime << " " << Note.Duration << " " << Note.Tone << " " << Note.Text << std::endl;
                    } // CurrentNote

                    if (CurrentLine < Tracks[CurrentTrack].Lines.size() - 1) // don"t write end of last sentence
                    {
                        SongFile << "- ";
                        const auto& nextLineStartBeat = track.Lines[CurrentLine + 1].StartBeat;
                        if (Relative)
                            SongFile << nextLineStartBeat << std::endl;
                        else
                        {
                            SongFile << nextLineStartBeat - RelativeSubTime << " " << nextLineStartBeat - RelativeSubTime;
                            RelativeSubTime = nextLineStartBeat;
                        }
                        SongFile << std::endl;
                    }
                } // CurrentLine
            }
            SongFile << "E" << std::endl;
        }
        catch (...)
        {
            return ssrFileError;
        }
        return ssrOK;
    }

    std::optional<SongHeader> SongIO::ReadTXTHeader(std::ifstream& SongFile, size_t& FileLineNo, const std::filesystem::path& Path, bool ReadCustomTags)
        /*var
            Line, Identifier: string;
    Value: string;
    SepPos: int; // separator position
    Done: byte;      // bit-vector of mandatory fields
    MedleyFlags: byte; //bit-vector for medley/preview tags
    EncFile: IPath; // encoded filename
    FullFileName: string;
        I, P: int;*/

        /*{ adds a custom header tag to the song
          if there is no ':' in the read line, Tag should be empty
          and the whole line should be in Content }*/

    {
        SongHeader out;
        auto AddCustomTag = [&](const std::string& Tag, const std::string& Content)
        {
            if (ReadCustomTags)
                out.CustomTags.emplace_back(Tag, Content);
        };
        
        ULog::Log.LogInfoLocation("Reading txt file");

        uint8_t Done = 0;
        uint8_t MedleyFlags = 0;
        out.BPM = { { 0, 0 } };
        
        std::string Line;

        //Read first Line
        if (!std::getline(SongFile, Line) && Line.empty())
        {
            ULog::Log.LogErrorLocation("File starts with empty line");
            return std::nullopt;
        }

        // check if file begins with a UTF-8 BOM, if so set encoding to UTF-8
        //TODO:: replace with c++ loc support
        if (CheckReplaceUTF8BOM(Line))
            out.Encoding = encUTF8;
        else
            out.Encoding = encAuto;

        //Read Lines while Line starts with # or its empty
        //Log.LogDebug(Line,'TSong.ReadTXTHeader');
        while (std::getline(SongFile, Line) && !Line.empty() && Line[0] == '#')
        {
            //Increase Line Number
            ++FileLineNo;
            const size_t SepPos = Line.find_first_of(':');

            //Line has no Seperator, ignore non header field
            if (SepPos == 0)
            {
                AddCustomTag("", Line.substr(1, Line.size() - 1));
                // read next line
                if (!std::getline(SongFile, Line))
                {
                    ULog::Log.LogError("File incomplete or not Ultrastar txt(A)");
                    return std::nullopt;
                }
                continue;
            }

            //Read Identifier and Value
            std::string Identifier = boost::algorithm::to_upper_copy(boost::algorithm::trim_copy(Line.substr(1, SepPos - 2))); //Uppercase is for Case Insensitive Checks
            std::string Value = boost::algorithm::trim_copy(Line.substr(SepPos, Line.size() - SepPos));

            //Check the Identifier (If Value is given)
            if (Value.empty())
            {
                ULog::Log.LogInfoLocation("Empty field \"" + Identifier + "\"");
                AddCustomTag(Identifier, "");
            }
            else
            {
                //-----------
                //Required Attributes
                //-----------
                if (Identifier == "TITLE")
                {
                    out.Title = Value;
                    //Add Title Flag to Done
                    Done |= 0b0001;
                }
                else if (Identifier == "ARTIST")
                {
                    out.Artist = Value;

                    //Add Artist Flag to Done
                    Done |= 0b0010;
                }
                //MP3 File
                else if (Identifier == "MP3")
                {
                    auto EncFile = std::filesystem::path(Value);
                    if (is_regular_file(Path / EncFile))
                    {
                        out.paths.Mp3 = EncFile;

						//Add Mp3 Flag to Done
                        Done |= 0b0100;
                    }
                    else
                    {
                        ULog::Log.LogError("Can't find audio file in song");
                    }
                }
                //Beats per Minute
                else if (Identifier == "BPM")
                {
					//SetLength(out.BPM, 1);
                    out.BPM[0].StartBeat = 0;
					//StringReplace(Value, ',', '.',[rfReplaceAll]);
                    out.BPM[0].BPM = std::stod(Value) * Mult * MultBPM;

					if (out.BPM[0].BPM != 0)
		            {
						//Add BPM Flag to Done
                        Done |= 0b1000;
		            }
					else
						ULog::Log.LogError("Was not able to convert String \"" + Value + "\" to number.");
                }
                //---------
                //Additional Header Information
                //---------

                // Gap
                else if (Identifier == "GAP")
                {
                    out.GAP = MiliSecDouble(std::stod(Value));
                }
                //Cover Picture
                else if (Identifier == "COVER")
                {
                    out.paths.Cover = std::filesystem::path(Value);
                }
                //Background Picture
                else if (Identifier == "BACKGROUND")
                {
                    out.paths.Background = std::filesystem::path(Value);
                }
                // Video File
                else if (Identifier == "VIDEO")
                {
					auto EncFile = std::filesystem::path(Value);
                    if (is_regular_file(Path / EncFile))
                        out.paths.Video = EncFile;
					else
						ULog::Log.LogError("Can't find video file in song");
                }
                // Video Gap
                else if (Identifier == "VIDEOGAP")
                {
                    out.VideoGAP = std::stod(Value);
                }
                //Genre Sorting
                else if (Identifier == "GENRE")
                {
                    out.Genre = Value;
                }
                //Edition Sorting
                else if (Identifier == "EDITION")
                {
                    out.Edition = Value;
                }
                //Creator Tag
                else if (Identifier == "CREATOR")
                {
                    out.Creator = Value;
                }
                //Language Sorting
                else if (Identifier == "LANGUAGE")
                {
                    out.Language = Value;
                }
                //Year Sorting
                else if (Identifier == "YEAR")
                {
                    try
                    {
                        out.Year = std::stoi(Value);
                    }
                    catch (...)
                    {}
                }
                // Song Start
                else if (Identifier == "START")
                {
                    out.Start = SecDouble(std::stod(Value));
                }
                // Song Ending
                else if (Identifier == "END")
                {
                    try
                    {
                        out.Finish = std::chrono::milliseconds(std::stoi(Value));
                    }
                    catch (...)
                    {}
                }
                // Resolution
                else if (Identifier == "RESOLUTION")
                {
                    try
                    {
                        out.Resolution = std::stoi(Value);
                    }
                    catch (...)
                    {}
                }
                // Notes Gap
                else if (Identifier == "NOTESGAP")
                {
                    try
                    {
                        out.NotesGAP = std::stoi(Value);
                    }
                    catch (...)
                    {}
                }
                // Relative Notes
                else if (Identifier == "RELATIVE")
                {
                    out.Relative = boost::algorithm::iequals(Value, "YES");
                }
                // File encoding
                else if (Identifier == "ENCODING")
                {
                    out.Encoding = ParseEncoding(Value, UIni::Ini.DefaultEncoding);
                }
                // PreviewStart
                else if (Identifier == "PREVIEWSTART")
                {
                    try
                    {
                        out.PreviewStart = SecDouble(std::stod(Value));
                        if (out.PreviewStart > SecDouble::zero())
                        {
                            MedleyFlags |= 0b001;
                            out.HasPreview = true;
                        }
                    }
                    catch (...)
                    {}
                }
				// MedleyStartBeat
                else if (Identifier == "MEDLEYSTARTBEAT" && !out.Relative)
                {
                    try
                    {
                        out.medley.StartBeat = std::stoi(Value);
                        MedleyFlags |= 0b010;
                    }
                    catch (...)
                    {}
                }
                // MedleyEndBeat
                else if (Identifier == "MEDLEYENDBEAT" && !out.Relative)
                {
                    try
                    {
                        out.medley.EndBeat = std::stoi(Value);
                        MedleyFlags |= 0b100;
                    }
                    catch (...)
                    {}
                }
                // Medley
                else if (Identifier == "CALCMEDLEY")
                {
                    if (boost::algorithm::iequals(Value, "OFF"))
                        out.CalcMedley = false;
                }
                // Duet Singer Name P1
                else if (std::ranges::contains(std::vector<std::string>{"DUETSINGERP1", "P1"}, Identifier))
                {
                    out.DuetNames[0] = Value;
                }
                // Duet Singer Name P2
                else if (std::ranges::contains(std::vector<std::string>{"DUETSINGERP2", "P2"}, Identifier))
                {
                    out.DuetNames[1] = Value;
                }
                // unsupported tag
                else
                {
                    AddCustomTag(Identifier, Value);
                }
            } // End check for non-empty Value

            // read next line
            if (std::getline(SongFile, Line))
            {
                ULog::Log.LogError("File incomplete or not Ultrastar txt (A)");
                return std::nullopt;
            }
        } // while

        bool noCover = out.paths.Cover.empty();
        bool noBackGround = out.paths.Background.empty();

        if (noCover || noBackGround)
        {
            for (const auto& entry : std::filesystem::directory_iterator(Path))
            {
                if (!is_regular_file(entry))
                    continue;
                auto path = entry.path();
                if (path.extension() != ".jpg")
                    continue;

                auto pathStr = path.stem().string();
                if (noCover && pathStr.ends_with("[CO]"))
                {
                    out.paths.Cover = path.filename();
                    noCover = false;
                }
                else if (noBackGround && pathStr.ends_with("[BG]"))
                {
                    out.paths.Background = path.filename();
                    noBackGround = false;
                }
                if (!noCover && !noBackGround)
                    break;
            }
        }

        //Check if all Required Values are given
        if (Done != 0b1111)
        {
            if ((Done & 0b1000) == 0) //No BPM Flag
                ULog::Log.LogError("File contains empty lines or BPM tag missing");
            else if ((Done & 0b0100) == 0) //No MP3 Flag
                ULog::Log.LogError("MP3 tag/file missing");
            else if ((Done & 0b0010) == 0) //No Artist Flag
                ULog::Log.LogError("Artist tag missing");
            else if ((Done & 0b0001) == 0) //No Title Flag
                ULog::Log.LogError("Title tag missing");
            else //unknown Error
                ULog::Log.LogError(std::format("File incomplete or not Ultrastar txt (B - {})",
                    Done));
            return std::nullopt;
        }
        else
        {  //check medley tags
            if ((MedleyFlags & 0b110) == 0b110) //MedleyStartBeat and MedleyEndBeat are both set
            {
                if (out.medley.StartBeat >= out.medley.EndBeat)
                    MedleyFlags ^= 0b110;
            }

            if ((MedleyFlags & 0x0001) == 0 || out.PreviewStart <= SecDouble::zero()) //PreviewStart is not set or <=0
            {
                if ((MedleyFlags & 0b010) == 0b010)
                    out.PreviewStart = UNote::GetTimeFromBeat(out.medley.StartBeat, out.BPM, out.GAP); //fallback to MedleyStart
                else
                    out.PreviewStart = SecDouble(0); //else set it to 0, it will be set in FindRefrainStart
            }

            if ((MedleyFlags & 0b110) == 0b110)
            {
                out.medley.Source = TMedleySource::msTag;

	            //calculate fade time
                out.medley.FadeIn_time = DEFAULT_FADE_IN_TIME;
                out.medley.FadeOut_time = DEFAULT_FADE_OUT_TIME;
            }
            else
                out.medley.Source = TMedleySource::msNone;
        }
    }

    int SongIO::calcStartBeat(int progress, int mult, int rel_p)
    {
        return (progress + rel_p) * mult;
    }

    //Load TXT Song Payload
    std::optional<SongPayLoad> SongIO::LoadSongTXTPayload(std::ifstream& SongFile, SongHeader& header, size_t& FileLineNo)
    {
        SongPayLoad result;
        auto& Song = result.Song;
        auto& Tracks = result.Tracks;
        
        //if (UNote::Player.size() == 2)
          //  Both = true;
        
        size_t LinePos = 0;
        try
        {
            size_t CurrentTrack = 0;

            //Search for Note Beginning
            bool NotesFound = false;
            std::string CurLine;
            while (std::getline(SongFile, CurLine))
            {
                ++FileLineNo;
                if (!CurLine.empty() && (std::set<char>({ ':', 'F', '*', 'R', 'G', 'P' }).contains(CurLine[0])))
                {
                    NotesFound = true;
                    break;
                }
            }

            if (!NotesFound)
            { //Song File Corrupted - No Notes
                ULog::Log.LogError("Could not load txt File, no notes found");
                //return "ERROR_CORRUPT_SONG_NO_NOTES";
                return std::nullopt;
            }
            if (CurLine[1] == 'P')
            {
                //UNote::CurrentSong->isDuet = true;
                Song.isDuet = true;
                Tracks.resize(2);
                CurrentTrack = -1;
            }
            else
                Tracks.resize(1);

            for (auto& track : Tracks)
            {
                track.Number = 1;
                track.CurrentLine = 0;
                //track.Resolution = Resolution;
                //track.NotesGAP = self.NotesGAP;
                track.ScoreValue = 0;

                //Add first line and set some standard values to fields
                //see void NewSentence for further explanation
                //concerning most of these values
                track.Lines.resize(1);
                auto& line = track.Lines.back();
                line.HighNote = -1;
                line.LastLine = false;
                line.BaseNote = std::numeric_limits<int>::max();
                line.ScoreValue = 0;
            }

            do
            {
                char tag = ParseLyricParam<char>(CurLine, LinePos, FileLineNo );
                if (tag == 'P')
                {
                    int Param1;
                    if (CurLine[1] == ' ')
                        Param1 = std::stoi(&CurLine[2]);
                    else
                        Param1 = std::stoi(&CurLine[1]);

                    if (Param1 == 1)
                    {
                        //TODO:: perform Duetchange with swap operation instead
                        //if (!DuetChange)
                            CurrentTrack = 0;
                        //else
                        //    CurrentTrack = 1;
                    }
                    else if (Param1 == 2)
                    {
                        //if (!DuetChange)
                            CurrentTrack = 1;
                        //else
                        //    CurrentTrack = 0;
                    }
                    else if (Param1 == 3)
                    {
                        CurrentTrack = 2;
                    }
                    else
                    {
                        ULog::Log.LogErrorLocation("Wrong P-Number on Line " + std::to_string(FileLineNo));
                        //return "WRONG_P_NUMBER";
                        return std::nullopt;
                    }
                }
                else if (tag == 'E')
                {
                    break;
                }
                else if (std::set{ ':', '*', 'F', 'R', 'G' }.contains(tag))
                {
                    // sets the rap icon if the song has rap notes
                    if (std::set{ 'R', 'G' }.contains(tag))
                    {
                        //USong::CurrentSong.hasRap = true;
                        Song.hasRap = true;
                    }
                    // read notes
                    int Param1 = ParseLyricParam<int>(CurLine, LinePos);
                    int duration = ParseLyricParam<int>(CurLine, LinePos);
                    int NoteP = ParseLyricParam<int>(CurLine, LinePos);
                    std::string_view ParamLyric = ParseLyricText(CurLine, LinePos);

                    //Check for ZeroNote
                    if (duration == 0)
                    {
                        ULog::Log.LogWarnLocation(std::format("Line{}: found note with length zero->converted to FreeStyle", FileLineNo));
                        tag = 'F';
                    }

                    const auto startP = 
                        [
                            &Param1 = std::as_const(Param1), 
							&Mult = std::as_const(Mult),
                            &Tracks = std::as_const(Tracks)
                        ](int track)
                    {
                        return calcStartBeat(Param1, Mult, Tracks[track].Rel);
                    };
                    const auto durationP = duration * Mult;
                    // add notes
                    if (CurrentTrack != 2)
                    {
                        // P1
                        if (Tracks[CurrentTrack].Lines.size() == 0 || Tracks[CurrentTrack].Lines.size() >= 5000)
                        {
                            ULog::Log.LogError(std::format
                            (
                                "Found faulty song.Did you forget a P1 or P2 tag ? \"{} {} {} {} {} -> Line: {}",
                                tag, Param1, duration, NoteP, ParamLyric, FileLineNo
                            ));
                            break;
                        }
                        ParseNote(Tracks[CurrentTrack], tag, startP(CurrentTrack), durationP, NoteP, ParamLyric);
                    }
                    else
                    {
                        // P1 + P2
                        ParseNote(Tracks[0], tag, startP(0), durationP, NoteP, ParamLyric);
                        ParseNote(Tracks[1], tag, startP(1), durationP, NoteP, ParamLyric);
                    }
                } // if
                else if (tag == '-')
                {
                    // reads sentence
                    const int Param1 = ParseLyricParam<int>(CurLine, LinePos);
                    int RelativeP = 0; //TODO:: also multiply by Mult?
                    if (header.Relative)
                        RelativeP = ParseLyricParam<int>(CurLine, LinePos); // read one more data for relative system

                    const auto startP =
                        [
                            &Param1 = std::as_const(Param1),
                            &Tracks = std::as_const(Tracks)
                        ](int track)
                    {
                        return calcStartBeat(Param1, Mult, Tracks[track].Rel);
                    };

                    // new sentence
                    if (!Song.isDuet)
                        // one singer
                        NewSentence(Tracks[CurrentTrack], startP(CurrentTrack), RelativeP, Song.Relative, Song.isDuet, FileLineNo);
                    else
                    {
                        // P1 + P2
                        NewSentence(Tracks[0], startP(0), RelativeP, Song.Relative, Song.isDuet, FileLineNo);
                        NewSentence(Tracks[1], startP(1), RelativeP, Song.Relative, Song.isDuet, FileLineNo);
                    }
                } // if
                else if (tag == 'B')
                {
                    header.BPM.emplace_back
                    (
                        ParseLyricParam<float>(CurLine, LinePos) + Tracks[0].Rel,
                        ParseLyricParam<float>(CurLine, LinePos) * Mult * MultBPM
                    );
                }
                ++FileLineNo;
            } while (std::getline(SongFile, CurLine)); // while
        }
        catch (const std::exception& e)
        {
            ULog::Log.LogError(std::format("Error loading in line {},{}: {}",
                FileLineNo, LinePos, e.what()));
            //return e.what();
            return std::nullopt;
        }

        for (size_t TrackIndex = 0; TrackIndex < Tracks.size(); ++TrackIndex)
        {
            //Load regardless of two or more players!
            //Because io will always be our bottleneck!
            //if (!Both && TrackIndex != 0)
              //  continue;

            auto& lines = Tracks[TrackIndex].Lines;

            if (lines.size() < 2)
            {
                ULog::Log.LogError("Error loading file: Can't find any line breaks in the file");
                //return "ERROR_CORRUPT_SONG_NO_BREAKS";
                return std::nullopt;
            }

            if (lines.back().HighNote < 0)
            {
                auto& Number = Tracks[TrackIndex].Number;
                --Number;
                lines.resize(Number);

                // HACK DUET ERROR
                if (!Song.isDuet)
                    ULog::Log.LogError("Error loading Song, sentence w/o note found in last line before E");
            }
        }
        for (size_t TrackIndex = 0; TrackIndex < Tracks.size(); ++TrackIndex)
        {
            if (Tracks[TrackIndex].Lines.size() > 0)
                Tracks[TrackIndex].Lines.back().LastLine = true;
        }
        return result;
    }

    std::optional<SongData> SongIO::LoadSongTXT(const std::filesystem::path& FileNamePath)
    {
        ULog::Log.LogInfoLocation(std::format("Trying to load song from txt: {}", FileNamePath.string()));
        if (!is_regular_file(FileNamePath))
            return std::nullopt;

        auto SongFile = std::ifstream(FileNamePath);
        if (!SongFile.is_open())
            return std::nullopt;

        SongData song;
        song.Path = FileNamePath;

        size_t FileLineNo; //TODO::custom tag reading
		auto header = ReadTXTHeader(SongFile, FileLineNo, song.Path.parent_path());
        if (!header)
            return std::nullopt;
        song.header = std::move(header.value());


        auto pLOpt = LoadSongTXTPayload(SongFile, song.header, FileLineNo);

        if (!pLOpt)
            return std::nullopt;
        song.payload = std::move(pLOpt.value());


        SongFile.close();

        song.md5 = fileHash(FileNamePath).value();

        return song;
    }


    /**
     * Parses the Line string starting from LinePos for a parameter.
     * Leading whitespace is trimmed, same applies to the first trailing whitespace.
     * After the call LinePos will point to the position after the first trailing
     * whitespace.
     *
     * Raises an EUSDXParseException if no string was found.
     *
     * Example:
     *   ParseLyricParam(Line:'Param0  Param1 Param2', LinePos:8, ...)
     *   -> Param:'Param1', LinePos:16 (= start of 'Param2')
     */
    template <>
    std::string_view SongIO::ParseLyricParam<>(std::string_view Line, size_t& LinePos, size_t FileLineNo)
    {
        const size_t beginPos = Line.find_first_not_of("\n ", LinePos);
        if (beginPos == std::string::npos)
            throw EUSDXParseException("String expected");

        const size_t endPos = Line.find_first_of("\n ", beginPos);
        LinePos = endPos;
        if (LinePos < Line.size() - 1)
            ++LinePos;

        return Line.substr(beginPos, endPos - beginPos);
    }

    template <>
    int SongIO::ParseLyricParam<>(std::string_view Line, size_t& LinePos, size_t FileLineNo)
    {
        const size_t OldLinePos = LinePos;
        const auto str = ParseLyricParam<std::string_view>(Line, LinePos);

        try
        {
            return std::stoi(std::string(str));
        }
        catch (...)
        {
            LinePos = OldLinePos;
            throw EUSDXParseException("Integer expected");
        }
	}

    template <>
    float SongIO::ParseLyricParam<>(std::string_view Line, size_t& LinePos, size_t FileLineNo)
    {
        const size_t OldLinePos = LinePos;
        const auto str = ParseLyricParam<std::string_view>(Line, LinePos);

        try
        {
            return std::stof(std::string(str));
        }
        catch (...)
        {
            LinePos = OldLinePos;
            throw EUSDXParseException("Float expected");
        }
    }

    template <>
    char SongIO::ParseLyricParam<>(std::string_view Line, size_t& LinePos, size_t FileLineNo)
    {
        const size_t OldLinePos = LinePos;
        std::string_view str;
        try
        {
			str = ParseLyricParam<std::string_view>(Line, LinePos);
        }
        catch (...)
        {
            LinePos = OldLinePos;
            throw EUSDXParseException("Character expected");
        }
        if (str.size() > 1 && str[0] != 'P')
        {
            ULog::Log.LogWarnLocation(std::format("In line {}: character expected but found \"{}\"",
                FileLineNo, str));
        }
        return str[0];
    }
    
    /**
     * Returns the rest of the line from LinePos as lyric text.
     * Leading and trailing whitespace is not trimmed.
     */
    std::string_view SongIO::ParseLyricText(std::string_view Line, size_t& LinePos)
    {
        if (LinePos >= Line.size())
            return "";

        LinePos = Line.size() - 1;
	    return Line.substr(LinePos, Line.size() - LinePos);
    }

    void SongIO::ParseNote(UMusic::TLines& track, char TypeP, int StartP, int DurationP, int NoteP, std::string_view LyricS)
    {
        auto lastLine = track.Lines.back();
        lastLine.Notes.emplace_back();
        lastLine.HighNote = lastLine.Notes.size() - 1;

        auto& highNote = lastLine.Notes[lastLine.HighNote];

        highNote.StartBeat = StartP;
        if (lastLine.HighNote == 0)
        {
            if (track.Number == 1)
                lastLine.StartBeat = -100;
            //StartBeat := highNote.StartBeat;
        }

        highNote.Duration = DurationP;

        // back to the normal system with normal, golden and now freestyle notes
        switch (TypeP)
        {
        case 'F':
            highNote.NoteType = UMusic::ntFreestyle;
            break;
        case ':':
            highNote.NoteType = UMusic::ntNormal;
            break;
        case '*':
            highNote.NoteType = UMusic::ntGolden;
            break;
        case 'R':
            highNote.NoteType = UMusic::ntRap;
            break;
        case 'G':
            highNote.NoteType = UMusic::ntRapGolden;
            break;
        default:
            break;
        }

        const auto score = highNote.Duration * UMusic::ScoreFactor[highNote.NoteType];
        //add this notes value ("notes length" * "notes scorefactor") to the current songs entire value
        track.ScoreValue += score;
        //and to the current lines entire value
        lastLine.ScoreValue += score;

        highNote.Tone = NoteP;

        //if a note w/ a lower pitch then the current basenote is found
        //we replace the basenote w/ the current notes pitch
        lastLine.BaseNote = std::min(highNote.Tone, lastLine.BaseNote);

        highNote.Color = 1; // default color to 1 for editor
        highNote.Text = LyricS;
        lastLine.Lyric += highNote.Text;
        lastLine.EndBeat = highNote.StartBeat + highNote.Duration;
    }

    void SongIO::NewSentence(UMusic::TLines& track, int StartBeatP, int RelativeP, bool relative, bool isDuet, size_t FileLineNo)
    {
        if (track.Lines.back().HighNote != -1)
        {
        	//create a new line
            track.Lines.resize(track.Number + 1);
			//Inc(track.High);
			++track.Number;
        }
        else
        {
        	//use old line if it there were no notes added since last call of NewSentence
			// HACK DUET ERROR
            if (!isDuet)
                ULog::Log.LogError(std::format("Error loading Song, sentence w / o note found in line {}",
                    FileLineNo));
        }

        auto& lastLine = track.Lines.back();
        lastLine.HighNote = -1;

        //set the current lines value to zero
        //it will be incremented w/ the value of every added note
        lastLine.ScoreValue = 0;

        //basenote is the pitch of the deepest note, it is used for note drawing.
        //if a note with a less value than the current sentences basenote is found,
        //basenote will be set to this notes pitch. Therefore the initial value of
        //this field has to be very high.
        lastLine.BaseNote = std::numeric_limits<decltype(lastLine.BaseNote)>::max();
        lastLine.StartBeat = StartBeatP;

        if (relative)
			track.Rel += RelativeP;

        lastLine.LastLine = false;
    }


    //Load XML Song
    function TSong.LoadXMLSong() : bool;
    var
        TrackIndex : int;
Both:       bool;
Param1:     int;
Param2:     int;
Param3:     int;
ParamS:     string;
    I, J:       int;
NoteIndex:  int;

NoteType:  char;
    SentenceEnd, Rest, Time: int;
Parser: TParser;
FileNamePath: IPath;
    {
        Result = false;
LastError = '';

FileNamePath = Path.Append(FileName);
    if not FileNamePath.IsFile() then
        {
        Log.LogError('File not found: "' + FileNamePath.ToNative + '"', 'TSong.LoadSong()');
    exit;
    }

MultBPM = 4; // multiply beat-count of note by 4
Mult = 1; // accuracy of measurement of note
    Tracks[0].ScoreValue = 0;
    Tracks[1].ScoreValue = 0;
    self.Relative = false;
    Rel[0] = 0;
Both = false;

    if Length(Player) = 2 then
        Both = true;

Parser = TParser.Create;
    Parser.Settings.DashReplacement = '~';

    for TrackIndex = 0 to High(Tracks) do
        {
        Tracks[TrackIndex].High = 0;
    Tracks[TrackIndex].Number = 1;
    Tracks[TrackIndex].CurrentLine = 0;
    Tracks[TrackIndex].Resolution = self.Resolution;
    Tracks[TrackIndex].NotesGAP = self.NotesGAP;
    Tracks[TrackIndex].ScoreValue = 0;

    //Add first line and set some standard values to fields
    //see void NewSentence for further explantation
    //concerning most of these values
    SetLength(Tracks[TrackIndex].Lines, 1);
    Tracks[TrackIndex].Lines[0].HighNote = -1;
    Tracks[TrackIndex].Lines[0].LastLine = false;
    Tracks[TrackIndex].Lines[0].BaseNote = High(Integer);
    Tracks[TrackIndex].Lines[0].ScoreValue = 0;
    }

    //Try to Parse the Song

    if Parser.ParseSong(FileNamePath) then
        {
        //Writeln('XML Inputfile Parsed succesful');

        //Start write parsed information to Song
        //Notes Part
        for I = 0 to High(Parser.SongInfo.Sentences) do
            {
            //Add Notes
            for J = 0 to High(Parser.SongInfo.Sentences[I].Notes) do
                {
                case Parser.SongInfo.Sentences[I].Notes[J].NoteTyp of
                NT_Normal : NoteType = ':';
NT_Golden:    NoteType = '*';
NT_Freestyle: NoteType = 'F';
NT_Rap:       NoteType = 'R';
NT_RapGolden: NoteType = 'G';
    }

Param1 = Parser.SongInfo.Sentences[I].Notes[J].Start;       //Note Start
Param2 = Parser.SongInfo.Sentences[I].Notes[J].Duration;    //Note Duration
Param3 = Parser.SongInfo.Sentences[I].Notes[J].Tone;        //Note Tone
ParamS = ' ' + Parser.SongInfo.Sentences[I].Notes[J].Lyric; //Note Lyric

    if not Both then
        // P1
        ParseNote(0, NoteType, (Param1 + Rel[0]) * Mult, Param2 * Mult, Param3, ParamS)
    else
        {
        // P1 + P2
        ParseNote(0, NoteType, (Param1 + Rel[0]) * Mult, Param2 * Mult, Param3, ParamS);
    ParseNote(1, NoteType, (Param1 + Rel[1]) * Mult, Param2 * Mult, Param3, ParamS);
    }

    } //J Forloop

    //Add Sentence break
    if (I < High(Parser.SongInfo.Sentences)) then
        {
        SentenceEnd = Parser.SongInfo.Sentences[I].Notes[High(Parser.SongInfo.Sentences[I].Notes)].Start + Parser.SongInfo.Sentences[I].Notes[High(Parser.SongInfo.Sentences[I].Notes)].Duration;
Rest = Parser.SongInfo.Sentences[I + 1].Notes[0].Start - SentenceEnd;

    //Calculate Time
    case Rest of
    0, 1: Time = Parser.SongInfo.Sentences[I + 1].Notes[0].Start;
        2:    Time = Parser.SongInfo.Sentences[I + 1].Notes[0].Start - 1;
        3:    Time = Parser.SongInfo.Sentences[I + 1].Notes[0].Start - 2;
    else
        if (Rest >= 4) then
            Time = SentenceEnd + 2
        else //Sentence overlapping :/
            Time = Parser.SongInfo.Sentences[I + 1].Notes[0].Start;
    }
    // new sentence
    if not Both then // P1
        NewSentence(0, (Time + Rel[0]) * Mult, Param2)
    else
        { // P1 + P2
        NewSentence(0, (Time + Rel[0]) * Mult, Param2);
    NewSentence(1, (Time + Rel[1]) * Mult, Param2);
    }

    }
    }
    //End write parsed information to Song
    Parser.Free;
    end
    else
        {
        Log.LogError('Could not parse inputfile: ' + FileNamePath.ToNative);
    exit;
    }

    for TrackIndex = 0 to High(Tracks) do
        {
        Tracks[TrackIndex].Lines[High(Tracks[TrackIndex].Lines)].LastLine = true;
    }

Result = true;
    }
}
