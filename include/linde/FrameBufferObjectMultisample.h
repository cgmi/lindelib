#ifndef LINDE_FBO_MULTISAMPLE_H
#define LINDE_FBO_MULTISAMPLE_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <map>

#include "linde.h"

namespace linde
{

class TextureMultisample;
class GLWindow;

/*
#################################################################################
#################################################################################
#################################################################################
################### FBO #########################################################
#################################################################################
#################################################################################
#################################################################################
#################################################################################
#################################################################################
*/
/**
* @author Thomas Lindemeier
* @date 03.06.2013
*
* University of Konstanz-
* Department for Computergraphics
*/
class FrameBufferObjectMultisample
{
    friend class GLWindow;

    GLuint								   m_handle;
    std::shared_ptr<TextureMultisample>    m_target;
    std::shared_ptr<TextureMultisample>    m_depth;

    GLboolean checkStatus();

	FrameBufferObjectMultisample(GLint filtering = GL_NEAREST);
public:
    ~FrameBufferObjectMultisample();

    GLvoid bind(GLboolean bind);

	void blitColor();
	void blitDepth();

	void setFiltering(GLint filtering);

    GLvoid attachTexture(const std::shared_ptr<TextureMultisample> &texture);
    GLvoid attachDepth(const std::shared_ptr<TextureMultisample> &texture);

    const std::shared_ptr<TextureMultisample> &getTarget() const;
	const std::shared_ptr<TextureMultisample> &getDepth() const;

	GLint m_filtering;
};

} // namespace linde

#endif // LINDE_FBO_MULTISAMPLE_H
