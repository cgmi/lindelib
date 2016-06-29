#include "Chai.h"
#include <chaiscript/utility/utility.hpp>
#include <linde/File.h>
#include <linde/Convolution.h>
#include <linde/ResourceHandler.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

cv::Mat_<glm::vec3> createMat3(int rows, int cols, const glm::vec3 & v)
{
    cv::Mat_<glm::vec3> m(rows, cols);
    m.setTo(cv::Scalar(v.x, v.y, v.z));
    return m;
}

cv::Mat_<float> createMat1(int rows, int cols, const float & v)
{
    cv::Mat_<float> m(rows, cols);
    m.setTo(v);
    return m;
}

cv::Mat_<glm::vec3> createMat3_vec_value(const glm::ivec2 & dim, const glm::vec3 & v)
{
    cv::Mat_<glm::vec3> m(dim.y, dim.x);
    m.setTo(cv::Scalar(v.x, v.y, v.z));
    return m;
}

cv::Mat_<float> createMat1_vec_value(const glm::ivec2 & dim, const float & v)
{
    cv::Mat_<float> m(dim.y, dim.x);
    m.setTo(v);
    return m;
}

cv::Mat_<glm::vec3> createMat3_vec(const glm::ivec2 & dim)
{
    return cv::Mat_<glm::vec3>(dim.y, dim.x);
}

cv::Mat_<float> createMat1_vec(const glm::ivec2 & dim)
{
    return cv::Mat_<float>(dim.y, dim.x);
}


std::string vec3_to_string(const glm::vec3 & v)
{
    return "("+std::to_string(v.x)+", "+std::to_string(v.y)+", "+std::to_string(v.z)+")";
}

std::string vec2_to_string(const glm::vec2 & v)
{
    return "("+std::to_string(v.x)+", "+std::to_string(v.y)+")";
}

std::string ivec2_to_string(const glm::ivec2 & v)
{
    return "("+std::to_string(v.x)+", "+std::to_string(v.y)+")";
}

