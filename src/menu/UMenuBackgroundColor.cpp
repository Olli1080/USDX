#include "UMenuBackgroundColor.h"

#include "../base/UGraphic.h"

namespace UMenuBackgroundColor
{
    TMenuBackgroundColor::TMenuBackgroundColor(const UThemes::TThemeBackground& ThemedSettings)
        : Color(ThemedSettings.Color)
    {
    }

    void TMenuBackgroundColor::Draw()
    {
        if (UGraphic::screen.Act != 1)
            return;

        //just clear once, even when using two screens
        glClearColor(Color.R, Color.G, Color.B, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}