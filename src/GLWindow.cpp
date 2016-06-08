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


static GLint GLFW_INSTANCES = 0;

GLWindow::GLWindow(GLuint width, GLuint height, const std::string & name,
                   GLint redBits, GLint greenBits, GLint blueBits, GLint alphaBits,
                   GLint depthBits, GLint stencilBits, GLuint samples,
                   GLboolean resizable, GLboolean visible, GLboolean sRGB_capable,
                   GLFWmonitor * monitor, GLFWwindow * shareContext)
    :
      m_glfwWindow(nullptr),
      m_textRenderer(nullptr),
      m_gui(),
      m_progressBar(nullptr),
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

    glfwSetWindowUserPointer(m_glfwWindow, this);

    glfwSetKeyCallback(m_glfwWindow, glfw_onKey);
    glfwSetScrollCallback(m_glfwWindow, glfw_onScroll);
    glfwSetCursorPosCallback(m_glfwWindow, glfw_onMouseMove);
    glfwSetMouseButtonCallback(m_glfwWindow, glfw_onMouse);
    glfwSetWindowSizeCallback(m_glfwWindow, glfw_onResize);

    GLFW_INSTANCES++;

    m_textRenderer = new TextRenderer;

    m_progressBar = std::make_shared<ProgressBar>(this);
    m_progressBar->set(-1);
}


GLWindow::~GLWindow()
{
    glfwDestroyWindow(m_glfwWindow);

    if (m_textRenderer) delete m_textRenderer;

    if (GLFW_INSTANCES <= 0)
    {
        glfwTerminate();
    }
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
    m_glfwWindow = glfwCreateWindow(width, height, name.c_str(), monitor, shareContext);

    initGLEW(m_glfwWindow);

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

    if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_SPACE)
        {
            m_gui.m_show = !m_gui.m_show;
        }
    }

	if(m_onKeyFunction)
		m_onKeyFunction(key, scancode, action, mods);
}

void GLWindow::internalOnMouse(GLint button, GLint action, GLint mods)
{
    makeContextCurrent();

    // if gui active
    if (m_gui.m_show)
    {

        // check if gui action could be requested
        if (button == GLFW_MOUSE_BUTTON_1)
        {
            m_gui.m_mouseLeftPressed = (action == GLFW_PRESS);
            if (m_gui.m_mouseLeftPressed)
            {
                GLdouble x, y;
                glfwGetCursorPos(m_glfwWindow, &x, &y);
                for (size_t i = 0; i < m_gui.m_elements.size(); i++)
                {
                    if (m_gui.m_elements[i]->update(x, y)) break;
                }
            }
        }
    } else
    {
		if (m_onMouseFunction)
			m_onMouseFunction(button, action, mods);
    }
}

void GLWindow::internalOnMouseMove(GLdouble x, GLdouble y)
{
    makeContextCurrent();

    // if gui active
    if (m_gui.m_show)
    {
        // if mousebutton was pressed for interaction
        if (m_gui.m_mouseLeftPressed)
        {
            for (size_t i = 0; i < m_gui.m_elements.size(); i++)
            {
                NeedButtonRelease* v = dynamic_cast<NeedButtonRelease*>(m_gui.m_elements[i].get());
                if (!v)
                {
                    m_gui.m_elements[i]->update(x, y);
                }
            }
        }
    } else
    {
		if (m_onMouseMoveFunction)
			m_onMouseMoveFunction(x, y);
    }
}

