#include "UJoystick.h"

#include "ULog.h"

namespace UJoystick
{

    /*
implementation

uses SysUtils,
     ULog;
*/

    void InitializeJoystick()
    {
        if (assigned(Joy))
    		return;
        ULog::Log.LogStatus("Initialize Joystick", "Initialization");
		Joy = TJoy::Create;
    }

    void FinalizeJoyStick()
    {
        if (!assigned(Joy))
            return;

        ULog::Log.LogStatus("Uninitialize Joystick", "Finalization");
        Joy.Destroy;
		Joy = nil;
    }

    bool HasJoyStick()
    {
		return assigned(Joy);
    }

    void OnJoystickPollEvent(SDL_Event Event)
    {
        // sanity check
        if (Joy == nil)
            return;
        
        switch (Event.type)
        {
        case SDL_JOYAXISMOTION:
            //with Event.jaxis do
        {
            ULog::Log.LogDebug(Format("JOYAXISMOTION [%d] Axis:%d  Value:%d  Time:%d", [which, axis, value, timestamp]), "TJoy::Polling");
            Joy.OnControllerMotion(which, axis, EnsureRange((1.0 * value) / JOYSTICK_AXIS_MAX_RANGE, -1.0, 1.0), timestamp, true);
        }
        case SDL_JOYHATMOTION:
        //with Event.jhat do
        {
            ULog::Log.LogDebug(Format("JOYHATMOTION [%d] Pad:%d  Value:%d  Time:%d", [which, hat, value, timestamp]), "TJoy::Polling");
            Joy.OnControllerDPad(which, hat, ifthen(value and SDL_HAT_LEFT < > 0, -1, ifthen(value and SDL_HAT_RIGHT < > 0, 1, 0)),
                ifthen(value and SDL_HAT_DOWN < > 0, -1, ifthen(value and SDL_HAT_UP < > 0, 1, 0)),
                true);
        }
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
          //  with Event.jbutton do
        {
            ULog::Log.LogDebug(Format("JOYBUTTON [%d] Button:%d  State:%d  Type:%d  Time:%d", [which, button, state, type_, timestamp]), "TJoy::Polling");
            Joy.OnControllerButton(which, button, ifthen(state = SDL_PRESSED, bsPressed, bsReleased), true);
        }

        case SDL_CONTROLLERDEVICEADDED:
        {
            ULog::Log.LogDebug(Format("CONTROLLERDEVICEADDED [DeviceID=%d]", [Event.cdevice.which]), "TJoy::Polling");
            Joy.OnControllerAdded(Event.cdevice.which);
        }
        case SDL_CONTROLLERDEVICEREMOVED:
        {
            ULog::Log.LogDebug(Format("CONTROLLERDEVICEREMOVED [InstanceID=%d]", [Event.cdevice.which]), "TJoy::Polling");
            Joy.OnControllerRemoved(Event.cdevice.which);
        }
        case SDL_CONTROLLERDEVICEREMAPPED:
        {
            ULog::Log.LogDebug(Format("CONTROLLERDEVICEREMAPPED [InstanceID=%d]", [Event.cdevice.which]), "TJoy::Polling");
            Joy.OnControllerRemapped(Event.cdevice.which);
        }
        case SDL_CONTROLLERAXISMOTION:
        //with Event.caxis do
        {
            ULog::Log.LogDebug(Format("CONTROLLERAXISMOTION [%d] Axis:%d  Value:%d  Time:%d", [which, axis, value, timestamp]), "TJoy::Polling");
            Joy.OnControllerMotion(which, axis, EnsureRange((1.0 * value) / JOYSTICK_AXIS_MAX_RANGE, -1.0, 1.0), timestamp);
        }
        case SDL_CONTROLLERBUTTONDOWN:
        //with Event.cbutton do
        {
            ULog::Log.LogDebug(Format("CONTROLLERBUTTONDOWN [%d] Button:%d  State:%d  Type:%d  Time:%d", [which, button, state, type_, timestamp]), "TJoy::Polling");
            Joy.OnControllerButton(which, button, ifthen(state = SDL_PRESSED, bsPressed, bsReleased));
        }
        case SDL_CONTROLLERBUTTONUP:
        //with Event.cbutton do
        {
            ULog::Log.LogDebug(Format("CONTROLLERBUTTONUP [%d] Button:%d  State:%d  Type:%d  Time:%d", [which, button, state, type_, timestamp]), "TJoy::Polling");
            Joy.OnControllerButton(which, button, ifthen(state == SDL_PRESSED, bsPressed, bsReleased));
        }
        case SDL_JOYDEVICEADDED:
        {
            Joy = TJoy::Create;
        }
        } // case
    }

