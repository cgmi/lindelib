#include "../include/linde/TextureMultisample.h"
#include "../include/linde/GLWindow.h"


namespace linde
{
/*
#################################################################################
#################################################################################
#################################################################################
################### texture #####################################################
#################################################################################
#################################################################################
#################################################################################
#################################################################################
#################################################################################
*/
TextureMultisample::TextureMultisample()
    : m_id(0),
    m_target(GL_TEXTURE_2D_MULTISAMPLE),
	m_samples(1),
    m_width(0),
    m_height(0),
    m_internalFormat(GL_RGBA),
    m_envMode(GL_REPLACE),
	m_created(GL_FALSE)
{

}

//Create empty texture
TextureMultisample::TextureMultisample(GLsizei width, GLsizei height, GLsizei numSamples, GLint internalFormat, GLboolean fixedSampleLocations, GLint envMode)
    : m_id(0),
    m_target(GL_TEXTURE_2D_MULTISAMPLE),
	m_samples(numSamples),
    m_width(width),
    m_height(height),
	m_fixedSampleLocations(fixedSampleLocations),
    m_internalFormat(internalFormat),
    m_envMode(envMode),
	m_created(GL_FALSE)
{

}

void TextureMultisample::update(GLsizei width, GLsizei height, GLsizei numSamples, GLint internalFormat, GLboolean fixedSampleLocations, GLint envMode)
{
	m_samples = numSamples;
    m_internalFormat = internalFormat;
	m_width = width;
	m_height = height;
	m_fixedSampleLocations = fixedSampleLocations;
    m_envMode = envMode;
}

TextureMultisample::~TextureMultisample()
{
    deleteTex();
}

void TextureMultisample::create()
{
    glGenTextures(1, &m_id);
    glBindTexture(m_target, m_id);

	glTexImage2DMultisample(m_target, m_samples, m_internalFormat, m_width, m_height, m_fixedSampleLocations);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_envMode);

	m_created = GL_TRUE;
}

void TextureMultisample::bind()
{
    glBindTexture(m_target, m_id);
}

void TextureMultisample::bindLocationUnit(GLuint unit, GLenum access, GLint level, GLboolean layered, GLint layer)
{
    glBindImageTexture(unit, m_id, level, layered, layer, access, m_internalFormat);
}


void TextureMultisample::unbind()
{
    glBindTexture(m_target, 0);
}


void TextureMultisample::deleteTex()
{
    if (m_id != 0)
    {
        glDeleteTextures(1, &m_id);
    }
}

GLuint TextureMultisample::id() const
{
    return m_id;
}

GLint TextureMultisample::getInternalFormat() const
{
    return m_internalFormat;
}

GLint TextureMultisample::getEnvMode() const
{
    return m_envMode;
}

GLenum TextureMultisample::getTarget() const
{
    return m_target;
}

void TextureMultisample::setEnvMode(GLint envMode)
{
    m_envMode = envMode;

    bind();

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_envMode);

	unbind();
}

void TextureMultisample::render(GLfloat posX, GLfloat posY, GLfloat width, GLfloat height)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // deactivate color clamping
    glClampColor(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
    glClampColor(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);
    glClampColor(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glColor4f(1.0, 1.0, 1.0, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    bind();

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glDisable(GL_DEPTH_TEST);

    enable2D();
    glPushMatrix();
    glTranslatef(posX, posY, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(width, 0.0f, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(width, height, 0.0f);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.0, height, 0.0f);
    glEnd();
    glPopMatrix();
    disable2D();

	unbind();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    glPopAttrib();
}

} // namespace linde
