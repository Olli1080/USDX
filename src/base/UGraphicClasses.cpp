#include "UGraphicClasses.h"

#include <numbers>

#include "UThemes.h"
#include "UGraphic.h"

namespace UGraphicClasses
{

    /*implementation

        uses
        SysUtils,
        Math,
        dglOpenGL,
        UCommon,
        UDrawTexture,
        UGraphic,
        UNote,
        USkins,
        UThemes;*/

        //TParticle
    TParticle::TParticle(double cX, double cY, int cScreen, uint8_t cLive, int cFrame, int cRecArrayIndex, TParticleType cStarType,
        uint32_t Player)
	        : X(cX), Y(cY), Screen(cScreen), Live(cLive), Frame(cFrame), RecIndex(cRecArrayIndex), StarType(cStarType),
			Alpha(neatFadeInAndOut()), // neat fade-in-and-out
        Scale{ {1} }, SurviveSentenceChange(false), SizeMod(1)
    {
        switch (cStarType)
        {
            case TParticleType::GoldenNote :
	        {
	            Tex = UGraphic::Tex_Note_Star.TexNum;
	            W = 20;
	            H = 20;
                Scale.resize(4);
	            Scale[1] = 0.8;
	            Scale[2] = 0.4;
	            Scale[3] = 0.3;

                Col = { {1, 0.7, 0.1}, {1, 1, 0.4}, {1, 1, 1}, {1, 1, 1} };
	        }
            case TParticleType::PerfectNote:
	        {
	            Tex = UGraphic::Tex_Note_Perfect_Star.TexNum;
	            W = 30;
	            H = 30;
                Col = { {1, 1, 0.95} };
	        }
            case TParticleType::NoteHitTwinkle:
	        {
	            Tex = UGraphic::Tex_Note_Star.TexNum;
	            Alpha = LinearFadeOut();  // linear fade-out
	            W = 15;
	            H = 15;
                Col = { TColor3f(1, 1, std::uniform_real_distribution<float>(0.09 * Live, 0.9)(rd)) };
	        }
            case TParticleType::PerfectLineTwinkle:
	        {
	            Tex = UGraphic::Tex_Note_Star.TexNum;
                W = std::uniform_int_distribution<int>(10, 20)(rd);
	            H = W;
                SizeMod = neatSizing();
	            SurviveSentenceChange = true;
	            // assign colors according to player given
                Scale.resize(3);
	            Scale[1] = 0.3;
	            Scale[2] = 0.2;
                Col.resize(3);
                switch (Player)
                {
                case 0:
                    UThemes::LoadColor(Col[0], "P1Light");
                    break;
                case 1:
                    UThemes::LoadColor(Col[0], "P2Light");
                    break;
                case 2:
                    UThemes::LoadColor(Col[0], "P3Light");
                    break;
                case 3:
                    UThemes::LoadColor(Col[0], "P4Light");
                    break;
                case 4:
                    UThemes::LoadColor(Col[0], "P5Light");
                    break;
                case 5:
                    UThemes::LoadColor(Col[0], "P6Light");
                    break;
                default:
                    UThemes::LoadColor(Col[0], "P1Light");
                }
                Col[1] = { 1, 1, 0.4 };
                Col[2] = TColor3f(Col[0].R + 0.5, Col[0].G + 0.5, Col[0].B + 0.5);

                auto dist = std::uniform_real_distribution<double>(-5, 5);
                mX = dist(rd);
                mY = dist(rd);
			}
			case TParticleType::ColoredStar:
		    {
		        Tex = UGraphic::Tex_Note_Star.TexNum;
                W = std::uniform_int_distribution<uint32_t>(10, 20)(rd);
		        H = W;
                SizeMod = neatSizing();
		        SurviveSentenceChange = true;
		        // assign colors according to player given
		        //SetLength(Scale, 1);
		        //SetLength(Col, 1);

                Col = { { (Player & 0xff) / 255.f, ((Player >> 8) & 0xff) / 255.f, ((Player >> 16) & 0xff) / 255.f } };
		        mX = 0;
		        mY = 0;
		    }
			case TParticleType::Flare:
		    {
		        Tex = UGraphic::Tex_Note_Star.TexNum;
		        W = 7;
		        H = 7;
                SizeMod = neatSizing();

                auto dist = std::uniform_real_distribution<double>(-5, 5);
                mX = dist(rd);
                mY = dist(rd);

		        Scale.resize(4);
		        Scale[1] = 0.8;
		        Scale[2] = 0.4;
		        Scale[3] = 0.3;

                Col = {
                	{1, 0.7, 0.1},
                	{1, 1, 0.4},
                	{1, 1, 1},
                	{1, 1, 1} };

		    }
    		default:    // just some random default values
		    {
		        Tex = UGraphic::Tex_Note_Star.TexNum;
		        Alpha = 1;
		        W = 20;
		        H = 20;

                Col = { {1, 1, 1} };
			}
		}
	}

