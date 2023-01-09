#include "UBeatTimer.h"

#include "UNote.h"

#ifdef LOG_SYNC
#include "ULog.h"
#endif

namespace UBeatTimer
{
    void Beat::SetMid(double mid)
    {
        Old = Current;
        Mid = mid;
        Current = std::floorl(Mid);
    }


    template<class ClockType>
    TLyricsState<ClockType>::TLyricsState()
    {
        // create a triggered timer, so we can Pause() it, set the time
        // and Resume() it afterwards for better synching.

        // reset state
        Reset();
    }

    template<class ClockType>
    bool TLyricsState<ClockType>::isPaused() const
    {
        return fTimer.GetState() == UTime::rtsPaused;
    }

    template<class ClockType>
    void TLyricsState<ClockType>::Pause()
    {
        fTimer.Pause();
    }

    template<class ClockType>
    void TLyricsState<ClockType>::Start(bool WaitForTrigger)
    {
        fTimer.Start(WaitForTrigger);
        fLastClock = -1;
        fAvgSyncDiff = nullptr;
    }

    template<class ClockType>
    void TLyricsState<ClockType>::Stop()
    {
        fTimer.Stop();
    }

    template<class ClockType>
    void TLyricsState<ClockType>::SetCurrentTime(const duration& Time)
    {
        fTimer.SetTime(Time);
        fLastClock = -1;
        fAvgSyncDiff = nullptr;
    }

    //{.$DEFINE LOG_SYNC}

    template<class ClockType>
    typename TLyricsState<ClockType>::duration TLyricsState<ClockType>::Synchronize(const duration& LyricTime)
    {
        constexpr auto AVG_HISTORY_FACTOR = 0.7;
        constexpr auto PAUSE_THRESHOLD = std::chrono::milliseconds(10); // 10ms
        constexpr auto FORWARD_THRESHOLD = std::chrono::milliseconds(10); // 10ms

        auto MasterClock = fSyncSource->GetClock();
        auto Result = LyricTime;
        // do not sync if lyrics are paused externally or if the timestamp is old
        if (isPaused() || (MasterClock == fLastClock))
            return Result;

        // calculate average time difference (some sort of weighted mean).
        // The bigger AVG_HISTORY_FACTOR is, the smoother is the average diff.
        // This is done as some timestamps might be wrong or even lower
        // than their predecessor.
        auto TimeDiff = MasterClock - LyricTime;
        if (!fAvgSyncDiff)
            fAvgSyncDiff = std::make_unique<SecDouble>(TimeDiff);
        else
            *fAvgSyncDiff = TimeDiff * (1.0 - AVG_HISTORY_FACTOR) +
            *fAvgSyncDiff * AVG_HISTORY_FACTOR;

#ifdef LOG_SYNC
        //Ulog::Log.LogError(Format("TimeDiff: %.3f", [TimeDiff]));
#endif

        // do not go backwards in time as this could mess up the score
        if (*fAvgSyncDiff > FORWARD_THRESHOLD)
        {
#ifdef LOG_SYNC
            ULog::Log.LogError("Sync: " + std::to_string(MasterClock) + " > " + std::to_string(LyricTime));
#endif

            Result = LyricTime + *fAvgSyncDiff;
            fTimer.SetTime(Result);
            fTimer.Start();
            fAvgSyncDiff = nullptr;
        }
        else if (*fAvgSyncDiff < -PAUSE_THRESHOLD)
        {
            // wait until timer and master clock are in sync (> 10ms)
            fTimer.Pause();

#ifdef LOG_SYNC
            ULog::Log.LogError("Pause: " + std::to_string(MasterClock) + " < " + std::to_string(LyricTime));
#endif
        }
        else if (fTimer.GetState == UTime::rtsPaused && *fAvgSyncDiff >= SecDouble::zero())
        {
            fTimer.Start();

#ifdef LOG_SYNC
            ULog::Log.LogError("Unpause: " + std::to_string(LyricTime));
#endif
        }
        fLastClock = MasterClock;
        return Result;
    }

    template<class ClockType>
    typename TLyricsState<ClockType>::duration TLyricsState<ClockType>::GetCurrentTime()
    {
        duration LyricTime = fTimer.GetTime();
        if (fSyncSource)
            return Synchronize(LyricTime);
        return LyricTime;
    }

    template<class ClockType>
    void TLyricsState<ClockType>::SetSyncSource(const std::shared_ptr<UTime::TSyncSource>& SyncSource)
    {
        fSyncSource = SyncSource;
    }

    /**
     * Resets the timer and state of the lyrics.
     * The timer will be stopped afterwards so you have to call Resume()
     * to start the lyrics timer.
     */
    template<class ClockType>
    void TLyricsState<ClockType>::Reset()
    {
        Stop();

        fSyncSource = nullptr;

        StartTime = 0;
        TotalTime = 0;

        beat = Beat();
        beatClick = Beat();
        beatDetect = Beat();
    }

    /**
     * Updates the beat information (CurrentBeat/MidBeat/...) according to the
     * current lyric time.
     */
    template<class ClockType>
    void TLyricsState<ClockType>::UpdateBeats()
    {
        //TODO:: not finished!
        duration CurLyricsTime = GetCurrentTime();

        beat.SetMid(UNote::GetMidBeat(CurLyricsTime - StartTime / 1000));
        beatClick.SetMid(UNote::GetMidBeat(CurLyricsTime - StartTime / 1000));

        // MidBeatD = MidBeat with additional GAP
        beatDetect.SetMid(-0.5 + UNote::GetMidBeat(CurLyricsTime - (StartTime + UIni::Ini.MicDelay) / 1000));
    }
}