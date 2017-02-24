#ifndef VERTEX_ARRAY
#define VERTEX_ARRAY

#include "lindeOpenGL.h"

#include <map>

/**
* @author Marc Spicker
* @date 24.02.2017
*
*/

// Usage:

// struct VertexData
// {
//    glm::vec4 position;
//    glm::vec4 normal;
//    glm::vec4 color;
//    glm::vec4 texture;
// };
//
// VertexData *data = new VertexData[vertices.size()];
//
// std::shared_ptr<linde::VertexArray> vertexArray = glWindow->createVertexArray(vertices.size());
// GLuint vboID = vertexArray->createVertexBuffer(vertices.size(), sizeof(VertexData), data, GL_STATIC_DRAW);
//
// vertexArray->addAttrib(vboID, GL_FLOAT, 4);
// vertexArray->addAttrib(vboID, GL_FLOAT, 4);
// vertexArray->addAttrib(vboID, GL_FLOAT, 4);
// vertexArray->addAttrib(vboID, GL_FLOAT, 4);
// vertexArray->finishAttribs();

namespace linde
{

struct AttribData
{
    GLenum attribType;
    GLint attribNumComponents;
    GLuint divisor;
};

class VertexArray : public GLObject
{

public:

    VertexArray(GLContext *glContext, const int elementCount);
    ~VertexArray();

    GLuint createVertexBuffer(const GLuint dataSize, const size_t typeSize, const void* data, const GLenum usage);
    void addAttrib(const GLuint buffer, const GLenum type, const GLint components, GLuint divisor = 0);
    void finishAttribs(const GLboolean normalize = GL_FALSE);
    void render(GLenum mode);
    void renderInstanced(GLenum mode, GLsizei primcount);

private:

    GLuint                                      m_vao_id;
    GLuint                                      m_elementCount;
    std::map<GLuint, std::vector<AttribData> >  m_vertexBufferAttribs;
};

} // namespace linde

#endif // VERTEX_ARRAY
