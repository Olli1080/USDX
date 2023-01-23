#include "UDrawTexture.h"

#include <gl/GL.h>

namespace UDrawTexture
{

    void DrawLine(const UTexture::TTextureCoords& coords, const UCommon::TRGB<double>& Col)
    {
        glColor3f(Col.R, Col.G, Col.B);
        glBegin(GL_LINES);
        glVertex2f(coords.X1, coords.Y1);
        glVertex2f(coords.X2, coords.Y2);
        glEnd();
    }

    void DrawQuad(const UTexture::Position2D& position, const UTexture::Size& size, const UCommon::TRGB<double>& Col)
    {
        glColor3f(Col.R, Col.G, Col.B);
        glBegin(GL_QUADS);
        glVertex2f(position.X, position.Y);
        glVertex2f(position.X, position.Y + size.H);
        glVertex2f(position.X + size.W, position.Y + size.H);
        glVertex2f(position.X + size.W, position.Y);
        glEnd();
    }

    void DrawTexture(const UTexture::TextureWrapper& texture)
    {
        const auto Col = UCommon::TRGBA::intensity(texture.Color, texture.Int);

        //with Texture
        //{
        glColor4f(Col.R, Col.G, Col.B, Col.A);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glDepthRange(0, 10);
        glDepthFunc(GL_LEQUAL);
        //    glDepthFunc(GL_GEQUAL);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //    glBlendFunc(GL_SRC_COLOR, GL_ZERO);

        glBindTexture(GL_TEXTURE_2D, texture.TexNum);

        const auto& Rot = texture.Rot;
        const auto& x = texture.position.X;
        const auto& y = texture.position.Y;

        const auto& w = texture.size.W;
        const auto& h = texture.size.H;

        const auto& scaleW = texture.sizeScale.H;
        const auto& scaleH = texture.sizeScale.H;

        double xN[4] = { x, x, x + w * scaleW, x + w * scaleW };
        double yN[4] = { y, y + h * scaleH, y + h * scaleH, y };

        if (Rot != 0)
        {
            const double midX = x + w / 2;
            const double midY = y + h / 2;

            const double sinRot = sin(Rot);
            const double cosRot = cos(Rot);

#pragma unroll
            for (size_t i = 0; i < 4; ++i)
            {
                const double xt = xN[i] - midX;
                const double yt = yN[i] - midY;

                xN[i] = midX + xt * cosRot - yt * sinRot;
                yN[i] = midY + yt * cosRot + xt * sinRot;
            }
        }
        /*{
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);    glVertex3f(x1, y1, z);
            glTexCoord2f(0, TexH); glVertex3f(x2, y2, z);
            glTexCoord2f(TexW, TexH); glVertex3f(x3, y3, z);
            glTexCoord2f(TexW, 0);    glVertex3f(x4, y4, z);
            glEnd;
        }
        {
            glBegin(GL_QUADS);
            glTexCoord2f(TexX1 * TexW, TexY1 * TexH); glVertex3f(x1, y1, z);
            glTexCoord2f(TexX1 * TexW, TexY2 * TexH); glVertex3f(x2, y2, z);
            glTexCoord2f(TexX2 * TexW, TexY2 * TexH); glVertex3f(x3, y3, z);
            glTexCoord2f(TexX2 * TexW, TexY1 * TexH); glVertex3f(x4, y4, z);
            glEnd;
        }*/
        double X1 = texture.coords.X1 * texture.ratio.W;
        double X2 = texture.coords.X2 * texture.ratio.W;

        double Y1 = texture.coords.Y1 * texture.ratio.H;
        double Y2 = texture.coords.Y2 * texture.ratio.H;

        double leftScaled = yN[1] * (1.0 - texture.extentScale.left);
        double rightScaled = yN[1] * (1.0 - texture.extentScale.right);

        const auto& z = texture.Z;

        glBegin(GL_QUADS);
        glTexCoord2f(X1, Y1);
        glVertex3f(xN[0], yN[0] + leftScaled, z);
        glTexCoord2f(X1, Y2);
        glVertex3f(xN[1], yN[1] - leftScaled, z);
        glTexCoord2f(X2, Y2);
        glVertex3f(xN[2], yN[2] - rightScaled, z);
        glTexCoord2f(X2, Y1);
        glVertex3f(xN[3], yN[3] + rightScaled, z);
        glEnd();
        //}
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
    }
}