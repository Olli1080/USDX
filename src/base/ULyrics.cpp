#include "ULyrics.h"

#include "TextGL.h"

namespace ULyrics
{

    TLyricLine::TLyricLine()
    {
        Reset();
    };

    TLyricLine::~TLyricLine()
    {
    }

    void TLyricLine::Reset()
    {
        Start = 0;
        StartNote = 0;
        Length = 0;
        LastLine = false;

        Text = "";
        Width = 0;

        // duet mode: players of that line (default: all)
        Players = 0xFF;

        Words.clear();
        CurWord = -1;
    }


    //{ TLyricEngine }

    /**
    *Initializes the engine.
    */
    TLyricEngine::TLyricEngine(double ULX, double ULY, double ULW, double ULH, double LLX, double LLY, double LLW, double LLH)
    {

        BPM = 0;
        Resolution = 0;
        LCounter = 0;
        QueueFull = false;

        UpperLine = TLyricLine();
        LowerLine = TLyricLine();
        QueueLine = TLyricLine();

        LastDrawBeat = 0;

        UpperLineX = ULX;
        UpperLineW = ULW;
        UpperLineY = ULY;
        UpperLineH = ULH;

        LowerLineX = LLX;
        LowerLineW = LLW;
        LowerLineY = LLY;
        LowerLineH = LLH;

        LoadTextures();
    }


    /**
    *Frees memory.
    */
    TLyricEngine::~TLyricEngine()
    {
        UpperLine.Free;
        LowerLine.Free;
        QueueLine.Free;
    }

    /**
    *Clears all cached Song specific Information.
    */
    void TLyricEngine::Clear(double cBPM, int cResolution)
    {
        BPM = cBPM;
        Resolution = cResolution;
        LCounter = 0;
        QueueFull = false;

        LastDrawBeat = 0;
    };


    /**
    *Loads textures needed for the drawing the lyrics,
    * player icons, a ball for the ball effectand the lyric indicator.
    */
    void TLyricEngine::LoadTextures()
        /*var
        I : int;*/
    {
        // lyric indicator (bar that indicates when the line start)
        IndicatorTex = UTexture::Texture.LoadTexture(Skin.GeUTexture::TTextureFileName('LyricHelpBar'), UTexture::TEXTURE_TYPE_TRANSPARENT, 0xFF00FF);

        // ball for current word hover in ball effect
        BallTex = UTexture::Texture.LoadTexture(Skin.GeUTexture::TTextureFileName('Ball'), UTexture::TEXTURE_TYPE_TRANSPARENT, 0);

        // duet mode: load player icon
        for (size_t I = 0; I < UIni::IMaxPlayerCount; ++I)
        {
            PlayerIconTex[I][0] = UTexture::Texture.LoadTexture(Skin.GeUTexture::TTextureFileName('LyricIcon_P' + std::to_string(I + 1)), UTexture::TEXTURE_TYPE_TRANSPARENT, 0);
            PlayerIconTex[I][1] = UTexture::Texture.LoadTexture(Skin.GeUTexture::TTextureFileName('LyricIconD_P' + std::to_string(I + 1)), UTexture::TEXTURE_TYPE_TRANSPARENT, 0);
        }
    }

