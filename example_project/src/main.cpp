#include <linde/linde.h>
#include <linde/GLWindow.h>

void buttonCallback()
{
    std::cout << "button pressed" <<std::endl;
}

void dropBoxCallback(int selected)
{
    std::cout << "button pressed" <<std::endl;
}


int main(int argc, char ** args)
{

    linde::GLWindow window(1024, 1024, "example_project");


    int selection = 0;
    std::vector<std::string>	options;
    options.push_back("shoe");
    options.push_back("t-shirt");
    options.push_back("hoody");
    options.push_back("pants");
    options.push_back("hat");
    std::shared_ptr<linde::DropDownBox> dropBox = window.addDropDownBox("clothes", options, &selection, dropBoxCallback);
    float radius = 1;
    window.addSlider<float>("blur radius", 1, 50, &radius);
    float value2 = 5;
    window.addSlider<float>("another slider there", -30.f, 60.f, &value2);
    float value3 = 5;
    window.addSlider<float>("and finally a slider", 0.f, 40.f, &value3);
    bool checked = true;
    window.addCheckBox("render image", &checked);
    window.addButton("a button", &buttonCallback);

    glClearColor(1.f, 1.f, 1.f, 1.f);

    auto renderStep = [&]()
    {
        glClear(GL_COLOR_BUFFER_BIT);

    };

    window.setRenderFunction(renderStep);

    return window.renderLoop(true);
}
