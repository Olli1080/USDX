#pragma once
/** UltraStar Deluxe - Karaoke Game
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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UGraphicClasses.pas $
 * $Id: UGraphicClasses.pas 2760 2010-12-15 10:02:08Z brunzelchen $
 **/

#include <vector>
#include <array>
#include <random>

#include "UCommon.h"
#include "UIni.h"

namespace UGraphicClasses
{
    /*uses
        UTexture,
        ULog,
        UIni,
        SDL2;*/

    constexpr auto DelayBetweenFrames = std::chrono::milliseconds(60); //TODO:: this assumes 15-16fps!
    constexpr int FramesAlive = 16;
    //type

    enum class TParticleType
    {
		GoldenNote, PerfectNote, NoteHitTwinkle, PerfectLineTwinkle, ColoredStar, Flare
    };

    typedef UCommon::TRGB<float> TColor3f;
    static inline std::random_device rd;

    struct ParticleLayer
    {
        TColor3f Col; // Color(s) of particle
        double Scale; // Scaling factors of particle layers
    };

    class TParticle
    {
    public:

        double X, Y;     //Position
		int Screen;
        uint32_t W, H; //dimensions of particle
        std::vector<ParticleLayer> Layers;
		//std::vector<TColor3f> Col; 
		//std::vector<double> Scale;      
	 	uint8_t Frame;     //act. Frame
        uint32_t Tex; //Tex num from Textur Manager
        uint8_t Live;     //How many Cycles before Kill
        int RecIndex;  //To which rectangle this particle belongs (only GoldenNote)
        TParticleType StarType;  // GoldenNote | PerfectNote | NoteHitTwinkle | PerfectLineTwinkle
        double Alpha;     // used for fading...
        double mX, mY;     // movement-vector for PerfectLineTwinkle
		double SizeMod;     // experimental size modifier
		bool SurviveSentenceChange;

        TParticle(double cX, double cY, int cScreen, uint8_t cLive, int cFrame, int cRecArrayIndex, TParticleType cStarType,
            uint32_t Player);

        double neatFadeInAndOut();
        double LinearFadeOut();
        double neatSizing();

        void Draw();
        void LiveOn();
    };

    struct RectanglePositions
    {
        double xTop, yTop, xBottom, yBottom;
	    int TotalStarCount;
		int CurrentStarCount;
		int	Screen;
    };

    struct PerfectNotePositions
    {
        double xPos, yPos;
		int Screen;
    };

    class TEffectManager
    {
    public:

        std::list<TParticle> Particle;
        std::chrono::system_clock::time_point LastTime;
        std::vector<RectanglePositions> RecArray;

		std::array<double, UIni::IMaxPlayerCount> TwinkleArray; // store x-position of last twinkle for every player
		std::vector<PerfectNotePositions> PerfNoteArray;

        UTexture::TextureWrapper::SPtr FlareTex;

        TEffectManager();
        //destructor  Destroy; override;
        void Draw();
        std::list<TParticle>::iterator Spawn(double X, double Y,
            int Screen,
            uint8_t Live,
			int StartFrame,
			int RecArrayIndex,  // this is only used with GoldenNotes
            TParticleType StarType,
            uint32_t Player         // for PerfectLineTwinkle
        );
        void SpawnRec();
        std::list<TParticle>::iterator Kill(std::list<TParticle>::iterator index);
        void KillAll();
        void SentenceChange(int CP);
        void SaveGoldenStarsRec(double Xtop, double Ytop, double Xbottom, double Ybottom);
        void SavePerfectNotePos(double Xtop, double Ytop);
        void GoldenNoteTwinkle(double Top, double Bottom, double Right, int Player);
        void SpawnPerfectLineTwinkle();
    };

    inline TEffectManager GoldenRec;
}