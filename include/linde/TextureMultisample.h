#ifndef LINDE_TEXTURE_MULTISAMPLING_H
#define LINDE_TEXTURE_MULTISAMPLING_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linde.h"

namespace linde
{

class GLWindow;
class FrameBufferObject;

/**
* @author Marc Spicker, Thomas Lindemeier, Sören Pirk
* @date 03.06.2016
*/
class TextureMultisample
{
    friend class GLWindow;
    friend class FrameBufferObject;

public:
	TextureMultisample();
	TextureMultisample(GLsizei width, GLsizei height, GLsizei numSamples, GLint internalFormat = GL_RGBA, GLboolean fixedSampleLocations = false, GLint envMode = GL_REPLACE);


    ~TextureMultisample();

	void bind();
	// used for binding at shader location for write andor read operations, acess:: GL_READ_ONLY, GL_WRITE_ONLY, or GL_READ_WRITE
	void bindLocationUnit(GLuint unit, GLenum access = GL_READ_WRITE, GLint level = 0, GLboolean layered = GL_TRUE, GLint layer = 0);
    void unbind();
    void create();
    void update(GLsizei width, GLsizei height, GLsizei numSamples, GLint internalFormat = GL_RGBA, GLboolean fixedSampleLocations = false, GLint envMode = GL_REPLACE);

    void setEnvMode(GLint envMode);

    void deleteTex();

    GLuint id() const;
    GLuint width() const { return m_width; }
    GLuint height() const { return m_height; }

	GLboolean isCreated() const { return m_created; }

    GLint getInternalFormat() const;

    GLint getEnvMode() const;

    GLenum getTarget() const;

private:
    GLuint  m_id;
    GLenum  m_target;
	GLsizei m_samples;
	GLsizei m_width;
	GLsizei m_height;
	GLboolean m_fixedSampleLocations;
    GLenum  m_internalFormat;
    GLint   m_envMode;

	GLboolean m_created;
};

}// namespace linde

#endif // LINDE_TEXTURE_MULTISAMPLING_H
