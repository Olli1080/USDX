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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/UTime.pas $
 * $Id: UTime.pas 3103 2014-11-22 23:21:19Z k-m_schindler $
 */

#include "../switches.h"

namespace UTime
{

    class TTime
    {
    public:
        TTime();
        double GetTime();
    };

    enum TRelativeTimerState { rtsStopped, rtsWait, rtsPaused, rtsRunning };

    template<typename ClockType>
    class TRelativeTimer
    {
    private:

	    typename ClockType::time_point AbsoluteTime;
	    typename ClockType::duration RelativeTime = ClockType::duration::zero();
        TRelativeTimerState State = rtsStopped;

    public:

        TRelativeTimer();

        void Start(bool WaitForTrigger = false);
        void Pause();
        void Stop();

        [[nodiscard]] typename ClockType::duration GetTime();
        void SetTime(const typename ClockType::duration& Time);
        [[nodiscard]] TRelativeTimerState GetState() const;
    };

    class TSyncSource
    {
    public:

        virtual ~TSyncSource() = default;

        virtual double GetClock() = 0;

        typedef std::shared_ptr<TSyncSource> SPtr;
    };

    void CountSkipTimeSet();
    void CountSkipTime();
    void CountMidTime();
    
    inline TRelativeTimer VideoBGTimer;

    /*
    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> TimePointMili;

    std::chrono::system_clock::time_point TimeNew;
    std::chrono::system_clock::time_point TimeOld;
    std::chrono::system_clock::time_point TimeMidTemp;

	SecDouble TimeSkip;
	SecDouble TimeMid;
	*/
}