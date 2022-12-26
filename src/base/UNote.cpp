#include "UNote.h"

SecDouble UNote::GetTimeForBeats(double BPM, double Beats)
{
    return std::chrono::duration_cast<SecDouble>(MinDouble(BPM * Beats));
}

double UNote::GetBeats(double BPM, SecDouble Time)
{
    return BPM * std::chrono::duration_cast<MinDouble>(Time).count();
}

void UNote::GetMidBeatSub(int BPMNum, SecDouble& Time, double& CurBeat)
{
    SecDouble NewTime;

    if (CurrentSong->BPM.size() == BPMNum)
    {
        // last BPM
        CurBeat = CurrentSong->BPM[BPMNum].StartBeat + GetBeats(CurrentSong->BPM[BPMNum].BPM, Time);
        Time = SecDouble(0);
    }
    else
    {
        // not last BPM
        // count how much time is it for start of the new BPM and store it in NewTime
        NewTime = GetTimeForBeats(CurrentSong->BPM[BPMNum].BPM, CurrentSong->BPM[BPMNum+1].StartBeat - CurrentSong->BPM[BPMNum].StartBeat);

        // compare it to remaining time
        if ((Time - NewTime) > SecDouble(0))
        {
            // there is still remaining time
            CurBeat = CurrentSong->BPM[BPMNum].StartBeat;
            Time -= NewTime;
        }
        else
        {
            // there is no remaining time
            CurBeat = CurrentSong->BPM[BPMNum].StartBeat + GetBeats(CurrentSong->BPM[BPMNum].BPM, Time);
            Time = SecDouble(0);
        }
    }
}

double UNote::GetMidBeat(SecDouble Time)
{
    double CurBeat;
    int CurBPM;

    try
    {
        // static BPM
        if (CurrentSong->BPM.size() == 1)
        {
            return GetBeats(CurrentSong->BPM[0].BPM, Time);
        }
        // variable BPM
        else if (CurrentSong->BPM.size() > 1)
        {
            CurBeat = 0;
            CurBPM = 0;
            while (Time > SecDouble(0))
            {
                GetMidBeatSub(CurBPM, Time, CurBeat);
                ++CurBPM;
            }
            return CurBeat;
        }
        // invalid BPM
        else
        {
            return 0.0;
        }
    }
    catch (const std::exception& e)
    {
        return 0.0;
    }
}

SecDouble UNote::GetTimeFromBeat(int Beat, std::shared_ptr<TSong> SelfSong)
{
    int CurBPM;
    std::shared_ptr<TSong> Song;

    Song = (SelfSong) ? SelfSong : CurrentSong;

    SecDouble out{ 0.0 };

    // static BPM
    if (Song->BPM.size() == 1)
    {
        return std::chrono::duration_cast<SecDouble>(Song->GAP) + GetTimeForBeats(Song->BPM[0].BPM, Beat);
    }
    // variable BPM
    else if (Song->BPM.size() > 1)
    {
        out = std::chrono::duration_cast<SecDouble>(Song->GAP);
        int CurBPM = 0;
        while (CurBPM <= Song->BPM.size() &&
                (Beat > Song->BPM[CurBPM].StartBeat))
        {
            if (CurBPM < Song->BPM.size() &&
                Beat >= Song->BPM[CurBPM+1].StartBeat)
            {
                // full range
                out += GetTimeForBeats(Song->BPM[CurBPM].BPM, Song->BPM[CurBPM+1].StartBeat - Song->BPM[CurBPM].StartBeat);
            }

            if (CurBPM == Song->BPM.size() ||
                Beat < Song->BPM[CurBPM+1].StartBeat)
            {
                // in the middle
                out += GetTimeForBeats(Song->BPM[CurBPM].BPM, Beat - Song->BPM[CurBPM].StartBeat);
            }
            ++CurBPM;
        }
        /*
        while (Time > 0)
        {
            GetMidBeatSub(CurBPM, Time, CurBeat);
            ++CurBPM;
        }
        */
       return out;
    }
    // invalid BPM
    else
    {
        out = SecDouble{0.0};
    }
    return out;
}












void UNote::NewBeatClick()
{
    int Count;
    // beat click

    if (!CurrentSong->isDuet || PlayersPlay == 1)
    {
        if (UIni::Ini.BeatClick == 1 &&
            ((LyricsState.CurrentBeatC + Tracks[0].Resolution + Tracks[0].NotesGAP) % Tracks[0].Resolution = 0))
        {
            AudioPlayback.PlaySound(SoundLib.Click);
        }

        for (int Count = 0; Count < Tracks[0].Lines[Tracks[0].CurrentLine].HighNote; ++Count) 
        {
            //basisbit todo
            if (Tracks[0].Lines[Tracks[0].CurrentLine].Notes[Count].StartBeat = LyricsState.CurrentBeatC)
            {
            // click assist
            if Ini.ClickAssist = 1
                AudioPlayback.PlaySound(SoundLib.Click);

            // drum machine
            /*
            TempBeat := LyricsState.CurrentBeat; // + 2;
            if (TempBeat mod 8 = 0) then Music.PlayDrum;
            if (TempBeat mod 8 = 4) then Music.PlayClap;
            //if (TempBeat mod 4 = 2) then Music.PlayHihat;
            if (TempBeat mod 4 <> 0) then Music.PlayHihat;
            */
            }
        }
    }
}
