#include "../include/linde/Convolution.h"
#include "../include/linde/GLContext.h"
#include "../include/linde/Shader.h"
#include "../include/linde/Texture.h"

namespace linde
{

cv::Mat_<double> createGaborKernel(cv::Size ksize, double sigma, double theta,
                                   double lambd, double gamma, double psi)
{
    double sigma_x = sigma;
    double sigma_y = sigma/gamma;
    int nstds = 3;
    int xmin, xmax, ymin, ymax;
    double c = cos(theta), s = sin(theta);

    if( ksize.width > 0 )
        xmax = ksize.width/2;
    else
        xmax = cvRound(std::max(fabs(nstds*sigma_x*c), fabs(nstds*sigma_y*s)));

    if( ksize.height > 0 )
        ymax = ksize.height/2;
    else
        ymax = cvRound(std::max(fabs(nstds*sigma_x*s), fabs(nstds*sigma_y*c)));

    xmin = -xmax;
    ymin = -ymax;

    cv::Mat_<double> kernel(ymax - ymin + 1, xmax - xmin + 1);
    double scale = 1;
    double ex = -0.5/(sigma_x*sigma_x);
    double ey = -0.5/(sigma_y*sigma_y);
    double cscale = CV_PI*2/lambd;

    for( int y = ymin; y <= ymax; y++ )
        for( int x = xmin; x <= xmax; x++ )
        {
            double xr = x*c + y*s;
            double yr = -x*s + y*c;

            double v = scale*std::exp(ex*xr*xr + ey*yr*yr)*cos(cscale*xr + psi);
            kernel.at<double>(ymax - y, xmax - x) = v;
        }

    return kernel;
}

//void Convolve(const cv::Mat_<float> &source, cv::Mat_<float> &output, const cv::Mat_<float> &kernel, const cv::Mat_<uchar> &mask)
//{
//    glm::ivec2 radius;
//    radius.x = kernel.cols / 2;
//    radius.y = kernel.rows / 2;

//    glm::ivec2 imageSize;
//    imageSize.x = source.cols;
//    imageSize.y = source.rows;

//    cv::Mat_<float> temp(source.size());

//    glm::ivec2 index;
//    for (int x_ = 0; x_ < imageSize.x; ++x_)
//    {
//        for (int y_ = 0; y_ < imageSize.y; ++y_)
//        {
//            float sum = 0.f;
//            index.x = x_;
//            index.y = y_;
//            for(int k = -radius.x; k <= radius.x; k++)
//            {
//                for(int l = -radius.y; l <= radius.y; l++)
//                {
//                    glm::ivec2 tPos(k, l);
//                    glm::ivec2 imagePos  = index + tPos;
//                    glm::ivec2 kernelPos = tPos + radius;

//                    // out of bounds
//                    if (imagePos.x >= imageSize.x || imagePos.y >= imageSize.y
//                            || imagePos.x < 0  || imagePos.y < 0) continue;

//                    // masked out
//                    if (mask(imagePos.y, imagePos.x) > 0)
//                    {
//                        float w = kernel(kernelPos.y, kernelPos.x);
//                        float v = source(imagePos.y, imagePos.x);

//                        sum += w*v;
//                    }
//                }
//            }
//            temp(index.y, index.x) = sum;
//        }
//    }
//    output = temp;
//}

GPU_Convolution::GPU_Convolution() :
    m_context(nullptr),
    m_shader(nullptr)
{

}

GPU_Convolution::GPU_Convolution(GLContext *context) :
    m_context(context)
{
    m_shader = m_context->createComputeShader("shaders/lindeLibShaders/SpatialConvolution.glsl");
}

GPU_Convolution::~GPU_Convolution()
{

}

void GPU_Convolution::operator()(const cv::Mat_<float> & source, cv::Mat_<float> & output,
                                 const cv::Mat_<float> & kernel, const cv::Mat_<uchar> & mask)
{
    output.create(source.size());
    output.setTo(0.f);

    std::shared_ptr<Texture> sourceTexture = m_context->createTexture(source);
    std::shared_ptr<Texture> outTexture = m_context->createTexture(output);
    std::shared_ptr<Texture> kernelTexture = m_context->createTexture(kernel);
    std::shared_ptr<Texture> maskTexture = m_context->createTexture(mask);

    this->operator ()(sourceTexture, outTexture, kernelTexture, maskTexture);

    outTexture->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, output.data);
    outTexture->unbind();
    cv::flip(output, output, 0);
}

void GPU_Convolution::operator()(const std::shared_ptr<Texture> &source, std::shared_ptr<Texture> &output,
                                 const std::shared_ptr<Texture> &kernel, const std::shared_ptr<Texture> &mask)
{
    m_shader->bind(true);
    source->bindLocationUnit(0, GL_READ_ONLY);
    output->bindLocationUnit(1, GL_WRITE_ONLY);
    kernel->bindLocationUnit(2, GL_READ_ONLY);
    mask->bindLocationUnit(3, GL_READ_ONLY);

    glm::vec3 workSize = m_shader->getWorkGroupSize();
    m_shader->dispatchCompute(source->width() / workSize.x + 1, source->height() / workSize.y + 1, 1);
    m_shader->memoryBarrier();
    m_shader->bind(false);
}

cv::Mat_<float> createGauss1stDerivativeKernel(cv::Size ksize, const float sigma, const float theta, bool normalized)
{
    if (ksize.width < 0)
    {
        ksize.width =  ksize.height = gaussKernelSizeFromSigma(sigma*2.5f);
    }

    cv::Mat_<float> kernel(ksize);

    int halfW = (kernel.rows - 1) / 2;
    int halfC = (kernel.cols - 1) / 2;

    for (int r = -halfW; r <= halfW; ++r)
    {
        for (int c = -halfC; c <= halfC; ++c)
        {
            kernel(r + halfW, c + halfC) = gauss1deriv(static_cast<float>(r), static_cast<float>(c), sigma, theta, normalized);
        }
    }
    return kernel;
}

cv::Mat_<float> createGauss2ndDerivativeKernel(cv::Size ksize, const float sigma, const float theta, bool normalized)
{
    if (ksize.width < 0)
    {
        ksize.width =  ksize.height = gaussKernelSizeFromSigma(sigma*2.5f) ;
    }

    cv::Mat_<float> kernel(ksize);

    int halfW = (kernel.rows - 1) / 2;
    int halfC = (kernel.cols - 1) / 2;

    for (int r = -halfW; r <= halfW; ++r)
    {
        for (int c = -halfC; c <= halfC; ++c)
        {
            kernel(r + halfW, c + halfC) = gauss2deriv(static_cast<float>(r), static_cast<float>(c), sigma, theta, normalized);
        }
    }
    return kernel;
}

} // namespace linde
