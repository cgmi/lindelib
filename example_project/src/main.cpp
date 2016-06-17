#include <linde/linde.h>
#include <linde/GLWindow.h>


int main(int argc, char ** args)
{

    linde::GLWindow window(1024, 1024, "example_project");

    glClearColor(1.f, 1.f, 1.f, 1.f);

    auto renderStep = [&]()
    {
        glClear(GL_COLOR_BUFFER_BIT);

    };

    window.setRenderFunction(renderStep);

    return window.renderLoop(true);
}
