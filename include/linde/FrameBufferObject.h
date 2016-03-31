#ifndef LINDE_FBO_H
#define LINDE_FBO_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <map>

#include "linde.h"

namespace linde
{

class Texture;
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
class FrameBufferObject
{
    friend class GLWindow;

    GLuint                      m_handle;
    std::shared_ptr<Texture>    m_target;
    std::shared_ptr<Texture>    m_depth;

    GLboolean checkStatus();

    FrameBufferObject();
public:
    ~FrameBufferObject();

    GLvoid bind(GLboolean bind);

    GLvoid attachTexture(const std::shared_ptr<Texture> &texture);
    GLvoid attachDepth(const std::shared_ptr<Texture> &texture);
    GLvoid attachDepth(GLuint width, GLuint height);

    const std::shared_ptr<Texture> &getTarget() const;
};

} // namespace linde

#endif // LINDE_FBO_H
