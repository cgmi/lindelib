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
                   GLFWmonitor * monitor, GLFWwindow * shareContext)
    :
      m_glfwWindow(nullptr, nullptr),
      m_textRenderer(nullptr),
      m_renderFunction(nullptr),
      m_onKeyFunction(nullptr)
{
    createWindow(width, height, name, redBits, greenBits, blueBits,
                 alphaBits, depthBits, stencilBits,
                 samples, resizable, visible, sRGB_capable, monitor, shareContext);

    m_renderFunction = [&]()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    };

    glfwSetWindowUserPointer(m_glfwWindow.get(), this);

    glfwSetKeyCallback(m_glfwWindow.get(), glfw_onKey);
    glfwSetScrollCallback(m_glfwWindow.get(), glfw_onScroll);
    glfwSetCursorPosCallback(m_glfwWindow.get(), glfw_onMouseMove);
    glfwSetMouseButtonCallback(m_glfwWindow.get(), glfw_onMouse);
    glfwSetWindowSizeCallback(m_glfwWindow.get(), glfw_onResize);

    m_textRenderer = std::make_unique<TextRenderer>();

}


GLWindow::~GLWindow()
{

}

void GLWindow::onError(GLint errorCode, const char* errorMessage)
{
    std::cerr << "GLWindow::ERROR_GLFW:\t"<< errorMessage << std::endl;
    std::cout << "press continue to exit";
    std::cin.get();
    exit(EXIT_FAILURE);
}

void GLWindow::initGLFW()
{
    GLboolean error = glfwInit();
    if (!error)
    {
        std::cerr << "could not init GLFW:" << std::endl;
        std::cout << "press enter to exit";
        std::cin.get();
        exit(EXIT_FAILURE);
    }
    glfwSetErrorCallback(GLWindow::onError);
}


void GLWindow::initGLEW(GLFWwindow* window)
{
    makeContextCurrent();

    //glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        std::cout << "press enter to exit";
        std::cin.get();
        exit(EXIT_FAILURE);
    }
}

void GLWindow::createWindow(GLuint width, GLuint height, const std::string & name,
                            GLint redBits, GLint greenBits, GLint blueBits, GLint alphaBits,
                            GLint depthBits, GLint stencilBits, GLuint samples,
                            GLboolean resizable, GLboolean visible, GLboolean sRGB_capable,
                            GLFWmonitor * monitor, GLFWwindow * shareContext)
{
    initGLFW();

    glfwWindowHint(GLFW_RED_BITS, redBits);
    glfwWindowHint(GLFW_GREEN_BITS, greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, blueBits);
    glfwWindowHint(GLFW_ALPHA_BITS, alphaBits);
    glfwWindowHint(GLFW_STENCIL_BITS, stencilBits);
    glfwWindowHint(GLFW_DEPTH_BITS, depthBits);
    glfwWindowHint(GLFW_SAMPLES, samples);
    glfwWindowHint(GLFW_RESIZABLE, resizable);
    glfwWindowHint(GLFW_VISIBLE, visible);
    glfwWindowHint(GLFW_SRGB_CAPABLE, sRGB_capable);
#ifdef DEBUG_BUILD
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
#endif
    m_glfwWindow = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>(glfwCreateWindow(width, height, name.c_str(), monitor, shareContext), glfwDestroyWindow);

    initGLEW(m_glfwWindow.get());

    // opengl error callback
#if DEBUG_BUILD
    if(glDebugMessageCallback)
    {
        std::cout << "Register OpenGL debug callback " << std::endl;
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openGlDebugCallback, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
                              GL_DONT_CARE,
                              GL_DONT_CARE,
                              0,
                              &unusedIds,
                              true);
    }
    else
    {
        std::cout << "glDebugMessageCallback not available" << std::endl;
    }
