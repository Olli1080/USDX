#include "UDisplay.h"

#include <chrono>

#include "../base/UIni.h"

namespace UDisplay
{

    /*uses
      TextGL,
      StrUtils,
      UCommandLine,
      UGraphic,
      UIni,
      UImage,
      ULog,
      UMain,
      UTexture,
      UTime,
      ULanguage,
      UPathUtils;*/

    TDisplay::TDisplay()
    {
        inherited Create;

        // create events for plugins
        ePreDraw = THookableEvent("Display.PreDraw");
        eDraw = THookableEvent("Display.Draw");

        // init popup
        CheckOK = false;
        NextScreen = nullptr;
        NextScreenWithCheck = nullptr;
        BlackScreen = false;

        // init fade
        FadeStartTime = 0;
        FadeEnabled = UIni::Ini.ScreenFade == 1;
        FadeFailed = false;
        DoneOnShow = false;

        glGenTextures(2, FadeTex.data());
        // TODO:: restore SupportsNPOT = (AnsiContainsStr(glGetString(GL_EXTENSIONS), "texture_non_power_of_two")) and not (AnsiContainsStr(glGetString(GL_EXTENSIONS), "Radeon X16"));
        InitFadeTextures();

        // set LastError for OSD to No Error
        OSD_LastError = "No Errors";

        // software cursor default values
        Cursor_LastMove = std::chrono::system_clock::time_point{}; //TODO:: check if 0
        Cursor_Visible = false;
        Cursor_Pressed = false;
        Cursor_X = -1;
        Cursor_Y = -1;
        Cursor_Fade = false;
        Cursor_HiddenByScreen = true;
        Cursor_Update = false;
    }

    TDisplay::~TDisplay()
    {
        glDeleteTextures(2, FadeTex.data());
        // inherited Destroy;
    }

