#include "../include/linde/GLWindow.h"
#include "../include/linde/FrameBufferObject.h"
#include "../include/linde/FrameBufferObjectMultisample.h"
#include "../include/linde/VertexBufferObject.h"
#include "../include/linde/Shader.h"
#include "../include/linde/Texture.h"
#include "../include//linde/TextureMultisample.h"
#include "../include/linde/ShaderStorageBuffer.h"
#include "../include/linde/TextRenderer.h"

#include <algorithm>

namespace linde
{

GLWindow::GLWindow(GLuint width, GLuint height, const std::string & name,
                   GLint redBits, GLint greenBits, GLint blueBits, GLint alphaBits,
                   GLint depthBits, GLint stencilBits, GLuint samples,
                   GLboolean resizable, GLboolean visible, GLboolean sRGB_capable,
                   GLFWmonitor * monitor, GLFWwindow * shareContext) :
    GLContext(width, height, name, redBits, greenBits, blueBits,
              alphaBits, depthBits, stencilBits,
              samples, resizable, visible, sRGB_capable, monitor, shareContext),
    m_renderFunction(nullptr),
    m_onKeyFunction(nullptr)
{ 
    m_renderFunction = [&]()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    };

    glfwSetWindowUserPointer(getGLFW(), this);

    glfwSetKeyCallback(getGLFW(), glfw_onKey);
    glfwSetScrollCallback(getGLFW(), glfw_onScroll);
    glfwSetCursorPosCallback(getGLFW(), glfw_onMouseMove);
    glfwSetMouseButtonCallback(getGLFW(), glfw_onMouse);
    glfwSetWindowSizeCallback(getGLFW(), glfw_onResize);
}


GLWindow::~GLWindow()
{

}

void GLWindow::glfw_onKey(GLFWwindow * window, GLint key, GLint scancode, GLint action, GLint mods)
{
    static_cast<GLWindow*>(glfwGetWindowUserPointer(window))->internalOnKey(key, scancode, action, mods);
}

void GLWindow::glfw_onMouse(GLFWwindow * window, GLint button, GLint action, GLint mods)
{
    static_cast<GLWindow*>(glfwGetWindowUserPointer(window))->internalOnMouse(button, action, mods);
}

void GLWindow::glfw_onMouseMove(GLFWwindow * window, GLdouble x, GLdouble y)
{
    static_cast<GLWindow*>(glfwGetWindowUserPointer(window))->internalOnMouseMove(x, y);
}

void GLWindow::glfw_onScroll(GLFWwindow * window, GLdouble xo, GLdouble yo)
{
    static_cast<GLWindow*>(glfwGetWindowUserPointer(window))->internalOnScroll(xo, yo);
}

void GLWindow::glfw_onResize(GLFWwindow * window, GLint width, GLint height)
{
    static_cast<GLWindow*>(glfwGetWindowUserPointer(window))->internalOnResize(width, height);
}


void GLWindow::internalOnKey(GLint key, GLint scancode, GLint action, GLint mods)
{
   if(m_onKeyFunction)
        m_onKeyFunction(key, scancode, action, mods);
}

void GLWindow::internalOnMouse(GLint button, GLint action, GLint mods)
{
   if (m_onMouseFunction)
        m_onMouseFunction(button, action, mods);

}

void GLWindow::internalOnMouseMove(GLdouble x, GLdouble y)
{
    if (m_onMouseMoveFunction)
        m_onMouseMoveFunction(x, y);
}


void GLWindow::internalOnScroll(GLdouble xo, GLdouble yo)
{
    if (m_onScrollFunction)
        m_onScrollFunction(xo, yo);
}

void GLWindow::internalOnResize(GLint width, GLint height)
{
    makeContextCurrent();

    glViewport(0, 0, width, height);
    if (m_onResizeFunction)
        m_onResizeFunction(width, height);
}

void GLWindow::update(bool waitForEvents)
{
    makeContextCurrent();

    swapBuffers();

    if (waitForEvents)
    {
        waitEvents();
    } else
    {
        pollEvents();
    }
}


void GLWindow::setOnKeyFunction(const std::function<void(GLint, GLint, GLint, GLint)>& onKey)
{
    m_onKeyFunction = onKey;
}

void GLWindow::setOnMouseFunction(const std::function<void(GLint, GLint, GLint)>& onMouse)
{
    m_onMouseFunction = onMouse;
}

void GLWindow::setOnMouseMoveFunction(const std::function<void(GLdouble, GLdouble)>& onMouseMove)
{
    m_onMouseMoveFunction = onMouseMove;
}

void GLWindow::setOnScrollFunction(const std::function<void(GLdouble, GLdouble)>& onScroll)
{
    m_onScrollFunction = onScroll;
}

void GLWindow::setOnResizeFunction(const std::function<void(GLint, GLint)>& onResize)
{
    m_onResizeFunction = onResize;
}

void GLWindow::setRenderFunction(const std::function<void ()> &renderStep)
{
    m_renderFunction = renderStep;
}

void GLWindow::renderOnce(bool waitForEvents)
{
    makeContextCurrent();
    m_renderFunction();
    update(waitForEvents);
}

int GLWindow::renderLoop(bool waitForEvents)
{
    while (!shouldClose())
    {
        renderOnce(waitForEvents);
    }
    return 0;
}


void GLWindow::setVisible(GLboolean show)
{
    if (show)
        glfwShowWindow(getGLFW());
    else
        glfwHideWindow(getGLFW());
}

void GLWindow::resize(GLuint width, GLuint height)
{
    makeContextCurrent();

    glfwSetWindowSize(getGLFW(), width, height);
    internalOnResize(width, height);
}

void GLWindow::pollEvents() const
{
    glfwPollEvents();
}

void GLWindow::waitEvents() const
{
    glfwWaitEvents();
}

void GLWindow::swapBuffers() const
{
    glfwSwapBuffers(getGLFW());
}

GLboolean GLWindow::shouldClose()
{
    return glfwWindowShouldClose(getGLFW());
}

GLint GLWindow::getWidth() const
{
    GLint width, height;
    glfwGetWindowSize(this->getGLFW(), &width, &height);
    return width;
}

GLint GLWindow::getHeight() const
{
    GLint width, height;
    glfwGetWindowSize(this->getGLFW(), &width, &height);
    return height;
}

void GLWindow::getSize(GLint & width, GLint & height) const
{
    glfwGetWindowSize(this->getGLFW(), &width, &height);
}

glm::vec2 GLWindow::getCursorPos() const
{
    double x, y;
    glfwGetCursorPos(getGLFW(), &x, &y);
    return glm::vec2(x, y);
}


int GLWindow::getMouseButtonState(int button) const
{
    return glfwGetMouseButton(getGLFW(), button);
}

} // namespace linde
