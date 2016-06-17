#ifndef LINDE_VBO_H
#define LINDE_VBO_H

#include "lindeOpenGL.h"


namespace linde
{



/*
#################################################################################
#################################################################################
#################################################################################
################### VBO ########################################################
#################################################################################
#################################################################################
#################################################################################
#################################################################################
#################################################################################
*/



/**
* @author Sören Pirk, Thomas Lindemeier
* @date 03.06.2013
*
* University of Konstanz-
* Department for Computergraphics
*/
class VertexBufferObject : public GLObject
{
public:
    struct DATA
    {
        GLfloat vx, vy, vz, vw;
        GLfloat nx, ny, nz, nw;
        GLfloat cx, cy, cz, cw;
        GLfloat tx, ty, tz, tw;
    };


public:
    VertexBufferObject(GLContext * glContext);
    ~VertexBufferObject();

    void bind();
    GLuint id() const;
    void release();
    void render();

    GLuint createVBO(GLenum target, GLuint dataSize, const void* data, GLenum usage);
    GLvoid setData(const DATA *data, GLenum usage, GLuint nrVertices, GLenum primitiveMode);
    GLvoid updateData(const DATA *data, GLenum usage, GLuint nrVertices, GLenum primitiveMode);
    GLvoid setIndexData(const GLvoid *data, GLenum usage, GLint nrIndices);
    GLvoid addAttrib(GLint attribLoc);
    GLvoid setVerticesToRender(GLuint nrVertices);
    GLvoid bindAttribs();
    GLvoid setDynamicRendering(GLboolean dynamicRendering);

    GLuint nrVertices() const;
    GLuint nrDynamicVertices() const;

private:
    GLuint      m_nrVertices;
    GLuint      m_nrDynamicVertices;
    GLuint      m_nrIndices;
    GLuint      m_nrDynamicIndices;
    GLuint      m_sizeBytesVertices;
    GLuint      m_sizeBytesIndices;
    GLuint      m_bufferId;
    GLuint      m_indexBufferId;
    GLenum      m_primitiveMode;
    GLenum      m_usage;
    GLuint      m_sizeAsStride;
    GLboolean   m_useIndexBuffer;
    std::vector<GLint> m_attribLocations;
    GLboolean       m_dynamicRendering;
};


}// namespace linde

#endif // LINDE_VBO_H
