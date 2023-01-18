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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UJoystick.pas $
 * $Id: UJoystick.pas 1485 2008-10-28 20:16:05Z tobigun $
 */
#include "../switches.h"

#include <SDL2/SDL_events.h>

namespace UJoystick
{
    /*uses
      Classes,
      strutils,
      typinfo, // for GetEnumName
      fgl, // TGFMap
      math,
      SDL2;*/
    constexpr double JOYSTICK_MOUSE_REPEAT = 1;
    constexpr double JOYSTICK_MOUSE_DEADZONE = 0.01;
    constexpr double JOYSTICK_MOUSE_DEFAULTSPEED = 550;

    constexpr double JOYSTICK_AXIS_PRESSED_THRESHOLD = 0.2;
    constexpr double JOYSTICK_AXIS_RELEASED_THRESHOLD = 0.7;
    constexpr double JOYSTICK_AXIS_REPEAT_THRESHOLD = 0.8;
    constexpr double JOYSTICK_AXIS_REPEAT_TIME = 150; // milliseconds to ignore repeated input
    constexpr double JOYSTICK_AXIS_MAX_RANGE = 32767; // SDL ranges -32768 to 32767

    // custom SDL mapping for Axis-mapping

    // Axis mapping names for legacy Joystick to button/key mapping
    SDL_HAT_AXIS_Center = 0;
    SDL_HAT_AXIS_X_pos = 1;
    SDL_HAT_AXIS_X_neg = 2;
    SDL_HAT_AXIS_X = 3;  // unused
    SDL_HAT_AXIS_Y_pos = 4;
    SDL_HAT_AXIS_Y_neg = 8;
    SDL_HAT_AXIS_Y = 12; // unused
    SDL_HAT_AXIS_X_pos_Y_pos = SDL_HAT_AXIS_X_pos or SDL_HAT_AXIS_Y_pos; // 5  Top Right
    SDL_HAT_AXIS_X_pos_Y_neg = SDL_HAT_AXIS_X_pos or SDL_HAT_AXIS_Y_neg; // 9  Bottom Right
    SDL_HAT_AXIS_Y_pos_X_neg = SDL_HAT_AXIS_Y_pos or SDL_HAT_AXIS_X_neg; // 6  Bottom Left
    SDL_HAT_AXIS_Y_neg_X_neg = SDL_HAT_AXIS_Y_neg or SDL_HAT_AXIS_X_neg; // 10 Top Left

    //type
    enum class TJoyButtonState { bsReleased, bsPressed };
    enum class TJoyControllerType { ctJoystick, ctGameController };

    struct TJoyMouse
    {
        X, Y: int;
        DeltaX, DeltaY: double;
    Time: LongInt;
    IsSet: bool;
    };

    PControllerDPadState = ^ TControllerDPadState;
    struct TControllerDPadState
    {
    X: bool;
    Y: bool;

    RawX: int;
    RawY: int;
    };

    PControllerAxisState = ^ TControllerAxisState;
    struct TControllerAxisState
    {
    Perc: double;

    Time: uint32_t; // used to prevent additional 'repeated' press when moving Axis into one side fast enough
    Pressed: bool;
    Repeat_: bool;
    WasRepeat: bool;
    };

    TControllerDPadIDStateMap = TFPGMap<int, TControllerDPadState>;
    TControllerAxisIDStateMap = TFPGMap<int, TControllerAxisState>;

    class TJoyController
    {
    private:
        _DeviceId : int;
    _Name: string;
    _Enabled: bool;

    LastMouseState: TJoyMouse;
    MouseRepeatThread: PSDL_Thread;
    MouseRepeatThreadFlag: bool;
    MouseMode: bool;

    protected:

        _Type : TJoyControllerType;
    DPadStates: TControllerDPadIDStateMap;
    AxesStates: TControllerAxisIDStateMap;

    private:
        function GetDeviceId() : int;
        function GetInstanceId() : int;
        function GetName() : string;

        function GetControllerType() : TJoyControllerType;
        function GetControllerAxesCount() : int; virtual;
        function GetControllerHatsCount() : int; virtual;
        function GetControllerButtonCount() : int; virtual;

        function SimulateKeyboard(Key: TSDL_KeyCode; Pressed: bool; NoMouseOverride: bool = false) : bool; virtual;

        void SimulateMouseSend();
        function SimulateMouse(ButtonId: byte; Pressed: bool) : bool; virtual; overload;
        function SimulateMouse(Axis: byte; Delta: real) : bool; virtual; overload;

    public:
        constructor Create(DeviceId : int; Name: string = ''); virtual;
        destructor Destroy(); override;

        property Name: string read GetName;
        property DeviceId: int read GetDeviceId;
        property InstanceId: int read GetInstanceId;

        function IsEnabled() : bool;
        void SetEnabled(Enable: bool);
        void Enable;
        void Disable;

        function GetJoystick() : Pointer; virtual; abstract;
        function ShouldIgnoreLegacy() : bool; virtual;

        // actions
        function HandleControllerDPad(ControllerId: int; PadId: int; X, Y: int) : bool;
        function HandleControllerMotion(ControllerId: int; Axis: int; Perc: double; Time: uint32_t) : bool;
        function HandleControllerButton(ButtonId: int; State: TJoyButtonState) : bool;