    /**
    *Adds LyricLine to queue.
    * The LyricEngine stores three lines in its queue :
    *UpperLine : the upper line displayed in the lyrics
    * LowerLine : the lower line displayed in the lyrics
    * QueueLine : an offscreen line that precedes LowerLine
    * If the queue is full the next call to AddLine will replace UpperLine with
    * LowerLine, LowerLine with QueueLineand QueueLine with the Line parameter.
    */
    void TLyricEngine::AddLine(UMusic::PLine Line)
        /*var
        LyricLine : TLyricLine;
        NoteIndex: int;*/
    {
        TLyricLine LyricLine;
        // only add lines, if there is space
        if (!IsQueueFull())
        {
            // set LyricLine to line to write to
            if (LineCounter() == 0)
                LyricLine = UpperLine;
            else if (LineCounter() == 1)
                LyricLine = LowerLine;
            else
            {
                // now the queue is full
                LyricLine = QueueLine;
                QueueFull = true;
            }
        }
        else
        {
        	// rotate lines (round-robin-like)
            LyricLine = UpperLine;
            UpperLine = LowerLine;
            LowerLine = QueueLine;
            QueueLine = LyricLine;
        }

        // reset line state
        LyricLine.Reset();

        // check if sentence has notes
        if (Line && !Line->Notes.empty())
        {
            const auto& line = *Line;
            const auto& notes = line.Notes;
            // copy values from SongLine to LyricLine
            LyricLine.Start     = line.StartBeat;
            LyricLine.StartNote = notes.front().StartBeat;
            LyricLine.Length    = notes.back().StartBeat + notes.back().Duration - notes.front().StartBeat;
            LyricLine.LastLine  = line.LastLine;

            // copy words
            LyricLine.Words.reserve(line.Notes.size());
            for (size_t NoteIndex = 0; NoteIndex < line.Notes.size(); ++NoteIndex)
            {
                auto& word = LyricLine.Words[NoteIndex];
                const auto& note = line.Notes[NoteIndex];

                word.Start = note.StartBeat;
                word.Length    = note.Duration;
                word.Text      = note.Text;
                word.Freestyle = note.NoteType == UMusic::ntFreestyle;

                LyricLine.Text += word.Text;
            }

            UpdateLineMetrics(LyricLine);
        }
        // increase the counter
        ++LCounter;
    }

    /**
    *Draws Lyrics.
    * Draw just manages the Lyrics, drawing is done by a call of DrawLyrics.
    * @param Beat : current Beat in Quarters
    */
    void TLyricEngine::Draw(double Beat)
    {
        DrawLyrics(Beat);
        LastDrawBeat = Beat;
    }

    /**
    *Main Drawing procedure.
    */
    void TLyricEngine::DrawLyrics(double Beat)
    {
        DrawLyricsLine(UpperLineX, UpperLineW, UpperLineY, UpperLineH, UpperLine, Beat);
        DrawLyricsLine(LowerLineX, LowerLineW, LowerLineY, LowerLineH, LowerLine, Beat);
    }

    /**
    *Draws a Player's icon.
    */
    void TLyricEngine::DrawPlayerIcon(uint8_t Player, bool Enabled, double X, double Y, double Size, double Alpha)
        /*var
        IEnabled : uint8_t;*/
    {
        uint8_t IEnabled;
        if (Enabled)
            IEnabled = 0;
        else
            IEnabled = 1;

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, PlayerIconTex[Player][IEnabled].TexNum);

