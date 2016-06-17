#ifndef LINDE_CONVOLUTION_H
#define LINDE_CONVOLUTION_H
//#include <opencv2/highgui/highgui.hpp>

#include "linde.h"
#include "File.h"

namespace linde
{

// Gauss functions
template <class T>
inline T gauss(const T &  x, const T & sigma)
{
    T sigmasquare = sigma * sigma;
    return exp(-x*x/(2*sigmasquare)) / (sqrt(TWO_PI<T>()*sigmasquare));
}

template <class T>
inline T gauss(const T & x, const T &  y, const T & sigma)
{
    T sigmasquare = sigma * sigma;
    return exp(-(x*x + y*y)/(2*sigmasquare)) / (TWO_PI<T>()*sigmasquare);
}

template <class T>
inline T gauss(const T & x_, const T &  y_, const T & sigma_x, const T & sigma_y, const T theta, bool normalized = true)
{
    const T x = x_*cos(theta)+y_*sin(theta);
    const T y = -x_*sin(theta)+y_*cos(theta);

    const T exponential = glm::exp(-((x*x)/(T(2)*sigma_x*sigma_x))+((y*y)/(T(2)*sigma_y*sigma_y)));
    if (normalized)
    {
        const T norm = (TWO_PI<T>()*sigma_x*sigma_y);
        return exponential / norm;
    }
    return exponential;
}

template <class T>
inline T gauss1deriv(const T & dx, const T & dy, const T & sigma, const T & theta, bool normalized = true)
{
    const T x = dx*cos(theta)+dy*sin(theta);
    const T y = -dx*sin(theta)+dy*cos(theta);
    T sigmasquare = sigma*sigma;

    if (!normalized)
    {
        return (-(x / sigmasquare)) * glm::exp(-(x*x + y*y) / (T(2)*sigmasquare));
    }

    return (-x/(TWO_PI<T>()*sigmasquare*sigmasquare)) * glm::exp(-(x*x + y*y) / (T(2)*sigmasquare));
}

template <class T>
inline T gauss2deriv(const T & dx, const T & dy, const T & sigma, const T & theta, bool normalized = true)
{
    const T x = dx*cos(theta)+dy*sin(theta);
    const T y = -dx*sin(theta)+dy*cos(theta);
    T sigmasquare = sigma*sigma;
    T xsquare = x*x;
    T ysquare = y*y;

    if (!normalized)
    {
        return ((x - sigmasquare) / (sigmasquare*sigmasquare)) * glm::exp(-(x*x + y*y) / (T(2)*sigmasquare));
    }

    return (-T(1) + (xsquare / sigmasquare)) * (exp(-(xsquare+ysquare)/(T(2)*sigmasquare))/(TWO_PI<T>()*sigmasquare*sigmasquare));
}

//
template <class T>
inline T laPlacianOfGaussian(const T & dx, const T & dy, const T & sigma)
{
    T sigmasquare = sigma*sigma;
    return -(T(1) / (PI<float>() * sigmasquare*sigmasquare)) * (T(1) - ((dx*dx + dy*dy) / (T(2) * sigmasquare))) * exp(-(dx*dx + dy*dy) / (T(2)*sigmasquare));
}

template <class T>
inline int gaussKernelSizeFromSigma(T sigma)
{
    return (int)(2.0 * std::floor( std::sqrt(-std::log(0.1) * 2 * (sigma*sigma)) ) + 1.0);
}

template <class T>
inline T gaussSigmaFromKernelSize(T size)
{
    return (T) (0.3*((size-1) * 0.5 - 1.0) + 0.8);
}

inline int gaussKernelRadiusFromSigma(float sigma)
{
    return (gaussKernelSizeFromSigma(sigma) - 1) / 2;
}

template <class T>
inline T gaussSigmaFromKernelRadius(int radius)
{
    return gaussSigmaFromKernelSize<T>(radius*2 + 1);
}

template <class Type>
inline void kernelScharrParametricX(cv::Mat_<Type> & kernel_x, const Type p1 = 0.183f)
{
    kernel_x.create(3, 3);
    kernel_x.setTo(Type(0.f));
    kernel_x(0, 0) = p1;
    kernel_x(2, 0) = p1;
    kernel_x(2, 2) = -p1;
    kernel_x(0, 2) = -p1;
    kernel_x(1, 0) = Type(1) - Type(2)*p1;
    kernel_x(1, 2) = Type(2)*p1 - Type(1);
}

// lambda = wavelength of sinusoidal factor
// theta orientation of the normal of the parallel stripes
// sigmaEnvelope = sigma of gaussian envelope
// phi = phase offset
// gamma = aspect ratio, ellipticity
/*
 Gabor filters and such. To be greatly extended to have full texture analysis.
 For the formulas and the explanation of the parameters see:
 http://en.wikipedia.org/wiki/Gabor_filter
*/
cv::Mat_<double> createGaborKernel( cv::Size ksize, double sigma, double theta,
                                    double lambd, double gamma, double psi);

cv::Mat_<float> createGauss1stDerivativeKernel(cv::Size ksize, const float sigma, const float theta, bool normalized = true);
cv::Mat_<float> createGauss2ndDerivativeKernel(cv::Size ksize, const float sigma, const float theta, bool normalized = true);

template <class T>
void ComputeGaborEnergy(const cv::Mat_<T> & source, cv::Mat_<T> & out,
                        const double lambda, const double theta,
                        const double gamma = 0.7,
                        const double sigmaEnvelope = -1.)
{
    const double psi = 0.;
    const double psi_2 = psi - (PI<double>() / 2.);
    const double sigma = (sigmaEnvelope < 0.0) ? 0.5*lambda : sigmaEnvelope;
    cv::Mat_<double> kernel_0 = createGaborKernel(cv::Size(-1, -1), sigma, theta, lambda, gamma, psi);
    cv::Mat_<double> kernel_1 = createGaborKernel(cv::Size(-1, -1), sigma, theta, lambda, gamma, psi_2);

    cv::Mat_<T> a0, a1;
    cv::filter2D(source, a0, -1, kernel_0, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);
    cv::filter2D(source, a1, -1, kernel_1, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);

    cv::Mat_<T> gaborEnergy(source.size());
    for (int l = 0; l < source.cols*source.rows; l++)
    {
        gaborEnergy(l) = glm::sqrt(glm::pow(a0(l), T(2.f)) + glm::pow(a1(l), T(2.f)));
    }

    cv::normalize(gaborEnergy, out, T(0.), T(1.), cv::NORM_MINMAX);
}

template <class T>
void ComputeGaborEnergy(const cv::Mat_<T> & source,
                        cv::Mat_<T> & responseSuperposition,
                        const double lambda, const int nrAngles = 6,
                        const double gamma = 0.7,
                        const double sigmaEnvelope = -1.)
{

    cv::Mat_<T> superposition(source.size());
    for (int l = 0; l < source.cols*source.rows; l++)
    {
        superposition(l) = T(0);
    }

    float s = 1.f / nrAngles;

    cv::Mat_<T> energy;
    for (int i = 0; i  < nrAngles; i++)
    {
        const double theta = mapRange<double>(i, 0, nrAngles, 0.f, PI<double>());

        ComputeGaborEnergy(source, energy, lambda, theta, gamma, sigmaEnvelope);

        for (int l = 0; l < source.cols*source.rows; l++)
        {
            superposition(l) += s*energy(l);
        }
    }
    responseSuperposition = superposition;
}

template <class T>
void ComputeGaussDerivativeEnergy(const cv::Mat_<T> & source, cv::Mat_<T> & out,
                                  const double sigma, const double theta)
{

    cv::Mat_<float> kernel_0 = createGauss1stDerivativeKernel(cv::Size(-1, -1), sigma, theta, true);
    cv::Mat_<float> kernel_1 = createGauss2ndDerivativeKernel(cv::Size(-1, -1), sigma, theta, true);

    cv::Mat_<T> gaussEnergy0, gaussEnergy1;
    cv::filter2D(source, gaussEnergy0, -1, kernel_0, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);
    cv::filter2D(source, gaussEnergy1, -1, kernel_1, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT);

    //    cv::Mat_<T> temp0(source.size()), temp1(source.size());
    //    for (int l = 0; l < source.cols*source.rows; l++)
    //    {
    //        temp0(l) = glm::abs(gaussEnergy0(l)) / 15.f;
    //        temp1(l) = glm::abs(gaussEnergy1(l)) / 15.f;
    //    }
    //    imSave("g0.png", temp0);
    //    imSave("g1.png", temp1);

    out.create(source.size());
    for (int l = 0; l < source.cols*source.rows; l++)
    {
        out(l) = glm::sqrt(glm::pow(gaussEnergy0(l), T(2)) + glm::pow(gaussEnergy1(l), T(2)));
    }
}

template <class T>
void ComputeGaussDerivativeEnergy(const cv::Mat_<T> & source,
                                  cv::Mat_<T> & responseSuperposition,
                                  const double sigma, const int nrAngles = 6.)
{

    cv::Mat_<T> superposition(source.size());
    for (int l = 0; l < source.cols*source.rows; l++)
    {
        superposition(l) = T(0);
    }

    float s = 1.f / nrAngles;

    cv::Mat_<T> energy;
    for (int i = 0; i  < nrAngles; i++)
    {
        const double theta = mapRange<double>(i, 0, nrAngles, 0.f, PI<double>());

        ComputeGaussDerivativeEnergy(source, energy, sigma, theta);

        for (int l = 0; l < source.cols*source.rows; l++)
        {
            superposition(l) += s*energy(l);
        }
    }
    responseSuperposition = superposition;
}

//void Convolve(const cv::Mat_<float> & source, cv::Mat_<float> & output,
//              const cv::Mat_<float> & kernel, const cv::Mat_<uchar> & mask);



class ComputeShader;
class GLWindow;
class Texture;

class GPU_Convolution
{
    GLWindow*                       m_window;
    std::shared_ptr<ComputeShader>  m_shader;

    GPU_Convolution();
public:
    GPU_Convolution(GLWindow*  window);
    ~GPU_Convolution();

    void operator()(const cv::Mat_<float> & source, cv::Mat_<float> & output, const cv::Mat_<float> & kernel, const cv::Mat_<uchar> & mask);
    void operator()(const std::shared_ptr<Texture> & source, std::shared_ptr<Texture> & output, const std::shared_ptr<Texture> & kernel, const std::shared_ptr<Texture> & mask);

};


} // namespace linde

#endif // LINDE_CONVOLUTION_H