    int MouseRepeatHandlerFunc(Data: Pointer)
    /*var
        JoyC : TJoyController;*/
    {
        auto JoyC = TJoyController(Data);
		while (JoyC != nil && JoyC.MouseRepeatThreadFlag)
        {
			JoyC.SimulateMouseSend();
			Sleep(JOYSTICK_MOUSE_REPEAT);
		}
		Result = 0;
    }

        constructor TJoy::Create;
        var
            Controller : TJoyController;
    Error: string;

    N: integer;
    I: integer;
    BestButtonCount: integer;
    BestIndex: integer;
        {
            inherited;

    Controllers = TControllerIDMap.Create;
        Controllers.Sorted  = true;
    Controller = nil;

        SDL_InitSubSystem(SDL_INIT_JOYSTICK or SDL_INIT_GAMECONTROLLER);
    N = SDL_NumJoysticks;
        ULog::Log.LogStatus(Format("Joystick count: %d", [N]), "TJoy::Create");
        for i  = 0 to n - 1 do
            {
            if not AddController(i, Error) then
                {
                ULog::Log.LogError(Error, "TJoy::Create");
        }
        }

        if Controllers.Count < 1 then
            {
            ULog::Log.LogStatus("No GameController/Joystick connected.", "TJoy::Create");
        Exit;
        }

        // if only 1 controller, just activate it
        if Controllers.Count = 1 then
            {
            Controller  = Controllers.Data[0];
        ULog::Log.LogStatus(Format("Using controller: %s", [Controllers.Data[0].Name]), "TJoy::Create");
        }


        if not assigned(Controller) then
            {
            // try finding game controller with best button count
            BestIndex  = -1;
    BestButtonCount = 0;
        for i  = 0 to Controllers.Count - 1 do
            {
            if Controllers.Data[i].ControllerType = ctGameController then
                {
                if Controllers.Data[i].ButtonCount > BestButtonCount then
                    {
                    BestButtonCount  = Controllers.Data[i].ButtonCount;
    BestIndex = i;
        }
        }
        }

        if BestIndex >= 0 then
            {
            Controller  = Controllers.Data[BestIndex];
        ULog::Log.LogStatus(Format("Using game controller: %s", [Controller.Name]), "TJoy::Create");
        }
        }

        // fallback using best legacy joystick
        if not assigned(Controller) then
            {
            // try finding joystick with best button count
            BestIndex  = -1;
    BestButtonCount = 0;
        for i  = 0 to Controllers.Count - 1 do
            {
            if Controllers.Data[i].ControllerType = ctJoystick then
                {
                if Controllers.Data[i].ButtonCount > BestButtonCount then
                    {
                    BestButtonCount  = Controllers.Data[i].ButtonCount;
    BestIndex = i;
        }
        }
        }

        if BestIndex >= 0 then
            {
            Controller  = Controllers.Data[BestIndex];
        ULog::Log.LogStatus(Format("Using legacy Joystick: %s", [Controller.Name]), "TJoy::Create");
        }
        }

        if not assigned(Controller) then
            {
            ULog::Log.LogWarn("Unable to find a suited controller", "TJoy::Create");
        Exit;
        }

        ULog::Log.LogStatus(Format("Enable input for controller %s [ID=%d]", [Controller.Name, Controller.DeviceId]), "TJoy::Create");
        Controller.Enable;
        }

        destructor TJoy::Destroy;
        var
            i, index: integer;
    Controller: TJoyController;
        {
            inherited;

        if Controllers <> nil then
            {

            for i  = 0 to Controllers.Count - 1 do
                if Controllers.Find(Controllers.Keys[i], index) and assigned(Controllers.Data[index]) then
                    Controllers.Data[index].Destroy();

        Controllers.Clear;
        Controllers.Free;
    Controllers = nil;
        }
        }

