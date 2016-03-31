#include "../include/linde/Diffusion.h"
#include "../include/linde/Texture.h"
#include "../include/linde/TensorField.h"
#include "../include/linde/Shader.h"

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
void diffusionAnisotropicPeronaMalik(const cv::Mat_<double> & src, cv::Mat_<double> & dst, uint iterations, const int flux, const double lambda, const double kappa)
{
    if (iterations == 0)
    {
        src.copyTo(dst);
        return;
    }

    const double dx = 1;
    const double dy = 1;
    const double dd = sqrt(2.f);
    const int w = src.cols;
    const int h = src.rows;
    const cv::Mat hN = (cv::Mat_<double>(3, 3) <<
                        0.f, 1.f, 0.f,
                        0.f, -1.f, 0.f,
                        0.f, 0.f, 0.f);
    const cv::Mat hS = (cv::Mat_<double>(3, 3) <<
                        0.f, 0.f, 0.f,
                        0.f, -1.f, 0.f,
                        0.f, 1.f, 0.f);
    const cv::Mat hE = (cv::Mat_<double>(3, 3) <<
                        0.f, 0.f, 0.f,
                        0.f, -1.f, 1.f,
                        0.f, 0.f, 0.f);
    const cv::Mat hW = (cv::Mat_<double>(3, 3) <<
                        0.f, 0.f, 0.f,
                        1.f, -1.f, 0.f,
                        0.f, 0.f, 0.f);
    const cv::Mat hNE = (cv::Mat_<double>(3, 3) <<
                         0.f, 0.f, 1.f,
                         0.f, -1.f, 0.f,
                         0.f, 0.f, 0.f);
    const cv::Mat hSE = (cv::Mat_<double>(3, 3) <<
                         0.f, 0.f, 0.f,
                         0.f, -1.f, 0.f,
                         0.f, 0.f, 1.f);
    const cv::Mat hSW = (cv::Mat_<double>(3, 3) <<
                         0.f, 0.f, 0.f,
                         0.f, -1.f, 0.f,
                         1.f, 0.f, 0.f);
    const cv::Mat hNW = (cv::Mat_<double>(3, 3) <<
                         1.f, 0.f, 0.f,
                         0.f, -1.f, 0.f,
                         0.f, 0.f, 0.f);

    cv::Mat_<double> temp = src.clone();
    cv::Mat_<double> dest(temp.size());

    for (uint i = 0; i < iterations; ++i)
    {
        // Finite differences
        cv::Mat_<double> nablaN;
        cv::Mat_<double> nablaS;
        cv::Mat_<double> nablaW;
        cv::Mat_<double> nablaE;
        cv::Mat_<double> nablaNE;
        cv::Mat_<double> nablaSE;
        cv::Mat_<double> nablaSW;
        cv::Mat_<double> nablaNW;
        std::vector<std::future<void> > futures;
        futures.push_back(std::async(std::launch::async,
                                     [&](){cv::filter2D(temp, nablaN, temp.depth(), hN, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT); }));
        futures.push_back(std::async(std::launch::async,
                                     [&](){cv::filter2D(temp, nablaS, temp.depth(), hS, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT); }));
        futures.push_back(std::async(std::launch::async,
                                     [&](){cv::filter2D(temp, nablaW, temp.depth(), hW, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT); }));
        futures.push_back(std::async(std::launch::async,
                                     [&](){cv::filter2D(temp, nablaE, temp.depth(), hE, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT); }));
        futures.push_back(std::async(std::launch::async,
                                     [&](){cv::filter2D(temp, nablaNE, temp.depth(), hNE, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT); }));
        futures.push_back(std::async(std::launch::async,
                                     [&](){cv::filter2D(temp, nablaSE, temp.depth(), hSE, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT); }));
        futures.push_back(std::async(std::launch::async,
                                     [&](){cv::filter2D(temp, nablaSW, temp.depth(), hSW, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT); }));
        futures.push_back(std::async(std::launch::async,
                                     [&](){cv::filter2D(temp, nablaNW, temp.depth(), hNW, cv::Point(-1, -1), 0.0, cv::BORDER_REFLECT); }));

        for (std::future<void> & f : futures)
        {
            f.wait();
        }
        futures.clear();

        cv::Mat_<double> cN;
        cv::Mat_<double> cS;
        cv::Mat_<double> cW;
        cv::Mat_<double> cE;
        cv::Mat_<double> cNE;
        cv::Mat_<double> cSE;
        cv::Mat_<double> cSW;
        cv::Mat_<double> cNW;

        if (flux == 0) // exponential flux
        {
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cN2; cv::pow(nablaN / kappa, 2.0, cN2); cv::exp(-cN2, cN); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cS2; cv::pow(nablaS / kappa, 2.0, cS2); cv::exp(-cS2, cS); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cW2; cv::pow(nablaW / kappa, 2.0, cW2); cv::exp(-cW2, cW); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cE2; cv::pow(nablaE / kappa, 2.0, cE2); cv::exp(-cE2, cE); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cNE2; cv::pow(nablaNE / kappa, 2.0, cNE2); cv::exp(-cNE2, cNE); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cSE2; cv::pow(nablaSE / kappa, 2.0, cSE2); cv::exp(-cSE2, cSE); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cSW2; cv::pow(nablaSW / kappa, 2.0, cSW2); cv::exp(-cSW2, cSW); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cNW2; cv::pow(nablaNW / kappa, 2.0, cNW2); cv::exp(-cNW2, cNW); }));
        }
        else // squared flux
        {
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cN2; cv::pow(nablaN / kappa, 2.0, cN2); cN = 1.0 / (1.0 + cN2); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cS2; cv::pow(nablaS / kappa, 2.0, cS2); cS = 1.0 / (1.0 + cS2); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cW2; cv::pow(nablaW / kappa, 2.0, cW2); cW = 1.0 / (1.0 + cW2); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cE2; cv::pow(nablaE / kappa, 2.0, cE2); cE = 1.0 / (1.0 + cE2); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cNE2; cv::pow(nablaNE / kappa, 2.0, cNE2); cNE = 1.0 / (1.0 + cNE2); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cSE2; cv::pow(nablaSE / kappa, 2.0, cSE2); cSE = 1.0 / (1.0 + cSE2); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cSW2; cv::pow(nablaSW / kappa, 2.0, cSW2); cSW = 1.0 / (1.0 + cSW2); }));
            futures.push_back(std::async(std::launch::async,
                                         [&](){cv::Mat_<double> cNW2; cv::pow(nablaNW / kappa, 2.0, cNW2); cNW = 1.0 / (1.0 + cNW2); }));
        }

        for (std::future<void> & f : futures)
        {
            f.wait();
        }
        futures.clear();

        // parallel for
        for (int x = 0; x < w; x++)
        {
            for (int y = 0; y < h; y++)
            {
                dest(y, x) = temp(y, x) + lambda * (
                            (1.f / sqr(dy)) * cN(y, x)	* nablaN(y, x) + (1.f / sqr(dy))	*	cS(y, x)	*	nablaS(y, x) +
                            (1.f / sqr(dx)) * cW(y, x)	* nablaW(y, x) + (1.f / sqr(dx))	*	cE(y, x)	*	nablaE(y, x) +
                            (1.f / sqr(dd)) * cNE(y, x)	* nablaNE(y, x) + (1.f / sqr(dd))	*	cSE(y, x)	*	nablaSE(y, x) +
                            (1.f / sqr(dd)) * cSW(y, x)	* nablaSW(y, x) + (1.f / sqr(dd))	*	cNW(y, x)	*	nablaNW(y, x)
                            );
            }
        }

        dest.copyTo(temp);
    }
    dest.copyTo(dst);
}



} // namespace linde