        void SwitchMouseModeFor(ButtonId: int; State: TJoyButtonState); virtual; abstract;

        // mapping
        function TranslateAxisToKey(Axis: int; Direction: int; out Key : TSDL_KeyCode) : bool; virtual;
        function TranslateAxisToMouseAxis(Axis: int; Direction: int; out MouseAxis : byte) : bool; virtual;
        function TranslateDPadToKey(Hat: int; Axis: int; Positive: bool; out Key : TSDL_KeyCode) : bool; virtual;
        function TranslateButtonToKey(ButtonId: int; State: TJoyButtonState; out Key : TSDL_KeyCode) : bool; virtual;
        function TranslateButtonToMouse(ButtonId: int; State: TJoyButtonState; out Button : int) : bool; virtual;

        property ControllerType: TJoyControllerType read GetControllerType;
        property Enabled: bool read IsEnabled;

        property AxesCount: int read GetControllerAxesCount;
        property HatsCount: int read GetControllerHatsCount;
        property ButtonCount: int read GetControllerButtonCount;

        // TODO: properly track/receive mouse coords in order to use relative mouse
        void OnMouseMove(X, Y: int);
    };

    TControllerIDMap = TFPGMap<int, TJoyController>;

    class TJoyControllerJoyStick : public TJoyController
    {
    private:
        JoyStick : PSDL_Joystick;

    public:
        constructor Create(DeviceId : int; JS: PSDL_Joystick; Name: string = ''); overload;
        destructor Destroy(); override;

        function GetJoystick() : Pointer; override;

        // mapping
        function TranslateAxisToKey(Axis: int; Direction: int; out Key : TSDL_KeyCode) : bool; override;
        function TranslateAxisToMouseAxis(Axis: int; Direction: int; out MouseAxis : byte) : bool; override;
        function TranslateDPadToKey(Hat: int; Axis: int; Positive: bool; out Key : TSDL_KeyCode) : bool;  override;
        function TranslateButtonToKey(ButtonId: int; State: TJoyButtonState; out Key : TSDL_KeyCode) : bool; override;
        function TranslateButtonToMouse(ButtonId: int; State: TJoyButtonState; out Button : int) : bool; override;

        void SwitchMouseModeFor(ButtonId: int; State: TJoyButtonState); override;
    };
    class TJoyControllerGameController : public TJoyController
    {
    private:
        GameController : PSDL_GameController;

    public:
        constructor Create(DeviceId : int; GC: PSDL_GameController; Name: string = ''); overload;
        destructor Destroy(); override;

        function GetJoystick() : Pointer; override;
        function ShouldIgnoreLegacy() : bool; override;

        // mapping
        function TranslateAxisToKey(Axis: int; Direction: int; out Key : TSDL_KeyCode) : bool; override;
        function TranslateAxisToMouseAxis(Axis: int; Direction: int; out MouseAxis : byte) : bool; override;
        function TranslateDPadToKey(Hat: int; Axis: int; Positive: bool; out Key : TSDL_KeyCode) : bool; override;
        function TranslateButtonToKey(ButtonId: int; State: TJoyButtonState; out Key : TSDL_KeyCode) : bool; override;
        function TranslateButtonToMouse(ButtonId: int; State: TJoyButtonState; out Button : int) : bool; override;

        void SwitchMouseModeFor(ButtonId: int; State: TJoyButtonState); override;
    };

    class TJoy
    {
    private:

        Controllers : TControllerIDMap;

    public:

        constructor Create;
        destructor Destroy; override;

    private:

        function AddController(DeviceId: int; out Error : string) : bool;
        function RemoveController(InstanceId: int) : bool;
        function GetControllerByDeviceId(Id: int; out Controller : TJoyController) : bool;
        function GetControllerByInstanceId(Id: int; out Controller : TJoyController) : bool;
        function HasControllerByDeviceId(Id: int) : bool;
        function HasControllerByInstanceId(Id: int) : bool;

    public:

        // events
        // A new Game controller has been inserted into the system
        void OnControllerAdded(DeviceId: int);

        // An opened Game controller has been removed 
        void OnControllerRemoved(InstanceId: int);

        // The controller mapping was updated
        void OnControllerRemapped(InstanceId: int);

        // Game controller DPad motion 
        void OnControllerDPad(id: int; PadId: int; X, Y: int; Legacy: bool = false);

        // Game controller axis motion 
        void OnControllerMotion(id: int; Axis: int; Perc: double; Time: uint32_t; Legacy: bool = false);

        // Game controller button pressed/released
        void OnControllerButton(id: int; ButtonId: int; State: TJoyButtonState; Legacy: bool = false);

        // called when real mouse moved
        void OnMouseMove(X, Y: int);
    };

TJoy Joy;

void InitializeJoystick();
void FinalizeJoyStick();
bool HasJoyStick();

void OnJoystickPollEvent(SDL_Event Event);

function ifthen(val:bool;const iftrue:TJoyButtonState; const iffalse:TJoyButtonState = bsReleased): TJoyButtonState; inline; overload;
function ifthen(val:bool;const iftrue:TSDL_KeyCode; const iffalse:TSDL_KeyCode = 0): TSDL_KeyCode; inline; overload;

int MouseRepeatHandlerFunc(Data: Pointer);// forward;
}