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

    chai.add(m);
}

void addImageTypes(chaiscript::ChaiScript &chai)
{
    addVectorTypes(chai);

    chaiscript::ModulePtr m(new chaiscript::Module());

    m->add(chaiscript::base_class<cv::Mat, cv::Mat_<glm::vec3> >());
    m->add(chaiscript::base_class<cv::Mat, cv::Mat_<float> >());

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

    // imgproc
    m->add(chaiscript::fun<cv::Mat_<glm::vec3>, const cv::Mat_<glm::vec3>&, float>(&gaussian_blur), "gaussian_blur");
    m->add(chaiscript::fun<cv::Mat_<glm::vec3>, const cv::Mat_<glm::vec3>&, float, int>(&ComputeGaussDerivativeEnergy), "gaussian_derivative_energy");

    // image show
    m->add(chaiscript::fun<void, const std::string&, const cv::Mat_<glm::vec3>&, int>(&imShow), "imshow");
    m->add(chaiscript::fun<void, const std::string&, const cv::Mat_<float>&, int>(&imShow), "imshow");

    chai.add(m);
}

void addLindeLibFunctions(chaiscript::ChaiScript &chai)
{
    addImageTypes(chai);
    addImageIO(chai);
}
