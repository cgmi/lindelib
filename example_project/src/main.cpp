#include <linde/linde.h>
#include <linde/GLWindow.h>
#include <linde/File.h>
#include <linde/Texture.h>
#include <linde/Shader.h>


#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

void buttonCallback()
{
    std::cout << "button pressed" <<std::endl;
}


int main(int argc, char ** args)
{

    myOut << std::numeric_limits<float>::min() << " " << std::numeric_limits<float>::epsilon() << " " << std::numeric_limits<float>::max();

    cv::Mat_<glm::vec3> image = linde::imLoad("../resource/landscape-photography-1.jpg");
    cv::resize(image, image, cv::Size(image.cols * 2.f, image.rows * 2.f));

    linde::GLWindow window(image.cols, image.rows, "example_01");

    std::shared_ptr<linde::Shader> shader = window.createPipelineShader("shaders/identity44.vert", "shaders/identity44.frag");

    cv::Mat_<glm::vec4> rgba_image(image.size());
    for (int i = 0; i < image.cols*image.rows; i++)
    {
        rgba_image(i).x = image(i).x;
        rgba_image(i).y = image(i).y;
        rgba_image(i).z = image(i).z;
        rgba_image(i).w = 1.f;
    }

    std::shared_ptr<linde::Texture> tex = window.createTexture(rgba_image.cols, rgba_image.rows, GL_RGBA, GL_RGBA, GL_FLOAT);
    cv::flip(rgba_image, rgba_image, 0);
    tex->create(rgba_image.data);

    int selection = 0;
    std::vector<std::string>	options;
    options.push_back("shoe");
    options.push_back("t-shirt");
    options.push_back("hoody");
    options.push_back("pants");
    options.push_back("hat");
    window.addDropDownBox("clothes", options, &selection);
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

        if (checked)
            tex->render(0.f, 0.f, window.getWidth(), window.getHeight());
    };

    return window.renderLoop(renderStep, true);
}