std::string mat_to_string(const cv::Mat & v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

template <class T>
cv::Mat_<T> filter2DWrapper(const cv::Mat_<T> & source, const cv::Mat_<float> & kernel)
{
    cv::Mat_<T> out;
    cv::filter2D(source, out, source.depth(), kernel, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);
    return out;
}

cv::Mat_<float> gaborWrapper(const glm::ivec2 & kernelSize, const float sigma, const float theta, const float lambda, const float gamma, const float psi)
{
    cv::Mat_<double> gabor = cv::getGaborKernel(cv::Size(kernelSize.x, kernelSize.y), sigma, theta, lambda, gamma, psi);
    cv::Mat_<float> out;
    gabor.convertTo(out, CV_32FC1, 1.0, 0.0);
    return out;
}


template <class T>
cv::Mat_<T> resizeWrapper(const cv::Mat_<T> & source, const glm::ivec2 & size, cv::InterpolationFlags interpolation)
{
    cv::Mat_<T> out;
    cv::resize(source, out, cv::Size(size.x, size.y), 0.0, 0.0, interpolation);
    return out;
}

cv::Mat_<glm::vec3> convert_image_wrapper(const cv::Mat_<glm::vec3> & source, std::function<void(const glm::vec3& a, glm::vec3&b)> conversion)
{
    cv::Mat_<glm::vec3> out(source.size());
    for (uint i = 0; i < source.total(); i++)
    {
        conversion(source(i), out(i));
    }
    return out;
}


void addVectorTypes(chaiscript::ChaiScript &chai)
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
                                                  { chaiscript::fun(&glm::vec3::z), "z" },

                                                  { chaiscript::fun<glm::vec3&, glm::vec3, glm::vec3 const &>(&glm::vec3::operator=), "=" },
                                                  { chaiscript::fun<glm::vec3&, glm::vec3, glm::vec3 const &>(&glm::vec3::operator+=), "+=" },
                                                  { chaiscript::fun<glm::vec3&, glm::vec3, glm::vec3 const &>(&glm::vec3::operator-=), "-=" },
                                                  { chaiscript::fun<glm::vec3&, glm::vec3, glm::vec3 const &>(&glm::vec3::operator/=), "/=" },
                                                  { chaiscript::fun<glm::vec3&, glm::vec3, glm::vec3 const &>(&glm::vec3::operator*=), "*=" }
                                              });


    chaiscript::utility::add_class<glm::vec2>(*m,
                                              "vec2",
    {
                                                  chaiscript::constructor<glm::vec2()>(),
                                                  chaiscript::constructor<glm::vec2(float, float)>()
                                              },
    {
                                                  { chaiscript::fun(&glm::vec2::x), "x" },
                                                  { chaiscript::fun(&glm::vec2::y), "y" },

                                                  { chaiscript::fun<glm::vec2&, glm::vec2, glm::vec2 const &>(&glm::vec2::operator=), "=" },
                                                  { chaiscript::fun<glm::vec2&, glm::vec2, glm::vec2 const &>(&glm::vec2::operator+=), "+=" },
                                                  { chaiscript::fun<glm::vec2&, glm::vec2, glm::vec2 const &>(&glm::vec2::operator-=), "-=" },
                                                  { chaiscript::fun<glm::vec2&, glm::vec2, glm::vec2 const &>(&glm::vec2::operator/=), "/=" },
                                                  { chaiscript::fun<glm::vec2&, glm::vec2, glm::vec2 const &>(&glm::vec2::operator*=), "*=" }
                                              });

    chaiscript::utility::add_class<glm::ivec2>(*m,
                                               "ivec2",
    {
                                                   chaiscript::constructor<glm::ivec2()>(),
                                                   chaiscript::constructor<glm::ivec2(int, int)>()
                                               },
    {
                                                   { chaiscript::fun(&glm::ivec2::x), "x" },
                                                   { chaiscript::fun(&glm::ivec2::y), "y" },

                                                   { chaiscript::fun<glm::ivec2&, glm::ivec2, glm::ivec2 const &>(&glm::ivec2::operator=), "=" },
                                                   { chaiscript::fun<glm::ivec2&, glm::ivec2, glm::ivec2 const &>(&glm::ivec2::operator+=), "+=" },
                                                   { chaiscript::fun<glm::ivec2&, glm::ivec2, glm::ivec2 const &>(&glm::ivec2::operator-=), "-=" },
                                                   { chaiscript::fun<glm::ivec2&, glm::ivec2, glm::ivec2 const &>(&glm::ivec2::operator/=), "/=" },
                                                   { chaiscript::fun<glm::ivec2&, glm::ivec2, glm::ivec2 const &>(&glm::ivec2::operator*=), "*=" }
                                               });

    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &, glm::vec3 const &>(&glm::operator+), "+");
    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &, glm::vec3 const &>(&glm::operator-), "-");
    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &, glm::vec3 const &>(&glm::operator/), "/");
    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &, glm::vec3 const &>(&glm::operator*), "*");
    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &, float const &>(&glm::operator+), "+");
    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &, float const &>(&glm::operator-), "-");
    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &, float const &>(&glm::operator/), "/");
    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &, float const &>(&glm::operator*), "*");
    m->add(chaiscript::fun<glm::vec3, float const &, glm::vec3 const &>(&glm::operator+), "+");
    m->add(chaiscript::fun<glm::vec3, float const &, glm::vec3 const &>(&glm::operator-), "-");
    m->add(chaiscript::fun<glm::vec3, float const &, glm::vec3 const &>(&glm::operator/), "/");
    m->add(chaiscript::fun<glm::vec3, float const &, glm::vec3 const &>(&glm::operator*), "*");

    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &, glm::vec2 const &>(&glm::operator+), "+");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &, glm::vec2 const &>(&glm::operator-), "-");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &, glm::vec2 const &>(&glm::operator/), "/");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &, glm::vec2 const &>(&glm::operator*), "*");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &, float const &>(&glm::operator+), "+");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &, float const &>(&glm::operator-), "-");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &, float const &>(&glm::operator/), "/");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &, float const &>(&glm::operator*), "*");
    m->add(chaiscript::fun<glm::vec2, float const &, glm::vec2 const &>(&glm::operator+), "+");
    m->add(chaiscript::fun<glm::vec2, float const &, glm::vec2 const &>(&glm::operator-), "-");
    m->add(chaiscript::fun<glm::vec2, float const &, glm::vec2 const &>(&glm::operator/), "/");
    m->add(chaiscript::fun<glm::vec2, float const &, glm::vec2 const &>(&glm::operator*), "*");

    m->add(chaiscript::fun<glm::ivec2, glm::ivec2 const &, glm::ivec2 const &>(&glm::operator+), "+");
    m->add(chaiscript::fun<glm::ivec2, glm::ivec2 const &, glm::ivec2 const &>(&glm::operator-), "-");
    m->add(chaiscript::fun<glm::ivec2, glm::ivec2 const &, glm::ivec2 const &>(&glm::operator/), "/");
    m->add(chaiscript::fun<glm::ivec2, glm::ivec2 const &, glm::ivec2 const &>(&glm::operator*), "*");
    m->add(chaiscript::fun<glm::ivec2, glm::ivec2 const &, int const &>(&glm::operator+), "+");
    m->add(chaiscript::fun<glm::ivec2, glm::ivec2 const &, int const &>(&glm::operator-), "-");
    m->add(chaiscript::fun<glm::ivec2, glm::ivec2 const &, int const &>(&glm::operator/), "/");
    m->add(chaiscript::fun<glm::ivec2, glm::ivec2 const &, int const &>(&glm::operator*), "*");
    m->add(chaiscript::fun<glm::ivec2, int const &, glm::ivec2 const &>(&glm::operator+), "+");
    m->add(chaiscript::fun<glm::ivec2, int const &, glm::ivec2 const &>(&glm::operator-), "-");
    m->add(chaiscript::fun<glm::ivec2, int const &, glm::ivec2 const &>(&glm::operator/), "/");
    m->add(chaiscript::fun<glm::ivec2, int const &, glm::ivec2 const &>(&glm::operator*), "*");

    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &>(&glm::normalize), "normalize");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &>(&glm::normalize), "normalize");

    m->add(chaiscript::fun<float, glm::vec2 const &, glm::vec2 const &>(&glm::distance), "distance");
    m->add(chaiscript::fun<float, glm::vec3 const &, glm::vec3 const &>(&glm::distance), "distance");
    m->add(chaiscript::fun<float, glm::vec2 const &, glm::vec2 const &>(&glm::distance2), "distance2");
    m->add(chaiscript::fun<float, glm::vec3 const &, glm::vec3 const &>(&glm::distance2), "distance2");
    m->add(chaiscript::fun<float, glm::vec2 const &>(&glm::length), "length");
    m->add(chaiscript::fun<float, glm::vec3 const &>(&glm::length), "length");

    m->add(chaiscript::fun(&vec3_to_string), "to_string");
    m->add(chaiscript::fun(&vec2_to_string), "to_string");
    m->add(chaiscript::fun(&ivec2_to_string), "to_string");

    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &, glm::vec3 const &>(&glm::pow), "pow");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &, glm::vec2 const &>(&glm::pow), "pow");
    m->add(chaiscript::fun<float, float, float>(&glm::pow), "pow");

    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &>(&glm::exp), "exp");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &>(&glm::exp), "exp");
    m->add(chaiscript::fun<float, float>(&glm::exp), "exp");

    m->add(chaiscript::fun<glm::vec3, glm::vec3 const &>(&glm::sqrt), "sqrt");
    m->add(chaiscript::fun<glm::vec2, glm::vec2 const &>(&glm::sqrt), "sqrt");
    m->add(chaiscript::fun<float, float>(&glm::sqrt), "sqrt");

    chai.add(m);
}

