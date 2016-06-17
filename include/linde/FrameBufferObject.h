#ifndef LINDE_FBO_H
#define LINDE_FBO_H

#include "lindeOpenGL.h"

namespace linde
{

class Texture;

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
class FrameBufferObject : public GLObject
{
    GLuint                      m_handle;
    std::shared_ptr<Texture>    m_target;
    std::shared_ptr<Texture>    m_depth;

    GLboolean checkStatus();

public:
    FrameBufferObject(GLContext * glContext);
    ~FrameBufferObject();

	GLuint id() const;

    GLvoid bind(GLboolean bind);

    GLvoid attachTexture(const std::shared_ptr<Texture> &texture);
    GLvoid attachDepth(const std::shared_ptr<Texture> &texture);
    GLvoid attachDepth(GLuint width, GLuint height);

    const std::shared_ptr<Texture> &getTarget() const;
	const std::shared_ptr<Texture> &getDepth() const;
};

} // namespace linde

#endif // LINDE_FBO_H