void GLWindow::renderGUI()
{
    // render gui
    if (m_gui.m_show)
    {
        makeContextCurrent();


        // render background
        const GLfloat h = m_gui.m_elementHeight + m_gui.m_nextAvailablePosition[1];
        enable2D();
        glTranslatef(0.f, 0.f, 0.0f);
        glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(getWidth(), 0.0f, 0.0f);
        glVertex3f(getWidth(), h, 0.0f);
        glVertex3f(0.0, h, 0.0f);
        glEnd();
        glDisable(GL_BLEND);
        disable2D();

        // update gui
        glm::vec4 color = m_textRenderer->getColor();
        m_textRenderer->setColor(m_gui.m_overlayColor);

        for (size_t i = 0; i < m_gui.m_elements.size(); i++)
        {
            m_gui.m_elements[i]->paint();
            if (m_gui.m_elements[i]->locked()) break;
        }

        m_textRenderer->setColor(m_gui.m_overlayColor);

        // write cursor pos
        glm::dvec2 xy;
        glfwGetCursorPos(m_glfwWindow, &(xy[0]), &(xy[1]));
        std::stringstream stream;
        stream << xy;
        m_textRenderer->render(stream.str(), glm::vec2(getWidth() - 90.f, 15.f));

        // reset color
        m_textRenderer->setColor(color);
    }
}

GLboolean GLWindow::isGUIActive() const
{
    return m_gui.m_show;
}

void GLWindow::internalOnScroll(GLdouble xo, GLdouble yo)
{
	if (m_onScrollFunction)
		m_onScrollFunction(xo, yo);
}

void GLWindow::internalOnResize(GLint width, GLint height)
{
    glViewport(0, 0, width, height);
	if (m_onResizeFunction)
		m_onResizeFunction(width, height);
}

GLFWwindow * GLWindow::getGLFW()
{
    return m_glfwWindow;
}


void GLWindow::update(bool waitForEvents)
{
    makeContextCurrent();

    renderGUI();
    m_progressBar->render();
    swapBuffers();

    if (waitForEvents)
    {
        waitEvents();
    } else
    {
        pollEvents();
    }

    for (auto &  shader : m_shaders)
    {
        if (!shader.expired())
        {
            shader.lock()->checkShaderReload();
        }
    }
    // remove invalid shaders;
    m_shaders.erase(std::remove_if(m_shaders.begin(),
                                   m_shaders.end(),
                                   [&](const std::weak_ptr<linde::AbstractShader> & p){return p.expired();}),
                    m_shaders.end());
}

void GLWindow::toggleGUI(bool show)
{
    m_gui.m_show = show;
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
        glfwShowWindow(m_glfwWindow);
    else
        glfwHideWindow(m_glfwWindow);
}

void GLWindow::resize(GLuint width, GLuint height)
{
    makeContextCurrent();

    glfwSetWindowSize(m_glfwWindow, width, height);
    internalOnResize(width, height);
}

void GLWindow::clearGUI()
{
    makeContextCurrent();

    m_gui.m_elements.clear();
    m_gui.m_mouseLeftPressed = GL_FALSE;
    m_gui.m_show = GL_FALSE;
    m_gui.m_overlayColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
    m_gui.m_nextAvailablePosition = glm::vec2(10.f, 10.f);
    m_gui.m_elementHeight = 14.f;
    m_gui.m_sliderWidth = 200.f;
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
    glfwSwapBuffers(m_glfwWindow);
}

GLboolean GLWindow::shouldClose()
{
    return glfwWindowShouldClose(m_glfwWindow);
}

void GLWindow::makeContextCurrent() const
{
    GLFWwindow * current = glfwGetCurrentContext();
    if (current != m_glfwWindow)
    {
        glfwMakeContextCurrent(m_glfwWindow);
    }
}

std::shared_ptr<ProgressBar> &GLWindow::getProgressBar()
{
    return m_progressBar;
}

GLint GLWindow::getWidth() const
{
    GLint width, height;
    glfwGetWindowSize(this->m_glfwWindow, &width, &height);
    return width;
}

GLint GLWindow::getHeight() const
{
    GLint width, height;
    glfwGetWindowSize(this->m_glfwWindow, &width, &height);
    return height;
}

void GLWindow::getSize(GLint & width, GLint & height) const
{
    glfwGetWindowSize(this->m_glfwWindow, &width, &height);
}

glm::vec2 GLWindow::getCursorPos() const
{
    double x, y;
    glfwGetCursorPos(m_glfwWindow, &x, &y);
    return glm::vec2(x, y);
}