void addImageTypes(chaiscript::ChaiScript &chai)
{
    addVectorTypes(chai);

    chaiscript::ModulePtr m(new chaiscript::Module());

    m->add(chaiscript::base_class<cv::Mat, cv::Mat_<glm::vec3> >());
    m->add(chaiscript::base_class<cv::Mat, cv::Mat_<float> >());

    m->add(chaiscript::fun(&mat_to_string), "to_string");
    m->add(chaiscript::fun(&cv::Mat::rows), "rows");
    m->add(chaiscript::fun(&cv::Mat::cols), "cols");
    m->add(chaiscript::fun(&cv::Mat::empty), "empty");
    m->add(chaiscript::fun(&cv::Mat::total), "total");
    m->add(chaiscript::constructor<cv::Mat_<glm::vec3>()>(), "Mat3");
    m->add(chaiscript::constructor<cv::Mat_<float>()>(), "Mat1");
    m->add(chaiscript::constructor<cv::Mat_<glm::vec3>(int, int)>(), "Mat3");
    m->add(chaiscript::constructor<cv::Mat_<float>(int, int)>(), "Mat1");
    m->add(chaiscript::fun(&createMat3), "Mat3");
    m->add(chaiscript::fun(&createMat1), "Mat1");
    m->add(chaiscript::fun(&createMat3_vec), "Mat3");
    m->add(chaiscript::fun(&createMat1_vec), "Mat1");
    m->add(chaiscript::fun(&createMat3_vec_value), "Mat3");
    m->add(chaiscript::fun(&createMat1_vec_value), "Mat1");

    m->add(chaiscript::fun<float&, cv::Mat_<float>, int, int>(&cv::Mat_<float>::operator()), "at");
    m->add(chaiscript::fun<glm::vec3&, cv::Mat_<glm::vec3>, int, int>(&cv::Mat_<glm::vec3>::operator()), "at");
    m->add(chaiscript::fun<float&, cv::Mat_<float>, int>(&cv::Mat_<float>::operator()), "at");
    m->add(chaiscript::fun<glm::vec3&, cv::Mat_<glm::vec3>, int>(&cv::Mat_<glm::vec3>::operator()), "at");

    m->add(chaiscript::fun<const float&, cv::Mat_<float>, int, int>(&cv::Mat_<float>::operator()), "at");
    m->add(chaiscript::fun<const glm::vec3&, cv::Mat_<glm::vec3>, int, int>(&cv::Mat_<glm::vec3>::operator()), "at");
    m->add(chaiscript::fun<const float&, cv::Mat_<float>, int>(&cv::Mat_<float>::operator()), "at");
    m->add(chaiscript::fun<const glm::vec3&, cv::Mat_<glm::vec3>, int>(&cv::Mat_<glm::vec3>::operator()), "at");

    chai.add(m);
}

