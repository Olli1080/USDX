#include "UTime.h"

namespace UTime
{
	/**
	 * TRelativeTimer
	 **/

	/**
	 * Creates a new relative timer.
	 * A relative timer works like a stop - watch.It can be paused and
	 * resumed afterwards, continuing with the counter it had when it was paused.
	 */
    template<typename ClockType>
    TRelativeTimer<ClockType>::TRelativeTimer()
        :
	AbsoluteTime(ClockType::now())
    {}

    /**
     * Starts the timer.
     * If WaitForTrigger is false the timer will be started immediately.
     * If WaitForTrigger is true the timer will be started when a trigger event
     * occurs.A trigger event is a call of one of the Get - / SetTime() methods.
     * In addition the timer can be started by calling this method again with
     * WaitForTrigger set to false.
     */
    template<typename ClockType>
    void TRelativeTimer<ClockType>::Start(bool WaitForTrigger)
    {
        switch (State)
        {
        case rtsStopped: [[fallthrough]]
        case rtsPaused:
        {
            if (WaitForTrigger)
            {
                State = rtsWait;
            }
            else
            {
                State = rtsRunning;
                AbsoluteTime = std::chrono::system_clock::now();
            }
            break;
        }
        case rtsWait:
        {
            if (!WaitForTrigger)
            {
                State = rtsRunning;
                AbsoluteTime = std::chrono::system_clock::now();
                RelativeTime = std::chrono::system_clock::duration::zero();
            }
            break;
        }
        case rtsRunning:
            return;
        }
    }

    /**
    * Pauses the timer and leaves the counter untouched.
    */
    template<typename ClockType>
    void TRelativeTimer<ClockType>::Pause()
    {
        if (State == rtsRunning)
        {
            // Important: GetTime() must be called in running state
            RelativeTime = GetTime();
            State = rtsPaused;
        }
    }

    /**
    * Stops the timer and sets its counter to 0.
    **/
    template<typename ClockType>
    void TRelativeTimer<ClockType>::Stop()
    {
        if (State != rtsStopped)
        {
            State = rtsStopped;
            RelativeTime = std::chrono::system_clock::duration::zero();
        }
    }

    /**
     * Returns the current counter of the timer.
     * If WaitForTrigger was true in Start() the timer will be started
     * if it was not already running.
     */
    template<typename ClockType>
    typename ClockType::duration TRelativeTimer<ClockType>::GetTime()
    {
        switch (State)
        {
        case rtsStopped: [[nodiscard]]
        case rtsPaused:
        {
            return RelativeTime;
        }
        case rtsRunning:
        {
            return RelativeTime + ClockType::now() - AbsoluteTime;
        }
        case rtsWait:
        {
            // start triggered
            State = rtsRunning;
            AbsoluteTime = ClockType::now();
            return RelativeTime;
        }
        }
        std::unreachable();
    }

    /**
    * Sets the counter of the timer.
    * If WaitForTrigger was true in Start() the timer will be started
    * if it was not already running.
    */
    template<typename ClockType>
    void TRelativeTimer<ClockType>::SetTime(const typename ClockType::duration& Time)
    {
        RelativeTime = Time;
        AbsoluteTime = ClockType::now();
        // start triggered
        if (State == rtsWait)
            State = rtsRunning;
    }

    template<typename ClockType>
    TRelativeTimerState TRelativeTimer<ClockType>::GetState() const
    {
        return State;
    }

    /*
    void CountSkipTimeSet()
    {
        TimeNew = std::chrono::system_clock::now();
    }

    void CountSkipTime()
    {
        TimeOld = TimeNew;
        TimeNew = std::chrono::system_clock::now();
        TimeSkip = TimeNew - TimeOld;
    }

    void CountMidTime()
    {
        TimeMidTemp = std::chrono::system_clock::now();
        TimeMid = TimeMidTemp - TimeNew;
    }
    */
}