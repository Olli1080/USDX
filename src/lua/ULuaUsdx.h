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

namespace ULuaUsdx
{
//uses ULua;

//{ some basic lua c functions from usdx table }

//{ Usdx.Time - returns sdl_time to have time numbers comparable with
  //            ultrastar deluxe ones. no arguments }
int Time();

//{ Usdx.Version - returns Usdx version string (the same that US_Version
  //language-constant does). no arguments }
int Version();

//{ Usdx.Hook - returns an hook table with name and Unhook function
              //arguments: event_name: string }
/*TODO hooktype*/int Hook(const std::string& event_name);

//{ Usdx.ShutMeDown - no results, no arguments
  //unloads the calling plugin }
int ShutMeDown();

void registerUSDX(sol::state& state)
{
    state["Usdx"] = state.create_table_with
    (
        "Version", Version,
        "Time", Time,
        "Hook", Hook,
        "ShutMeDown", ShutMeDown
    );
}
/*
const
  ULuaUsdx_Lib_f: array [0..4] of lual_reg = (
    (name:'Version'; func:ULuaUsdx_Version),
    (name:'Time'; func:ULuaUsdx_Time),
    (name:'Hook'; func:ULuaUsdx_Hook),
    (name:'ShutMeDown'; func:ULuaUsdx_ShutMeDown),
    (name:nil;func:nil)
  );
*/
//implementation
//uses sdl2, ULuaCore, ULuaUtils, UHookableEvent, UConfig;

//{ Usdx.Time - returns sdl_time to have time numbers comparable with
  //            ultrastar deluxe ones. no arguments }
int Time()
//  var top: Integer;
{
  //remove arguments (if any)
  top = lua_gettop(L);

  if (top > 0) then
    lua_pop(L, top);

  //push result
  lua_pushinteger(L, SDL_GetTicks);
  Result = 1; //one result
}

//{ Usdx.Version - returns Usdx version string (the same that US_Version
  //language-constant does). no arguments }
int Version()
//  var top: Integer;
{
  //remove arguments (if any)
  top = lua_gettop(L);

  if (top > 0) then
    lua_pop(L, top);

  //push result
  lua_pushstring(L, PChar(USDXVersionStr()));
  Result = 1; //one result
}

//{ Usdx.Hook - returns an hook table with name and Unhook function
  //            arguments: event_name: string; function_name: string }
int Hook(const std::string& event_name/*TODO:: more arguments?*/)
/*var
  EventName: String;
  FunctionName: String;
  P: TLuaPlugin;
  Event: THookableEvent;*/
{
  EventName = luaL_checkstring(L, 1);
  FunctionName = luaL_checkstring(L, 2);

  P = Lua_GetOwner(L);

  lua_pop(L, lua_gettop(L)); //clear stack

  Result = 1;

  Event = LuaCore.GetEventByName(EventName);
  if (Event <> nil) then
  {
    Event.Hook(L, P.Id, FunctionName);
  }
  else
    luaL_error(L, PChar('event does not exist: ' + EventName));
}

int ShutMeDown()
  /*var
    top: Integer;
    P: TLuaPlugin;*/
{
  Result = 0;
  
  //remove arguments (if any)
  top = lua_gettop(L);

  if (top > 0) then
    lua_pop(L, top);

  P = Lua_GetOwner(L);

  P.ShutMeDown;
}
}