void addImageIO(chaiscript::ChaiScript &chai)
{
    chaiscript::ModulePtr m(new chaiscript::Module());

    // image load and save
    m->add(chaiscript::fun<cv::Mat_<glm::vec3>, const std::string&>(&linde::imLoad), "imload");
    m->add(chaiscript::fun<cv::Mat_<float>, const std::string&>(&linde::imLoadSingleChannel), "imload_single_channel");

    m->add(chaiscript::fun<bool, const std::string&, const cv::Mat_<glm::vec3>&>(&linde::imSave), "imsave");
    m->add(chaiscript::fun<bool, const std::string&, const cv::Mat_<float>&>(&linde::imSave), "imsave");

    // image show
    m->add(chaiscript::fun<void, const std::string&, const cv::Mat_<glm::vec3>&, int>(&imShow), "imshow");
    m->add(chaiscript::fun<void, const std::string&, const cv::Mat_<float>&, int>(&imShow), "imshow");

    // Resource
    m->add(chaiscript::fun(&ResourceHandler::getResource), "getResource");

    chai.add(m);
}

void addImageProc(chaiscript::ChaiScript &chai)
{
    chaiscript::ModulePtr m(new chaiscript::Module());

    chaiscript::utility::add_class<cv::InterpolationFlags>(*m,
                                                           "InterpolationFlags",
    { { cv::INTER_NEAREST, "INTER_NEAREST" },
      { cv::INTER_LINEAR, "INTER_LINEAR" },
      { cv::INTER_CUBIC, "INTER_CUBIC" },
      { cv::INTER_AREA, "INTER_AREA" },
      { cv::INTER_LANCZOS4, "INTER_LANCZOS4" }

                                                           }
                                                           );
    m->add(chaiscript::fun(&resizeWrapper<glm::vec3>), "resize");
    m->add(chaiscript::fun(&resizeWrapper<float>), "resize");

    // imgproc
    m->add(chaiscript::fun<cv::Mat_<glm::vec3>, const cv::Mat_<glm::vec3>&, float>(&gaussian_blur), "gaussian_blur");
    m->add(chaiscript::fun<cv::Mat_<glm::vec3>, const cv::Mat_<glm::vec3>&, float, int>(&ComputeGaussDerivativeEnergy), "gaussian_derivative_energy");
    m->add(chaiscript::fun(&filter2DWrapper<glm::vec3>), "convolve");
    m->add(chaiscript::fun(&filter2DWrapper<float>), "convolve");
    m->add(chaiscript::fun(&gaborWrapper), "createGaborKernel");
    m->add(chaiscript::fun(&linde::createGauss1stDerivativeKernel), "createGauss1stDerivativeKernel");
    m->add(chaiscript::fun(&linde::createGauss2ndDerivativeKernel), "createGauss2ndDerivativeKernel");

    chai.add(m);
}

