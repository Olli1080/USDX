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

#include <sol/sol.hpp>

#include "../base/ULog.h"

namespace ULuaLog
{
inline void registerLuaLog(sol::state& state)
{
    auto& Log = ULog::Log;

    state["Log"] = state.create_table_with
    (
        "LogError", sol::overload(
            [&Log](const std::string& str) { Log.LogError(str); },
            [&Log](const std::string& str, const std::string& ctx) { Log.LogError(str, ctx); }
        ),
        "LogMsg", sol::overload(
            [&Log](const std::string& str, int level) { Log.LogMsg(str, level); },
            [&Log](const std::string& str, const std::string& ctx, int level) { Log.LogMsg(str, ctx, level); }
        ),
        "BenchmarkStart", std::bind_front(ULog::TLog::BenchmarkStart, Log),
        "BenchmarkEnd", std::bind_front(ULog::TLog::BenchmarkEnd, Log),
        "LogBenchmark", std::bind_front(ULog::TLog::LogBenchmark, Log),
        "LogDebug", std::bind_front(ULog::TLog::LogDebug, Log),
        "LogInfo", std::bind_front(ULog::TLog::LogInfo, Log),
        "LogStatus", std::bind_front(ULog::TLog::LogStatus, Log),
        "LogWarn", std::bind_front(ULog::TLog::LogWarn, Log),
        "LogCritical", std::bind_front(ULog::TLog::LogCritical, Log),
        "CriticalError", std::bind_front(ULog::TLog::CriticalError, Log),
        "SetLogLevel", std::bind_front(ULog::TLog::SetLogLevel, Log),
        "GetLogLevel", std::bind_front(ULog::TLog::GetLogLevel, Log)
    );
}
}