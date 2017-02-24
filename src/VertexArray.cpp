#include "../include/linde/VertexArray.h"

namespace linde
{

VertexArray::VertexArray(GLContext *glContext, const int elementCount) :
    GLObject(glContext),
    m_elementCount(elementCount)
{
    glGenVertexArrays(1, &m_vao_id);
}

VertexArray::~VertexArray()
{
    for(const auto &elem : m_vertexBufferAttribs)
    {
        GLuint bufferID = elem.first;
        glDeleteBuffers(1, &bufferID);
    }

    glDeleteBuffers(1, &m_vao_id);
}

GLuint VertexArray::createVertexBuffer(const GLuint dataSize, const size_t typeSize, const void* data, const GLenum usage)
{
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, dataSize * typeSize, data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_vertexBufferAttribs.insert(std::pair<uint, std::vector<AttribData> >(vertexBuffer, std::vector<AttribData>()));

    return vertexBuffer;
}

void VertexArray::addAttrib(const GLuint buffer, const GLenum type, const GLint components,  GLuint divisor)
{
    AttribData data;
    data.attribType = type;
    data.attribNumComponents = components;
    data.divisor = divisor;
    m_vertexBufferAttribs[buffer].push_back(data);
}

void VertexArray::finishAttribs(const GLboolean normalize)
{
    glBindVertexArray(m_vao_id);

    GLuint vertexAttribArrayID = 0;
    for(const auto &elem : m_vertexBufferAttribs)
    {
        GLuint bufferID = elem.first;
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);

        const std::vector<AttribData> &attribs = elem.second;

        // calculate stride: size of all elements
        size_t stride = 0;
        for(size_t a = 0; a < attribs.size(); ++a)
        {
            const AttribData &attrib = attribs[a];

            switch (attrib.attribType) {
            case GL_UNSIGNED_INT:
                stride += attrib.attribNumComponents * sizeof(GLuint);
                break;

            case GL_INT:
                stride += attrib.attribNumComponents * sizeof(GLint);
                break;

            case GL_FLOAT:
                stride += attrib.attribNumComponents * sizeof(GLfloat);
                break;

            default:
                std::cerr << "Unknown attrib type for vertex array!" << std::endl;
                break;
            }
        }

        // current offset for attrib
        size_t offset = 0;

        for(size_t a = 0; a < attribs.size(); ++a)
        {
            const AttribData &attrib = attribs[a];
            glEnableVertexAttribArray(vertexAttribArrayID);
            glVertexAttribPointer(vertexAttribArrayID, attrib.attribNumComponents, attrib.attribType, normalize, stride, (GLvoid*)offset);

            if(attrib.divisor != 0)
            {
                glVertexAttribDivisor(vertexAttribArrayID, attrib.divisor);
            }

            switch (attrib.attribType) {
            case GL_UNSIGNED_INT:
                offset += attrib.attribNumComponents * sizeof(GLuint);
                break;

            case GL_INT:
                offset += attrib.attribNumComponents * sizeof(GLint);
                break;

            case GL_FLOAT:
                offset += attrib.attribNumComponents * sizeof(GLfloat);
                break;

            default:
                std::cerr << "Unknown attrib type for vertex array!" << std::endl;
                break;
            }

            ++vertexAttribArrayID;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VertexArray::render(GLenum mode)
{
    glBindVertexArray(m_vao_id);

    glDrawArrays(mode, 0, m_elementCount);

    glBindVertexArray(0);
}

void VertexArray::renderInstanced(GLenum mode, GLsizei primcount)
{
    glBindVertexArray(m_vao_id);

    glDrawArraysInstanced(mode, 0, m_elementCount, primcount);

    glBindVertexArray(0);
}

} // namespace linde
