#include "Chai.h"
#include <chaiscript/utility/utility.hpp>
#include <linde/File.h>
#include <linde/Convolution.h>

#include <opencv2/highgui/highgui.hpp>


template <class T>
void imShow(const std::string & window, const cv::Mat_<T> & image, int waitmsecs)
{
    if (cv::DataType<T>::channels == 3)
    {
        cv::Mat_<T> bgr;
        cv::cvtColor(image, bgr, cv::COLOR_RGB2BGR);
        cv::imshow(window, bgr);
    }
    else
    {
        cv::imshow(window, image);
    }
    cv::waitKey(waitmsecs);
}

template <class T>
cv::Mat_<T> gaussian_blur(const cv::Mat_<T>& source, float sigma)
{
    cv::Mat_<T> out;
    cv::GaussianBlur(source, out, cv::Size(-1, -1), sigma, 0, cv::BORDER_REFLECT);
    return out;
}

template <class T>
cv::Mat_<T> ComputeGaussDerivativeEnergy(const cv::Mat_<T> & source,
                             const float sigma, const int nrAngles)
{
    cv::Mat_<T> out;
    linde::ComputeGaussDerivativeEnergy(source, out, sigma, nrAngles);
    return out;
}

void addLindeLibFunctions(chaiscript::ChaiScript &chai)
{
    chaiscript::ModulePtr m(new chaiscript::Module());

    // vector types
    chaiscript::utility::add_class<glm::vec3>(*m,
                                   "vec3",
    {
        chaiscript::constructor<glm::vec3()>(),
        chaiscript::constructor<glm::vec3(float, float, float)>()
    },
    {
        { chaiscript::fun(&glm::vec3::x), "x" },
        { chaiscript::fun(&glm::vec3::y), "y" },
        { chaiscript::fun(&glm::vec3::z), "z" }
    });


    chaiscript::utility::add_class<glm::vec2>(*m,
                                   "vec2",
    {
        chaiscript::constructor<glm::vec2()>(),
        chaiscript::constructor<glm::vec2(float, float)>()
    },
    {
        { chaiscript::fun(&glm::vec2::x), "x" },
        { chaiscript::fun(&glm::vec2::y), "y" }
    });

    chaiscript::utility::add_class<glm::ivec2>(*m,
                                   "ivec2",
    {
        chaiscript::constructor<glm::ivec2()>(),
        chaiscript::constructor<glm::ivec2(int, int)>()
    },
    {
        { chaiscript::fun(&glm::ivec2::x), "x" },
        { chaiscript::fun(&glm::ivec2::y), "y" }
    });

    // image types
    chaiscript::utility::add_class<cv::Mat_<glm::vec3>>(*m,
                                   "Mat3f",
    {
        chaiscript::constructor<cv::Mat_<glm::vec3>()>(),
        chaiscript::constructor<cv::Mat_<glm::vec3>(int, int)>()
    },
    {
        { chaiscript::fun(&cv::Mat_<glm::vec3>::rows), "rows" },
        { chaiscript::fun(&cv::Mat_<glm::vec3>::cols), "cols" },
       // { chaiscript::fun<const glm::vec3&, int, int>(&cv::Mat_<glm::vec3>::at), "at" }

    });
    chaiscript::utility::add_class<cv::Mat_<float>>(*m,
                                   "Mat1f",
    {
        chaiscript::constructor<cv::Mat_<float>()>(),
        chaiscript::constructor<cv::Mat_<float>(int, int)>()
    },
    {
        { chaiscript::fun(&cv::Mat_<float>::rows), "rows" },
        { chaiscript::fun(&cv::Mat_<float>::cols), "cols" },
       // { chaiscript::fun<const glm::vec3&, int, int>(&cv::Mat_<glm::vec3>::at), "at" }

    });

    chai.add(m);

    // image load and save
    chai.add(chaiscript::fun<cv::Mat_<glm::vec3>, const std::string&>(&linde::imLoad), "imload");
    chai.add(chaiscript::fun<cv::Mat_<float>, const std::string&>(&linde::imLoadSingleChannel), "imload_single_channel");

    chai.add(chaiscript::fun<bool, const std::string&, const cv::Mat_<glm::vec3>&>(&linde::imSave), "imsave");
    chai.add(chaiscript::fun<bool, const std::string&, const cv::Mat_<float>&>(&linde::imSave), "imsave");

    // imgproc
    chai.add(chaiscript::fun<cv::Mat_<glm::vec3>, const cv::Mat_<glm::vec3>&, float>(&gaussian_blur), "gaussian_blur");
    chai.add(chaiscript::fun<cv::Mat_<glm::vec3>, const cv::Mat_<glm::vec3>&, float, int>(&ComputeGaussDerivativeEnergy), "gaussian_derivative_energy");

    // image show
    chai.add(chaiscript::fun<void, const std::string&, const cv::Mat_<glm::vec3>&, int>(&imShow), "imshow");
    chai.add(chaiscript::fun<void, const std::string&, const cv::Mat_<float>&, int>(&imShow), "imshow");
}
