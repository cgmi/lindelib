#include "../include/linde/TextRenderer.h"



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


    TextRenderer::TextRenderer(GLContext *glContext) :
        GLObject(glContext),
        m_fontSize(12),
        m_xscale(1.0f),
        m_yscale(1.0f),
        m_color(0.0f, 0.0f, 0.0f, 1.0f),
#ifdef OS_WIN
        m_font("arial.ttf"),
        m_fontPath("C:/Windows/Fonts/")
#elif defined __linux__
        m_font("FreeSans.ttf"),
        m_fontPath("/usr/share/fonts/truetype/freefont/")
#endif
        {
            init();
        }

    TextRenderer::~TextRenderer()
    {
        FT_Done_Face(m_face);
        FT_Done_FreeType(m_ft);

		glDeleteTextures(1, &m_tex);
    }

    GLvoid TextRenderer::init()
    {
        if (FT_Init_FreeType(&m_ft))
        {
            std::cerr << "Could not init freetype library - press enter to continue" << std::endl;
            std::cin.get();
        }

        std::stringstream stream;
        stream << m_fontPath << m_font;
        std::string fontLoc = stream.str();
        if (FT_New_Face(m_ft, fontLoc.c_str(), 0, &m_face))
        {
            std::cerr << "Could not open font " << fontLoc << " - press enter to continue" << std::endl;
            std::cin.get();
        }
        FT_Set_Pixel_Sizes(m_face, 0, m_fontSize);
	
		glGenTextures(1, &m_tex);
    }

	GLvoid TextRenderer::setColor(const  glm::vec4 & color)
    {
        m_color = color;
    }

    GLvoid TextRenderer::setFontSize(GLint size)
    {
        m_fontSize = size;
        FT_Set_Pixel_Sizes(m_face, 0, m_fontSize);
    }

    GLvoid TextRenderer::setFont(const std::string & font)
    {
        m_font = font;
        std::stringstream stream;
        stream << m_fontPath << m_font;
        std::string fontLoc = stream.str();
        if (FT_New_Face(m_ft, fontLoc.c_str(), 0, &m_face))
        {
            std::cerr << "Could not open font " << fontLoc << " - press enter to continue" << std::endl;
            std::cin.get();
        }
        FT_Set_Pixel_Sizes(m_face, 0, m_fontSize);
    }

    GLvoid TextRenderer::setScale(GLfloat x_scale, GLfloat y_scale)
    {
        m_xscale = x_scale;
        m_yscale = y_scale;
    }

	GLint TextRenderer::getFontSize() const
	{
		return m_fontSize;
	}

	glm::vec4 TextRenderer::getColor() const
	{
		return m_color;
	}


	GLvoid TextRenderer::render(const std::string & text, const  glm::vec2 & screenPos) const
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

        glColor4fv(&m_color[0]);
		  
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        enable2D();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_TEXTURE_2D);

        GLfloat x = screenPos[0];
        GLfloat y = screenPos[1];
        std::string::const_iterator it;
        FT_Error error;
        for (it = text.begin(); it < text.end(); it++)
        {
            error = FT_Load_Char(m_face, *it, FT_LOAD_RENDER);
            if (error)
            {
                std::cerr << "Could not load character:\t" << *it << " - error " << error << std::endl;
                continue;
            }

            FT_GlyphSlot g = m_face->glyph;

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_ALPHA,
                g->bitmap.width,
                g->bitmap.rows,
                0,
                GL_ALPHA,
                GL_UNSIGNED_BYTE,
                g->bitmap.buffer
                );

            GLfloat x2 = x + g->bitmap_left * m_xscale;
            GLfloat y2 = y - g->bitmap_top * m_yscale;
            GLfloat w = g->bitmap.width * m_xscale;
            GLfloat h = g->bitmap.rows * m_yscale;

            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(x2, y2);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(x2 + w, y2);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(x2 + w, y2 + h);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(x2, y2 + h);
            glEnd();

            x += (g->advance.x >> 6) * m_xscale;
            y += (g->advance.y >> 6) * m_yscale;
        }

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        disable2D();     

        glPopClientAttrib();
        glPopAttrib();
    }

}


