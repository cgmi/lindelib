#ifndef LINDE_TEXTURE_H
#define LINDE_TEXTURE_H

#include "GLContext.h"

namespace linde
{

/**
* @author Thomas Lindemeier, Sören Pirk
*/
class Texture : public GLObject
{
public:
    Texture(GLContext * glContext);
    Texture(GLContext *glContext, GLsizei width, GLsizei height,
            GLint internalFormat = GL_RGB32F, GLenum format = GL_RGB, GLint type = GL_FLOAT,
            GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR,
            GLint envMode = GL_REPLACE, GLint wrapMode = GL_REPEAT);
    ~Texture();

    void bind();
    // used for binding at shader location for write andor read operations, acess:: GL_READ_ONLY, GL_WRITE_ONLY, or GL_READ_WRITE
    void bindLocationUnit(GLuint unit, GLenum access = GL_READ_WRITE, GLint level = 0, GLboolean layered = GL_TRUE, GLint layer = 0);
    void unbind();
    void create(void* data = nullptr);
    void upload(void* data);
    void update(GLsizei width, GLsizei height,
                GLint internalFormat = GL_RGB32F, GLenum format = GL_RGB, GLint type = GL_FLOAT,
                GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR,
                GLint envMode = GL_REPLACE, GLfloat wrapMode = GL_REPEAT);

    void setWrapMode(GLfloat wrap);
    void setEnvMode(GLint envMode);
    void setFilter(GLint minFilter, GLint magFilter);
    void setMaxIsotropy(GLfloat anisotropy);

    void deleteTex();

    void render(GLfloat posX, GLfloat posY, GLfloat width, GLfloat height);

    GLuint id() const;
    GLuint width() const { return m_width; }
    GLuint height() const { return m_height; }

    GLboolean isCreated() const { return m_created; }

    GLint getInternalFormat() const;

    GLenum getFormat() const;

    GLint getBorder() const;

    GLenum getType() const;

    GLint getMinFilter() const;

    GLint getMagFilter() const;

    GLfloat getWrap() const;

    GLint getEnvMode() const;

    GLenum getTarget() const;

private:
    GLuint  m_id;
    GLsizei m_width;
    GLsizei m_height;
    GLenum  m_target;
    GLint   m_mipLevel;
    GLint   m_internalFormat;
    GLenum  m_format;
    GLint   m_border;
    GLenum  m_type;
    GLint   m_minFilter;
    GLint   m_magFilter;
    GLfloat m_wrap;
    GLint   m_envMode;
    GLboolean m_createMipMaps;
    GLfloat m_maxAnisotropy;
    GLboolean m_manualMipMaps;

    GLboolean m_created;
};

}// namespace linde

#endif // LINDE_TEXTURE_H
