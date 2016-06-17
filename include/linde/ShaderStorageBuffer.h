#ifndef LINDE_SHADER_STORAGE_BUFFER
#define LINDE_SHADER_STORAGE_BUFFER

#include "lindeOpenGL.h"


namespace linde
{	
class ShaderStorageBufferObject : public GLObject
{
    GLuint m_handle;

public:
    ShaderStorageBufferObject(GLContext * glContext);
    ~ShaderStorageBufferObject();

    // retun current buffer size in bytes
    GLint getSizeInBytes() const;
    // allocate buffer
    void create(void * dataStart, GLuint nBytes);
    // upload to previous allocated buffer
    void upload(void * dataStart, GLuint nBytes);
    // download to host ram
    void download(void * dataStart, GLuint nBytes) const;

    void bind(GLboolean bind) const;
    // bind to location to address it in a shader
    void bindBase(GLuint location) const;


    GLuint getHandle() const;
};




} // namespace linde

#endif // LINDE_SHADER_STORAGE_BUFFER