        function TJoy::AddController(DeviceId: integer; out Error : string) : boolean;
        var
            JoyStick : PSDL_Joystick;
    GameController:    PSDL_GameController;
    Controller: TJoyController;
    s: string;
    {
        Result = true;
        if SDL_IsGameController(DeviceId) = SDL_TRUE then
        {
        s = SDL_GameControllerNameForIndex(DeviceId);
    ULog::Log.LogStatus(Format("Is a game controller: %s [ID=%d]",[s, DeviceId]), "TJoy::AddController");
GameController = SDL_GameControllerOpen(DeviceId);
    if GameController = nil then
    {
    Error = "SDL_GameControllerOpen failed";
Result = false;
    }
    else
        {
        Controller = TJoyControllerGameController.Create(DeviceId, GameController, s);
    Controllers.Add(Controller.InstanceId, Controller);
    }
    }
    else
        {
        s = SDL_JoystickNameForIndex(DeviceId);
    ULog::Log.LogStatus(Format("Is a Joystick: %s [ID=%d]",[s, DeviceId]), "TJoy::AddController");
JoyStick = SDL_JoystickOpen(DeviceId);
    if JoyStick = nil then
        {
        Error = "SDL_JoystickOpen failed";
Result = false;
    }
    else
        {
        Controller = TJoyControllerJoyStick.Create(DeviceId, JoyStick, s);
    Controllers.Add(Controller.InstanceId, TJoyControllerJoyStick.Create(DeviceId, JoyStick, s));
    }
    }

    if Controller <> nil then
        {
        ULog::Log.LogStatus(Format("Controller@%d  %d Buttons  %d Axes  %d Hats/DPads",[DeviceId, Controller.ButtonCount, Controller.AxesCount, Controller.HatsCount]), "TJoy::AddController");
    }
    }

    function TJoy::RemoveController(InstanceId: integer) : boolean;
    var Controller : TJoyController;
    {
        Result = false;
    if GetControllerByInstanceId(InstanceId, Controller) then
        {
        Controllers.Remove(Controller.DeviceId);
    Controller.Destroy;
Result = true;
    }
    else ULog::Log.LogError(Format("Unable to disconnect controller with Instance ID: %d",[InstanceId]), "TJoy::RemoveController");
    }

    function TJoy::GetControllerByInstanceId(Id: integer; out Controller : TJoyController) : boolean;
    var
        index : integer;
    {
        Result = false;
    if Controllers.Find(Id, index) then
        {
        Controller = Controllers.Data[index];
Result = true;
    }
    }

    function TJoy::GetControllerByDeviceId(Id: integer; out Controller : TJoyController) : boolean;
    var
        i : integer;
    {
        Result = false;

    for i = 0 to Controllers.Count - 1 do
        {
        if (Controllers.Data[i] <> nil) and (Id = Controllers.Data[i].DeviceId) then
            {
            Controller = Controllers.Data[i];
Result = true;
    Exit;
    }
    }
    }

    function TJoy::HasControllerByInstanceId(Id: integer) : boolean;
    var Controller : TJoyController;
    {
        Result = GetControllerByInstanceId(Id, Controller);
    }

    function TJoy::HasControllerByDeviceId(Id: integer) : boolean;
    var Controller : TJoyController;
    {
        Result = GetControllerByDeviceId(Id, Controller);
    }

    void TJoy::OnControllerAdded(DeviceId: integer);
    var Error : string;
    {
        if not HasControllerByDeviceId(DeviceId) then
            {
            if not AddController(DeviceId, Error) then ULog::Log.LogError(Error, "TJoy::Connect");
    }
    }

    void TJoy::OnControllerRemoved(InstanceId: integer);
    {
        RemoveController(InstanceId);
    }

    void TJoy::OnControllerRemapped(InstanceId: integer);
    {
        // TODO: TJoy::OnControllerRemapped. Not implemented
        }

    void TJoy::OnControllerDPad(id: integer; PadId: integer; X, Y: integer; Legacy: boolean);
    var
        Controller : TJoyController;
    {

        // ignore unknown or disabled input
        if not GetControllerByInstanceId(id, Controller) or not Controller.IsEnabled() then Exit;

        // ignore lecacy events for given controller
        if Legacyand Controller.ShouldIgnoreLegacy() then Exit;

        ULog::Log.LogInfo(Format("DPad   [%d] PadId:%d  X: %d  Y: %d",[id, PadId, X, Y]), "TJoy::Input");
        if not Controller.HandleControllerDPad(id, PadId, X, Y) then
            {
            ULog::Log.LogWarn(Format("Unable to handle DPad input of ""%s"" [ID:%d]",[Controller.Name, id]), "TJoy::Input");
        }
        }

