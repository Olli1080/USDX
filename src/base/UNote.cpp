#include "UNote.h"

#include "UFiles.h"

SecDouble UNote::GetTimeForBeats(double BPM, double Beats)
{
    return std::chrono::duration_cast<SecDouble>(MinDouble(BPM * Beats));
}

double UNote::GetBeats(double BPM, SecDouble Time)
{
    return BPM * std::chrono::duration_cast<MinDouble>(Time).count();
}

void UNote::GetMidBeatSub(size_t BPMNum, SecDouble& Time, double& CurBeat, const std::vector<USong::TBPM>& BPM)
{
	if (BPM.size() - 1 == BPMNum)
    {
        // last BPM
        CurBeat = BPM[BPMNum].StartBeat + GetBeats(BPM[BPMNum].BPM, Time);
        Time = SecDouble(0);
    }
    else
    {	    
	    // not last BPM
        // count how much time is it for start of the new BPM and store it in NewTime
        SecDouble NewTime = GetTimeForBeats(BPM[BPMNum].BPM, BPM[BPMNum + 1].StartBeat - BPM[BPMNum].StartBeat);

        // compare it to remaining time
        if (Time - NewTime > SecDouble(0))
        {
            // there is still remaining time
            CurBeat = BPM[BPMNum].StartBeat;
            Time -= NewTime;
        }
        else
        {
            // there is no remaining time
            CurBeat = BPM[BPMNum].StartBeat + GetBeats(BPM[BPMNum].BPM, Time);
            Time = SecDouble(0);
        }
    }
}

SecDouble UNote::GetMidBeat(SecDouble Time)
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
            while (Time > SecDouble::zero())
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

SecDouble UNote::GetTimeFromBeat(int Beat, const std::vector<UFiles::TBPM>& BPM, const MiliSecDouble& GAP)
{
	// static BPM
    if (BPM.size() == 1)
    {
        return std::chrono::duration_cast<SecDouble>(GAP) + GetTimeForBeats(BPM[0].BPM, Beat);
    }
    // variable BPM
    if (BPM.size() > 1)
    {
	    auto out = std::chrono::duration_cast<SecDouble>(GAP);
        size_t CurBPM = 0;
        while (CurBPM < BPM.size() &&
                Beat > BPM[CurBPM].StartBeat)
        {
            if (CurBPM < BPM.size() - 1 &&
                Beat >= BPM[CurBPM + 1].StartBeat)
            {
                // full range
                out += GetTimeForBeats(BPM[CurBPM].BPM, BPM[CurBPM + 1].StartBeat - BPM[CurBPM].StartBeat);
            }

            if (CurBPM == BPM.size() - 1 ||
                Beat < BPM[CurBPM + 1].StartBeat)
            {
                // in the middle
                out += GetTimeForBeats(BPM[CurBPM].BPM, Beat - BPM[CurBPM].StartBeat);
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
    return SecDouble{0.0};
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
            if (Tracks[0].Lines[Tracks[0].CurrentLine].Notes[Count].StartBeat == LyricsState.CurrentBeatC)
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