    void TDisplay::InitFadeTextures()
    {
        if (!SupportsNPOT)
        {
            TexW = std::round(std::pow(2, std::ceil(std::log2(ScreenW / Screens))));
            TexH = std::round(std::pow(2, std::ceil(std::log2(ScreenH))));
        }
        else
        {
            TexW = ScreenW / Screens;
            TexH = ScreenH;
        }
#pragma unroll
        for (size_t i = 0; i < 2; ++i)
        {
            glBindTexture(GL_TEXTURE_2D, FadeTex[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, TexW, TexH, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        }
    }

    bool TDisplay::Draw()
    /*var
        S : integer;
FadeStateSquare: real;
    FadeW, FadeH:    real;
    FadeCopyW, FadeCopyH: integer;
glError:         glEnum;
*/
    {
        bool Result = true;

        for (size_t S = 1; S <= Screens; ++S)
        {
            ScreenAct = S;

            // if Screens = 1 then ScreenX := 0;
            // if (Screens = 2) and (S = 1) then ScreenX := -1;
            // if (Screens = 2) and (S = 2) then ScreenX := 1;
            ScreenX = 0;

            glViewPort((S - 1) * ScreenW div Screens, 0, ScreenW div Screens, ScreenH);

            // popup check was successful... move on
            if (CheckOK)
            {
                if (NextScreenWithCheck)
                {
                    NextScreen = NextScreenWithCheck;
                    NextScreenWithCheck = nullptr;
                    CheckOk = false;
                }
                else
                {
                    // on end of game fade to black before exit
                    BlackScreen = true;
                }
            }

            if (!NextScreen && !BlackScreen)
            {
                ePreDraw.CallHookChain(false);
                CurrentScreen.Draw();

                // popup
                if (ScreenPopupError && ScreenPopupError.Visible)
                    ScreenPopupError.Draw();
                else if (ScreenPopupInfo && ScreenPopupInfo.Visible)
                    ScreenPopupInfo.Draw();
                else if (ScreenPopupCheck && ScreenPopupCheck.Visible)
                    ScreenPopupCheck.Draw();
                else if (ScreenPopupInsertUser && ScreenPopupInsertUser.Visible)
                    ScreenPopupInsertUser.Draw();
                else if (ScreenPopupSendScore && ScreenPopupSendScore.Visible)
                    ScreenPopupSendScore.Draw();
                else if (ScreenPopupScoreDownload && ScreenPopupScoreDownload.Visible)
                    ScreenPopupScoreDownload.Draw();
                else if (ScreenPopupHelp && ScreenPopupHelp.Visible)
                    ScreenPopupHelp.Draw();

                // fade
                FadeStartTime = 0;
                if (UIni::Ini.ScreenFade == 1 && !FadeFailed)
                    FadeEnabled = true;
                else
                    FadeEnabled = false;

                eDraw.CallHookChain(false);
            }
            else
            {
                // disable fading if initialization failed
                if (FadeFailed)
                    FadeEnabled = false;

                if (FadeEnabled)
                {
                    // create fading texture if we're just starting
                    if (FadeStartTime == 0)
                    {
                        // draw screen that will be faded
                        ePreDraw.CallHookChain(false);
                        CurrentScreen.Draw;
                        eDraw.CallHookChain(false);

                        // clear OpenGL errors, otherwise fading might be disabled due to some
                        // older errors in previous OpenGL calls.
                        glGetError();

                        FadeCopyW = ScreenW / Screens;
                        FadeCopyH = ScreenH;

                        // it is possible that our fade textures are too small after a window
                        // resize. In that case resize the fade texture to fit the requirements.
                        if (TexW < FadeCopyW || TexH < FadeCopyH)
                            InitFadeTextures();

                        // copy screen to texture
                        glBindTexture(GL_TEXTURE_2D, FadeTex[S - 1]);
                        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (S - 1) * ScreenW / Screens, 0,
                                            FadeCopyW, FadeCopyH);

                        glError = glGetError();
                        if (glError != GL_NO_ERROR)
                        {
                            FadeFailed = true;
                            Log.LogError("Fading disabled: $" + IntToHex(glError, 4), "TDisplay::Draw");
                        }

                        if (!BlackScreen && S == 1 && !DoneOnShow)
                        {
                            NextScreen.OnShow;
                            DoneOnShow = true;
                        }

                        // set fade time once on second screen (or first if screens = 1)
                        if (Screens == 1 || S == 2)
                            FadeStartTime = SDL_GetTicks;
                    } // end texture creation in first fading step

                    if (!BlackScreen)
                    {
                        ePreDraw.CallHookChain(false);
                        NextScreen.Draw; // draw next screen
                        eDraw.CallHookChain(false);
                    }
                    else if (ScreenAct == 1)
                    {
                        // draw black screen
                        glClearColor(0, 0, 0, 1);
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    }

                    // and draw old screen over it... slowly fading out
                    if (FadeStartTime == 0)
                        FadeStateSquare = 0; // for first screen if screens = 2
                    else
                        FadeStateSquare = sqr((SDL_GetTicks - FadeStartTime) / FADE_DURATION);

                    if (FadeStateSquare < 1)
                    {
                        FadeW = (ScreenW / Screens) / TexW;
                        FadeH = ScreenH / TexH;

                        glBindTexture(GL_TEXTURE_2D, FadeTex[S - 1]);
                        // TODO: check if glTexEnvi() gives any speed improvement
                        // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                        glColor4f(1, 1, 1, 1 - FadeStateSquare);

                        glEnable(GL_TEXTURE_2D);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        glEnable(GL_BLEND);
                        glBegin(GL_QUADS);
                        glTexCoord2f((0 + FadeStateSquare / 2) * FadeW, (0 + FadeStateSquare / 2) * FadeH);
                        glVertex2f(0, RenderH);

                        glTexCoord2f((0 + FadeStateSquare / 2) * FadeW, (1 - FadeStateSquare / 2) * FadeH);
                        glVertex2f(0, 0);

                        glTexCoord2f((1 - FadeStateSquare / 2) * FadeW, (1 - FadeStateSquare / 2) * FadeH);
                        glVertex2f(RenderW, 0);

                        glTexCoord2f((1 - FadeStateSquare / 2) * FadeW, (0 + FadeStateSquare / 2) * FadeH);
                        glVertex2f(RenderW, RenderH);
                        glEnd();
                        glDisable(GL_BLEND);
                        glDisable(GL_TEXTURE_2D);

                        // reset to default
                        // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                    }
                }

                // there is no need to init next screen if it is a black screen
                else if (!BlackScreen)
                {
                    NextScreen.OnShow();
                }

                if ((FadeStartTime + FADE_DURATION < SDL_GetTicks) || !FadeEnabled || FadeFailed && (Screens == 1 || S == 2))
                {
                    // fade out complete...
                    FadeStartTime = 0;
                    DoneOnShow = false;
                    CurrentScreen.onHide();
                    CurrentScreen.ShowFinish = false;
                    CurrentScreen = NextScreen;
                    NextScreen = nullptr;
                    if (!BlackScreen)
                    {
                        CurrentScreen.OnShowFinish;
                        CurrentScreen.ShowFinish = true;
                        Cursor_Update = true;
                    }
                    else
                    {
                        return false;
                    }
                }
            } // if

            // Draw OSD only on first Screen if Debug Mode is enabled
            if ((UIni::Ini.Debug == 1 || Params.Debug) && S == 1)
            {
                DrawDebugInformation();
                if (Console_Draw)
                    DrawDebugConsole();
            }
            else if (Console_Draw)
            {
                // clear flag to prevent drawing console when toggling debug
                // TODO: considers using event in ScreenOptions for clearing debug
                Console_Draw = false;
            }

            if (!BlackScreen)
                DrawCursor();
        } // for
    }

    // sets SDL_ShowCursor depending on options set in Ini
    void TDisplay::SetCursor()
    {
        int Cursor = 0;

        if (CurrentScreen != @ScreenSing || Cursor_HiddenByScreen)
        { // hide cursor on singscreen
            if (UIni::Ini.Mouse == UIni::MouseMode::OFF && UIni::Ini.FullScreen == 0)
                // show sdl (os) cursor in window mode even when mouse support is off
                Cursor = SDL_ENABLE;
            else if (UIni::Ini.Mouse == UIni::MouseMode::SYSTEM)
                // show sdl (os) cursor when hardware cursor is selected
                Cursor = SDL_ENABLE;

            if (UIni::Ini.Mouse != UIni::MouseMode::GAME)
                Cursor_HiddenByScreen = false;
        }
        else if (UIni::Ini.Mouse != UIni::MouseMode::GAME)
            Cursor_HiddenByScreen = true;

        SDL_ShowCursor(Cursor);

        if (UIni::Ini.Mouse == UIni::MouseMode::GAME)
        {
            if (Cursor_HiddenByScreen)
            {
                // show software cursor
                Cursor_HiddenByScreen = false;
                Cursor_Visible = false;
                Cursor_Fade = false;
            }
            else if (CurrentScreen == @ScreenSing)
            {
                // hide software cursor in singscreen
                Cursor_HiddenByScreen = true;
                Cursor_Visible = false;
                Cursor_Fade = false;
            }
        }
    }

    // called by MoveCursor and OnMouseButton to update last move and start fade in
    void TDisplay::UpdateCursorFade()
    {
        auto Ticks = std::chrono::system_clock::now();

        // fade in on movement(or button press) if not first movement
        if (!Cursor_Visible && Cursor_LastMove != std::chrono::system_clock::time_point{})
        {
            Cursor_LastMove = Ticks;
            if (Cursor_Fade) // we use a trick here to consider progress of fade out
            {
                auto progress = 1.0 - std::chrono::duration<double>(Ticks - Cursor_LastMove) / std::chrono::duration<double>(CURSOR_FADE_OUT_TIME);
                Cursor_LastMove -= std::chrono::duration_cast<std::chrono::system_clock::duration>(CURSOR_FADE_IN_TIME * progress);
            }
            Cursor_Visible = true;
            Cursor_Fade = true;
        }
        else if (!Cursor_Fade)
        {
            Cursor_LastMove = Ticks;
        }
    }

    // called when cursor moves, positioning of software cursor
    void TDisplay::MoveCursor(double X, double Y)
    {
        if (UIni::Ini.Mouse != UIni::MouseMode::GAME)
            return;

        if (X != Cursor_X || Y != Cursor_Y)
        {
            Cursor_X = X;
            Cursor_Y = Y;

            UpdateCursorFade();
        }
    }

    // called when left or right mousebutton is pressed or released
    void TDisplay::OnMouseButton(bool Pressed)
    {
        if (UIni::Ini.Mouse != UIni::MouseMode::GAME)
            return;
        
        Cursor_Pressed = Pressed;
        UpdateCursorFade();
    }

    // draws software cursor
    void TDisplay::DrawCursor()
    // var
    //   Alpha : single;
    // Ticks: cardinal;
    // DrawX: double;
    {
        if (UIni::Ini.Mouse != UIni::MouseMode::GAME)
            return;

        if (Screens == 1 || (ScreenAct - 1 == std::round(Cursor_X + 16) / RenderW))
        { // draw software cursor
            auto Ticks = std::chrono::system_clock::now();
            auto dt = Ticks - Cursor_LastMove;

            if (Cursor_Visible && dt >= CURSOR_AUTOHIDE_TIME)
            { // start fade out after 5 secs w/o activity
                Cursor_Visible = false;
                Cursor_LastMove = Ticks;
                Cursor_Fade = true;
            }

            double Alpha;
            // fading
            if (Cursor_Fade)
            {
                if (Cursor_Visible)
                { // fade in
                    if (dt >= CURSOR_FADE_IN_TIME)
                        Cursor_Fade = false;
                    else
                        Alpha = std::sin(dt * 0.5 * std::numbers::pi / CURSOR_FADE_IN_TIME) * CURSOR_ALPHA_VISIBLE;
                }
                else
                { // fade out
                    if (dt >= CURSOR_FADE_OUT_TIME)
                        Cursor_Fade = false;
                    else
                        Alpha = std::cos(dt * 0.5 * std::numbers::pi / CURSOR_FADE_OUT_TIME) * CURSOR_ALPHA_VISIBLE;
                }
            }

            // no else if here because we may turn off fade in if block
            if (!Cursor_Fade)
            {
                Alpha = (Cursor_Visible)
                            ? CURSOR_ALPHA_VISIBLE // alpha when cursor visible and not fading
                            : 0.0;                 // alpha when cursor is hidden
            }

            if (Alpha > 0 && !Cursor_HiddenByScreen)
            {
                double DrawX = Cursor_X;
                if (ScreenAct == 2)
                    DrawX -= RenderW;
                glColor4f(1, 1, 1, Alpha);
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_BLEND);
                glDisable(GL_DEPTH_TEST);

                if (Cursor_Pressed && Tex_Cursor_Pressed.TexNum > 0)
                    glBindTexture(GL_TEXTURE_2D, Tex_Cursor_Pressed.TexNum);
                else
                    glBindTexture(GL_TEXTURE_2D, Tex_Cursor_Unpressed.TexNum);

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex2f(DrawX, Cursor_Y);

                glTexCoord2f(0, 1);
                glVertex2f(DrawX, Cursor_Y + 32);

                glTexCoord2f(1, 1);
                glVertex2f(DrawX + 32, Cursor_Y + 32);

                glTexCoord2f(1, 0);
                glVertex2f(DrawX + 32, Cursor_Y);
                glEnd();

                glDisable(GL_BLEND);
                glDisable(GL_TEXTURE_2D);
            }
        }
    }

    bool TDisplay::ShouldHandleInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown, bool &SuppressKey)
    {
        if (Console_Draw)
        {
            SuppressKey = true;
            return true;
        }

        if (NextScreen)
            return NextScreen->ShouldHandleInput(PressedKey, CharCode, PressedDown, SuppressKey);
        else if (CurrentScreen)
            return CurrentScreen->ShouldHandleInput(PressedKey, CharCode, PressedDown, SuppressKey);
        else
            return true;
    }