        void TJoy::OnControllerMotion(id: integer; Axis: integer; Perc: double; Time: Cardinal; Legacy: boolean);
        var
            Controller : TJoyController;
        {

            // ignore unknown or disabled input
            if not GetControllerByInstanceId(id, Controller) or not Controller.IsEnabled() then Exit;

            // ignore lecacy events for given controller
            if Legacyand Controller.ShouldIgnoreLegacy() then Exit;

            ULog::Log.LogInfo(Format("Motion [%d] Axis:%d  Perc: %0.3f",[id, Axis, Perc]), "TJoy::Input");
            if not Controller.HandleControllerMotion(id, Axis, Perc, Time) then
                {
                ULog::Log.LogWarn(Format("Unable to handle axis input of ""%s"" [ID:%d]",[Controller.Name, id]), "TJoy::Input");
            }
            }

            void TJoy::OnControllerButton(id: integer; ButtonId: integer; State: TJoyButtonState; Legacy: boolean);
            var
                Controller : TJoyController;
            i, index: integer;
            {

                // ignore unknown or disabled input
                if not GetControllerByInstanceId(id, Controller) or not Controller.IsEnabled() then Exit;

                // ignore lecacy events for given controller
                if Legacyand Controller.ShouldIgnoreLegacy() then Exit;

                ULog::Log.LogInfo(Format("Button [%d] Button: %d  State: %s",[id, ButtonId, GetEnumName(TypeInfo(TJoyButtonState), Ord(State))]), "TJoy::Input");
                if not Controller.HandleControllerButton(ButtonId, State) then
                    {
                    ULog::Log.LogDebug(Format("Unable to handle button input of ""%s"" [ID:%d]",[Controller.Name, id]), "TJoy::Input");
                }
                }

            // workaround for relative mouse
            // TODO: implement simulating mouse properly (access to current cached mouse coors)
            void TJoy::OnMouseMove(X, Y: integer);
            var
                i, index: integer;
            {
                for i = 0 to Controllers.Count - 1 do
                    if Controllers.Find(Controllers.Keys[i], index) and Controllers.Data[index].IsEnabled() then
                        Controllers.Data[index].OnMouseMove(X, Y);
            }


            // Controllers
            // ~~~~~~~~~~~~~~~~~~~~~~~

            constructor TJoyController.Create(DeviceId: integer; Name: string);
            {
                //inherited;

                DPadStates = TControllerDPadIDStateMap.Create;
            DPadStates.Sorted = true;
        AxesStates = TControllerAxisIDStateMap.Create;
            AxesStates.Sorted = true;

        MouseMode = false;

            self._DeviceId = DeviceId;
            self._Name = Name;
            }

            destructor TJoyController.Destroy();
            {
                inherited;

        MouseRepeatThread = nil;

            DPadStates.Free;
            AxesStates.Free;
            }

            function TJoyController.GetDeviceId() : integer;
            {
                Result = _DeviceId;
            }
            function TJoyController.GetInstanceId() : integer;
            {
                Result = SDL_JoystickInstanceID(GetJoystick());
            }
            function TJoyController.GetName() : string;
            {
                Result = _Name;
            }

            function TJoyController.IsEnabled() : boolean;
            {
                Result = _Enabled;
            }
            void TJoyController.Enable;
            {
                SetEnabled(true);
            }
            void TJoyController.Disable;
            {
                SetEnabled(false);
            }

            void TJoyController.SetEnabled(Enable: boolean);
            {
                _Enabled = Enable;
            }

            function TJoyController.GetControllerType() : TJoyControllerType;
            {
                Result = _Type
                }

            function TJoyController.ShouldIgnoreLegacy() : boolean;
            {
                Result = false;
            }

            // TODO: Move to Joystick manager
            function TJoyController.SimulateKeyboard(Key: TSDL_KeyCode; Pressed: boolean; NoMouseOverride: boolean) : boolean;
            var
                JoyEvent : TSDL_Event;
        TempName: string;
            {
                Result = true;

            ULog::Log.LogInfo(Format("Simulate Key: %s  Pressed: %d",[SDL_GetScancodeName(SDL_GetScancodeFromKey(Key)), integer(Pressed)]), "TJoy::Simulate");

            // switch back to non-mouse mode (alias keyboard simulation)
            if not NoMouseOverride then MouseMode = false;

        JoyEvent = Default(TSDL_Event);
            JoyEvent.type_ = ifthen(Pressed, SDL_KEYDOWN, SDL_KEYUP);
            JoyEvent.key.keysym.sym = Key;
            JoyEvent.key.keysym.scancode = SDL_GetScancodeFromKey(Key);

            // always send empty/zero unicode char as workaround. Check UMain.CheckEvents
            JoyEvent.key.keysym.unicode = 0;

            SDL_PushEvent(@JoyEvent);
            }

