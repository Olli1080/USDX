#include "UMenuStatic.h"

#include <gl/GL.h>

#include "../base/UCommon.h"
#include "UDisplay.h"
#include "UDrawTexture.h"

namespace UMenuStatic
{
	void TStatic::Draw()
	{
		if (!Visible)
			return;

		UDrawTexture::DrawTexture(*Texture);

		//Reflection Mod
		if (!Reflection) // Draw Reflections
			return;

		//Bind Tex and GL Attributes
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);

		glDepthRange(0, 10);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, Texture->TexNum);

		auto Col = UCommon::TRGBA::intensity(Texture->Color, Texture->Int);
		auto X1 = Texture->coords.X1 * Texture->ratio.W;
		auto X2 = Texture->coords.X2 * Texture->ratio.W;

		auto Y1 = Texture->coords.Y1 * Texture->ratio.H * 0.5;
		auto Y2 = Texture->coords.Y2 * Texture->ratio.H;

		auto xEnd = Texture->position.X + Texture->size.W * Texture->sizeScale.W;

		auto yStart = Texture->position.Y + Texture->size.H * Texture->sizeScale.H + Reflectionspacing;
		auto yEnd = yStart + Texture->size.H * Texture->sizeScale.H / 2.0;

		//Draw
		glBegin(GL_QUADS);//Top Left
		glColor4f(Col.R, Col.G, Col.B, Texture->Color.A - 0.3);
		glTexCoord2f(X1, Y2);
		glVertex3f(Texture->position.X, yStart, Texture->Z);

		//Bottom Left
		glColor4f(Col.R, Col.G, Col.B, 0);
		glTexCoord2f(X1, Y1);
		glVertex3f(Texture->position.X, yEnd, Texture->Z);


		//Bottom Right
		glColor4f(Col.R, Col.G, Col.B, 0);
		glTexCoord2f(X2, Y1);
		glVertex3f(xEnd, yEnd, Texture->Z);

		//Top Right
		glColor4f(Col.R, Col.G, Col.B, Texture->Color.A - 0.3);
		glTexCoord2f(X2, Y2);
		glVertex3f(xEnd, yStart, Texture->Z);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}

	TStatic::TStatic(const UTexture::TextureWrapper::SPtr& Texture)
		: Texture(Texture)
	{}

	std::optional<UMenuInteract::TMouseOverRect> TStatic::GetMouseOverArea() const
	{
		if (UDisplay::Display.Cursor_HiddenByScreen)
			return std::nullopt;

		return UMenuInteract::TMouseOverRect{ Texture->position, Texture->size };
	}
}