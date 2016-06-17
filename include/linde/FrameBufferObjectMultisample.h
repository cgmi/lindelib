#ifndef LINDE_FBO_MULTISAMPLE_H
#define LINDE_FBO_MULTISAMPLE_H


#include "lindeOpenGL.h"

namespace linde
{

class TextureMultisample;
class FrameBufferObject;
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
* @author Marc Spicker, Thomas Lindemeier
* @date 03.06.2016
*
* University of Konstanz-
* Department for Computergraphics
*/
class FrameBufferObjectMultisample
{
    GLuint								   m_handle;
    std::shared_ptr<TextureMultisample>    m_target;
    std::shared_ptr<TextureMultisample>    m_depth;

    GLboolean checkStatus();

public:
    FrameBufferObjectMultisample(GLint filtering = GL_NEAREST);
    ~FrameBufferObjectMultisample();

	GLuint id() const;

    GLvoid bind(GLboolean bind);

	void blit(const std::shared_ptr<FrameBufferObject> &fbo);
	void blitColor(const std::shared_ptr<FrameBufferObject> &fbo);
	void blitDepth(const std::shared_ptr<FrameBufferObject> &fbo);

	void setFiltering(GLint filtering);

    GLvoid attachTexture(const std::shared_ptr<TextureMultisample> &texture);
    GLvoid attachDepth(const std::shared_ptr<TextureMultisample> &texture);

    const std::shared_ptr<TextureMultisample> &getTarget() const;
	const std::shared_ptr<TextureMultisample> &getDepth() const;

	GLint m_filtering;
};

} // namespace linde

#endif // LINDE_FBO_MULTISAMPLE_H
