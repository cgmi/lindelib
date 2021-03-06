#ifndef GLCONTEXT_H
#define GLCONTEXT_H

#include "lindeOpenGL.h"

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
class VertexArray;
class FrameBufferObject;
class FrameBufferObjectMultisample;
class ShaderStorageBufferObject;
class TextRenderer;

/**
     * @brief The GLWindow class
     * @author Thomas Lindemeier
     * University of Konstanz
     */
class GLContext
{
    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>	m_glfwWindow;
    std::unique_ptr<TextRenderer>   					m_textRenderer;

    void initGLFW();
    void initGLEW(GLFWwindow* window);

protected:
    GLFWwindow *getGLFW() const;

    GLContext(GLuint width, GLuint height, const std::string & name,
              GLint redBits, GLint greenBits, GLint blueBits, GLint alphaBits,
              GLint depthBits, GLint stencilBits, GLuint samples,
              GLboolean resizable, GLboolean visible, GLboolean sRGB_capable,
              GLFWmonitor * monitor, GLFWwindow * shareContext);

public:
    GLContext();
    virtual ~GLContext();

    static
    void onError(GLint errorCode, const char* errorMessage);

    void makeContextCurrent() const;

    void renderText(
            const std::string & text,
            const glm::vec2 & pos,
            const glm::vec4 & color = glm::vec4(0.f, 0.f, 0.f, 1.f),
            GLint fontSize = 12);


    std::shared_ptr<Texture>						createTexture(GLsizei width, GLsizei height,
                                                                  GLint internalFormat = GL_RGB32F, GLenum format = GL_RGB, GLint type = GL_FLOAT,
                                                                  GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR,
                                                                  GLint envMode = GL_REPLACE, GLint wrapMode = GL_REPEAT, GLboolean createMipMaps = GL_FALSE);
    std::shared_ptr<Texture>						createTexture(const cv::Mat_<glm::vec3> & source,
                                                                  GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR,
                                                                  GLint envMode = GL_REPLACE, GLint wrapMode = GL_REPEAT, GLboolean createMipMaps = GL_FALSE);
    std::shared_ptr<Texture>						createTexture(const cv::Mat_<glm::vec4> & source,
                                                                  GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR,
                                                                  GLint envMode = GL_REPLACE, GLint wrapMode = GL_REPEAT, GLboolean createMipMaps = GL_FALSE);
    std::shared_ptr<Texture>						createTexture(const cv::Mat_<float> & source,
                                                                  GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR,
                                                                  GLint envMode = GL_REPLACE, GLint wrapMode = GL_REPEAT, GLboolean createMipMaps = GL_FALSE);
    std::shared_ptr<Texture>						createTexture(const cv::Mat_<uchar> & source,
                                                                  GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR,
                                                                  GLint envMode = GL_REPLACE, GLint wrapMode = GL_REPEAT, GLboolean createMipMaps = GL_FALSE);
    std::shared_ptr<TextureMultisample>				createTextureMultisample(GLsizei width, GLsizei height, GLsizei samples,
                                                                             GLenum internalFormat = GL_RGBA, GLboolean fixedSampleLocation = GL_FALSE);

    std::shared_ptr<Shader>                         createPipelineShader(const std::string &vertexSource, const std::string &fragSource);
    std::shared_ptr<Shader>                         createPipelineShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragSource);
    std::shared_ptr<ComputeShader>                  createComputeShader(const std::string &source);

    std::shared_ptr<VertexBufferObject>				createVertexBufferObject();
    std::shared_ptr<VertexArray>                    createVertexArray();

    std::shared_ptr<FrameBufferObject>				createFramebufferObject();
    std::shared_ptr<FrameBufferObjectMultisample>   createFramebufferObjectMultisample();
    std::shared_ptr<ShaderStorageBufferObject>		createShaderStoragebufferObject();
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

#endif // GLCONTEXT_H