#endif

    // check viewport size
    GLint dims;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &dims);
    if (width > static_cast<GLuint>(dims) || height > static_cast<GLuint>(dims))
    {
        std::cerr << "OPENGL: " << "maximum GL_MAX_RENDERBUFFER_SIZE size exceeded: width: " << width << ">" << dims << " height: " << height << ">" << dims << std::endl;
    } else
    {
        myDebug << "GL_MAX_RENDERBUFFER_SIZE: " << dims << std::endl;
    }

    glViewport(0,0,width,height);

    glDisable(GL_FRAMEBUFFER_SRGB);
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
    makeContextCurrent();

    if(m_onKeyFunction)
        m_onKeyFunction(key, scancode, action, mods);
}

void GLWindow::internalOnMouse(GLint button, GLint action, GLint mods)
{
    makeContextCurrent();

    if (m_onMouseFunction)
        m_onMouseFunction(button, action, mods);

}

void GLWindow::internalOnMouseMove(GLdouble x, GLdouble y)
{
    makeContextCurrent();

    if (m_onMouseMoveFunction)
        m_onMouseMoveFunction(x, y);
}


void GLWindow::internalOnScroll(GLdouble xo, GLdouble yo)
{
    makeContextCurrent();

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

GLFWwindow * GLWindow::getGLFW()
{
    return m_glfwWindow.get();
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
        glfwShowWindow(m_glfwWindow.get());
    else
        glfwHideWindow(m_glfwWindow.get());
}

void GLWindow::resize(GLuint width, GLuint height)
{
    makeContextCurrent();

    glfwSetWindowSize(m_glfwWindow.get(), width, height);
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
    glfwSwapBuffers(m_glfwWindow.get());
}

GLboolean GLWindow::shouldClose()
{
    return glfwWindowShouldClose(m_glfwWindow.get());
}

void GLWindow::makeContextCurrent() const
{
    GLFWwindow * current = glfwGetCurrentContext();
    if (current != m_glfwWindow.get())
    {
        glfwMakeContextCurrent(m_glfwWindow.get());
    }
}

GLint GLWindow::getWidth() const
{
    GLint width, height;
    glfwGetWindowSize(this->m_glfwWindow.get(), &width, &height);
    return width;
}

GLint GLWindow::getHeight() const
{
    GLint width, height;
    glfwGetWindowSize(this->m_glfwWindow.get(), &width, &height);
    return height;
}

void GLWindow::getSize(GLint & width, GLint & height) const
{
    glfwGetWindowSize(this->m_glfwWindow.get(), &width, &height);
}

glm::vec2 GLWindow::getCursorPos() const
{
    double x, y;
    glfwGetCursorPos(m_glfwWindow.get(), &x, &y);
    return glm::vec2(x, y);
}


int GLWindow::getMouseButtonState(int button) const
{
    return glfwGetMouseButton(m_glfwWindow.get(), button);
}

void GLWindow::renderText(const std::string & text, const glm::vec2 & pos, const glm::vec4 & color, GLint fontSize)
{
    makeContextCurrent();

    if (m_textRenderer->getFontSize() != fontSize)
    {
        m_textRenderer->setFontSize(fontSize);
    }

    m_textRenderer->setColor(color);

    m_textRenderer->render(text, pos);
}


std::shared_ptr<Texture> GLWindow::createTexture(GLsizei width, GLsizei height, GLint internalFormat, GLenum format, GLint type, GLint minFilter, GLint magFilter, GLint envMode, GLint wrapMode)
{
    makeContextCurrent();

    return std::shared_ptr<Texture>(new Texture(width, height, internalFormat, format, type, minFilter, magFilter, envMode, wrapMode));
}

std::shared_ptr<Texture> GLWindow::createTexture(const cv::Mat_<glm::vec3> & source,
                                                 GLint minFilter, GLint magFilter,
                                                 GLint envMode, GLint wrapMode)
{
    makeContextCurrent();

    std::shared_ptr<Texture> tex(new Texture(source.cols, source.rows, GL_RGB32F, GL_RGB, GL_FLOAT, minFilter, magFilter, envMode, wrapMode));
    cv::Mat_<glm::vec3> flipped;
    cv::flip(source, flipped, 0);
    tex->create(flipped.data);

    return tex;
}

std::shared_ptr<Texture> GLWindow::createTexture(const cv::Mat_<glm::vec4> & source,
                                                 GLint minFilter, GLint magFilter,
                                                 GLint envMode, GLint wrapMode)
{
    makeContextCurrent();

    std::shared_ptr<Texture> tex(new Texture(source.cols, source.rows, GL_RGBA32F, GL_RGBA, GL_FLOAT, minFilter, magFilter, envMode, wrapMode));
    cv::Mat_<glm::vec4> flipped;
    cv::flip(source, flipped, 0);
    tex->create(flipped.data);

    return tex;
}

std::shared_ptr<Texture> GLWindow::createTexture(const cv::Mat_<float> & source,
                                                 GLint minFilter, GLint magFilter,
                                                 GLint envMode, GLint wrapMode)
{
    makeContextCurrent();

    std::shared_ptr<Texture> tex(new Texture(source.cols, source.rows, GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT, minFilter, magFilter, envMode, wrapMode));
    cv::Mat_<float> flipped;
    cv::flip(source, flipped, 0);
    tex->create(flipped.data);

    return tex;
}

std::shared_ptr<Texture> GLWindow::createTexture(const cv::Mat_<uchar> & source,
                                                 GLint minFilter, GLint magFilter,
                                                 GLint envMode, GLint wrapMode)
{
    makeContextCurrent();

    std::shared_ptr<Texture> tex(new Texture(source.cols, source.rows, GL_LUMINANCE8, GL_RED, GL_UNSIGNED_BYTE, minFilter, magFilter, envMode, wrapMode));
    cv::Mat_<uchar> flipped;
    cv::flip(source, flipped, 0);
    tex->create(flipped.data);

    return tex;
}

std::shared_ptr<TextureMultisample> GLWindow::createTextureMultisample(GLsizei width, GLsizei height, GLsizei samples, GLenum internalFormat, GLboolean fixedSampleLocation)
{
    makeContextCurrent();

    return std::shared_ptr<TextureMultisample>(new TextureMultisample(width, height, samples, internalFormat, fixedSampleLocation));
}

Shader* GLWindow::createPipelineShader(const std::string &vertexSource, const std::string &fragSource)
{
    makeContextCurrent();

    m_shaders.push_back(std::make_unique<Shader>(vertexSource, fragSource));
    return dynamic_cast<Shader*>(m_shaders.back().get());
}

Shader* GLWindow::createPipelineShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragSource)
{
    makeContextCurrent();

    m_shaders.push_back(std::make_unique<Shader>(vertexSource, geometrySource, fragSource));
    return dynamic_cast<Shader*>(m_shaders.back().get());
}

ComputeShader* GLWindow::createComputeShader(const std::string & source)
{
    makeContextCurrent();

    m_shaders.push_back(std::make_unique<ComputeShader>(source));
    return dynamic_cast<ComputeShader*>(m_shaders.back().get());
}

std::shared_ptr<VertexBufferObject>    GLWindow::createVertexBufferObject()
{
    makeContextCurrent();

    return std::shared_ptr<VertexBufferObject>(new VertexBufferObject);
}

std::shared_ptr<FrameBufferObject> GLWindow::createFramebufferObject()
{
    makeContextCurrent();

    return std::shared_ptr<FrameBufferObject>(new FrameBufferObject);
}

std::shared_ptr<FrameBufferObjectMultisample> GLWindow::createFramebufferObjectMultisample()
{
    makeContextCurrent();

    return std::shared_ptr<FrameBufferObjectMultisample>(new FrameBufferObjectMultisample);
}

std::shared_ptr<ShaderStorageBufferObject>   GLWindow::createShaderStoragebufferObject()
{
    makeContextCurrent();

    return  std::shared_ptr<ShaderStorageBufferObject>(new ShaderStorageBufferObject);
}

} // namespace linde
