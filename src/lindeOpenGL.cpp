#include "linde/lindeOpenGL.h"

namespace linde
{
// http://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/
void  APIENTRY openGlDebugCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        void* userParam)
{

    std::cerr << "---------------------opengl-callback-start------------" << std::endl;
    std::cerr << "message: "<< message << std::endl;
    std::cerr << "type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cerr << "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cerr << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cerr << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cerr << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cerr << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cerr << "OTHER";
        break;
    }
    std::cerr << std::endl;

    std::cerr << "id: " << id << std::endl;
    std::cerr << "severity: ";
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW:
        std::cerr << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cerr << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        std::cerr << "HIGH";
        break;
    }
    std::cerr << std::endl;
    std::cerr << "---------------------opengl-callback-end--------------" << std::endl;
    std::cerr << std::endl;
}

void checkGLError(const char * file, const char* function, int line)
{
    GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
        std::string error;

        switch (err)
        {
        case GL_INVALID_OPERATION:      error = "GL_INVALID_OPERATION";      break;
        case GL_INVALID_ENUM:           error = "GL_INVALID_ENUM";           break;
        case GL_INVALID_VALUE:          error = "GL_INVALID_VALUE";          break;
        case GL_OUT_OF_MEMORY:          error = "GL_OUT_OF_MEMORY";             break;
        case GL_STACK_OVERFLOW:         error = "GL_STACK_OVERFLOW";            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "GL_INVALID_FRAMEBUFFER_OPERATION";  break;
            //  case GL_CONTEXT_LOST:  error = "GL_CONTEXT_LOST";  break;
        }

        std::cerr << "ERROR::" << error.c_str() << "\n\tfile:\t"<< file << "\n\tfunction:\t"<< function << ((line == -1) ? " " : "\n\tline:\t" + std::to_string(line)) << std::endl;
        err = glGetError();
    }
}

// project to 2D
GLvoid enable2D()
{
    GLint iViewport[4];
    glGetIntegerv(GL_VIEWPORT, iViewport);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(iViewport[0], iViewport[0] + iViewport[2], iViewport[1] + iViewport[3], iViewport[1], -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

GLvoid glEnable2D()
{
    enable2D();
}

// render texture on screen, enable2D and disable2D get automatically called
GLvoid renderTexture(GLuint texture, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // deactivate color clamping
    glClampColor(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
    glClampColor(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);
    glClampColor(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glColor4f(1.0, 1.0, 1.0, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glDisable(GL_DEPTH_TEST);


    enable2D();
    glPushMatrix();
    glTranslatef(posX, posY, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(width, 0.0f, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(width, height, 0.0f);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.0, height, 0.0f);
    glEnd();
    glPopMatrix();
    disable2D();

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    glPopAttrib();
}


//
GLvoid disable2D()
{
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

GLvoid glDisable2D()
{
    disable2D();
}
} // namespace linde
