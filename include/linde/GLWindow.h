#ifndef LINDE_GLWINDOW_H
#define LINDE_GLWINDOW_H
#include "GLContext.h"

#include <functional>


namespace linde
{



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
///  ////////////////////////////////////////////////////////////////////////////////////////////////
///  ////////////////////////////////////////////////////////////////////////////////////////////////
///  ////////////////////////////////////////////////////////////////////////////////////////////////
///  ////////////////////////////////////////////////////////////////////////////////////////////////
///  ////////////////////////////////////////////////////////////////////////////////////////////////
///  ////////////////////////////////////////////////////////////////////////////////////////////////
///  ////////////////////////////////////////////////////////////////////////////////////////////////
///  ////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractShader;
class Shader;
class ComputeShader;
class Texture;
class TextureMultisample;
class VertexBufferObject;
class FrameBufferObject;
class FrameBufferObjectMultisample;
class ShaderStorageBufferObject;
class TextRenderer;

/**
     * @brief The GLWindow class
     * @author Thomas Lindemeier
     * University of Konstanz
     */
class GLWindow : public GLContext
{
    std::function<void()>								m_renderFunction;
    std::function<void(GLint, GLint, GLint, GLint)>     m_onKeyFunction;
    std::function<void(GLint, GLint, GLint)>			m_onMouseFunction;
    std::function<void(GLdouble, GLdouble)>				m_onMouseMoveFunction;
    std::function<void(GLdouble, GLdouble)>				m_onScrollFunction;
    std::function<void(GLint, GLint)>					m_onResizeFunction;

public:
    GLWindow(GLuint width, GLuint height, const std::string & name = "window",
             GLint redBits = 32, GLint greenBits = 32, GLint blueBits = 32, GLint alphaBits = 32,
             GLint depthBits = 64, GLint stencilBits = 16, GLuint samples = 4,
             GLboolean resizable = true,
             GLboolean visible = true,
             GLboolean sRGB_capable = true,
             GLFWmonitor * monitor = nullptr, GLFWwindow * shareContext = nullptr);

    virtual ~GLWindow();

private:
    static void glfw_onKey(GLFWwindow * window, GLint key, GLint scancode, GLint action, GLint mods);
    static void glfw_onMouse(GLFWwindow * window, GLint button, GLint action, GLint mods);
    static void glfw_onMouseMove(GLFWwindow * window, GLdouble x, GLdouble y);
    static void glfw_onScroll(GLFWwindow * window, GLdouble xo, GLdouble yo);
    static void glfw_onResize(GLFWwindow * window, GLint width, GLint height);

    void internalOnKey(GLint key, GLint scancode, GLint action, GLint mods);
    void internalOnMouse(GLint button, GLint action, GLint mods);
    void internalOnMouseMove(GLdouble x, GLdouble y);
    void internalOnScroll(GLdouble xo, GLdouble yo);
    void internalOnResize(GLint width, GLint height);

    void waitEvents() const;
    void swapBuffers() const;

public:
    // only poll and process events from the OS
    void pollEvents() const;
    // swap buffers, processing events
    void update(bool waitForEvents = false);

    // setting a custom onKey function
    void setOnKeyFunction(const std::function<void(GLint, GLint, GLint, GLint)> & onKey);
    // setting a custom onMouse function
    void setOnMouseFunction(const std::function<void(GLint, GLint, GLint)> &onMouse);
    // setting a custom onMouseMove function
    void setOnMouseMoveFunction(const std::function<void(GLdouble, GLdouble)> &onMouseMove);
    // setting a custom onScroll function
    void setOnScrollFunction(const std::function<void(GLdouble, GLdouble)> &onScroll);
    // setting a custom onResize function
    void setOnResizeFunction(const std::function<void(GLint, GLint)> &onResize);
    // setting the render function to be called by renderOnce
    void setRenderFunction(const std::function<void()> & renderStep);

    // single render step of the given render function and call to update
    void renderOnce(bool waitForEvents = false);
    // loops renderOnce and updates
    int renderLoop(bool waitForEvents = true);

    void setVisible(GLboolean show);
    void resize(GLuint width, GLuint height);

    GLint getWidth() const;
    GLint getHeight() const;

    void
    getSize(GLint & width, GLint & height) const;

    glm::vec2
    getCursorPos() const;

    int
    getMouseButtonState(int button) const;

    GLboolean shouldClose();

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



} // namespace

#endif //LINDE_GLWINDOW_H