            // TODO: Move to Joystick manager
            void TJoyController.SimulateMouseSend();
            var
                MouseEvent : TSDL_Event;
        DiffTime: integer;
        Speed: real;
            {

            // directly set mouse which switches button simulation for mouse
            MouseMode = true;

    DiffTime = SDL_GetTicks() - LastMouseState.Time;
    Speed = (DiffTime / 1000.0) * JOYSTICK_MOUSE_DEFAULTSPEED;

        MouseEvent.type_ = SDL_MOUSEMOTION;
        MouseEvent.button.x = LastMouseState.X + Round(LastMouseState.DeltaX * Speed);
        MouseEvent.button.y = LastMouseState.Y + Round(LastMouseState.DeltaY * Speed);
        SDL_PushEvent(@MouseEvent);
        }

            // TODO: Move to Joystick manager
            function TJoyController.SimulateMouse(ButtonId: byte; Pressed: boolean) : boolean;
            var
                JoyEvent : TSDL_Event;
            {
                Result = true;

        JoyEvent = Default(TSDL_Event);
            JoyEvent.type_ = ifthen(Pressed, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
            JoyEvent.button.button = ButtonId;

            // current SDL2 event handling requires having a valid position when sending mouse button presses
            // TODO: clean up mouse position emulation once SDL2 input handling is handling mouse button and motion input separately
            JoyEvent.button.X = LastMouseState.X;
            JoyEvent.button.Y = LastMouseState.Y;

            SDL_PushEvent(@JoyEvent);
            }

            // TODO: Move to Joystick manager
            function TJoyController.SimulateMouse(Axis: byte; Delta: real) : boolean;
            var
                mouseX, mouseY: integer;
            {
                Result = true;

            if not LastMouseState.IsSet then
                {
                SDL_GetMouseState(@mouseX, @mouseY);
            LastMouseState.X = mousex;
            LastMouseState.Y = mousey;
            LastMouseState.Time = SDL_GetTicks();
            }

            if Axis = 0 then LastMouseState.DeltaX = Delta
            else LastMouseState.DeltaY = Delta;

            SimulateMouseSend();

            // check whether we should stop the thread or re-create it, in order to send repeated simulated mouse events
            if (abs(LastMouseState.DeltaX) < JOYSTICK_MOUSE_DEADZONE) and (abs(LastMouseState.DeltaY) < JOYSTICK_MOUSE_DEADZONE) then
                {
                if assigned(MouseRepeatThread) then
                    {
                    MouseRepeatThreadFlag = false;
        MouseRepeatThread = nil;
            }
            }
                else if not assigned(MouseRepeatThread) then
                    {
                    MouseRepeatThreadFlag = true;
        MouseRepeatThread = SDL_CreateThread(@MouseRepeatHandlerFunc, nil, Self);
            }
            }

            function TJoyController.HandleControllerDPad(ControllerId: integer; PadId: integer; X, Y: integer) : boolean;
            var
                Index : integer;
        State: TControllerDPadState;
        Key: TSDL_KeyCode;
            {
                Result = false;

        State = Default(TControllerDPadState);
            if not DPadStates.Find(PadId, index) then DPadStates.Add(PadId, State);
        State = DPadStates.Data[index];

            if (X < > 0) xor State.X then
                {
                if TranslateDPadToKey(PadId, ifthen(State.RawX + X > 0, SDL_HAT_AXIS_X_pos, SDL_HAT_AXIS_X_neg), X < > 0, Key) then
                    SimulateKeyboard(Key, X < > 0);
            }
            if (Y < > 0) xor State.Y then
                {
                if TranslateDPadToKey(PadId, ifthen(State.RawY + Y > 0, SDL_HAT_AXIS_Y_pos, SDL_HAT_AXIS_Y_neg), Y < > 0, Key) then
                    SimulateKeyboard(Key, Y < > 0);
            }

            State.X = X < > 0;
            State.Y = Y < > 0;
            State.RawX = ifthen(State.X, X, 0);
            State.RawY = ifthen(State.Y, Y, 0);
            DPadStates.Data[index] = State;
        Result = true;
            }

            function TJoyController.HandleControllerMotion(ControllerId: integer; Axis: integer; Perc: double; Time: Cardinal) : boolean;
            var
                Index : integer;
        State: TControllerAxisState;
        MouseAxis: Byte;
        Key: TSDL_KeyCode;
            {
                Result = false;

        State = Default(TControllerAxisState);
            if not AxesStates.Find(Axis, index) then AxesStates.Add(Axis, State);
        State = AxesStates.Data[index];

            if TranslateAxisToMouseAxis(Axis, Sign(Perc), MouseAxis) then {
                SimulateMouse(MouseAxis, Perc);
            }
            else if TranslateAxisToKey(Axis, Sign(Perc), Key) then
                {
                if not State.Pressed and not State.Repeat_ then
                    {
                    if (abs(Perc) > JOYSTICK_AXIS_PRESSED_THRESHOLD) then
                        {
                        State.Pressed = true;
            State.Time = Time;
            SimulateKeyboard(Key, true);
            }
            }
                    else if not State.Repeat_ then
                        {
                        if (abs(Perc) < JOYSTICK_AXIS_PRESSED_THRESHOLD) then {
                            State.Pressed = false;
            if not State.WasRepeat then SimulateKeyboard(Key, false);
            State.WasRepeat = false;
            } else if (abs(Perc) > JOYSTICK_AXIS_REPEAT_THRESHOLD) then {
                State.Repeat_ = true;

            if (Time - State.Time > JOYSTICK_AXIS_REPEAT_TIME) then {
                State.Pressed = true;
            SimulateKeyboard(Key, true);
            }
            }
            } else if /*State.Pressed and*/ (abs(Perc) < JOYSTICK_AXIS_RELEASED_THRESHOLD) then
                {
                State.Repeat_ = false;
            State.WasRepeat = true;
            if State.Pressed then SimulateKeyboard(Key, false);
            }
            }

            State.Perc = Perc;
            AxesStates.Data[index] = State;
        Result = true;
            }

            function TJoyController.HandleControllerButton(ButtonId: integer; State: TJoyButtonState) : boolean;
            var
                Key : TSDL_KeyCode;
        Button: integer;
            {
                Result = false;

            SwitchMouseModeFor(ButtonId, State);
            if MouseMode then
                {
                if TranslateButtonToMouse(ButtonId, State, Button) then {
                    Result = SimulateMouse(Button, State = bsPressed);
            } else if TranslateButtonToKey(ButtonId, State, Key) then {
                Result = SimulateKeyboard(Key, State = bsPressed, true);
            }
            } else if TranslateButtonToKey(ButtonId, State, Key) then {
                Result = SimulateKeyboard(Key, State = bsPressed);
            }
            }

            function TJoyController.GetControllerButtonCount() : integer;
            {
                Result = SDL_JoystickNumButtons(GetJoystick());
            }

            function TJoyController.GetControllerAxesCount() : integer;
            {
                Result = SDL_JoystickNumAxes(GetJoystick());
            }

            function TJoyController.GetControllerHatsCount() : integer;
            {
                Result = SDL_JoystickNumHats(GetJoystick());
            }

            function TJoyController.TranslateAxisToKey(Axis: integer; Direction: integer; out Key : TSDL_KeyCode) : boolean;
            {
                Result = false;
            }

            function TJoyController.TranslateAxisToMouseAxis(Axis: integer; Direction: integer; out MouseAxis : byte) : boolean;
            {
                Result = false;
            }

            function TJoyController.TranslateDPadToKey(Hat: integer; Axis: integer; Positive: boolean; out Key : TSDL_KeyCode) : boolean;
            {
                Result = false;
            }
            function TJoyController.TranslateButtonToKey(ButtonId: integer; State: TJoyButtonState; out Key : TSDL_KeyCode) : boolean;
            {
                Result = false;
            }
            function TJoyController.TranslateButtonToMouse(ButtonId: integer; State: TJoyButtonState; out Button : integer) : boolean;
            {
                Result = false;
            }

            void TJoyController.OnMouseMove(X, Y: integer);
            {
                LastMouseState.X = X;
            LastMouseState.Y = Y;
            //LastMouseState.DeltaX := 0.0;
            //LastMouseState.DeltaY := 0.0;
            LastMouseState.Time = SDL_GetTicks();
            LastMouseState.IsSet = true;
            }

            constructor TJoyControllerJoyStick.Create(DeviceId: integer; JS: PSDL_Joystick; Name: string);
            {
                inherited
                Create(DeviceId, Name);

        _Type = ctJoystick;
            self.JoyStick = JS;
            }

            destructor TJoyControllerJoyStick.Destroy();
            {
                inherited;

            if assigned(JoyStick) then
                {
                SDL_JoystickClose(JoyStick);
        JoyStick = nil;
            }
            }

            function TJoyControllerJoyStick.GetJoystick() : Pointer;
            {
                Result = JoyStick;
            }

            constructor TJoyControllerGameController.Create(DeviceId: integer; GC: PSDL_GameController; Name: string);
            {
                inherited
                Create(DeviceId, Name);

        _Type = ctGameController;
            self.GameController = GC;
            }

            destructor TJoyControllerGameController.Destroy();
            {
                inherited;

            if assigned(GameController) then
                {
                SDL_GameControllerClose(GameController);
        GameController = nil;
            }
            }

            function TJoyControllerGameController.GetJoystick() : Pointer;
            {
                Result = SDL_GameControllerGetJoystick(GameController);
            }

            function TJoyControllerGameController.ShouldIgnoreLegacy() : boolean;
            {
                Result = true;
            }


            // Helper 
            // ~~~~~~~~~~~~~~~~~~~~~~~

            function ifthen(val:boolean; const iftrue : TJoyButtonState; const iffalse : TJoyButtonState = bsReleased) : TJoyButtonState;
            {
                if val then result = iftrue else result = iffalse;
            }

            function ifthen(val:boolean; const iftrue : TSDL_KeyCode; const iffalse : TSDL_KeyCode = 0) : TSDL_KeyCode;
            {
                if val then result = iftrue else result = iffalse;
            }

            // Key mapping 
            // ~~~~~~~~~~~~~~~~~~~~~~~

            function TJoyControllerJoyStick.TranslateAxisToKey(Axis: integer; Direction: integer; out Key : TSDL_KeyCode) : boolean;
            {
                Result = false;
            }

            function TJoyControllerJoyStick.TranslateAxisToMouseAxis(Axis: integer; Direction: integer; out MouseAxis : byte) : boolean;
            {
                Result = true;
            case Axis of
            0, 1: MouseAxis = Axis;
                otherwise Result = false;
                }
                }