void addColorFunctions(chaiscript::ChaiScript &chai)
{
    chaiscript::ModulePtr m(new chaiscript::Module());

    m->add(chaiscript::fun(&linde::convert_cmy2rgb), "convert_cmy2rgb");
    m->add(chaiscript::fun(&linde::convert_lab2xyz), "convert_lab2xyz");
    m->add(chaiscript::fun(&linde::convert_xyz2lab), "convert_xyz2lab");
    m->add(chaiscript::fun(&linde::convert_lab2LCHab), "convert_lab2LCHab");
    m->add(chaiscript::fun(&linde::convert_LCHab2lab), "convert_LCHab2lab");
    m->add(chaiscript::fun(&linde::convert_ryb2rgb), "convert_ryb2rgb");
    m->add(chaiscript::fun(&linde::convert_rgb2cmy), "convert_rgb2cmy");
    m->add(chaiscript::fun(&linde::convert_cmy2rgb), "convert_cmy2rgb");
    m->add(chaiscript::fun(&linde::convert_rgb2xyz), "convert_rgb2xyz");
    m->add(chaiscript::fun(&linde::convert_xyz2rgb), "convert_xyz2rgb");
    m->add(chaiscript::fun(&linde::convert_srgb2rgb), "convert_srgb2rgb");
    m->add(chaiscript::fun(&linde::convert_rgb2srgb), "convert_rgb2srgb");
    m->add(chaiscript::fun(&linde::convert_lab2srgb), "convert_lab2srgb");
    m->add(chaiscript::fun(&linde::convert_srgb2lab), "convert_srgb2lab");
    m->add(chaiscript::fun(&linde::convert_lab2rgb), "convert_lab2rgb");
    m->add(chaiscript::fun(&linde::convert_rgb2lab), "convert_rgb2lab");
    m->add(chaiscript::fun(&linde::convert_xyz2srgb), "convert_xyz2srgb");
    m->add(chaiscript::fun(&linde::convert_hsv2srgb), "convert_hsv2srgb");
    m->add(chaiscript::fun(&linde::convert_srgb2hsv), "convert_srgb2hsv");
    m->add(chaiscript::fun(&linde::convert_srgb2xyz), "convert_srgb2xyz");
    m->add(chaiscript::fun(&linde::convert_xyz2xyY), "convert_xyz2xyY");
    m->add(chaiscript::fun(&linde::convert_xyY2xyz), "convert_xyY2xyz");
    m->add(chaiscript::fun(&linde::convert_Luv2XYZ), "convert_Luv2XYZ");
    m->add(chaiscript::fun(&linde::convert_XYZ2Luv), "convert_XYZ2Luv");
    m->add(chaiscript::fun(&linde::convert_Luv2LCHuv), "convert_Luv2LCHuv");
    m->add(chaiscript::fun(&linde::convert_LCHuv2Luv), "convert_LCHuv2Luv");
    m->add(chaiscript::fun(&linde::convert_Yuv2rgb), "convert_Yuv2rgb");
    m->add(chaiscript::fun(&linde::convert_rgb2Yuv), "convert_rgb2Yuv");
    m->add(chaiscript::fun(&linde::convert_rgb2L_alpha_beta), "convert_rgb2L_alpha_beta");
    m->add(chaiscript::fun(&linde::convert_L_alpha_beta2rgb), "convert_L_alpha_beta2rgb");

    m->add(chaiscript::fun(&convert_image_wrapper), "convert");

    chai.add(m);
}


void addLindeLibFunctions(chaiscript::ChaiScript &chai)
{
    addImageTypes(chai);
    addImageIO(chai);
    addImageProc(chai);
    addColorFunctions(chai);
}
