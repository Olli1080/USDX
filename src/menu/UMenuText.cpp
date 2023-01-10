#include "UMenuText.h"

#include <boost/algorithm/string/trim.hpp>

#include "../base/TextGL.h"
#include "../base/"

/*uses
        UGraphic,
        UDisplay,
        UUnicodeStringHelper,
    { $IFDEF MSWINDOWS }
        LazUTF8,
    { $ELSE }
        UUnicodeUtils,
    { $ENDIF }
    StrUtils;*/

namespace UMenuText
{
    void TText::SetSelect(bool Value)
    {
        SelectBool = Value;

		// set cursor visible
		SelectBlink = true;
		STicks = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) / 550;
    }

    void TText::SetText(const std::string& Value)
    /*var
        NextPos : cardinal;  // next pos of a space etc.
		LastPos:     cardinal;  // last pos "
		LastBreak:   cardinal;  // last break
		isBreak:     bool;   // true if the break is not caused because the text is out of the area
		FirstWord:   word;      // is first word after break?
		Len:         word;      // length of the tiles array
		DecFontSize: bool = true; // how to handle strings that are too long to fit a given height, either decrease font size or cut off with ellipsis
		*/
    {
        size_t NextPos;
        size_t LastPos;
        size_t LastBreak;
        bool isBreak;
        uint16_t Len;
        uint16_t FirstWord;

        bool DecFontSize = true;

        //TODO:: adapt strings because pascal string start with index 1

        auto GetNextPos = [&]() -> bool
        {
            LastPos = NextPos;

            size_t T1;
            // next space (if width is given)
            if (W > 0)
                T1 = Value.find_first_of(' ', LastPos);
            else
                T1 = Value.size();

            /*{// next -
            T2 = PosEx('-', Value, LastPos + 1); }*/

            // next break
            size_t T3 = Value.find_first_of('\n', LastPos);

            if (T1 == std::string::npos)
                T1 = Value.size();
            /*{if T2 = 0 then
                T2 = Length(Value); }*/
            if (T3 == std::string::npos)
                T3 = Value.size();

            // get nearest pos
            NextPos = std::min(T1, T3/*{ min(T2, T3) }*/);

            if (LastPos == Value.size())
                NextPos = 0;

            isBreak = NextPos == T3 && NextPos != Value.size();
            return NextPos != 0;
        };

        auto AddBreak = [&](size_t From, size_t bTo) -> void
        {
            if (isBreak || bTo - From >= 1)
            {
                ++Len;
                TextTiles.emplace_back(boost::algorithm::trim_copy(Value.substr(From, bTo - From)));

                LastBreak = bTo + (isBreak ? 2 : 1);
                FirstWord = 0;
            }
        };

        isBreak = false;
        // set TextString
		TextString = Value;

        // set cursor visible
		SelectBlink = true;
		STicks = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) / 550;

        // exit if there is no need to create tiles
        if (W <= 0 && Value.find_first_of('\n') == std::string::npos)
        {
            TextTiles = { Value };
			return;
        }

        // create tiles
        // reset text array
        TextTiles.clear();
		Len = 0;

        // reset counter vars
		LastPos = 0;
		NextPos = 0;
		LastBreak = 0;
		FirstWord = 0;

        if (W > 0)
        {
            // set font properties
            TextGL::SetFontFamily(Font);
            TextGL::SetFontStyle(Style);
            TextGL::SetFontSize(Size);
        }

        // go through text
        while (GetNextPos())
        {
            // break in text
            if (isBreak)
            {
                // look for break before the break
                if (TextGL::glTextWidth(Value.substr(LastBreak, NextPos - LastBreak + 1)) > W && NextPos - LastPos > 1)
                {
                    isBreak = false;
					// not the first word after break, so we don't have to break within a word
					if (FirstWord > 1)
					{
						// add break before actual position, because there the text fits the area
						AddBreak(LastBreak, LastPos);
					}
					else // first word after break break within the word
					{
						// to do
						// AddBreak(LastBreak, LastBreak + 155);
					}
				}
			    isBreak= true;
		        // add break from text
		        AddBreak(LastBreak, NextPos);
            }
            // text comes out of the text area -> create break
            else if (TextGL::glTextWidth(Value.substr(LastBreak, NextPos - LastBreak + 1)) > W)
            {
                // not the first word after break, so we don't have to break within a word
                if (FirstWord > 1)
                {
                    // add break before actual position, because there the text fits the area
                    AddBreak(LastBreak, LastPos);
                }
                else // first word after break -> break within the word
                {
                    // to do
                    // AddBreak(LastBreak, LastBreak + 155);
                }
			}
        //}
            ++FirstWord;
            }
        // add ending
        AddBreak(LastBreak, Value.size() + 1);

        // if a height is given, there are only two ugly solutions (but prettier than overlapping text, e.g. artist/title in song menu)
        // a) decrease font size
        // b) cut off the last lines
        // until the text fits within the given height
        if (H > 0 && Len * Size > H)
        {
            if (DecFontSize)
            {
                Size = Size - 1; // round(Size * 0.95 * 10) / 10;
                SetText(Value);
            }
            else
            {
                Len = std::trunc(H / Size);
                TextTiles.resize(Len);
				TextTiles.back() += "...";
			}
        }
    }

        void TText::DeleteLastLetter()
        {
            SetText(TextString.substr(0, TextString.size() - 1));
        }

        void TText::Draw()
        /*var
            X2, Y2: real;
        tmpText2, Text2:  UTF8String;
    I:      integer;
    Ticks:  cardinal;*/
        {
            if (Visible)
            {
	            TextGL::SetFontFamily(Font);
	            TextGL::SetFontStyle(Style);
	            TextGL::SetFontSize(Size);
	            TextGL::SetFontItalic(false);

				glColor4f(ColR * Int, ColG * Int, ColB * Int, Alpha);

				// reflection
                if (Reflection)
                    TextGL::SetFontReflection(true, ReflectionSpacing);
				else
					TextGL::SetFontReflection(false, 0);

        // if selected set blink...
        if (SelectBool)
        {
            const auto Ticks = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) / 550;
			if (Ticks != STicks)
            {
				// change visibility
				STicks = Ticks;
				SelectBlink = !SelectBlink;
			}
        }

        /*{if (false) // no width set draw as one long string
            {
            if (!(SelectBool AND SelectBlink))
                Text2 = Text
            else
                Text2 = Text + '|';

        case Align of
        0: X2= X;
            1: X2= X - glTextWidth(Text2) / 2;
            2: X2= X - glTextWidth(Text2);
            }

            SetFontPos(X2, Y);
            glPrint(Text2);
            SetFontStyle(ftNormal); // reset to default
            end
        else
            {}*/
        // now use always:
        // draw text as many strings
		Y2 = Y + MoveY;
        for I = 0 to High(TextTiles)
            {
            tmpText2 = TextTiles[I];

        if (not (SelectBool and SelectBlink)) or (I <> High(TextTiles)) then
            {
            Text2 = TextTiles[I];
        end
        else
            {
            if (Writable) then
                Text2 = TextTiles[I] + '|'
            else
                Text2 = TextTiles[I];
        }

        case Align of
        1: X2= X + MoveX - glTextWidth(tmpText2) / 2; // centered
        2: X2= X + MoveX - glTextWidth(tmpText2); // right aligned 
        else X2 = X + MoveX; // left aligned(default)
        }

        SetFontPos(X2, Y2);

        SetFontZ(Z);

        glPrint(Text2);

        /*{if Size >= 10 then
            Y2 = Y2 + Size * 0.93
        else}*/
        if (Style = ftBold) then
            Y2 = Y2 + Size * 0.93
        else
            Y2 = Y2 + Size * 0.85;
        }

        // reset to default
        SetFontFamily(0);
        SetFontStyle(ftRegular);

        //}
        }
        }

        TText::TText()
        {
            Create(0, 0, '');
        }

        TText::TText(double X, double Y, const std::string& Text)
        {
            Create(X, Y, 0, 0, 0, ftRegular, 30, 0, 0, 0, 0, Text, false, 0, 0, false);
        }

        TText::TText(double ParX, double ParY, double ParW, double ParH, int ParFont, int ParStyle, double ParSize, double ParColR, double ParColG, double ParColB, int ParAlign, const std::string& ParText, bool ParReflection, double ParReflectionSpacing, double ParZ, bool Writable)
        {
            inherited Create;
		    Alpha= 1;
		    X= ParX;
		    Y= ParY;
		    W= ParW;
		    H= ParH;
		    Z= ParZ;
		    Font= ParFont;
		    Style= ParStyle;
		    Size= ParSize;
		    Text= ParText;
		    ColR= ParColR;
		    ColG= ParColG;
		    ColB= ParColB;
		    Int= 1;
		    Align= ParAlign;
		    SelectBool= false;
		    Visible= true;
		    Reflection= ParReflection;
		    ReflectionSpacing= ParReflectionSpacing;
		    Writable = Writable;
        }

        UMenuInteract::TMouseOverRect TText::GetMouseOverArea()
        /*var
            W1 : real;*/
        {
            if (!Display.Cursor_HiddenByScreen) then
                {
                if (Align = 0) then
                    {
                    Result.X = X;
        Result.Y = Y;
        Result.W = glTextWidth(Text);
        Result.H = Size;
        }

        if (Align = 1) then
            {
            Result.X = X - glTextWidth(Text) / 2;
        Result.Y = Y;
        Result.W = glTextWidth(Text);
        Result.H = Size;
        }

        if (Align = 2) then
            {
            if (W < > 0) then
                Result.X = X - W
            else
                Result.X = X - glTextWidth(Text);

        Result.Y = Y;

        if (W < > 0) then
            Result.W = W
        else
            Result.W = glTextWidth(Text);

        Result.H = Size;
        }
        }
        }
}