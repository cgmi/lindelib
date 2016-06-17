#include "../include/linde/Texture.h"

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
Texture::Texture(GLContext *glContext) :
    GLObject(glContext),
    m_id(0),
    m_width(0),
    m_height(0),
    m_target(GL_TEXTURE_2D),
    m_mipLevel(0),
    m_internalFormat(GL_RGBA),
    m_format(GL_RGBA),
    m_border(0),
    m_type(GL_UNSIGNED_BYTE),
    m_minFilter(GL_LINEAR_MIPMAP_LINEAR),
    m_magFilter(GL_LINEAR),
    m_wrap(GL_CLAMP),
    m_envMode(GL_REPLACE),
    m_createMipMaps(GL_FALSE),
    m_maxAnisotropy(16.0f),
	m_manualMipMaps(GL_FALSE),
	m_created(GL_FALSE)
{

}

//Create empty texture
Texture::Texture(GLContext * glContext,
                 GLsizei width, GLsizei height,
    GLint internalFormat, GLenum format, GLint type,
    GLint minFilter, GLint magFilter,
    GLint envMode, GLint wrapMode) :
    GLObject(glContext),
    m_id(0),
    m_width(width),
    m_height(height),
    m_target(GL_TEXTURE_2D),
    m_mipLevel(0),
    m_internalFormat(internalFormat),
    m_format(format),
    m_border(0),
    m_type(type),
    m_minFilter(minFilter),
    m_magFilter(magFilter),
    m_wrap(wrapMode),
    m_envMode(envMode),
    m_createMipMaps(GL_FALSE),
    m_maxAnisotropy(1.0f),
    m_manualMipMaps(GL_FALSE),
	m_created(GL_FALSE)
{

}

void Texture::update(GLsizei width, GLsizei height,
    GLint internalFormat, GLenum format, GLint type,
    GLint minFilter, GLint magFilter,
    GLint envMode, GLfloat wrapMode)
{
    m_width = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_format = format;
    m_type = type;
    m_minFilter = minFilter;
    m_magFilter = magFilter;
    m_envMode = envMode;
    m_wrap = wrapMode;
}

Texture::~Texture()
{
    deleteTex();
}


void Texture::upload(void * data)
{
	if (!m_created)
	{
		create(data);
		return;
	}

    if (!m_id)
    {
        std::cerr << "create texture first" << std::endl;
    }

    bind();

    if (m_createMipMaps)
    {
        glTexParameteri(m_target, GL_GENERATE_MIPMAP, GL_TRUE);
    }
    glTexImage2D(m_target, m_mipLevel, m_internalFormat, m_width, m_height, m_border, m_format, m_type, data);


    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, m_minFilter);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, m_magFilter);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_envMode);

    glTexParameterf(m_target, GL_TEXTURE_WRAP_S, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_T, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_R, m_wrap);

    glTexParameterf(m_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);

	unbind();
}


void Texture::create(void * data)
{
    glGenTextures(1, &m_id);
    glBindTexture(m_target, m_id);

    if (m_createMipMaps)
    {
        glTexParameteri(m_target, GL_GENERATE_MIPMAP, GL_TRUE);
    }
    glTexImage2D(m_target, m_mipLevel, m_internalFormat, m_width, m_height, m_border, m_format, m_type, data);


    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, m_minFilter);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, m_magFilter);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_envMode);

    glTexParameterf(m_target, GL_TEXTURE_WRAP_S, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_T, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_R, m_wrap);

    glTexParameterf(m_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);

	m_created = GL_TRUE;
}

void Texture::bind()
{
    glBindTexture(m_target, m_id);
}

void Texture::bindLocationUnit(GLuint unit, GLenum access, GLint level, GLboolean layered, GLint layer)
{
    glBindImageTexture(unit, m_id, level, layered, layer, access, m_internalFormat);
}


void Texture::unbind()
{
    glBindTexture(m_target, 0);
}


void Texture::deleteTex()
{
    if (m_id != 0)
    {
        glDeleteTextures(1, &m_id);
    }
}

GLuint Texture::id() const
{
    return m_id;
}

GLint Texture::getInternalFormat() const
{
    return m_internalFormat;
}

GLenum Texture::getFormat() const
{
    return m_format;
}

GLint Texture::getBorder() const
{
    return m_border;
}

GLenum Texture::getType() const
{
    return m_type;
}

GLint Texture::getMinFilter() const
{
    return m_minFilter;
}

GLint Texture::getMagFilter() const
{
    return m_magFilter;
}

GLfloat Texture::getWrap() const
{
    return m_wrap;
}

GLint Texture::getEnvMode() const
{
    return m_envMode;
}

GLenum Texture::getTarget() const
{
    return m_target;
}

void Texture::setWrapMode(GLfloat wrap)
{
    m_wrap = wrap;
    
    bind();

    glTexParameterf(m_target, GL_TEXTURE_WRAP_S, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_T, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_R, m_wrap);

	unbind();
}

void Texture::setEnvMode(GLint envMode)
{
    m_envMode = envMode;

    bind();

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_envMode);

	unbind();
}

void Texture::setFilter(GLint minFilter, GLint magFilter)
{
    m_minFilter = minFilter;
    m_magFilter = magFilter;

    bind();

    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, m_minFilter);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, m_magFilter);

	unbind();
}

void Texture::setMaxIsotropy(GLfloat anisotropy)
{
    m_maxAnisotropy = anisotropy;

    bind();

    glTexParameterf(m_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);

	unbind();
}


void Texture::render(GLfloat posX, GLfloat posY, GLfloat width, GLfloat height)
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