    double TParticle::neatFadeInAndOut()
    {
        return -cos((Frame + 1) / FramesAlive * 2 * std::numbers::pi) + 1;
    }

    double TParticle::LinearFadeOut()
    {
        return Live / FramesAlive;
    }

    double TParticle::neatSizing()
    {
        return -cos((Frame + 1) * 5 * 2 * std::numbers::pi / FramesAlive) * 0.5 + 1.1;
    }

    void TParticle::LiveOn()
    {
        //Live = 0 => Live forever  <blindy> ?? but if this is 0 they would be killed in the Manager at Draw
        if (Live > 0)
            --Live;

        // animate frames
        Frame = (Frame + 1) % 16;

        // make our particles do funny stuff (besides being animated)
        // changes of any particle-values throughout its life are done here
        switch (StarType)
        {
        case TParticleType::GoldenNote: [[fallthrough]]
        case TParticleType::PerfectNote: [[fallthrough]]
        case TParticleType::ColoredStar:
        {
            Alpha = neatFadeInAndOut(); // neat fade-in-and-out
        }
        case TParticleType::NoteHitTwinkle:
        {
            Alpha = LinearFadeOut();  // linear fade-out
        }
        case TParticleType::PerfectLineTwinkle:
        {
            Alpha = neatFadeInAndOut(); // neat fade-in-and-out
            SizeMod = neatSizing();
            // move around
            X += mX;
            Y += mY;
        }
        case TParticleType::Flare:
        {
            Alpha = neatFadeInAndOut(); // neat fade-in-and-out
            SizeMod = neatSizing();
            // move around
            X += mX;
            Y += mY;
            mY += 1.8;
            //          mX := mX/2;
        }
        }
    }

    void TParticle::Draw()
    //var
      //  L : uint32_t;
    {
        /* if screens = 2 and playerplay <= 3 the 2nd screen shows the
          textures of screen 1 */
        if (Screens == 2 && PlayersPlay <= 3)
            ScreenAct = 1;

        if (ScreenAct != Screen)
            return;
        
	        glEnable(GL_TEXTURE_2D);
		    glBindTexture(GL_TEXTURE_2D, Tex);
		    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		    glEnable(GL_BLEND);

			// this draws (multiple) texture(s) of our particle
            for (const auto& layer : Layers)
            {
                const auto& Col = layer.Col;
                const auto& Scale = layer.Scale;

                glColor4f(Col.R, Col.G, Col.B, Alpha);

                const auto x1 = X - W * Scale * SizeMod;
                const auto x2 = X + W * Scale * SizeMod;

                const auto y1 = Y - H * Scale * SizeMod;
                const auto y2 = Y + H * Scale * SizeMod;

                constexpr float t0 = 1.f / FramesAlive;
                const float t1 = t0 * Frame;
                const float t2 = t1 + t0;

                glBegin(GL_QUADS);
                glTexCoord2f(t1, 0);   glVertex2f(x1, y1);
                glTexCoord2f(t2, 0);    glVertex2f(x1, y2);
                glTexCoord2f(t2, 1);    glVertex2f(x2, y2);
                glTexCoord2f(t1, 1);    glVertex2f(x2, y1);
                glEnd();
            }
            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            glColor4f(1, 1, 1, 1);
    }
    // end of TParticle

