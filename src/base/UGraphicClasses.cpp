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
        :
        X(cX), Y(cY), Screen(cScreen), Live(cLive), Frame(cFrame), RecIndex(cRecArrayIndex), StarType(cStarType),
        Alpha(neatFadeInAndOut()), // neat fade-in-and-out
        SurviveSentenceChange(false), SizeMod(1)
    {
        switch (cStarType)
        {
        case TParticleType::GoldenNote:
        {
            Tex = UGraphic::Tex_Note_Star.TexNum;
            W = 20;
            H = 20;
            Layers = {
                {{1, 0.7, 0.1} , 1},
                {{1, 1, 0.4}, 0.8},
                {{1, 1, 1}, 0.4},
                {{1, 1, 1}, 0.3}
            };
        }
        case TParticleType::PerfectNote:
        {
            Tex = UGraphic::Tex_Note_Perfect_Star.TexNum;
            W = 30;
            H = 30;
            Layers = { {{1, 1, 0.95}, 1} };
        }
        case TParticleType::NoteHitTwinkle:
        {
            Tex = UGraphic::Tex_Note_Star.TexNum;
            Alpha = LinearFadeOut();  // linear fade-out
            W = 15;
            H = 15;
            Layers = { {TColor3f(1, 1, std::uniform_real_distribution<float>(0.09 * Live, 0.9)(rd)), 1} };
        }
        case TParticleType::PerfectLineTwinkle:
        {
            Tex = UGraphic::Tex_Note_Star.TexNum;
            W = std::uniform_int_distribution<int>(10, 20)(rd);
            H = W;
            SizeMod = neatSizing();
            SurviveSentenceChange = true;

            // assign colors according to player given
            TColor3f Col;
            switch (Player)
            {
            case 0:
                UThemes::LoadColor(Col, "P1Light");
                break;
            case 1:
                UThemes::LoadColor(Col, "P2Light");
                break;
            case 2:
                UThemes::LoadColor(Col, "P3Light");
                break;
            case 3:
                UThemes::LoadColor(Col, "P4Light");
                break;
            case 4:
                UThemes::LoadColor(Col, "P5Light");
                break;
            case 5:
                UThemes::LoadColor(Col, "P6Light");
                break;
            default:
                UThemes::LoadColor(Col, "P1Light");
            }

            Layers = {
                {Col, 1},
                {{1, 1, 0.4}, 0.3},
                {{Col.R + 0.5, Col.G + 0.5, Col.B + 0.5}, 0.4}
            };

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
            Layers = { {{ (Player & 0xff) / 255.f, ((Player >> 8) & 0xff) / 255.f, ((Player >> 16) & 0xff) / 255.f }, 1} };
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

            Layers = {
                {{1, 0.7, 0.1}, 1},
                {{1, 1, 0.4}, 0.8},
                {{1, 1, 1}, 0.4},
                {{1, 1, 1}, 0.3}
            };

        }
        default: // just some random default values
        {
            Tex = UGraphic::Tex_Note_Star.TexNum;
            Alpha = 1;
            W = 20;
            H = 20;

            Layers = { {{1, 1, 1}, 1} };
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
        TwinkleArray{ 0 }
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
    /*void TEffectManager::KillAll()
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
    }*/

    void TEffectManager::SentenceChange(int CP)
        /*var
        c : uint32_t;
        p: int;*/
    {
        auto it = Particle.begin();
        while (it != Particle.end())
        {
            if (it->SurviveSentenceChange)
                ++it;
            else
                Particle.erase(it);
        }
        RecArray.clear(); // remove GoldenRec positions
        PerfNoteArray.clear(); // remove PerfectNote positions

        for (size_t c = 0; c < TwinkleArray.size(); ++c)
        {
            if (!CurrentSong.isDuet || c % 2 == CP)
                TwinkleArray[c] = 0; // reset GoldenNoteHit memory
        }
    }

    void TEffectManager::GoldenNoteTwinkle(double Top, double Bottom, double Right, int Player)
        //Twinkle stars while golden note hit
        // this is called from UDraw.pas, SingDrawPlayerCzesc
    {
        // make sure we spawn only one time at one position
        if (TwinkleArray[Player] >= Right)
            return;

        for (size_t P = 0; P < RecArray.size(); ++P)  // Are we inside a GoldenNoteRectangle?
        {
            const auto& rec = RecArray[P];

            double H = (Top + Bottom) / 2; // helper...

            if (rec.xBottom >= Right && rec.xTop <= Right && rec.yTop <= H && rec.yBottom >= H
                && rec.Screen == ScreenAct)
            {
                int XKatze, YKatze, LKatze;
                TwinkleArray[Player] = Right; // remember twinkle position for this player
                for (size_t C = 0; C < 10; ++C)
                {
                    YKatze = std::uniform_int_distribution<int>(ceil(Top), ceil(Bottom))(rd);
                    XKatze = std::uniform_int_distribution<int>(-7, 3)(rd);
                    LKatze = std::uniform_int_distribution<int>(7, 13)(rd);
                    Spawn(std::ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, TParticleType::NoteHitTwinkle, 0);
                }
                for (size_t C = 0; C < 3; ++C)
                {
                    YKatze = std::uniform_int_distribution<int>(ceil(Top) - 6, ceil(Top))(rd);
                    XKatze = std::uniform_int_distribution<int>(-5, 1)(rd);
                    LKatze = std::uniform_int_distribution<int>(4, 7)(rd);
                    Spawn(std::ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, TParticleType::NoteHitTwinkle, 0);
                }
                for (size_t C = 0; C < 3; ++C)
                {
                    YKatze = std::uniform_int_distribution<int>(ceil(Bottom), ceil(Bottom) + 6)(rd);
                    XKatze = std::uniform_int_distribution<int>(-5, 1)(rd);
                    LKatze = std::uniform_int_distribution<int>(4, 7)(rd);
                    Spawn(std::ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, TParticleType::NoteHitTwinkle, 0);
                }
                for (size_t C = 0; C < 3; ++C)
                {
                    YKatze = std::uniform_int_distribution<int>(ceil(Top) - 10, ceil(Top) - 6)(rd);
                    XKatze = std::uniform_int_distribution<int>(-5, 1)(rd);
                    LKatze = std::uniform_int_distribution<int>(1, 4)(rd);
                    Spawn(std::ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, TParticleType::NoteHitTwinkle, 0);
                }
                for (size_t C = 0; C < 3; ++C)
                {
                    YKatze = std::uniform_int_distribution<int>(ceil(Bottom) + 6, ceil(Bottom) + 10)(rd);
                    XKatze = std::uniform_int_distribution<int>(-5, 1)(rd);
                    LKatze = std::uniform_int_distribution<int>(1, 4)(rd);
                    Spawn(std::ceil(Right) + XKatze, YKatze, ScreenAct, LKatze, 0, -1, TParticleType::NoteHitTwinkle, 0);
                }
                return; // found a matching GoldenRec, did spawning stuff... done
            }
        }
    }

    void TEffectManager::SaveGoldenStarsRec(double Xtop, double Ytop, double Xbottom, double Ybottom)
    {
        for (const auto& rec : RecArray)
        {
            // Do we already have that "new" position?
            if (ceil(rec.xTop) == ceil(Xtop) &&
                ceil(rec.yTop) == ceil(Ytop) &&
                ScreenAct == rec.Screen)
                return; // it"s already in the array, so we don"t have to create a new one
        }
        // we got a new position, add the new positions to our array
        RecArray.emplace_back(Xtop, Ytop, Xbottom, Ybottom, ceil(Xbottom - Xtop) / 12 + 3, 0, ScreenAct);
    }

    void TEffectManager::SavePerfectNotePos(double Xtop, double Ytop)
    {
        for (const auto& rec : PerfNoteArray)
        {  // Do we already have that "new" position?
            if (ceil(rec.xPos) == ceil(Xtop) && ceil(rec.yPos) == ceil(Ytop) &&
                rec.Screen == ScreenAct)
                return; // it"s already in the array, so we don"t have to create a new one
        } //for

        // we got a new position, add the new positions to our array
        PerfNoteArray.emplace_back(Xtop, Ytop, ScreenAct);

        for (size_t P = 0; P < 3; ++P)
        {
            int XKatze = std::uniform_int_distribution<int>(ceil(Xtop) - 5, ceil(Xtop) + 10)(rd);
            int YKatze = std::uniform_int_distribution<int>(ceil(Ytop) - 5, ceil(Ytop) + 10)(rd);
            int RandomFrame = std::uniform_int_distribution<int>(0, 14)(rd);
            Spawn(XKatze, YKatze, ScreenAct, 16 - RandomFrame, RandomFrame, -1, TParticleType::PerfectNote, 0);
        } //for

    }

    void TEffectManager::SpawnPerfectLineTwinkle()
        /*var
        P, I, Life: uint32_t;
        Left, Right, Top, Bottom: uint32_t;
        cScreen, Nstars: int;*/
    {
        // calculation of coordinates done with hardcoded values like in UDraw.pas
        // might need to be adjusted if drawing of SingScreen is modified
        // coordinates may still be a bit weird and need adjustment
        uint32_t Left = (UIni::Ini.SingWindow == 0) ? 130 : 30;
        uint32_t Right = 770;
        uint32_t Top, Bottom;
        int cScreen;

        // spawn effect for every player with a perfect line
        for (size_t P = 0; P < PlayersPlay; ++P)
        {
            if (!Player[P].LastSentencePerfect)
                continue;
            // 3 and 6 players in 1 screen
            if (UIni::Ini.screen_info.Screens == 0)
            {
                if (PlayersPlay == 4)
                {
                    if (P <= 1)
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
                if (PlayersPlay == 6)
                {
                    if (P <= 2)
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
            switch (PlayersPlay)
            {
            case 1:
            {
                Bottom = UGraphic::Skin_P2_NotesB + 10;
                Top = Bottom - 105;
                cScreen = 1;
                break;
            }
            case 2: [[fallthrough]]
            case 4:
            {
                switch (P)
                {
                case 0: [[fallthrough]]
                case 2:
                {
                    Bottom = UGraphic::Skin_P1_NotesB + 10;
                    Top = Bottom - 105;
                    break;
                }
                default:
                {
                    Bottom = UGraphic::Skin_P2_NotesB + 10;
                    Top = Bottom - 105;
                }
                }
                switch (P)
                {
                case 0: [[fallthrough]]
                case 1:
                    cScreen = 1;
                    break;
                default:
                {
                    if (UIni::Ini.screen_info.Screens == 1)
                        cScreen = 2;
                    else
                        cScreen = 1;
                }
                }
                break;
            }
            case 3: [[fallthrough]]
            case 6:
            {
                switch (P)
                {
                case 0: [[fallthrough]]
                case 3:
                {
                    Top = 130;
                    Bottom = Top + 85;
                    break;
                }
                case 1:
                case 4:
                {
                    Top = 255;
                    Bottom = Top + 85;
                    break;
                }
                case 2:
                case 5:
                {
                    Top = 380;
                    Bottom = Top + 85;
                    break;
                }
                }
                switch (P)
                {
                case 0: [[fallthrough]]
                case 1: [[fallthrough]]
                case 2:
                    cScreen = 1;
                    break;
                default:
                {
                    if (UIni::Ini.screen_info.Screens == 1)
                        cScreen = 2;
                    else
                        cScreen = 1;
                }
                }
                break;
            }
            }

            // spawn Sparkling Stars inside calculated coordinates
            int Nstars = 80;
            if (UIni::Ini.screen_info.Screens == 0 && PlayersPlay > 3)
                Nstars = 40;

            for (size_t i = 0; i < Nstars; ++i)
            {
                auto horizontal = std::uniform_real<double>(Left, Right);
                auto vertical = std::uniform_real<double>(Top, Bottom);

                uint32_t Life = std::uniform_int_distribution<int>(8, 16)(rd);
                Spawn(
                    horizontal(rd), vertical(rd),
                    cScreen, Life, 16 - Life, -1, TParticleType::PerfectLineTwinkle, P);

                //spawn also on second screen if the amount of players is <=3
                if (Screens == 2 && PlayersPlay <= 3)
                    Spawn(horizontal(rd), vertical(rd),
                        2, Life, 16 - Life, -1, TParticleType::PerfectLineTwinkle, P);
            }
        }
    }
}