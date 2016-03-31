#ifndef DIFFUSION_H
#define DIFFUSION_H

#include "linde.h"
#include <future>

#include <opencv2/imgproc/imgproc.hpp>

namespace linde
{
// http://www.mathworks.com/matlabcentral/fileexchange/14995-anisotropic-diffusion--perona---malik-/content/anisodiff_Perona-Malik/anisodiff2D.m
// Perona-Malik Diffusion
// Arguments:
// kappa controls conduction as a function of gradient.If kappa is low
// small intensity gradients are able to block conduction and hence diffusion
// across step edges.A large value reduces the influence of intensity
// gradients on conduction.
//
// lambda controls speed of diffusion(you usually want it at a maximum of
// 0.25)
//
// Diffusion equation 0 favours high contrast edges over low contrast ones.
// Diffusion equation 1 favours wide regions over smaller ones.
// flux 0 = exp
// flux 1 = sqr
void diffusionAnisotropicPeronaMalik(const cv::Mat_<double> & src, cv::Mat_<double> & dst, uint iterations, const int flux = 0, const double lambda = 0.125, const double kappa = 0.02);


void gpuDiffusionAnisotropicPeronaMalik(const cv::Mat_<float> & src, cv::Mat_<float> & dst, uint iterations, const int flux = 0, const float lambda = 0.125f, const float kappa = 0.02f);


/**
    * @author Thomas Lindemeier
    *
    * University of Konstanz-
    * Department for Computergraphics
    */
template <class T>
class Diffusion
{

protected:

    inline void
    divergence(const T & left, const T & right, const T & top, const T & bottom, T & div) const
    {
        T dx, dy;
        derive_x(left, right, dx);
        derive_x(top, bottom, dy);
        div = dx + dy;
    }

    inline void
    derive_x(const T & pre, const T & post, T & dx) const
    {
        dx = (post - pre) * 0.5f;
    }

    inline void
    derive_xx(const T & pre, const T & center, const T & post, T & dxx) const
    {
        dxx = post + pre - (center + center);
    }

    inline void
    derive_xy(const T & top_left, const T & bottom_right, const T & bottom_left, const T & top_right, T & dxy) const
    {
        dxy = 0.5f * (top_left + bottom_right - (bottom_left + top_right));
    }

    inline void
    firstDerivative(const T & left, const T & right, const T & top, const T & bottom, T & dx, T & dy) const
    {
        derive_x(left, right, dx);
        derive_x(top, bottom, dy);
    }

    inline void
    secondDerivative(const T & top_left, const T & top, const T & top_right,
                     const T & left, const T & center, const T & right,
                     const T & bottom_left, const T & bottom, const T & bottom_right, T & dxx, T & dxy, T & dyy) const
    {
        dxx = derive_xx(left, center, right);
        dxy = derive_xy(left, center, right);
        dyy = derive_xx(top, center, bottom);
    }

    void computeNeighborhood(int x, int y, const cv::Mat_<T> & data,
                             T & top_left, T & top, T & top_right, T & left, T & center, T & right, T & bottom_left, T & bottom, T & bottom_right) const
    {
        const int px = cv::borderInterpolate(x - 1, data.cols, cv::BORDER_REFLECT);
        const int nx = cv::borderInterpolate(x + 1, data.cols, cv::BORDER_REFLECT);
        const int py = cv::borderInterpolate(y - 1, data.rows, cv::BORDER_REFLECT);
        const int ny = cv::borderInterpolate(y + 1, data.rows, cv::BORDER_REFLECT);

        top_left = data(py, px);
        left = data(y, px);
        bottom_left = data(ny, px);
        top = data(py, x);
        center = data(y, x);
        bottom = data(ny, x);
        top_right = data(py, nx);
        right = data(y, nx);
        bottom_right = data(ny, nx);
    }



public:
    Diffusion()
    {

    }


    virtual ~Diffusion(){}




};

} // namespace linde

#endif // DIFFUSION_H