    // TEffectManager
    TEffectManager::TEffectManager()
        :
		LastTime(std::chrono::system_clock::now()),
        TwinkleArray{0}
    {}

    void TEffectManager::Draw()
    {
        const auto CurrentTime = std::chrono::system_clock::now();
        //Manage particle life
        if (CurrentTime - LastTime > DelayBetweenFrames)
        {
            LastTime = CurrentTime;
            for (auto& particle : Particle)
                particle.LiveOn(); //TODO:: maybe accumulate dead particles already?
        }

        auto it = Particle.begin();
        //Kill dead particles
        while (it != Particle.end())
        {
            if (it->Live <= 0)
                it = Kill(it);
            else
                ++it;
        }
        //Draw
        for (auto& particle : Particle)
            particle.Draw();
    }

// this method creates just one particle
std::list<TParticle>::iterator TEffectManager::Spawn(double X, double Y,
    int Screen,
    uint8_t Live,
    int StartFrame,
    int RecArrayIndex,
    TParticleType StarType,
    uint32_t Player)
{
    Particle.emplace_back(X, Y, Screen, Live, StartFrame, RecArrayIndex, StarType, Player);
    return --Particle.end();
}

// manage Sparkling of GoldenNote Bars
void TEffectManager::SpawnRec()
/*var
Xkatze, Ykatze    : double;
RandomFrame: int;
P: int; // P as seen on TV as Positionman*/
{
    //AKI BUG

    //Spawn a random amount of stars within the given coordinates
    //RandomRange(0,14) <- this one starts at a random frame, 16 is our last frame - would be senseless to start a particle with 16, cause it would be dead at the next frame
    for (size_t P = 0; P < RecArray.size(); ++P)
    {
        auto& rec = RecArray[P];

        for (auto& csc = rec.CurrentStarCount; csc <= rec.TotalStarCount; ++csc)
        {
            auto Xkatze = std::uniform_real_distribution<double>(std::ceil(rec.xTop), std::ceil(rec.xBottom))(rd);
            auto Ykatze = std::uniform_real_distribution<double>(std::ceil(rec.yTop), std::ceil(rec.yBottom))(rd);
            int RandomFrame = std::uniform_int_distribution<int>(0, 14)(rd);

            // Spawn a GoldenNote Particle
            Spawn(Xkatze, Ykatze, rec.Screen, 16 - RandomFrame, RandomFrame, P, TParticleType::GoldenNote, 0);
        }
    }
    Draw();
}

// kill one particle (with given index in our particle array)
std::list<TParticle>::iterator TEffectManager::Kill(std::list<TParticle>::iterator Index)
{
    // delete particle indexed by Index,
    // erase from list
    if (Index->RecIndex != -1) // if it is a GoldenNote particle...
        --RecArray[Index->RecIndex].CurrentStarCount; // take care of its associated GoldenRec
    return Particle.erase(Index);
}

// clean up all particles and management structures
void TEffectManager::KillAll()
{
    //It"s the kill all kennies rotuine
    while Length(Particle) > 0 do  // kill all existing particles
        Kill(0);
    SetLength(RecArray, 0);  // remove GoldenRec positions
    SetLength(PerfNoteArray, 0); // remove PerfectNote positions
    for c = 0 to 5 do
    {
        TwinkleArray[c] = 0; // reset GoldenNoteHit memory
    }
}

void TEffectManager::SentenceChange(CP: int);
var
c : uint32_t;
p: int;
{

    c = 0;
    while c <= High(Particle) do
    {
        if Particle[c].SurviveSentenceChange then
            inc(c)
        else
            Kill(c);
    }

    SetLength(RecArray, 0);  // remove GoldenRec positions
    SetLength(PerfNoteArray, 0); // remove PerfectNote positions
    for c = 0 to 5 do
    {
        if not CurrentSong.isDuet or ((c mod 2) = CP)  then
            TwinkleArray[c] = 0; // reset GoldenNoteHit memory
    }
}

void TeffectManager.GoldenNoteTwinkle(Top, Bottom, Right: double; Player: int);
//Twinkle stars while golden note hit
// this is called from UDraw.pas, SingDrawPlayerCzesc
var
C, P, XKatze, YKatze, LKatze: int;
H: double;
{
    // make sure we spawn only one time at one position
    if (TwinkleArray[Player] < Right) then
        for P = 0 to high(RecArray) do  // Are we inside a GoldenNoteRectangle?
        {
            H = (Top + Bottom) / 2; // helper...
            with RecArray[P] do
                if ((xBottom >= Right) and (xTop <= Right) and
                    (yTop <= H) and (yBottom >= H))
                    and (Screen = ScreenAct) then
                {
                    TwinkleArray[Player] = Right; // remember twinkle position for this player
                    for C = 1 to 10 do
                    {
                        Ykatze = RandomRange(ceil(Top), ceil(Bottom));
                        XKatze = RandomRange(-7, 3);
                        LKatze = RandomRange(7, 13);
                        Spawn(Ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, NoteHitTwinkle, 0);
                    }
                    for C = 1 to 3 do
                    {
                        Ykatze = RandomRange(ceil(Top) - 6, ceil(Top));
                        XKatze = RandomRange(-5, 1);
                        LKatze = RandomRange(4, 7);
                        Spawn(Ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, NoteHitTwinkle, 0);
                    }
                    for C = 1 to 3 do
                    {
                        Ykatze = RandomRange(ceil(Bottom), ceil(Bottom) + 6);
                        XKatze = RandomRange(-5, 1);
                        LKatze = RandomRange(4, 7);
                        Spawn(Ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, NoteHitTwinkle, 0);
                    }
                    for C = 1 to 3 do
                    {
                        Ykatze = RandomRange(ceil(Top) - 10, ceil(Top) - 6);
                        XKatze = RandomRange(-5, 1);
                        LKatze = RandomRange(1, 4);
                        Spawn(Ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, NoteHitTwinkle, 0);
                    }
                    for C = 1 to 3 do
                    {
                        Ykatze = RandomRange(ceil(Bottom) + 6, ceil(Bottom) + 10);
                        XKatze = RandomRange(-5, 1);
                        LKatze = RandomRange(1, 4);
                        Spawn(Ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, NoteHitTwinkle, 0);
                    }

                    exit; // found a matching GoldenRec, did spawning stuff... done
                }
        }
}

void TEffectManager::SaveGoldenStarsRec(Xtop, Ytop, Xbottom, Ybottom: double);
var
P : int;   // P like used in Positions
NewIndex: int;
{
    for P = 0 to high(RecArray) do  // Do we already have that "new" position?
    {
        if (ceil(RecArray[P].xTop) = ceil(Xtop)) and
            (ceil(RecArray[P].yTop) = ceil(Ytop)) and
            (ScreenAct = RecArray[p].Screen) then
            exit; // it"s already in the array, so we don"t have to create a new one
    }

    // we got a new position, add the new positions to our array
    NewIndex = Length(RecArray);
    SetLength(RecArray, NewIndex + 1);
    RecArray[NewIndex].xTop = Xtop;
    RecArray[NewIndex].yTop = Ytop;
    RecArray[NewIndex].xBottom = Xbottom;
    RecArray[NewIndex].yBottom = Ybottom;
    RecArray[NewIndex].TotalStarCount = ceil(Xbottom - Xtop) div 12 + 3;
    RecArray[NewIndex].CurrentStarCount = 0;
    RecArray[NewIndex].Screen = ScreenAct;
}

void TEffectManager::SavePerfectNotePos(Xtop, Ytop: double);
var
P : int;   // P like used in Positions
NewIndex: int;
RandomFrame: int;
Xkatze, Ykatze : int;
{
    for P = 0 to high(PerfNoteArray) do  // Do we already have that "new" position?
    {
        with PerfNoteArray[P] do
            if (ceil(xPos) = ceil(Xtop)) and (ceil(yPos) = ceil(Ytop)) and
                (Screen = ScreenAct) then
                exit; // it"s already in the array, so we don"t have to create a new one
    } //for

    // we got a new position, add the new positions to our array
    NewIndex = Length(PerfNoteArray);
    SetLength(PerfNoteArray, NewIndex + 1);
    PerfNoteArray[NewIndex].xPos = Xtop;
    PerfNoteArray[NewIndex].yPos = Ytop;
    PerfNoteArray[NewIndex].Screen = ScreenAct;

    for P = 0 to 2 do
    {
        Xkatze = RandomRange(ceil(Xtop) - 5, ceil(Xtop) + 10);
        Ykatze = RandomRange(ceil(Ytop) - 5, ceil(Ytop) + 10);
        RandomFrame = RandomRange(0, 14);
        Spawn(Xkatze, Ykatze, ScreenAct, 16 - RandomFrame, RandomFrame, -1, PerfectNote, 0);
    } //for

}

void TEffectManager::SpawnPerfectLineTwinkle();
var
P, I, Life: uint32_t;
Left, Right, Top, Bottom: uint32_t;
cScreen, Nstars: int;
{
    // calculation of coordinates done with hardcoded values like in UDraw.pas
    // might need to be adjusted if drawing of SingScreen is modified
    // coordinates may still be a bit weird and need adjustment
    if Ini.SingWindow = 0 then
    {
    Left = 130;
    }
    else
    {
        Left = 30;
    }
    Right = 770;

    // spawn effect for every player with a perfect line
    for P = 0 to PlayersPlay - 1 do
        if Player[P].LastSentencePerfect then
        {
            // 3 and 6 players in 1 screen
            if (Ini.Screens = 0) then
                {
                if (PlayersPlay = 4) then
                    {
                    if (P <= 1) then
                        {
                        Left = 30;
Right = 385;
    }
                    else
                        {
                        Left = 415;
Right = 770;
    }
    }

    if (PlayersPlay = 6) then
        {
        if (P <= 2) then
            {
            Left = 30;
Right = 385;
    }
        else
            {
            Left = 415;
Right = 770;
    }
    }
    }

                // calculate area where notes of this player are drawn
                case PlayersPlay of
                1: {
                    Bottom = Skin_P2_NotesB + 10;
                Top = Bottom - 105;
                cScreen = 1;
                    }
                    2, 4 : {
                        case P of
                        0, 2: {
                        Bottom = Skin_P1_NotesB + 10;
                Top = Bottom - 105;
                    }
                else {
                    Bottom = Skin_P2_NotesB + 10;
            Top = Bottom - 105;
                }
                }
                case P of
                0, 1: cScreen = 1;
                            else
                                {
                                if (Ini.Screens = 1) then
                                    cScreen = 2
                                else
                                    cScreen = 1;
                }
        }
}
3, 6: {
    case P of
    0, 3: {
        Top = 130;
        Bottom = Top + 85;
    }
    1, 4 : {
        Top = 255;
        Bottom = Top + 85;
    }
    2, 5 : {
        Top = 380;
        Bottom = Top + 85;
    }
}
case P of
0, 1, 2: cScreen = 1;
            else
            {
                if (Ini.Screens = 1) then
                    cScreen = 2
                else
                    cScreen = 1;
    }
    }
    }
    }

    // spawn Sparkling Stars inside calculated coordinates
    Nstars = 80;
    if (Ini.Screens = 0) and (PlayersPlay > 3) then
        Nstars = 40;

    for I = 0 to Nstars do
    {
        Life = RandomRange(8, 16);
        Spawn(RandomRange(Left, Right), RandomRange(Top, Bottom), cScreen, Life, 16 - Life, -1, PerfectLineTwinkle, P);

        //spawn also on second screen if the amount of players is <=3
        if (Screens = 2) and (PlayersPlay <= 3) then
            Spawn(RandomRange(Left, Right), RandomRange(Top, Bottom), 2, Life, 16 - Life, -1, PerfectLineTwinkle, P);
    }
    }
    }
}