                function TJoyControllerJoyStick.TranslateDPadToKey(Hat: integer; Axis: integer; Positive: boolean; out Key : TSDL_KeyCode) : boolean;
                {
                    Result = false;

                if Hat = 0 then
                    {
                    case Axis of
                    SDL_HAT_AXIS_X_pos : Key = SDLK_RIGHT;
            SDL_HAT_AXIS_X_neg: Key = SDLK_LEFT;
            SDL_HAT_AXIS_Y_pos: Key = SDLK_UP;
            SDL_HAT_AXIS_Y_neg: Key = SDLK_DOWN;
                otherwise Exit;
                }
            Result = true;
                }
                else if Hat = 1 then
                    {
                    case Axis of
                    SDL_HAT_AXIS_X_pos : Key = SDLK_KP_6;
            SDL_HAT_AXIS_X_neg: Key = SDLK_KP_4;
            SDL_HAT_AXIS_Y_pos: Key = SDLK_KP_8;
            SDL_HAT_AXIS_Y_neg: Key = SDLK_KP_2;
            SDL_HAT_AXIS_X_pos_Y_neg: Key = SDLK_KP_3;
            SDL_HAT_AXIS_X_pos_Y_pos: Key = SDLK_KP_9;
            SDL_HAT_AXIS_Y_neg_X_neg: Key = SDLK_KP_7;
            SDL_HAT_AXIS_Y_pos_X_neg: Key = SDLK_KP_1;

                // TODO: consider invalid buttons presses as NumPad5?
                //SDL_HAT_AXIS_X, SDL_HAT_AXIS_Y: Key := SDLK_KP_5;
                otherwise Exit;
                }
            Result = true;
                }
                }

                function TJoyControllerJoyStick.TranslateButtonToMouse(ButtonId: integer; State: TJoyButtonState; out Button : integer) : boolean;
                {
                    Result = true;

                case ButtonId of
                0: Button = SDL_BUTTON_LEFT;
                    1: Button = SDL_BUTTON_RIGHT;

                    // TODO: Mouse wheel

                    otherwise Result = false;
                    }
                    }

                    function TJoyControllerJoyStick.TranslateButtonToKey(ButtonId: integer; State: TJoyButtonState; out Key : TSDL_KeyCode) : boolean;
                    {
                        Result = true;

                        // default configuration with 4 buttons
                        case ButtonId of
                        0: Key = SDLK_RETURN;
                            1: Key = SDLK_ESCAPE;
                            2: Key = SDLK_m;
                            3: Key = SDLK_r;
                            otherwise Result = false;
                            }
                            }

                            void TJoyControllerJoyStick.SwitchMouseModeFor(ButtonId: integer; State: TJoyButtonState);
                            {
                                // no need to do anything in here.
                                // whether we should switching Mosue mode is handled in DPad tracking, see SimulateKeyboard SimulateMouse
                                // this method is required for controllers sending the digital pad buttons as Button presses (and not as Hat motion)
                                }

                            function TJoyControllerGameController.TranslateAxisToKey(Axis: integer; Direction: integer; out Key : TSDL_KeyCode) : boolean;
                            {

                                Result = false;

                            if Direction > 0 then
                                {
                                case Axis of
                                SDL_CONTROLLER_AXIS_RIGHTX : Key = SDLK_RIGHT;
                        SDL_CONTROLLER_AXIS_RIGHTY: Key = SDLK_UP;
                            otherwise Exit;
                            }
                        Result = true;
                            }

                            else if Direction < 0 then
                                {
                                case Axis of
                                SDL_CONTROLLER_AXIS_RIGHTX : Key = SDLK_LEFT;
                        SDL_CONTROLLER_AXIS_RIGHTY: Key = SDLK_DOWN;
                            otherwise Exit;
                            }
                        Result = true;
                            }
                            }

                            function TJoyControllerGameController.TranslateAxisToMouseAxis(Axis: integer; Direction: integer; out MouseAxis : byte) : boolean;
                            {
                                Result = false;

                            case Axis of
                                SDL_CONTROLLER_AXIS_LEFTX : MouseAxis = 0;
                            SDL_CONTROLLER_AXIS_LEFTY: MouseAxis = 1;
                                otherwise Exit;
                                }
                            Result = true;
                                }

                                function TJoyControllerGameController.TranslateDPadToKey(Hat: integer; Axis: integer; Positive: boolean; out Key : TSDL_KeyCode) : boolean;
                                {
                                    Result = false;
                                }

                                function TJoyControllerGameController.TranslateButtonToMouse(ButtonId: integer; State: TJoyButtonState; out Button : integer) : boolean;
                                {
                                    Result = true;

                                case ButtonId of
                                    SDL_CONTROLLER_BUTTON_A : Button = SDL_BUTTON_LEFT;
                                SDL_CONTROLLER_BUTTON_B: Button = SDL_BUTTON_RIGHT;
                                    SDL_CONTROLLER_BUTTON_LEFTSTICK,
                                        SDL_CONTROLLER_BUTTON_RIGHTSTICK: Button = SDL_BUTTON_MIDDLE;

                                    // TODO: Mouse wheel
                                    // Button := SDL_BUTTON_WHEELUP

                                    otherwise Result = false;
                                    }
                                    }

                                    function TJoyControllerGameController.TranslateButtonToKey(ButtonId: integer; State: TJoyButtonState; out Key : TSDL_KeyCode) : boolean;
                                    {

                                        Result = true;
                                    case ButtonId of
                                        SDL_CONTROLLER_BUTTON_DPAD_UP : Key = SDLK_UP;
                                    SDL_CONTROLLER_BUTTON_DPAD_DOWN: Key = SDLK_DOWN;
                                    SDL_CONTROLLER_BUTTON_DPAD_LEFT: Key = SDLK_LEFT;
                                    SDL_CONTROLLER_BUTTON_DPAD_RIGHT: Key = SDLK_RIGHT;
                                        otherwise Result = false;
                                        }

                                    // for mouse mode switching
                                    if Result then
                                        {
                                        MouseMode = false;
                                    Exit;
                                    }

                                Result = true;
                                    if MouseMode then
                                        {
                                        case ButtonId of
                                        SDL_CONTROLLER_BUTTON_START : Key = SDLK_RETURN;
                                SDL_CONTROLLER_BUTTON_BACK: Key = SDLK_ESCAPE;
                                    otherwise Result = false;
                                    }
                                    }
                                    else
                                        {
                                        case ButtonId of
                                        SDL_CONTROLLER_BUTTON_A : Key = SDLK_RETURN;
                                SDL_CONTROLLER_BUTTON_B: Key = SDLK_ESCAPE;
                                SDL_CONTROLLER_BUTTON_Y: Key = SDLK_m;
                                SDL_CONTROLLER_BUTTON_X: Key = SDLK_r;
                                    otherwise Result = false;
                                    }
                                    }
                                    }

                                    void TJoyControllerGameController.SwitchMouseModeFor(ButtonId: integer; State: TJoyButtonState);
                                    {
                                        if MouseMode then
                                            {
                                            case ButtonId of
                                            SDL_CONTROLLER_BUTTON_DPAD_UP,
                                            SDL_CONTROLLER_BUTTON_DPAD_DOWN,
                                            SDL_CONTROLLER_BUTTON_DPAD_LEFT,
                                            SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                                            MouseMode = false;
                                                }
                                                }
        }
    }