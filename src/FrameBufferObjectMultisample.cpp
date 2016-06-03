#include "../include/linde/FrameBufferObjectMultisample.h"
#include "../include/linde/FrameBufferObject.h"
#include "../include/linde/TextureMultisample.h"
#include "../include/linde/GLWindow.h"

namespace linde
{

FrameBufferObjectMultisample::FrameBufferObjectMultisample(GLint filtering) :
	m_target(nullptr),
	m_depth(nullptr),
	m_filtering(filtering)
{
	glGenFramebuffers(1, &m_handle);
}

FrameBufferObjectMultisample::~FrameBufferObjectMultisample()
{
    glDeleteFramebuffers(1, &m_handle);
}

GLuint FrameBufferObjectMultisample::id() const
{
	return m_handle;
}

GLvoid FrameBufferObjectMultisample::bind(GLboolean bind)
{
    if (bind)
    {
		glEnable(GL_MULTISAMPLE);
        glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_MULTISAMPLE);
    }
}

void FrameBufferObjectMultisample::blit(const std::shared_ptr<FrameBufferObject> &fbo)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->id());
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);
	glBlitFramebuffer(0, 0, m_target->width(), m_target->height(), 0, 0, m_target->width(), m_target->height(), GL_COLOR_BUFFER_BIT, m_filtering);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObjectMultisample::blitColor(const std::shared_ptr<FrameBufferObject> &fbo)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->id());
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);
	glBlitFramebuffer(0, 0, m_target->width(), m_target->height(), 0, 0, m_target->width(), m_target->height(), GL_COLOR_BUFFER_BIT, m_filtering);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObjectMultisample::blitDepth(const std::shared_ptr<FrameBufferObject> &fbo)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->id());
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);
	glBlitFramebuffer(0, 0, m_target->width(), m_target->height(), 0, 0, m_target->width(), m_target->height(), GL_DEPTH_BUFFER_BIT, m_filtering);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObjectMultisample::setFiltering(GLint filtering)
{
	m_filtering = filtering;
}

GLvoid FrameBufferObjectMultisample::attachTexture(const std::shared_ptr<TextureMultisample> &texture)
{
    m_target = texture;

    if (!m_target->isCreated())
    {
        m_target->create();
    }

    bind(true);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_target->id(), 0);

    bind(false);

    checkStatus();
}

GLvoid FrameBufferObjectMultisample::attachDepth(const std::shared_ptr<TextureMultisample> &texture)
{
    m_depth = texture;

    if (!m_depth->isCreated())
    {
        m_depth->create();
    }

    bind(true);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_depth->id(), 0);

    bind(false);

    checkStatus();
}

const std::shared_ptr<TextureMultisample> & FrameBufferObjectMultisample::getTarget() const
{
    return m_target;
}

const std::shared_ptr<TextureMultisample> & FrameBufferObjectMultisample::getDepth() const
{
	return m_depth;
}


GLboolean FrameBufferObjectMultisample::checkStatus()
{
    bind(true);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    GLboolean result = false;

    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        //std::cerr << "FRAMEBUFFERMULTISAMPLE::Complete" << std::endl;
        result = true;
        break;

    case GL_FRAMEBUFFER_UNDEFINED:
        std::cerr << "[ERROR]FRAMEBUFFERMULTISAMPLE:: Framebuffer undefined." << std::endl;
        result = false;
        break;
    
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr << "[ERROR]FRAMEBUFFERMULTISAMPLE:: Framebuffer attachment incomplete." << std::endl;
		result = false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr << "[ERROR]FRAMEBUFFERMULTISAMPLE:: Framebuffer does not have any attachments." << std::endl;
		result = false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr << "[ERROR]FRAMEBUFFERMULTISAMPLE:: Draw buffer incomplete." << std::endl;
		result = false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr << "[ERROR]FRAMEBUFFERMULTISAMPLE:: Read buffer incomplete." << std::endl;
		result = false;
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr << "[ERROR]FRAMEBUFFERMULTISAMPLE:: Internal formats not supported." << std::endl;
		result = false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cerr << "[ERROR]FRAMEBUFFERMULTISAMPLE:: Samples or sample locations not the same for all attached buffers." << std::endl;
		result = false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cerr << "[ERROR]FRAMEBUFFERMULTISAMPLE:: Framebuffer attachment is layered." << std::endl;
		result = false;
		break;

    default:
        std::cerr << "[ERROR]FRAMEBUFFERMULTISAMPLE::Unknow error" << std::endl;
        result = false;
        break;
    }

    bind(false);

    return result;
}

} // namespace linde
