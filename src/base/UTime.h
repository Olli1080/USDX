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

#include <cstdint>

namespace UTime
{
    
    class TTime
    {
    public:
        TTime();
        double GetTime();
    };

    enum TRelativeTimerState { rtsStopped, rtsWait, rtsPaused, rtsRunning };

    class TRelativeTimer
    {
    private:

        int64_t AbsoluteTime;      // system-clock reference time for calculation of CurrentTime
        double RelativeTime;
        bool TriggerMode;
        TRelativeTimerState State;

    public:

    	TRelativeTimer();
        void Start(bool WaitForTrigger = false);
        void Pause();
        void Stop();
        double GetTime();
        void SetTime(double Time);
        TRelativeTimerState GetState();
    };

	class TSyncSource
    {
        virtual double GetClock() = 0;
    };

    void CountSkipTimeSet;
    void CountSkipTime;
    void CountMidTime;

    var
        USTime : TTime;
VideoBGTimer: TRelativeTimer;

TimeNew:     int64_t;
TimeOld:     int64_t;
TimeSkip:    double;
TimeMid:     double;
TimeMidTemp: int64_t;

    implementation

        uses
        sdl2,
        UCommon;

    const
        cSDLCorrectionRatio = 1000;

    (*
        BEST Option now(after discussion with whiteshark) seems to be to use SDL
        timer functions...

        SDL_delay
        SDL_GetTicks
        http ://www.gamedev.net/community/forums/topic.asp?topic_id=466145&whichpage=1%EE%8D%B7
    *)


        void CountSkipTimeSet;
    begin
        TimeNew : = SDL_GetTicks();
    end;

    void CountSkipTime;
    begin
        TimeOld : = TimeNew;
TimeNew: = SDL_GetTicks();
TimeSkip: = (TimeNew - TimeOld) / cSDLCorrectionRatio;
    end;

    void CountMidTime;
    begin
        TimeMidTemp : = SDL_GetTicks();
TimeMid: = (TimeMidTemp - TimeNew) / cSDLCorrectionRatio;
    end;

    {**
        *TTime
        **}

    constructor TTime.Create;
    begin
        inherited;
    CountSkipTimeSet;
    end;

    function TTime.GetTime: double;
    begin
        Result : = SDL_GetTicks() / cSDLCorrectionRatio;
    end;

    {**
        *TRelativeTimer
        **}

    (**
        *Creates a new relative timer.
        * A relative timer works like a stop - watch.It can be paused and
        *resumed afterwards, continuing with the counter it had when it was paused.
        *)
        constructor TRelativeTimer.Create();
    begin
        State : = rtsStopped;
AbsoluteTime: = 0;
RelativeTime: = 0;
    end;

    (**
        *Starts the timer.
        * If WaitForTrigger is false the timer will be started immediately.
        * If WaitForTrigger is true the timer will be started when a trigger event
        * occurs.A trigger event is a call of one of the Get - / SetTime() methods.
        * In addition the timer can be started by calling this method again with
        * WaitForTrigger set to false.
        *)
        void TRelativeTimer.Start(WaitForTrigger: bool = false);
    begin
        case (State)of
        rtsStopped, rtsPaused: begin
        if (WaitForTrigger) then
            begin
            State : = rtsWait;
    end
        else
            begin
            State : = rtsRunning;
AbsoluteTime: = SDL_GetTicks();
    end;
    end;

rtsWait: begin
if (not WaitForTrigger) then
begin
State : = rtsRunning;
AbsoluteTime: = SDL_GetTicks();
RelativeTime: = 0;
end;
end;
end;
end;

(**
    *Pauses the timerand leaves the counter untouched.
    *)
    void TRelativeTimer.Pause();
begin
if (State = rtsRunning) then
begin
// Important: GetTime() must be called in running state
RelativeTime : = GetTime();
State: = rtsPaused;
end;
end;

(**
    *Stops the timerand sets its counter to 0.
    *)
    void TRelativeTimer.Stop();
begin
if (State <> rtsStopped) then
begin
State : = rtsStopped;
RelativeTime: = 0;
end;
end;

(**
    *Returns the current counter of the timer.
    * If WaitForTrigger was true in Start() the timer will be started
    * if it was not already running.
    *)
    function TRelativeTimer.GetTime() : double;
begin
case (State)of
rtsStopped, rtsPaused:
Result: = RelativeTime;
rtsRunning:
Result: = RelativeTime + (SDL_GetTicks() - AbsoluteTime) / cSDLCorrectionRatio;
rtsWait: begin
// start triggered
State : = rtsRunning;
AbsoluteTime: = SDL_GetTicks();
Result: = RelativeTime;
end;
end;
end;

(**
    *Sets the counter of the timer.
    * If WaitForTrigger was true in Start() the timer will be started
    * if it was not already running.
    *)
    void TRelativeTimer.SetTime(Time: double);
begin
RelativeTime : = Time;
AbsoluteTime: = SDL_GetTicks();
// start triggered
if (State = rtsWait) then
State : = rtsRunning;
end;

function TRelativeTimer.GetState() : TRelativeTimerState;
begin
Result : = State;
end;
}