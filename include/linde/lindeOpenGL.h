#ifndef LINDEOPENGL
#define LINDEOPENGL

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linde.h"

namespace linde
{


#define VERTEX_POSITION 0
#define VERTEX_NORMAL   1
#define VERTEX_COLOR    2
#define VERTEX_TEXTURE  3

// project to 2D
GLvoid enable2D();
GLvoid glEnable2D();
//
GLvoid disable2D();
GLvoid glDisable2D();

// render texture on screen, enable2D and disable2D get automatically called
GLvoid renderTexture(GLuint texture, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height);

void checkGLError(const char *file, const char* function, int line = -1);

#ifdef DEBUG_BUILD
#define CHECK_OPENGL_ERROR checkGLError(__FILE__, __func__, __LINE__)
#else
#define CHECK_OPENGL_ERROR 0
#endif

void  APIENTRY openGlDebugCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        void* userParam);

} // namespace linde

#endif // LINDEOPENGL