        glColor4f(1, 1, 1, Alpha);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(X, Y);
        glTexCoord2f(0, 1); glVertex2f(X, Y + Size);
        glTexCoord2f(1, 1); glVertex2f(X + Size, Y + Size);
        glTexCoord2f(1, 0); glVertex2f(X + Size, Y);
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }

    /**
    *Draws the Ball over the LyricLine if needed.
    */
    void TLyricEngine::DrawBall(double XBall, double YBall, double Alpha)
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, BallTex.TexNum);

        glColor4f(1, 1, 1, Alpha);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(XBall - 10, YBall);
        glTexCoord2f(0, 1); glVertex2f(XBall - 10, YBall + 20);
        glTexCoord2f(1, 1); glVertex2f(XBall + 10, YBall + 20);
        glTexCoord2f(1, 0); glVertex2f(XBall + 10, YBall);
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }

    void TLyricEngine::DrawLyricsWords(TLyricLine LyricLine, double X, double Y, int StartWord, int EndWord)
        /*var
        I : int;
        PosX: double;
        CurWord: PLyricWord;*/
    {
		double PosX = X - LyricLine.Words[StartWord].X;
        

        // set word positions and line size and draw the line
        for (size_t I = StartWord; I <= EndWord; ++I)
        {
            const auto& CurWord = LyricLine.Words[I];
	        TextGL::SetFontItalic(CurWord.Freestyle);
	        TextGL::SetFontPos(PosX + CurWord.X, Y);
	        TextGL::glPrint(CurWord.Text);
        }
    }

    void TLyricEngine::UpdateLineMetrics(TLyricLine LyricLine)
        /*var
        I : int;
        PosX: double;
        TextSoFar: std::string;
        CurWord: PLyricWord;
        RequestWidth, RequestHeight: double;*/
    {
        double PosX = 0;
		std::string TextSoFar = "";

        // setup font
        TextGL::SetFontFamily(FontFamily);
        TextGL::SetFontStyle(FontStyle);
        TextGL::ResetFont();

        // check if line is lower or upper line and set sizes accordingly
        // Note: at the moment upper and lower lines have same width/height
        // and this function is just called by AddLine() but this may change
        // so that it is called by DrawLyricsLine().
        //if (LyricLine = LowerLine) then
        //{
        //  RequestWidth = LowerLineW;
        //  RequestHeight = LowerLineH;
        //end
        //else
        //{
        double RequestWidth = UpperLineW;
        double RequestHeight = UpperLineH;
        //};

        // set font size to a reasonable value
        LyricLine.Height = RequestHeight * 0.9;
        TextGL::SetFontSize(LyricLine.Height);
        LyricLine.Width = TextGL::glTextWidth(LyricLine.Text);

        // change font-size to fit into the lyric bar
        if (LyricLine.Width > RequestWidth)
        {
            LyricLine.Height = std::trunc(LyricLine.Height * (RequestWidth / LyricLine.Width));
            // the line is very loooong, set font to at least 1px
            LyricLine.Height = std::max(1.0, LyricLine.Height);

            TextGL::SetFontSize(LyricLine.Height);
            LyricLine.Width = TextGL::glTextWidth(LyricLine.Text);
        }

        // calc word positions and widths
        for (size_t I = 0; I < LyricLine.Words.size(); ++I)
        {
            auto& CurWord = LyricLine.Words[I];

            // - if current word is italic but not the next word get the width of the
            // italic font to avoid overlapping.
            if (CurWord.Freestyle)
	            TextGL::SetFontItalic(true);

            CurWord.Width = TextGL::glTextWidth(CurWord.Text);
			TextSoFar = TextSoFar + CurWord.Text;
            CurWord.X = PosX + TextGL::glTextWidth(TextSoFar) - CurWord.Width;
            TextGL::SetFontItalic(false);

            if (I + 1 < LyricLine.Words.size() &&
                CurWord.Freestyle != LyricLine.Words[I + 1].Freestyle)
            {
				PosX = CurWord.X + CurWord.Width;
				TextSoFar = "";
            }
        }
    }


    /**
    *Draws one LyricLine
    */
    void TLyricEngine::DrawLyricsLine(double X, double W, double Y, double H, TLyricLine Line, double Beat)
        /*
        var
        CurWord : PLyricWord;     // current word
        LastWord:       PLyricWord;     // last word in line
        NextWord:       PLyricWord;     // word following current word
        Progress:       double;           // progress of singing the current word
        LyricX, LyricY: double;           // left/top lyric position
        WordY: double;                    // word y-position
        LyricsEffect: TLyricsEffect;
        Alpha: double;    // alphalevel to fade out at end
        ClipPlaneEq: array[0..3] of GLdouble; // clipping plane for slide effect

        OutlineColor_act: TRGB;                  // outline color actual line
        OutlineColor_dis: TRGB;                  // outline color next line
        OutlineColor_en:  TRGB;                  // outline color sing line
        */
        /*{// duet mode
                IconSize: double;                 // size of player icons
        IconAlpha: double;                // alpha level of player icons
        }*/
    {
        // do not draw empty lines
        if (Line.Words.empty())
            return;
        /*
    {
        // duet mode
    IconSize: = (2 * Height);
    IconAlpha: = Frac(Beat / (Resolution * 4));

        DrawPlayerIcon(0, True, X, Y + (42 - IconSize) / 2, IconSize, IconAlpha);
        DrawPlayerIcon(1, True, X + IconSize + 1, Y + (42 - IconSize) / 2, IconSize, IconAlpha);
        DrawPlayerIcon(2, True, X + (IconSize + 1) * 2, Y + (42 - IconSize) / 2, IconSize, IconAlpha);
    }
    */
    // set font size and style
        TextGL::SetFontFamily(FontFamily);
        TextGL::SetFontStyle(FontStyle);
        TextGL::ResetFont();

        // hack to OptionsJukebox lyrics demo
        if (Display.CurrentScreen = @ScreenOptionsJukebox && FontStyle == 2)
            TextGL::SetFontSize(Line.Height * 0.8);
        else
            TextGL::SetFontSize(Line.Height);

        // set outline
        if (Display.CurrentScreen == @ScreenJukebox || Display.CurrentScreen == @ScreenOptionsJukebox)
        {
            if (UIni::Ini.juke_box_fill_colors.SingLineOutlineColor != 2)
                OutlineColor_act = UThemes::GetLyricOutlineColor(UIni::Ini.CurrentJukeboxSingLineOutlineColor);
            else
            {
                if (Display.CurrentScreen == @ScreenJukebox)
                    OutlineColor_act = ScreenJukeboxOptions.GetJukeboxOptionsLyricOtherOutlineColor(0);
                else
                    OutlineColor_act = UThemes::GetJukeboxLyricOtherOutlineColor(0);
            }

            if (UIni::Ini.juke_box_fill_colors.ActualLineOutlineColor != 2)
                OutlineColor_en = UThemes::GetLyricOutlineColor(UIni::Ini.CurrentJukeboxActualLineOutlineColor);
            else
            {
                if (Display.CurrentScreen == @ScreenJukebox)
                    OutlineColor_en = ScreenJukeboxOptions.GetJukeboxOptionsLyricOtherOutlineColor(1);
                else
                    OutlineColor_en = UThemes::GetJukeboxLyricOtherOutlineColor(1);
            }

            if (UIni::Ini.CurrentJukeboxNextLineOutlineColor != 2)
                OutlineColor_dis = GetLyricOutlineColor(UIni::Ini.CurrentJukeboxNextLineOutlineColor);
            else
            {
                if (Display.CurrentScreen == @ScreenJukebox)
                    OutlineColor_dis = ScreenJukeboxOptions.GetJukeboxOptionsLyricOtherOutlineColor(2);
                else
                    OutlineColor_dis = UThemes::GetJukeboxLyricOtherOutlineColor(2);
            }
        }
        else
        {
            OutlineColor_act = UThemes::GetLyricOutlineColor(0);
            OutlineColor_en = UThemes::GetLyricOutlineColor(0);
            OutlineColor_dis = UThemes::GetLyricOutlineColor(0);
        }

        // center lyrics
        LyricX = X + (W - Line.Width) / 2;
        LyricY = Y + (H - Line.Height) / 2;
        // get lyrics effect

        TLyricsEffect LyricsEffect;
        if (Display.CurrentScreen == @ScreenJukebox || 
            Display.CurrentScreen == @ScreenOptionsJukebox)
            LyricsEffect = TLyricsEffect(UIni::Ini.JukeboxEffect);
        else
            LyricsEffect = TLyricsEffect(UIni::Ini.LyricsEffect);

        double Alpha;
        if (Display.CurrentScreen != @ScreenJukebox && 
            Display.CurrentScreen != @ScreenOptionsJukebox)
            Alpha = 1.0;
        else
        {
            if (Display.CurrentScreen == @ScreenOptionsJukebox)
                Alpha = UIni::ILyricsAlphaVals[UIni::Ini.JukeboxAlpha];
            else
                Alpha = ScreenJukebox.LyricsAlpha;
        }

        // check if this line is active (at least its first note must be active)
        if (Beat >= Line.StartNote)
        {
            // if this line just got active, CurWord is -1,
            // this means we should try to make the first word active
            if (Line.CurWord == -1)
                Line.CurWord = 0;

            // check if the current active word is still active.
            // Otherwise proceed to the next word if there is one in this line.
            // Note: the max. value of Line.CurWord is High(Line.Words)
            if (Line.CurWord < Line.Words.size() - 1 &&
                Beat >= Line.Words[Line.CurWord + 1].Start)
            {
                ++Line.CurWord;
            }

            // determine current and last word in this line.
            // If the end of the line is reached use the last word as current word.
            auto& LastWord = Line.Words.back();
            auto& CurWord = Line.Words[Line.CurWord];
            if (Line.CurWord + 1 < Line.Words.size())
                NextWord = @Line.Words[Line.CurWord + 1];
            else
                NextWord = nil;

            // calc the progress of the lyrics effect
            double Progress = std::clamp((Beat - CurWord.Start) / CurWord.Length, 0.0, 1.0);

            // last word of this line finished, but this line did not hide -> fade out
            if (Display.CurrentScreen != @ScreenJukebox &&
                Display.CurrentScreen != @ScreenOptionsJukebox)
            {
                if (Line.LastLine &&
                    Beat > LastWord.Start + LastWord.Length)
                {
                    Alpha = 1 - (Beat - (LastWord.Start + LastWord.Length)) / 15;
                    if (Alpha < 0)
                        Alpha = 0;
                }
            }
            else
            {
                if (Display.CurrentScreen == @ScreenOptionsJukebox)
                    Alpha = UIni::ILyricsAlphaVals[UIni::Ini.JukeboxAlpha];
                else
                    Alpha = ScreenJukebox.LyricsAlpha;
            }

            // outline color
            TextGL::SetOutlineColor(OutlineColor_act.R, OutlineColor_act.G, OutlineColor_act.B, Alpha);

            // draw sentence before current word
            if (std::ranges::contains(std::set{ lfxSimple, lfxBall, lfxShift }, LyricsEffect))
                // only highlight current word and not that ones before in this line
                glColor4f(LineColor_en.R, LineColor_en.G, LineColor_en.B, Alpha);
            else
                glColor4f(LineColor_act.R, LineColor_act.G, LineColor_act.B, Alpha);

            DrawLyricsWords(Line, LyricX, LyricY, 0, Line.CurWord - 1);

            // draw rest of sentence (without current word)
            glColor4f(LineColor_en.R, LineColor_en.G, LineColor_en.B, Alpha);

            if (NextWord != nil)
            {
                // outline color
                TextGL::SetOutlineColor(OutlineColor_en.R, OutlineColor_en.G, OutlineColor_en.B, Alpha);

                DrawLyricsWords(Line, LyricX + NextWord.X, LyricY,
                    Line.CurWord + 1, Line.Words.size() - 1);
            }

            // outline color
            TextGL::SetOutlineColor(OutlineColor_act.R, OutlineColor_act.G, OutlineColor_act.B, Alpha);

            // draw current word
            if (std::ranges::contains(std::set{ lfxSimple, lfxBall, lfxShift }, LyricsEffect)
                // hack to OptionsJukebox lyrics demo
                || ((LyricsEffect = lfxSlide) && (Display.CurrentScreen == @ScreenOptionsJukebox)))
            {
                if (LyricsEffect == lfxShift)
                    WordY = LyricY - 8 * (1 - Progress);
                else
                    WordY = LyricY;

                // change the color of the current word
                glColor4f(LineColor_act.R, LineColor_act.G, LineColor_act.B, Alpha);

                DrawLyricsWords(Line, LyricX + CurWord.X, WordY, Line.CurWord, Line.CurWord);
            }
            // change color and zoom current word
            else if (LyricsEffect == lfxZoom)
            {
                glPushMatrix();

                // zoom at word center
                glTranslatef(LyricX + CurWord.X + CurWord.Width / 2,
                    LyricY + Line.Height / 2, 0);
                glScalef(1.0 + (1 - Progress) * 0.5, 1.0 + (1 - Progress) * 0.5, 1);

                glColor4f(LineColor_act.R, LineColor_act.G, LineColor_act.B, Alpha);

                DrawLyricsWords(Line, -CurWord.Width / 2, -Line.Height / 2, Line.CurWord, Line.CurWord);

                glPopMatrix();
            }
            // split current word into active and non-active part
            else if (LyricsEffect == lfxSlide)
            {
                // enable clipping and set clip equation coefficients to zeros
                glEnable(GL_CLIP_PLANE0);
                FillChar(ClipPlaneEq[0], SizeOf(ClipPlaneEq), 0);

                glPushMatrix();
                glTranslatef(LyricX + CurWord.X, LyricY, 0);

                // clip non-active right part of the current word
                ClipPlaneEq[0] = -1;
                ClipPlaneEq[3] = CurWord.Width * Progress;
                glClipPlane(GL_CLIP_PLANE0, @ClipPlaneEq);
                // and draw active left part
                glColor4f(LineColor_act.R, LineColor_act.G, LineColor_act.B, Alpha);

                DrawLyricsWords(Line, 0, 0, Line.CurWord, Line.CurWord);

                // clip active left part of the current word
                ClipPlaneEq[0] = -ClipPlaneEq[0];
                ClipPlaneEq[3] = -ClipPlaneEq[3];
                glClipPlane(GL_CLIP_PLANE0, @ClipPlaneEq);
                // and draw non-active right part
                glColor4f(LineColor_en.R, LineColor_en.G, LineColor_en.B, Alpha);

                DrawLyricsWords(Line, 0, 0, Line.CurWord, Line.CurWord);

                glPopMatrix();

                glDisable(GL_CLIP_PLANE0);
            }

            // draw the ball onto the current word
            if (LyricsEffect == lfxBall)
            {
                DrawBall(LyricX + CurWord.X + CurWord.Width * Progress,
                    LyricY - 15 - 15 * sin(Progress * std::numbers::pi), Alpha);
            }

        }
        else
        {
            // this section is called if the whole line can be drawn at once and no
            // word is highlighted.

            // enable the upper, disable the lower line
            if (Line == UpperLine)
            {
                // outline color
                TextGL::SetOutlineColor(OutlineColor_en.R, OutlineColor_en.G, OutlineColor_en.B, Alpha);

                glColor4f(LineColor_en.R, LineColor_en.G, LineColor_en.B, Alpha);
            }
            else
            {
                // outline color
                TextGL::SetOutlineColor(OutlineColor_dis.R, OutlineColor_dis.G, OutlineColor_dis.B, Alpha);

                glColor4f(LineColor_dis.R, LineColor_dis.G, LineColor_dis.B, Alpha);
            }
            DrawLyricsWords(Line, LyricX, LyricY, 0, High(Line.Words));
        }
        // reset
        TextGL::SetOutlineColor(0, 0, 0, -1);
    }

    /**
    *@returns a reference to the upper line
    */
    TLyricLine TLyricEngine::GetUpperLine() const
    {
        return UpperLine;
    }

    /**
    *@returns a reference to the lower line
    */
    TLyricLine TLyricEngine::GetLowerLine() const
    {
        return LowerLine;
    }

    /**
    *@returns the index of the upper line
    */
    int TLyricEngine::GetUpperLineIndex() const
    {
    	constexpr int QUEUE_SIZE = 3;
        // no line in queue
        if (LineCounter() <= 0)
            return -1;
        // no line has been removed from queue yet
        if (LineCounter() <= QUEUE_SIZE)
            return 0;
        // lines have been removed from queue already
        return LineCounter() - QUEUE_SIZE;
    }
}
