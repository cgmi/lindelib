#include "../include/linde/FrameBufferObject.h"
#include "../include/linde/Texture.h"
#include "../include/linde/GLWindow.h"

namespace linde
{



FrameBufferObject::FrameBufferObject() :
    m_target(nullptr),
    m_depth(nullptr)
{
    glGenFramebuffers(1, &m_handle);
}

FrameBufferObject::~FrameBufferObject()
{
    glDeleteFramebuffers(1, &m_handle);
}

GLvoid FrameBufferObject::bind(GLboolean bind)
{
    if (bind)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

GLvoid FrameBufferObject::attachTexture(const std::shared_ptr<Texture> &texture)
{
    m_target = texture;

    if (!m_target->isCreated())
    {
        m_target->create(nullptr);
    }

    bind(true);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_target->id(), 0);

    bind(false);

    checkStatus();
}

GLvoid FrameBufferObject::attachDepth(const std::shared_ptr<Texture> &texture)
{
    m_depth = texture;

    if (!m_depth->isCreated())
    {
        m_depth->create(nullptr);
    }

    bind(true);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->id(), 0);

    bind(false);

    checkStatus();
}

GLvoid FrameBufferObject::attachDepth(GLuint width, GLuint height)
{
    std::shared_ptr<Texture> depth(new Texture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
                                                               GL_LINEAR, GL_LINEAR,
                                                               GL_REPLACE, GL_REPEAT));
    attachDepth(depth);
}

const std::shared_ptr<Texture> & FrameBufferObject::getTarget() const
{
    return m_target;
}

const std::shared_ptr<Texture> & FrameBufferObject::getDepth() const
{
	return m_depth;
}


GLboolean FrameBufferObject::checkStatus()
{
    bind(true);

    //glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    GLboolean result = false;

    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        //std::cerr << "FRAMEBUFFER::Complete" << std::endl;
        result = true;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        std::cerr << "[ERROR]FRAMEBUFFER::incomplete: Attachment is NOT complete" << std::endl;
        result = false;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        std::cerr << "[ERROR]FRAMEBUFFER::incomplete: No image is attached to FBO" << std::endl;
        result = false;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        std::cerr << "[ERROR]FRAMEBUFFER::incomplete: Attached images have different dimensions" << std::endl;
        result = false;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        std::cout << "[ERROR]FRAMEBUFFER::incomplete: Color attached images have different internal formats" << std::endl;
        result = false;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        std::cerr << "[ERROR]FRAMEBUFFER::incomplete: Draw buffer" << std::endl;
        result = false;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        std::cerr << "[ERROR]FRAMEBUFFER::incomplete: Read buffer" << std::endl;
        result = false;
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        std::cerr << "[ERROR]FRAMEBUFFER::Unsupported by FBO implementation" << std::endl;
        result = false;
        break;
    default:
        std::cerr << "[ERROR]FRAMEBUFFER::Unknow error" << std::endl;
        result = false;
        break;
    }

    bind(false);

    return result;
}



} // namespace linde