    bool TDisplay::ParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown)
    {
        if (Console_Draw && ConsoleParseInput(PressedKey, CharCode, PressedDown))
            return false;

        if (NextScreen)
            return NextScreen->ParseInput(PressedKey, CharCode, PressedDown);
        else if (CurrentScreen)
            return CurrentScreen->ParseInput(PressedKey, CharCode, PressedDown);
        else
            return true;
    }

    bool TDisplay::ParseMouse(int MouseButton, bool BtnDown, int X, int Y)
    {
        if (Console_Draw && ConsoleParseMouse(MouseButton, BtnDown, X, Y))
            return false;

        if (NextScreen)
            return NextScreen->ParseMouse(MouseButton, BtnDown, X, Y);
        else if (CurrentScreen)
            return CurrentScreen->ParseMouse(MouseButton, BtnDown, X, Y);
        else
            return true;
    }

    // abort fading to the next screen, may be used in OnShow, or during fade process
    void TDisplay::AbortScreenChange()
    // var
    //   Temp : PMenu;
    {
        // this is some kind of "hack" it is based on the
        // code that is used to change the screens in TDisplay::Draw
        // we should rewrite this whole behaviour, as it is not well
        // structured and not well extendable. Also we should offer
        // a possibility to change screens to plugins
        // change this code when restructuring is done
        if (NextScreen)
        {
            // we have to swap the screens
            std::swap(CurrentScreen, NextScreen);

            // and call the OnShow void of the previous screen
            // because it was already called by default fade void
            NextScreen.OnShow();
        }
    }

    // fades to specific screen(playing specified sound) returns old screen
    PMenu TDisplay::FadeTo(PMenu Screen, const TAudioPlaybackStream aSound)
    {
        auto Result = CurrentScreen;
        if (Result)
        {
            if (aSound)
                Result.FadeTo(Screen, aSound);
            else
                Result.FadeTo(Screen);
        }
        return Result;
    }

    void TDisplay::UpdateCursor()
    {
        UpdateCursorFade();
        Cursor_Update = true;
    }

    bool TDisplay::NeedsCursorUpdate()
    {
        bool Result = Cursor_Update && Cursor_Visible && !Cursor_Fade;
        Cursor_Update = false;
        return Result;
    }

    void TDisplay::OnWindowResized()
    {
        // update cursor position once the window has been resized, otherwise the cursor jumps
        Cursor_Update = true;

        if (NextScreen)
            NextScreen->OnWindowResized();
        else if (CurrentScreen)
            CurrentScreen->OnWindowResized();
    }

    void TDisplay::SaveScreenShot()
    // var
    //   Num : integer;
    // FileName : IPath;
    // Prefix : UTF8String;
    // ScreenData : PChar;
    // Surface : PSDL_Surface;
    // Success : boolean;
    // Align : integer;
    // RowSize : integer;
    {
        // Exit if Screenshot-path does not exist or read-only
        if (ScreenshotsPath.IsUnset)
            return;

        //TODO:: use datetime instead ^^
        for (Num = 1 to 9999)
        {
            // fill prefix to 4 digits with leading "0", e.g. "0001"
            Prefix = Format("screenshot%.4d", [Num]);
            FileName = ScreenshotsPath.Append(Prefix + ".jpg");
            if (!FileName.Exists())
                break;
        }

        // we must take the row-alignment (4byte by default) into account
        glGetIntegerv(GL_PACK_ALIGNMENT, @Align);
        // calc aligned row-size
        RowSize = ((ScreenW * 3 + (Align - 1)) / Align) * Align;

        GetMem(ScreenData, RowSize * ScreenH);
        glReadPixels(0, 0, ScreenW, ScreenH, GL_RGB, GL_UNSIGNED_BYTE, ScreenData);
        // on big endian machines (powerpc) this may need to be changed to
        // Needs to be tests. KaMiSchi Sept 2008
        // in this case one may have to add " glext, " to the list of used units
        //  glReadPixels(0, 0, ScreenW, ScreenH, GL_BGR, GL_UNSIGNED_BYTE, ScreenData);
        Surface = SDL_CreateRGBSurfaceFrom(ScreenData, ScreenW, ScreenH, 24, RowSize, 0x0000FF, 0x00FF00, 0xFF0000, 0);

        Success = WriteJPGImage(FileName, Surface, 95);
        //  Success := WriteBMPImage(FileName, Surface);
        // Success := WritePNGImage(FileName, Surface);
        if (Success)
            ScreenPopupInfo.ShowPopup(Format(Language.Translate("SCREENSHOT_SAVED"), [FileName.GetName.ToUTF8()])); 
        else 
            ScreenPopupError.ShowPopup(Language.Translate("SCREENSHOT_FAILED"));

        SDL_FreeSurface(Surface);
        FreeMem(ScreenData);
    }

    //------------
    // DrawDebugInformation - void draw fps and some other informations on screen
    //------------
    void TDisplay::DrawDebugInformation()
    // var
    //   Ticks : cardinal;
    {
        // Some White Background for information
        glEnable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glColor4f(1, 1, 1, 0.5);
        glBegin(GL_QUADS);
        glVertex2f(690, 35);
        glVertex2f(690, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 35);
        glEnd();
        glDisable(GL_BLEND);

        // set font specs
        SetFontFamily(0);
        SetFontStyle(ftRegular);
        SetFontSize(21);
        SetFontItalic(false);
        glColor4f(0, 0, 0, 1);

        // calculate fps
        Ticks = SDL_GetTicks();
        if (Ticks >= NextFPSSwap)
        {
            LastFPS = FPSCounter * 4;
            FPSCounter = 0;
            NextFPSSwap = Ticks + 250;
        }

        ++FPSCounter;

        // draw text

        // fps
        SetFontPos(695, 0);
        glPrint("FPS: " + InttoStr(LastFPS));

        // muffins
        SetFontPos(695, 13);
        glColor4f(0.8, 0.5, 0.2, 1);
        glPrint("Muffins!");

        glColor4f(1, 1, 1, 1);
    }

    void TDisplay::ToggleConsole()
    {
        Console_Draw = !Console_Draw;
        Console_ScrollOffset = 0;
    }

    bool TDisplay::ConsoleParseInput(uint32_t PressedKey, char32_t CharCode, bool PressedDown)
    {
        if (!PressedDown)
            return false;

        // Key Down
        int SDL_ModState = SDL_GetModState() & (KMOD_LSHIFT | KMOD_RSHIFT | KMOD_LCTRL | KMOD_RCTRL | KMOD_LALT | KMOD_RALT);

        switch (PressedKey)
        {
        case SDLK_PAGEUP:
            Console_ScrollOffset = std::min(Console_ScrollOffset + 3, ULog::Log.ConsoleCount - 1);
            break;
        case SDLK_PAGEDOWN:
            Console_ScrollOffset = std::max(Console_ScrollOffset - 3, 0);
            break;
        case SDLK_HOME:
            Console_ScrollOffset = ULog::Log.ConsoleCount - 1;
            break;
        case SDLK_END:
            Console_ScrollOffset = 0;
            break;
        case SDLK_DELETE:
            if (SDL_ModState && KMOD_CTRL != 0)
                ULog::Log.ClearConsoleLog();
            break;
        default:
            return false;
        }
        return true;
    }

    bool TDisplay::ConsoleParseMouse(int MouseButton, bool BtnDown, int X, int Y)
    {
        if (!BtnDown)
            return false;

        // button Down
        switch (MouseButton) // TODO:: https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent
        {
        case SDL_BUTTON_WHEELUP:
            Console_ScrollOffset = std::min(Console_ScrollOffset + 3, Log.ConsoleCount - 1);
            break;
        case SDL_BUTTON_WHEELDOWN:
            Console_ScrollOffset = std::max(Console_ScrollOffset - 3, 0);
            break;
        default:
            return false;
        }
        return true;
    }

    //------------
    // DrawDebugInformation - void draw fps and some other informations on screen
    //------------
    void TDisplay::DrawDebugConsole()
    // var
    //   I,
    //   LineCount : integer;
    // YOffset, ScaleF, FontSize : real;
    // PosX, PosY : real;
    // W, H : real;
    // ScrollPad, ScrollW : real;
    // OldStretch : real;
    {
        FontSize = 16.0;
        W = 800.0;
        H = 400.0;
        ScrollPad = 5.0;
        ScrollW = 10.0;

        // Some black background
        glEnable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glColor4f(0, 0, 0, 0.85);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(0, H);
        glVertex2f(W, H);
        glVertex2f(W, 0);
        glEnd();
        glDisable(GL_BLEND);

        // scale sizes to DPI/aspect
        ScaleF = (1.0 * ScreenH) / (1.0 * ScreenW);
        FontSize = FontSize * 600.0 / (1.0 * ScreenH);
        ScrollW = ScrollW * 800.0 / (1.0 * ScreenW);
        ScrollPad = ScrollPad * ScaleF;

        // set font specs
        SetFontFamily(0);
        SetFontStyle(ftRegular);
        SetFontSize(FontSize);
        SetFontItalic(false);
        SetFontReflection(false, 0);
        glColor4f(1, 1, 1, 1);

        OldStretch = Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Stretch;
        Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Stretch = 1.4 * ScaleF * std::min(1.3, std::max(0.8, std::pow((1.0 * ScreenW) / 800.0, 1.2)));

        // don"t draw anything else if nothing"s logged
        if (Log.ConsoleCount < 1)
            return;

        // draw log buffer
        YOffset = H; // start at bottom
        LineCount = 0;
        I = Log.ConsoleCount - 1 - Console_ScrollOffset;
        while (I >= 0 && YOffset > 0)
        {
            YOffset -= FontSize;
            SetFontPos(5, YOffset);
            glPrint(Log.GetConsole(i));

            --i;
            ++LineCount;
        }

        // draw scoll bar
        glEnable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glColor4f(0.33, 0.33, 0.33, 1);
        glBegin(GL_QUADS);
        glVertex2f(W - ScrollPad - ScrollW, ScrollPad);     // top left
        glVertex2f(W - ScrollPad, ScrollPad);               // top right
        glVertex2f(W - ScrollPad, H - ScrollPad);           // bottom right
        glVertex2f(W - ScrollPad - ScrollW, H - ScrollPad); // bottom left
        glEnd();

        // visible height bar + offset
        YOffset = H * ((1.0 * LineCount) / (1.0 * Log.ConsoleCount));
        PosY = 0;
        if (I > 0)
            PosY = (H - 2.0 * ScrollPad) * std::max(0.0, I) / (1.0 * Log.ConsoleCount);

        glColor4f(1, 1, 1, 1);
        glBegin(GL_QUADS);
        glVertex2f(W - ScrollPad - ScrollW, ScrollPad + PosY);           // top left
        glVertex2f(W - ScrollPad, ScrollPad + PosY);                     // top right
        glVertex2f(W - ScrollPad, ScrollPad + PosY + YOffset);           // bottom right
        glVertex2f(W - ScrollPad - ScrollW, ScrollPad + PosY + YOffset); // bottom left
        glEnd();
        glDisable(GL_BLEND);

        Fonts[CurrentFont.FontFamily][CurrentFont.FontStyle].Font.Stretch = OldStretch;
        glColor4f(1, 1, 1, 1);
    }
}