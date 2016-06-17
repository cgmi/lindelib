
#include "../include/linde/ShaderStorageBuffer.h"

namespace linde
{
	
	
    GLint ShaderStorageBufferObject::getSizeInBytes() const
	{
        GLint size;
        bind(true);
		glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &size);
		myDebug << "SSBO::query:size: " << size << std::endl;
        bind(false);
		return size;
	}

		
    void ShaderStorageBufferObject::create(void * dataStart, GLuint nBytes)
	{
		if (m_handle > 0)
		{
			glDeleteBuffers(1, &m_handle);
		}
		glGenBuffers(1, &m_handle);

        bind(true);

		glBufferData(GL_SHADER_STORAGE_BUFFER,
			nBytes,
			dataStart,
			GL_STATIC_DRAW);
		myDebug << "SSBO::allocated:size: " << nBytes << std::endl;

        bind(false);
	}


    void ShaderStorageBufferObject::upload(void * dataStart, GLuint nBytes)
	{
		// check if enough bytes allocated
        if (nBytes != (GLuint)getSizeInBytes())
		{
			myDebug << "SSBO::reallocated:size: " << nBytes << std::endl;
			create(dataStart, nBytes);
			return;
		}

        bind(true);

		void * data = glMapBufferRange(GL_SHADER_STORAGE_BUFFER,
			0,
			nBytes,
			GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_WRITE_BIT);
		myDebug << "SSBO::upload:size: " << nBytes << std::endl;

		memcpy(data, dataStart, nBytes);

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        bind(false);
	}


    void ShaderStorageBufferObject::download(void * dataStart, GLuint nBytes) const
	{
        bind(true);

		void * data = glMapBufferRange(GL_SHADER_STORAGE_BUFFER,
			0,
			nBytes,
			GL_MAP_READ_BIT);

		myDebug << "SSBO::download:size: " << nBytes << std::endl;

		memcpy(dataStart, data, nBytes);

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        bind(false);
	}

    void ShaderStorageBufferObject::bind(GLboolean bind) const
	{
        if (bind)
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_handle);
        }
        else
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
	}

    void ShaderStorageBufferObject::bindBase(GLuint location) const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, m_handle);
	}




    ShaderStorageBufferObject::ShaderStorageBufferObject() :
		m_handle(0)
	{
		
	}

    ShaderStorageBufferObject::~ShaderStorageBufferObject()
	{
		if (m_handle > 0)
			glDeleteBuffers(1, &m_handle);
	}

    GLuint ShaderStorageBufferObject::getHandle() const
	{
		return m_handle;
	}


} // namespace linde
