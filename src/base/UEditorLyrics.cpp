#include "UEditorLyrics.h"

#include "UMusic.h"

namespace UEditorLyrics
{

    /*    uses
        TextGL,
        UGraphic,
        UDrawTexture,
        Math,
        USkins;*/

    void TEditorLyrics::X(double Value)
    {
        XR = Value;
    }

    void TEditorLyrics::Y(double Value)
    {
        YR = Value;
    }

    double TEditorLyrics::ClientX() const
    {
        return Words[0].X;
    }

    void TEditorLyrics::Align(int Value)
    {
        AlignI = Value;
    }

    double TEditorLyrics::GetSize() const
    {
        return SizeR;
    }

    void TEditorLyrics::SetSize(double Value)
    {
        SizeR = Value;
    }

    void TEditorLyrics::SetSelected(int Value)
    {
        SelectedI = Value;
        Refresh();
    }

    void TEditorLyrics::FontFamily(int Value)
    {
        FontFamilyI = Value;
    }

    void TEditorLyrics::FontStyle(int Value)
    {
        FontStyleI = Value;
    }

    void TEditorLyrics::AddWord(std::string Text)
    {
        double x;

        if (Words.empty())
        {
            x = XR;
        }
        else
        {
            const auto& prevWord = Words.back();
            x = prevWord.X + prevWord.Width;
        }

        Words.emplace_back
        (
            x, YR,
            TextGL::glTextWidth(Text),
            Text,
            DColR, DColG, DColB
        );

        TextGL::SetFontFamily(FontFamilyI);
        TextGL::SetFontStyle(FontStyleI);
        TextGL::SetFontSize(SizeR);
        TextGL::SetFontItalic(Italic);

        Refresh();
    };

    void TEditorLyrics::AddLine(int CurrentTrack, int CurrentLine)
    {
        Clear();
        if (!UMusic::Tracks[CurrentTrack].Lines[CurrentLine].Notes.empty())
        {
            for (int CurrentNote = 0; CurrentNote <= UMusic::Tracks[CurrentTrack].Lines[CurrentLine].HighNote; ++CurrentNote)
            {
                Italic = UMusic::Tracks[CurrentTrack].Lines[CurrentLine].Notes[CurrentNote].NoteType = UMusic::ntFreestyle;
                AddWord(UMusic::Tracks[CurrentTrack].Lines[CurrentLine].Notes[CurrentNote].Text);
            }
            /*{}
            else
            {
                Italic = false;
                AddWord(' ', false);
                Text: = ' ';
            }
            */
        }
        Selected(-1);
    }

    void TEditorLyrics::Clear()
    {
        Words.clear();
        SelectedI = -1;
    }

    void TEditorLyrics::Refresh()
    {
        if (AlignI == 1) // center
        {
            double TotalWidth = 0;
            for (const auto& word : Words)
                TotalWidth += word.Width;

            Words[0].X = XR - TotalWidth / 2.0;
            for (size_t WordIndex = 1; WordIndex < Words.size(); ++WordIndex)
                Words[WordIndex].X = Words[WordIndex - 1].X + Words[WordIndex - 1].Width;
        }
    }

    void TEditorLyrics::Draw()
    {
        for (const auto& word : Words)
        {
            TextGL::SetFontFamily(FontFamilyI);
            TextGL::SetFontStyle(FontStyleI);
            TextGL::SetFontPos(word.X + 10.0 * ScreenX, word.Y);
            TextGL::SetFontSize(SizeR);
            TextGL::SetFontItalic(Italic);
            glColor3f(word.ColR, word.ColG, word.ColB);
            TextGL::glPrint(word.Text);
        }
    }

}