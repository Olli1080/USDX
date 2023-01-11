#include "UGraphic.h"

#include "UImage.h"
#include "TextGL.h"
#include "UAvatars.h"
#include "UCommandLine.h"
#include "ULog.h"
#include "UPathUtils.h"
#include "USongs.h"
#include "../menu/UDisplay.h"

namespace UGraphic
{

    /*uses
      Classes,
      UDisplay,
      UCommandLine,
      UPathUtils;*/

    void LoadFontTextures()
    {
        ULog::Log.LogStatusLocation("Building Fonts");
        TextGL::BuildFonts();
    }

    void UnloadFontTextures()
    {
        ULog::Log.LogStatusLocation("Kill Fonts");
        TextGL::KillFonts();
    }

    void LoadTextures()
        /*var
            P : int;
        R, G, B: double;
    Col:     int;*/
    {
        ULog::Log.LogStatus("Loading Textures", "LoadTextures");

        ULog::Log.LogStatus("Loading Textures - A", "LoadTextures");

        Tex_Note_Perfect_Star = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("NotePerfectStar"), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
        Tex_Note_Star = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("NoteStar"), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0xffffff);
        Tex_Ball = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("Ball"), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0xff00ff);
        Tex_Lyric_Help_Bar = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("LyricHelpBar"), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);

        Tex_SelectS_ArrowL = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("Select_ArrowLeft"), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
        Tex_SelectS_ArrowR = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("Select_ArrowRight"), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);

        Tex_Cursor_Unpressed = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("Cursor"), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);

        if (Skin.GetTextureFileName("Cursor_Pressed").IsSet)
            Tex_Cursor_Pressed = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("Cursor_Pressed"), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
        else
            Tex_Cursor_Pressed.TexNum = 0;

        //TimeBar mod
        Tex_TimeProgress = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("TimeBar"));
        Tex_JukeboxTimeProgress = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("JukeboxTimeBar"));
        //eoa TimeBar mod

        //SingBar Mod
        Tex_SingBar_Back = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("SingBarBack"), UTexture::TTextureType::TEXTURE_TYPE_PLAIN, 0);
        Tex_SingBar_Bar = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("SingBarBar"), UTexture::TTextureType::TEXTURE_TYPE_PLAIN, 0);
        Tex_SingBar_Front = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("SingBarFront"), UTexture::TTextureType::TEXTURE_TYPE_PLAIN, 0);
        //end Singbar Mod

        ULog::Log.LogStatus("Loading Textures - B", "LoadTextures");

        //Line Bonus PopUp
        for (size_t P = 0; P <= 8; ++P)
        {
            switch (P)
            {
            case 0:
            {
                R = 1;
                G = 0;
                B = 0;
            }
            case 1: [[nodiscard]]
            case 2: [[nodiscard]]
            case 3:
            {
                R = 1;
                G = (P * 0.25);
                B = 0;
            }
            case 4:
            {
                R = 1;
                G = 1;
                B = 0;
            }
            case 5: [[nodiscard]]
            case 6: [[nodiscard]]
            case 7:
            {
                R = 1 - ((P - 4) * 0.25);
                G = 1;
                B = 0;
            }
            case 8:
            {
                R = 0;
                G = 1;
                B = 0;
            }
            default:
            {
                R = 1;
                G = 0;
                B = 0;
            }
            }

            Col = 0x10000 * std::round(R * 255) + 0x100 * std::round(G * 255) + std::round(B * 255);
            Tex_SingLineBonusBack[P] = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("LineBonusBack"), UTexture::TTextureType::TEXTURE_TYPE_COLORIZED, Col);
        }

        ULog::Log.LogStatus("Loading Textures - C", "LoadTextures");

        //## rating pictures that show a picture according to your rate ##
        for (size_t P = 0; P < 8; ++P)
        {
            Tex_Score_Ratings[P] = UTexture::Texture.LoadTexture(Skin.GetTextureFileName("Rating_" + IntToStr(P)), UTexture::TTextureType::TEXTURE_TYPE_TRANSPARENT, 0);
        }

        ULog::Log.LogStatus("Loading Textures - Done", "LoadTextures");
    }

    const std::filesystem::path WINDOW_ICON = "icons/ultrastardx-icon.png";

    void Initialize3D(const std::string& Title)
        /*var
            Icon : PSDL_Surface;*/
    {
        ULog::Log.LogStatus("SDL_Init", "UGraphic.Initialize3D");
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
        {
            ULog::Log.LogCritical("SDL_Init Failed", "UGraphic.Initialize3D");
        }
        InitializeScreen();
        // load icon image (must be 32x32 for win32)
        auto Icon = UImage::ImageLoad(UPathUtils::ResourcesPath / WINDOW_ICON);
        if (Icon)
            SDL_SetWindowIcon(Screen.get(), Icon.get());

        SDL_SetWindowTitle(Screen.get(), Title.c_str());

        // workaround for buggy Intel 3D driver on Linux
        //SDL_putenv("texture_tiling=false");  //ToDo: on linux, check if this is still necessary with SDL 2

        SDL_SetWindowTitle(Screen.get(), (Title + " - Initializing screen").c_str());


        SDL_SetWindowTitle(Screen.get(), (Title + " - Initializing texturizer").c_str());
        Texture = TTextureUnit.Create;
        UTexture::Texture.Limit = 1920; //currently, Full HD is all we want. switch to 64bit target before going further up

        //LoadTextures;
        SDL_SetWindowTitle(Screen.get(), (Title + " - Initializing video modules").c_str());
        // Note: do not initialize video modules earlier. They might depend on some
        // SDL video functions or OpenGL extensions initialized in InitializeScreen()
        InitializeVideo();

        SDL_SetWindowTitle(Screen.get(), (Title + " - Initializing 3D").c_str());
        ULog::Log.LogStatus("TDisplay.Create", "UGraphic.Initialize3D");
        UDisplay::Display = TDisplay.Create;
        //Display.SetCursor;

        SDL_SetWindowTitle(Screen.get(), (Title + " - Loading font textures").c_str());
        ULog::Log.LogStatus("Loading Font Textures", "UGraphic.Initialize3D");
        LoadFontTextures();

        // Show the Loading Screen
        SDL_SetWindowTitle(Screen.get(), (Title + " - Loading first screen").c_str());
        ULog::Log.LogStatus("Loading Loading Screen", "UGraphic.Initialize3D");
        LoadLoadingScreen;

        // Covers Cache
        SDL_SetWindowTitle(Screen.get(), (Title + " - Loading and checking songs").c_str());
        ULog::Log.LogStatus("Loading and checking songs", "UGraphic.Initialize3D");
        Covers = TCoverDatabase.Create;

        // Category Covers
        ULog::Log.LogStatus("Creating Category Covers Array", "Initialization");
        UCatCovers::CatCovers = TCatCovers.Create;

        // Avatars Cache
        ULog::Log.LogStatus("Creating Avatars Cache", "Initialization");
        UAvatars::Avatars = TAvatarDatabase.Create;

        // Songs
        ULog::Log.LogStatus("Creating Song Array", "Initialization");
        USongs::Songs = TSongs.Create;

        ULog::Log.LogStatus("Creating 2nd Song Array", "Initialization");
        USongs::CatSongs = TCatSongs.Create;

        SDL_SetWindowTitle(Screen.get(), (Title + " - Loading textures").c_str());
        ULog::Log.LogStatus(" Loading Textures", "UGraphic.Initialize3D");
        LoadTextures();

        // this would be run in the loadingthread
        SDL_SetWindowTitle(Screen.get(), (Title + " - Loading screens").c_str());
        ULog::Log.LogStatus(" Loading Screens", "UGraphic.Initialize3D");
        LoadScreens(Title);

        SDL_SetWindowTitle(Screen.get(), Title.c_str());
        UDisplay::Display.CurrentScreen^ .FadeTo(@ScreenMain);

        // work around to force a good screen initialization on MacOS
#ifdef __APPLE__
        UpdateVideoMode();
#endif

        ULog::Log.BenchmarkEnd(2);
        ULog::Log.LogBenchmark("--> Loading Screens", 2);

        ULog::Log.LogStatus("Finish", "Initialize3D");
    }

    void SwapBuffers()
    {
        SDL_GL_SwapWindow(Screen.get());
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, Render.W, Render.H, 0, -1, 100);
		glMatrixMode(GL_MODELVIEW);
    }

    void Finalize3D()
    {
        UnloadFontTextures();
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void Reinitialize3D()
    {
        InitializeScreen();
    }

    void InitializeScreen()
    /*var
        S : string;
I:      int;
    W, H:   int;
    X, Y:   int; // offset for re-positioning
Depth:  Integer;
    Borderless, Fullscreen: boolean;
Split: boolean;
Disp: TSDL_DisplayMode;

    label
        NoDoubledResolution;
        */
    {
        if (UCommandLine::Params.Screens != -1)
            screen.screens = UCommandLine::Params.Screens + 1;
        else
            screen.screens = UIni::Ini.screen_info.Screens + 1;

    bool Split;
    switch (UCommandLine::Params.Split)
    {
    case UCommandLine::TSplitMode::spmSplit:
        Split = true;
        break;
    case UCommandLine::TSplitMode::spmNoSplit:
        Split = false;
        break;
    default:
        Split = UIni::Ini.screen_info.Split == 1;
    } // case

    // check whether to start in fullscreen, windowed mode or borderless mode (windowed fullscreen).
    // The command-line parameters take precedence over the ini settings.
	bool Borderless = (UIni::Ini.screen_info.FullScreen == 2) && (UCommandLine::Params.ScreenMode != UCommandLine::scmFullscreen);
	bool Fullscreen = ((UIni::Ini.screen_info.FullScreen == 1) || (UCommandLine::Params.ScreenMode == UCommandLine::scmFullscreen)) &&
		(UCommandLine::Params.ScreenMode != UCommandLine::scmWindowed);

// If there is a resolution in Parameters, use it, else use the Ini value
// check for a custom resolution (in the format of WIDTHxHEIGHT) or try validating ID from TIni
    int W, H;
    if (UCommon::ParseResolutionString(UCommandLine::Params.CustomResolution, W, H))
        ULog::Log.LogStatusLocation(std::format("Use custom resolution from Command line: {} x {}", W, H));
    else if (UIni::Ini.GetResolution(UCommandLine::Params.Resolution, S) && UCommon::ParseResolutionString(S, W, H))
        ULog::Log.LogStatusLocation(std::format("Use resolution by index from command line: {} x {} [{}]", W, H, UCommandLine::Params.Resolution));
	else if (Fullscreen)
	{
		ULog::Log.LogStatusLocation("Use config fullscreen resolution");
		S = UIni::Ini.GetResolutionFullscreen(W, H);

		// fullscreen resolution shouldn't be doubled as it would not allow running double fullscreen
		// in a specific resolution if desired and required for some TVs/monitors/display devices
		Goto NoDoubledResolution;
	}
	else
	{
		ULog::Log.LogStatusLocation("Use config resolution");
		S = UIni::Ini.GetResolution(W, H);

		// hackfix to prevent a doubled resolution twice as GetResolution(int,int) is already doubling the resolution
		Goto NoDoubledResolution;
	}
    // hacky fix to support multiplied resolution (in width) in multiple screen setup (Screens=2 and more)
	// TODO: RattleSN4K3: Improve the way multiplied screen resolution is applied and stored (see UGraphics::InitializeScreen; W := W * Screens)
	if (screen.screens > 1 && !Split)
		W *= screen.screens;

NoDoubledResolution:

ULog::Log.LogStatusLocation("Creating window");

// TODO: use SDL renderer (for proper scale in "double fullscreen"). Able to choose rendering mode (OpenGL, OpenGL ES, Direct3D)
if (Borderless)
{
	ULog::Log.LogStatusLocation("Set Video Mode...   Borderless fullscreen");
	CurrentWindowMode = Mode::Borderless;
    screen.window = std::unique_ptr<SDL_Window, WindowDeleter>(SDL_CreateWindow("UltraStar Deluxe loading...",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE), WindowDeleter());
}
else if (Fullscreen)
{
	ULog::Log.LogStatusLocation("Set Video Mode...   Fullscreen");
	CurrentWindowMode = Mode::Fullscreen;
    screen.window = std::unique_ptr<SDL_Window, WindowDeleter>(SDL_CreateWindow("UltraStar Deluxe loading...",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE), WindowDeleter());
}
else
{
	ULog::Log.LogStatusLocation("Set Video Mode...   Windowed");
	CurrentWindowMode = Mode::WINDOWED;
	screen.window = std::unique_ptr<SDL_Window, WindowDeleter>(SDL_CreateWindow("UltraStar Deluxe loading...",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE), WindowDeleter());
}

//SDL_ShowCursor(0);    just to be able to debug while having mosue cursor

if (!screen.window)
{
	ULog::Log.LogCriticalLocation("Creating window failed");
}
else
{
	X = 0; Y = 0;

// check if created window has the desired size, otherwise override the config resolution value
if SDL_GetWindowDisplayMode(screen, @Disp) = 0 then
{
if (Disp.w < W) or (Disp.h < H) then
    {
    ULog::Log.LogStatus(Format("Video resolution (%s) exceeded possible size (%s). Override stored config resolution!", [BuildResolutionString(W, H), BuildResolutionString(Disp.w, Disp.h)]), "SDL_SetVideoMode");
Ini.SetResolution(Disp.w, Disp.h, true);
}
else if Fullscreen and ((Disp.w > W) or (Disp.h > H)) then
{
ULog::Log.LogStatus(Format("Video resolution not used. Using native fullscreen resolution (%s)", [BuildResolutionString(Disp.w, Disp.h)]), "SDL_SetVideoMode");
Ini.SetResolution(Disp.w, Disp.h, false, true);
}

X = Disp.w - Screen.w;
Y = Disp.h - Screen.h;
}

// if screen is out of the visisble desktop area, move it back
// this likely happens when creating a Window bigger than the possible desktop size
if (SDL_GetWindowFlags(screen) and SDL_WINDOW_FULLSCREEN = 0) and ((screen.x < 0) or (screen.Y < 0)) then
    {
    // TODO: update SDL2
    //SDL_GetWindowBordersSize(screen, w, h, nil, nil);
    ULog::Log.LogStatus("Bad position for window. Re-position to (0,0)", "SDL_SetVideoMode");
SDL_SetWindowPosition(screen, x, y + x);
}
}

//LoadOpenGL();
glcontext = SDL_GL_CreateContext(Screen);
InitOpenGL();

//   ActivateRenderingContext(
ReadExtensions;
ReadImplementationProperties;
ULog::Log.LogInfo("OpenGL vendor " + glGetString(GL_VENDOR), "UGraphic.InitializeScreen");
if not (glGetError = GL_NO_ERROR) then
{
ULog::Log.LogInfo("an OpenGL Error happened.", "UGraphic.InitializeScreen");
}
S = glGetString(GL_RENDERER);
ULog::Log.LogInfo("OpenGL renderer " + S, "UGraphic.InitializeScreen");
ULog::Log.LogInfo("OpenGL version " + glGetString(GL_VERSION), "UGraphic.InitializeScreen");

if (Pos("GDI Generic", S) > 0) or // Microsoft
(Pos("Software Renderer", S) > 0) or // Apple
(Pos("Software Rasterizer", S) > 0) or // Mesa (-Ddri-drivers=swrast)
(Pos("softpipe", S) > 0) or // Mesa (-Dgallium-drivers=swrast -Dllvm=false)
(Pos("llvmpipe", S) > 0) or // Mesa (-Dgallium-drivers=swrast -Dllvm=true)
(Pos("SWR", S) > 0) or // Mesa (-Dgallium-drivers=swr)
(Pos("Mesa X11", S) > 0) or // Mesa (-Dglx=xlib)
(Pos("SwiftShader", S) > 0) then // Google; OpenGL ES, D3D9 & Vulkan only so far, but who knows...
SoftwareRendering  = true
else
SoftwareRendering  = false;

// define virtual (Render) and double (Screen) screen size
RenderW = 800;
RenderH = 600;
ScreenW = Screen.w;
ScreenH = Screen.h;
// Ausganswerte für die State-Machine setzen
SDL_GL_SetSwapInterval(1); // VSYNC (currently Windows only)

{// clear screen once window is being shown
// Note: SwapBuffers uses RenderW/H, so they must be defined before
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    SwapBuffers; }
}

function HasWindowState(Flag: int) : boolean;
{
Result  = SDL_GetWindowFlags(screen) and Flag < > 0;
}

void UpdateResolution();
var
Disp : TSDL_DisplayMode;
Event: TSDL_event;
{
if CurrentWindowMode = Mode_Borderless then Exit;
case CurrentWindowMode of
Mode_Fullscreen :
    {
        SDL_GetWindowDisplayMode(screen, @Disp); // TODO: verify if not failed
    Ini.GetResolutionFullscreen(Disp.W, Disp.H); // we use the fullscreen resolution without being doubled, true fullscreen uses non-multiplied one
    SDL_SetWindowDisplayMode(screen, @Disp);
    SDL_SetWindowSize(screen, Disp.W, Disp.H);
    }
Mode_Windowed:
    {
        Ini.GetResolution(Disp.W, Disp.H);
    SDL_SetWindowSize(screen, Disp.W, Disp.H);

    // re-center window if it wasn"t moved already, keeps it centered
    if not HasValidPosition then
        {
        SDL_SetWindowPosition(screen, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    // simulate window re-drawing, otherwise the context will be different sized
    Event.user.type_  = SDL_WINDOWEVENT;
    Event.window.event  = SDL_WINDOWEVENT_RESIZED;
    Event.window.data1  = Disp.W;
    Event.window.data2  = Disp.H;
    SDL_PushEvent(@Event);
    }
    }
    }

    void UpdateVideoMode();
    var
        Mode : FullscreenModes;
    {
        if Ini.Fullscreen = 1 then Mode  = Mode_Fullscreen
        else if Ini.FullScreen = 2 then Mode  = Mode_Borderless
        else Mode  = Mode_Windowed;

    SetVideoMode(Mode);
    }

    void SetVideoMode(Mode: FullscreenModes);
    var
        w, h: int;
Disp: TSDL_DisplayMode;
    {
        if Mode = CurrentWindowMode then Exit;
    if Mode >= Mode_Fullscreen then
        {
        Mode  = Mode and not Mode_Borderless;
    SDL_GetWindowDisplayMode(screen, @Disp);
    SDL_SetWindowFullscreen(screen, SDL_WINDOW_FULLSCREEN);

    Ini.GetResolutionFullscreen(Disp.W, Disp.H);
    SDL_SetWindowDisplayMode(screen, @Disp);
    SDL_SetWindowSize(screen, Disp.W, Disp.H);
    }
    else if Mode = Mode_Borderless then
        {
        // calls window-resize event which updates screen sizes
        SDL_SetWindowFullscreen(screen, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else if Mode = Mode_Windowed then
        {
        WindowModeDirty  = true; // set window size dirty to restore old size after switching from fullscreen
    SDL_SetWindowFullscreen(screen, SDL_WINDOW_RESIZABLE); // calls window-resize event which updates screen sizes

ScreenW = LastW; ScreenH = LastH;
    if not HasValidSize then Ini.GetResolution(ScreenW, ScreenH);
    SDL_SetWindowSize(screen, ScreenW, ScreenH);
    }

CurrentWindowMode = Mode;
    }

    function SwitchVideoMode(Mode: FullscreenModes) : FullscreenModes;
    {
        if Mode = Mode_Windowed then Mode  = CurrentWindowMode;
    SetVideoMode(CurrentWindowMode xor Mode);
Result = CurrentWindowMode;
    }

    void OnWindowMoved(x, y: int);
    {
        if CurrentWindowMode <> Mode_Windowed then Exit;
    if (SDL_GetWindowFlags(screen) and (SDL_WINDOW_MINIMIZED or SDL_WINDOW_MAXIMIZED) < > 0) then Exit;

    if not WindowModeDirty then
        {
        HasValidPosition  = true;
LastX = x;
LastY = y;
    }
    }

    void OnWindowResized(w, h: int);
    {
        if WindowModeDirty and not HasWindowState(SDL_WINDOW_FULLSCREEN) then
            {
            if not HasValidSize then
                {
                LastH  = ScreenH;
LastW = ScreenW;
    }

    // restoring from maximized state will additionally call a SDL_WINDOWEVENT_RESIZED event
    // we keep the dirty flag to still revert to the last none-maximized stored position and size
    if HasWindowState(SDL_WINDOW_MINIMIZED or SDL_WINDOW_MAXIMIZED) then
        {
        SDL_RestoreWindow(screen);
    }
    else
        {
        WindowModeDirty  = false;
    }

    // override render size
ScreenW = LastW;
ScreenH = LastH;
    SDL_SetWindowPosition(screen, LastX, LastY);

    // if there wasn"t a windowed mode before, center window
    if not HasValidPosition then
        {
        SDL_SetWindowPosition(screen, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    }
    else
        {
        // override render size
        ScreenW  = w; ScreenH = h;

    if not HasWindowState(SDL_WINDOW_MAXIMIZED or SDL_WINDOW_FULLSCREEN) then
        {
        HasValidSize  = true;
LastW = w;
LastH = h;
    }
    }

    if CurrentWindowMode = Mode_Fullscreen then
        {
        Screen.W  = ScreenW;
    Screen.H  = ScreenH;
    }
    else
        {
        SDL_SetWindowSize(screen, ScreenW, ScreenH);
    }

    if assigned(Display) then
        {
        Display.OnWindowResized(); // notify display window has changed
    }
    }

    void LoadLoadingScreen;
    {
        ScreenLoading  = TScreenLoading.Create;
    ScreenLoading.OnShow;
    Display.CurrentScreen  = @ScreenLoading;
    SwapBuffers;
    ScreenLoading.Draw;
    Display.Draw;
    SwapBuffers;
    }

    void LoadScreens(Title: string);
    {
        SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenMain & ScreenName"));
ScreenMain = TScreenMain.Create;
ScreenName = TScreenName.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenLevel & ScreenSong"));
ScreenLevel = TScreenLevel.Create;
ScreenSong = TScreenSong.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenSongMenu & ScreenJukebox"));
ScreenSongMenu = TScreenSongMenu.Create;
ScreenJukebox = TScreenJukebox.Create;
    ULog::Log.BenchmarkEnd(3); ULog::Log.LogBenchmark("====> Screen Jukebox", 3); ULog::Log.BenchmarkStart(3);
ScreenJukeboxOptions = TScreenJukeboxOptions.Create;
    ULog::Log.BenchmarkEnd(3); ULog::Log.LogBenchmark("====> Screen Jukebox Options", 3); ULog::Log.BenchmarkStart(3);
ScreenJukeboxPlaylist = TScreenJukeboxPlaylist.Create;
    ULog::Log.BenchmarkEnd(3); ULog::Log.LogBenchmark("====> Screen Jukebox Playlist", 3); ULog::Log.BenchmarkStart(3);
ScreenTop5 = TScreenTop5.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenOptions & ScreenOptionsGame"));
ScreenOptions = TScreenOptions.Create;
ScreenOptionsGame = TScreenOptionsGame.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenOptionsGraphics & ScreenOptionsSound & ScreenOptionsInput"));
ScreenOptionsGraphics = TScreenOptionsGraphics.Create;
ScreenOptionsSound = TScreenOptionsSound.Create;
ScreenOptionsInput = TScreenOptionsInput.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenOptionsLyrics & ScreenOptionsThemes"));
ScreenOptionsLyrics = TScreenOptionsLyrics.Create;
ScreenOptionsThemes = TScreenOptionsThemes.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenOptionsRecord & ScreenOptionsAdvanced"));
ScreenOptionsRecord = TScreenOptionsRecord.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenOptionsAdvanced"));
ScreenOptionsAdvanced = TScreenOptionsAdvanced.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenOptionsNetwork"));
ScreenOptionsNetwork = TScreenOptionsNetwork.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenOptionsWebCam"));
ScreenOptionsWebcam = TScreenOptionsWebcam.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenOptionsJukebox"));
ScreenOptionsJukebox = TScreenOptionsJukebox.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenEditConvert & ScreenEditSub & ScreenEdit"));
ScreenEditConvert = TScreenEditConvert.Create;
ScreenEditSub = TScreenEditSub.Create;
ScreenEdit = TScreenEdit.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenOpen"));
ScreenOpen = TScreenOpen.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenAbout"));
ScreenAbout = TScreenAbout.Create;
    ULog::Log.BenchmarkEnd(3); ULog::Log.LogBenchmark("====> Screen About", 3); ULog::Log.BenchmarkStart(3);
    //ScreenSingModi :=         TScreenSingModi.Create;
    //ULog::Log.BenchmarkEnd(3); ULog::Log.LogBenchmark("====> Screen Sing with Modi support", 3); ULog::Log.BenchmarkStart(3);
ScreenSongJumpto = TScreenSongJumpto.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenPopupCheck & ScreenPopupError & ScreenPopupHelp"));
ScreenPopupCheck = TScreenPopupCheck.Create;
ScreenPopupError = TScreenPopupError.Create;
ScreenPopupHelp = TScreenPopupHelp.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenPopupInfo & ScreenScoreX & ScreenPartyNewRound"));
ScreenPopupInfo = TScreenPopupInfo.Create;
ScreenPopupInsertUser = TScreenPopupInsertUser.Create;
ScreenPopupSendScore = TScreenPopupSendScore.Create;
ScreenPopupScoreDownload = TScreenPopupScoreDownload.Create;
ScreenPartyNewRound = TScreenPartyNewRound.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenPartyScore & ScreenPartyWin"));
ScreenPartyScore = TScreenPartyScore.Create;
ScreenPartyWin = TScreenPartyWin.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenPartyOptions & ScreenPartyPlayer"));
ScreenPartyOptions = TScreenPartyOptions.Create;
ScreenPartyPlayer = TScreenPartyPlayer.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenPartyRounds & ScreenTournamentX & ScreenStatMain"));
ScreenPartyRounds = TScreenPartyRounds.Create;
ScreenPartyTournamentRounds = TScreenPartyTournamentRounds.Create;
ScreenPartyTournamentPlayer = TScreenPartyTournamentPlayer.Create;
ScreenPartyTournamentOptions = TScreenPartyTournamentOptions.Create;
ScreenPartyTournamentWin = TScreenPartyTournamentWin.Create;
ScreenStatMain = TScreenStatMain.Create;
    SDL_SetWindowTitle(Screen, PChar(Title + " - Loading ScreenStatDetail & ScreenCredits"));
ScreenStatDetail = TScreenStatDetail.Create;
ScreenCredits = TScreenCredits.Create;
    SDL_SetWindowTitle(Screen, PChar(Title));
    }

    function LoadingThreadFunction : int;
    {
        LoadScreens(USDXVersionStr);
Result = 1;
    }

    void UnloadScreens;
    {
        ScreenMain.Free;
    ScreenName.Free;
    ScreenLevel.Free;
    ScreenSong.Free;
    //ScreenSing.Free;
    ScreenScore.Free;
    ScreenOptions.Free;
    ScreenOptionsGame.Free;
    ScreenOptionsGraphics.Free;
    ScreenOptionsSound.Free;
    ScreenOptionsInput.Free;
    ScreenOptionsLyrics.Free;
    ScreenOptionsThemes.Free;
    ScreenOptionsRecord.Free;
    ScreenOptionsAdvanced.Free;
    ScreenOptionsNetwork.Free;
    ScreenOptionsWebcam.Free;
    ScreenOptionsJukebox.Free;
    ScreenEditSub.Free;
    ScreenEdit.Free;
    ScreenEditConvert.Free;
    ScreenJukebox.Free;
    ScreenJukeboxOptions.Free;
    ScreenJukeboxPlaylist.Free;
    ScreenTop5.Free;
    ScreenOpen.Free;
    ScreenAbout.Free;
    //ScreenSingModi.Free;
    ScreenSongMenu.Free;
    ScreenSongJumpto.Free;
    ScreenPopupCheck.Free;
    ScreenPopupError.Free;
    ScreenPopupInfo.Free;
    ScreenPopupInsertUser.Free;
    ScreenPopupSendScore.Free;
    ScreenPopupScoreDownload.Free;
    ScreenPartyNewRound.Free;
    ScreenPartyScore.Free;
    ScreenPartyWin.Free;
    ScreenPartyOptions.Free;
    ScreenPartyPlayer.Free;
    ScreenPartyRounds.Free;
    ScreenPartyTournamentRounds.Free;
    ScreenPartyTournamentPlayer.Free;
    ScreenPartyTournamentOptions.Free;
    ScreenPartyTournamentWin.Free;
    ScreenStatMain.Free;
    ScreenStatDetail.Free;
    }
}