int GLWindow::getMouseButtonState(int button) const
{
    return glfwGetMouseButton(m_glfwWindow, button);
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

std::shared_ptr<Shader> GLWindow::createPipelineShader(const std::string &vertexSource, const std::string &fragSource)
{
    makeContextCurrent();

    std::shared_ptr<Shader> shader(new Shader(vertexSource, fragSource));
    m_shaders.push_back(shader);
    return shader;
}

std::shared_ptr<Shader> GLWindow::createPipelineShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragSource)
{
    makeContextCurrent();

    std::shared_ptr<Shader> shader(new Shader(vertexSource, geometrySource, fragSource));
    m_shaders.push_back(shader);
    return shader;
}

std::shared_ptr<ComputeShader> GLWindow::createComputeShader(const std::string & source)
{
    makeContextCurrent();

    std::shared_ptr<ComputeShader> shader(new ComputeShader(source));
    m_shaders.push_back(shader);
    return shader;
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


std::shared_ptr<Label> GLWindow::addLabel(const std::string & text)
{
    makeContextCurrent();

    std::shared_ptr<Label> label = std::make_shared<Label>(
                this,
                m_gui.m_nextAvailablePosition[0],
            m_gui.m_nextAvailablePosition[1],
            m_gui.m_elementHeight,
            m_gui.m_elementHeight);
    label->setText(text);
    label->setColor(m_gui.m_overlayColor);
    m_gui.m_elements.push_back(label);

    m_gui.m_nextAvailablePosition[1] += GUIElement::GUI_DISPLACEMENT_FACTOR * m_gui.m_elementHeight;

    return label;
}

std::shared_ptr<CheckBox> GLWindow::addCheckBox(const std::string & text, bool * checked)
{
    makeContextCurrent();

    std::shared_ptr<CheckBox> box = std::make_shared<CheckBox>(
                this,
                m_gui.m_nextAvailablePosition[0],
            m_gui.m_nextAvailablePosition[1],
            m_gui.m_elementHeight,
            m_gui.m_elementHeight,
            checked);
    box->setText(text);
    box->setColor(m_gui.m_overlayColor);
    m_gui.m_elements.push_back(box);

    m_gui.m_nextAvailablePosition[1] += GUIElement::GUI_DISPLACEMENT_FACTOR * m_gui.m_elementHeight;

    return box;
}

std::shared_ptr<Button> GLWindow::addButton(const std::string & text, const std::function<void()> & callback)
{
    makeContextCurrent();

    std::shared_ptr<Button> button = std::make_shared<Button>(
                this,
                m_gui.m_nextAvailablePosition[0],
            m_gui.m_nextAvailablePosition[1],
            m_gui.m_elementHeight,
            m_gui.m_elementHeight,
            callback);
    button->setText(text);
    button->setColor(m_gui.m_overlayColor);
    m_gui.m_elements.push_back(button);

    m_gui.m_nextAvailablePosition[1] += GUIElement::GUI_DISPLACEMENT_FACTOR * m_gui.m_elementHeight;

    return button;
}

std::shared_ptr<DropDownBox>  GLWindow::addDropDownBox(const std::string & text, const std::vector<std::string> & options, int * selection, const std::function<void(int)> &callback)
{
    makeContextCurrent();

    std::shared_ptr<DropDownBox> box = std::make_shared<DropDownBox>(
                this,
                m_gui.m_nextAvailablePosition[0],
            m_gui.m_nextAvailablePosition[1],
            m_gui.m_sliderWidth,
            m_gui.m_elementHeight,
            options, selection, callback);

    box->setText(text);
    box->setColor(m_gui.m_overlayColor);
    m_gui.m_elements.push_back(box);

    m_gui.m_nextAvailablePosition[1] += GUIElement::GUI_DISPLACEMENT_FACTOR * m_gui.m_elementHeight;

    return box;
}

} // namespace linde
