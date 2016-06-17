#ifndef LINDE_TEXT_RENDERER_H
#define LINDE_TEXT_RENDERER_H


#include "lindeOpenGL.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace linde
{

/*
#################################################################################
#################################################################################
#################################################################################
################### text rendering ##############################################
#################################################################################
#################################################################################
#################################################################################
#################################################################################
#################################################################################
*/


    /**
    * @author Thomas Lindemeier
    * Use freetype library to create bitmaps from font files.
    * Render text on screen.
    */
    class TextRenderer
    {
        GLint m_fontSize;
        FT_Library m_ft;
        FT_Face m_face;

        GLfloat m_xscale;
        GLfloat m_yscale;
        glm::vec4 m_color;

        GLvoid init();

        std::string m_font;
        const std::string m_fontPath;

		GLuint m_tex;

    public:
        TextRenderer();
        ~TextRenderer();

		GLvoid render(const std::string & text, const  glm::vec2 & pos) const;

		GLvoid setColor(const  glm::vec4 & color);
        GLvoid setFontSize(GLint size);
        GLvoid setFont(const std::string & font);
        GLvoid setScale(GLfloat x_scale, GLfloat y_scale);

		GLint getFontSize() const;

		glm::vec4 getColor() const;
    };

} // namespace linde

#endif // LINDE_TEXT_RENDERER_H
