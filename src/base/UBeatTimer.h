#pragma once
/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 */
#include "../switches.h"

#include "UIni.h"
#include "UTime.h"

namespace UBeatTimer
{
    struct Beat
    {
        int Old = -1;        // previous beat
        int Current = -1;    // current beat (rounded)
        double Mid = -1;     // current beat (float)

        void SetMid(double mid);
    };
	/**
	 * TLyricsState contains all information concerning the
	 * state of the lyrics, e.g. the current beat or duration of the lyrics.
	 */
    template<class ClockType = std::chrono::system_clock>
    class TLyricsState
    {
    public:

        typedef typename ClockType::duration duration;

    private:


        UTime::TRelativeTimer<ClockType> fTimer; // keeps track of the current time
        std::shared_ptr<UTime::TSyncSource> fSyncSource;
        std::unique_ptr<SecDouble> fAvgSyncDiff;
        double fLastClock;       // last master clock value
        // Note: do not use Timer.GetState() to check if lyrics are paused as
        // Timer.Pause() is used for synching.

        duration Synchronize(const duration& LyricTime);

    public:

        Beat beat;
        // now we use this for super synchronization!
        // only used when analyzing voice
        Beat beatDetect;
        // we use this for audible clicks
        Beat beatClick;

        int OldLine;    // previous displayed sentence

        double StartTime;       // time till start of lyrics (= Gap)
        double TotalTime;       // total song time

        TLyricsState();

        [[nodiscard]] bool isPaused() const;

        /**
         * Resets the LyricsState state.
         */
        void Reset();

        void UpdateBeats();

        /**
         * Sets a master clock for this LyricsState. If no sync-source is set
         * or SyncSource is nil the internal timer is used.
         */
        void SetSyncSource(const std::shared_ptr<UTime::TSyncSource>& SyncSource);

        /**
         * Starts the timer. This is either done
         * - immediately if WaitForTrigger is false or
         * - after the first call to GetCurrentTime()/SetCurrentTime() or Start(false)
         */
        void Start(bool WaitForTrigger = false);

        /**
         * Pauses the timer.
         * The counter is preserved and can be resumed by a call to Start().
         */
        void Pause();

        /**
         * Stops the timer.
         * The counter is reset to 0.
         */
        void Stop();

        /**
         * Returns/Sets the current song time (in seconds) used as base-timer for lyrics etc.
         * If GetCurrentTime()/SetCurrentTime() if Start() was called
         */
        duration GetCurrentTime();
        void SetCurrentTime(const typename ClockType::duration& Time);
    };

    /*uses
      UNote,
      ULog,
      SysUtils,
      Math;*/

}