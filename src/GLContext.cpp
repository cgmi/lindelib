#include "../include/linde/GLContext.h"

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

GLContext::GLContext() :
    GLContext(640, 480, "",
              8, 8, 8, 8,
              8, 8, 4,
              false,
              false,
              true,
              nullptr,
              nullptr)
{

}


GLContext::~GLContext()
{

}

void GLContext::onError(GLint errorCode, const char* errorMessage)
{
    std::cerr << "GLWindow::ERROR_GLFW:\t"<< errorMessage << std::endl;
    std::cout << "press continue to exit";
    std::cin.get();
    exit(EXIT_FAILURE);
}

void GLContext::initGLFW()
{
    GLboolean error = glfwInit();
    if (!error)
    {
        std::cerr << "could not init GLFW:" << std::endl;
        std::cout << "press enter to exit";
        std::cin.get();
        exit(EXIT_FAILURE);
    }
    glfwSetErrorCallback(GLContext::onError);
}


void GLContext::initGLEW(GLFWwindow* window)
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

GLFWwindow *GLContext::getGLFW() const
{
    return m_glfwWindow.get();
}

GLContext::GLContext(GLuint width, GLuint height, const std::string & name,
                     GLint redBits, GLint greenBits, GLint blueBits, GLint alphaBits,
                     GLint depthBits, GLint stencilBits, GLuint samples,
                     GLboolean resizable, GLboolean visible, GLboolean sRGB_capable,
                     GLFWmonitor * monitor, GLFWwindow * shareContext) :
    m_glfwWindow(nullptr, nullptr),
    m_textRenderer(nullptr)
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


void GLContext::makeContextCurrent() const
{
    GLFWwindow * current = glfwGetCurrentContext();
    if (current != m_glfwWindow.get())
    {
        glfwMakeContextCurrent(m_glfwWindow.get());
    }
}

void GLContext::renderText(const std::string & text, const glm::vec2 & pos, const glm::vec4 & color, GLint fontSize)
{
    makeContextCurrent();

    if (m_textRenderer->getFontSize() != fontSize)
    {
        m_textRenderer->setFontSize(fontSize);
    }

    m_textRenderer->setColor(color);

    m_textRenderer->render(text, pos);
}

std::shared_ptr<Texture> GLContext::createTexture(GLsizei width, GLsizei height, GLint internalFormat, GLenum format, GLint type, GLint minFilter, GLint magFilter, GLint envMode, GLint wrapMode, GLboolean createMipMaps)
{
    return std::make_shared<Texture>(this, width, height, internalFormat, format, type, minFilter, magFilter, envMode, wrapMode, createMipMaps);
}

std::shared_ptr<Texture> GLContext::createTexture(const cv::Mat_<glm::vec3> & source,
                                                  GLint minFilter, GLint magFilter,
                                                  GLint envMode, GLint wrapMode, GLboolean createMipMaps)
{
    std::shared_ptr<Texture> tex = std::make_shared<Texture>(this, source.cols, source.rows, GL_RGB32F, GL_RGB, GL_FLOAT, minFilter, magFilter, envMode, wrapMode, createMipMaps);
    cv::Mat_<glm::vec3> flipped;
    cv::flip(source, flipped, 0);
    tex->create(flipped.data);

    return tex;
}

std::shared_ptr<Texture> GLContext::createTexture(const cv::Mat_<glm::vec4> & source,
                                                  GLint minFilter, GLint magFilter,
                                                  GLint envMode, GLint wrapMode, GLboolean createMipMaps)
{
    std::shared_ptr<Texture> tex = std::make_shared<Texture>(this, source.cols, source.rows, GL_RGBA32F, GL_RGBA, GL_FLOAT, minFilter, magFilter, envMode, wrapMode, createMipMaps);
    cv::Mat_<glm::vec4> flipped;
    cv::flip(source, flipped, 0);
    tex->create(flipped.data);

    return tex;
}

std::shared_ptr<Texture> GLContext::createTexture(const cv::Mat_<float> & source,
                                                  GLint minFilter, GLint magFilter,
                                                  GLint envMode, GLint wrapMode, GLboolean createMipMaps)
{
    std::shared_ptr<Texture> tex = std::make_shared<Texture>(this, source.cols, source.rows, GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT, minFilter, magFilter, envMode, wrapMode, createMipMaps);
    cv::Mat_<float> flipped;
    cv::flip(source, flipped, 0);
    tex->create(flipped.data);

    return tex;
}

std::shared_ptr<Texture> GLContext::createTexture(const cv::Mat_<uchar> & source,
                                                  GLint minFilter, GLint magFilter,
                                                  GLint envMode, GLint wrapMode, GLboolean createMipMaps)
{
    std::shared_ptr<Texture> tex = std::make_shared<Texture>(this, source.cols, source.rows, GL_LUMINANCE8, GL_RED, GL_UNSIGNED_BYTE, minFilter, magFilter, envMode, wrapMode, createMipMaps);
    cv::Mat_<uchar> flipped;
    cv::flip(source, flipped, 0);
    tex->create(flipped.data);

    return tex;
}

std::shared_ptr<TextureMultisample> GLContext::createTextureMultisample(GLsizei width, GLsizei height, GLsizei samples, GLenum internalFormat, GLboolean fixedSampleLocation)
{
    return std::make_shared<TextureMultisample>(this, width, height, samples, internalFormat, fixedSampleLocation);
}

std::shared_ptr<Shader> GLContext::createPipelineShader(const std::string &vertexSource, const std::string &fragSource)
{
    return std::make_shared<Shader>(this, vertexSource, fragSource);
}

std::shared_ptr<Shader> GLContext::createPipelineShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragSource)
{
    return std::make_shared<Shader>(this, vertexSource, geometrySource, fragSource);
}

std::shared_ptr<ComputeShader> GLContext::createComputeShader(const std::string & source)
{
    return std::make_shared<ComputeShader>(this, source);
}

std::shared_ptr<VertexBufferObject> GLContext::createVertexBufferObject()
{
    return std::make_shared<VertexBufferObject>(this);
}

std::shared_ptr<FrameBufferObject> GLContext::createFramebufferObject()
{
    return std::make_shared<FrameBufferObject>(this);
}

std::shared_ptr<FrameBufferObjectMultisample> GLContext::createFramebufferObjectMultisample()
{
    return std::make_shared<FrameBufferObjectMultisample>(this);
}

std::shared_ptr<ShaderStorageBufferObject>   GLContext::createShaderStoragebufferObject()
{
    return  std::make_shared<ShaderStorageBufferObject>(this);
}

} // namespace linde
