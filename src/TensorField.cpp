#include "../include/linde/TensorField.h"

#include "../include/linde/Interpolation.h"
#include "../include/linde/Convolution.h"
#include "../include/linde/File.h"
#include "../include/linde/FrameBufferObject.h"
#include "../include/linde/Shader.h"
#include "../include/linde/Texture.h"
#include "../include/linde/Diffusion.h"
#include "../include/linde/ShaderStorageBuffer.h"
#include "../include/linde/GLWindow.h"
#include "../include/linde/MultiGridDiffusion.h"

#include <fstream>
#include <cmath>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

/**
* @author Thomas Lindemeier
*
* University of Konstanz-
* Department for Computergraphics
*/



namespace linde
{

StructureTensor2x2::StructureTensor2x2(float E, float F, float G) :
    E(E), F(F), G(G)
{

}

StructureTensor2x2::StructureTensor2x2() :
    E(0.0), F(0.0), G(0.0)
{

}

void StructureTensor2x2::set(float E, float F, float G)
{
    this->E = E;
    this->F = F;
    this->G = G;
}



glm::vec2 StructureTensor2x2::getMinEigenvector() const
{
    const float det = sqrt(pow(E - G, 2.f) + 4.f * F*F);
    return glm::vec2(2.f*F, G - E - det);
}

glm::vec2 StructureTensor2x2::getMaxEigenvector() const
{
    const float det = sqrt(pow(E - G, 2.f) + 4.f * F*F);
    return glm::vec2(2.f*F, G - E + det);
}

float StructureTensor2x2::getMinEigenvalue() const
{
    const float det = sqrt(pow(E - G, 2.f) + 4.f * F*F);
    return (E + G - det) * 0.5f;
}

float StructureTensor2x2::getMaxEigenvalue() const
{
    const float det = sqrt(pow(E - G, 2.f) + 4.f * F*F);
    return (E + G + det) * 0.5f;
}

float StructureTensor2x2::getAnisotropy() const
{
    /*float b = getMaxEigenvalue() + getMinEigenvalue();

        if (b == 0.0f) return 0.0f;

        return (getMaxEigenvalue() - getMinEigenvalue())/(b);*/

    const float deno = E + G;

    if (deno == 0.f) return 0.f;

    const float det = pow(E - G, 2.f) + 4 * pow(F, 2.f);
    return sqrt(det) / deno;
}

float StructureTensor2x2::getOrientation() const
{
    const float denom = (E - G);

    if(denom == 0.f) return 0.f;

    return 0.5f * glm::atan((2.f * F) / denom) + HALF_PI<float>();
}

float StructureTensor2x2::getMagnitude() const
{
    return sqrt(sqr(E) + sqr(F) + sqr(G));
}


StructureTensor2x2 & StructureTensor2x2::operator = (const StructureTensor2x2 &a)
{
    this->E = a.E;
    this->F = a.F;
    this->G = a.G;

    return *this;
}

StructureTensor2x2 & StructureTensor2x2::operator += (const StructureTensor2x2 &a)
{
    this->E += a.E;
    this->F += a.F;
    this->G += a.G;

    return *this;
}
StructureTensor2x2 & StructureTensor2x2::operator += (float s)
{
    this->E += s;
    this->F += s;
    this->G += s;

    return *this;
}
StructureTensor2x2 & StructureTensor2x2::operator -= (const StructureTensor2x2 &a)
{
    this->E -= a.E;
    this->F -= a.F;
    this->G -= a.G;

    return *this;
}
StructureTensor2x2 & StructureTensor2x2::operator -= (float s)
{
    this->E -= s;
    this->F -= s;
    this->G -= s;

    return *this;
}

StructureTensor2x2 & StructureTensor2x2::operator *= (float s)
{
    this->E *= s;
    this->F *= s;
    this->G *= s;

    return *this;
}
StructureTensor2x2 & StructureTensor2x2::operator /= (float s)
{
    float k = 1.0f / s;
    this->E *= k;
    this->F *= k;
    this->G *= k;

    return *this;
}

StructureTensor2x2 operator + (const StructureTensor2x2 &a, const StructureTensor2x2 &b)
{
    StructureTensor2x2 r;
    r.E = a.E + b.E;
    r.F = a.F + b.F;
    r.G = a.G + b.G;
    return r;
}

StructureTensor2x2 operator / (const StructureTensor2x2 &a, const StructureTensor2x2 &b)
{
    StructureTensor2x2 r;
    r.E = a.E / b.E;
    r.F = a.F / b.F;
    r.G = a.G / b.G;
    return r;
}

StructureTensor2x2 operator + (const StructureTensor2x2 &a, float s)
{
    StructureTensor2x2 r;
    r.E = a.E + s;
    r.F = a.F + s;
    r.G = a.G + s;
    return r;
}

StructureTensor2x2 operator + (float s, const StructureTensor2x2 &a)
{
    StructureTensor2x2 r;
    r.E = a.E + s;
    r.F = a.F + s;
    r.G = a.G + s;
    return r;
}

StructureTensor2x2 operator - (const StructureTensor2x2 &a, const StructureTensor2x2 &b)
{
    StructureTensor2x2 r;
    r.E = a.E - b.E;
    r.F = a.F - b.F;
    r.G = a.G - b.G;
    return r;
}

StructureTensor2x2 operator - (const StructureTensor2x2 &a, float s)
{
    StructureTensor2x2 r;
    r.E = a.E - s;
    r.F = a.F - s;
    r.G = a.G - s;
    return r;
}


StructureTensor2x2 operator * (const StructureTensor2x2 &a, float s)
{
    StructureTensor2x2 r;
    r.E = a.E + s;
    r.F = a.F + s;
    r.G = a.G + s;
    return r;
}

StructureTensor2x2 operator * (float s, const StructureTensor2x2 &a)
{
    StructureTensor2x2 r;
    r.E = a.E * s;
    r.F = a.F * s;
    r.G = a.G * s;
    return r;
}

StructureTensor2x2 operator / (const StructureTensor2x2 &a, float s)
{
    float k = 1.0f / s;
    StructureTensor2x2 r;
    r.E = a.E * k;
    r.F = a.F * k;
    r.G = a.G * k;
    return r;
}


//Comparison
bool operator == (const StructureTensor2x2 &a, const StructureTensor2x2 &b)
{
    return (a.E == b.E && a.F == b.F && a.G == b.G);
}

bool operator != (const StructureTensor2x2 &a, const StructureTensor2x2 &b)
{
    return (a.E != b.E || a.F != b.F || a.G != b.G);
}

std::ostream & operator<< (std::ostream & output, const StructureTensor2x2 & v)
{
    output << "E:\t" << v.E << "\tF:\t" << v.F << "\tG:\t" << v.G;

    return output;
}

StructureTensor2x2 StructureTensor2x2::fromAngle(float rad)
{
    float dx = cos(rad);
    float dy = sin(rad);

    return StructureTensor2x2(dx*dx, dx*dy, dy*dy);
}


/*
    ###################################################################################
    ###################################################################################
    ############################### Tensor Field ######################################
    ###################################################################################
    ###################################################################################
    */

StructureTensorField::StructureTensorField(void) :
    rows(0), cols(0)
{
}

StructureTensorField::StructureTensorField(int rows, int cols)
{
    create(rows, cols);
}


StructureTensorField::~StructureTensorField(void)
{
}

void StructureTensorField::create(int rows, int cols)
{
    this->rows = rows;
    this->cols = cols;
    m_tensors.create(rows, cols);
}

const StructureTensor2x2 &StructureTensorField::operator()(int i, int j) const
{
    return m_tensors.operator ()(i, j);
}

StructureTensor2x2 &StructureTensorField::operator()(int i, int j)
{
    return m_tensors.operator ()(i, j);
}

const StructureTensor2x2 &StructureTensorField::operator()(int i) const
{
    return m_tensors.operator ()(i);
}

StructureTensor2x2 &StructureTensorField::operator()(int i)
{
    return m_tensors.operator ()(i);
}


// tensor is as follows: |dx2 dxy|
//						 |dxy dy2|
void StructureTensorField::computeStructureTensors(const cv::Mat_<glm::vec3> & image,
                                                   const float innerSigma, const float outerSigma)
{
    rows = image.rows;
    cols = image.cols;

    cv::Mat_<glm::dvec3> image_cv;
    image.convertTo(image_cv, CV_64FC3);

    // compute derivation according to "Image and Video Abstraction by Coherence-Enhancing Filtering"
    // http://onlinelibrary.wiley.com/doi/10.1111/j.1467-8659.2011.01882.x/full
    cv::Mat_<glm::dvec3> dxTemp(image.size()), dyTemp(image.size());
    cv::Sobel(image, dxTemp, dxTemp.depth(), 1, 0, 3, 1.f/8.f);
    cv::Sobel(image, dyTemp, dyTemp.depth(), 0, 1, 3, 1.f/8.f);

    // inner blur
    if (innerSigma > 0)
    {
        const int k_size = gaussKernelSizeFromSigma(innerSigma);
        cv::GaussianBlur(dxTemp, dxTemp, cv::Size(k_size, k_size), innerSigma, 0.0, cv::BORDER_REFLECT);
        cv::GaussianBlur(dyTemp, dyTemp, cv::Size(k_size, k_size), innerSigma, 0.0, cv::BORDER_REFLECT);
    }

    // second order tensors
    cv::Mat_<float> dx2(dxTemp.size()), dy2(dxTemp.size()), dxy(dxTemp.size());
    for (int i = 0; i < dxTemp.cols * dxTemp.rows; i++)
    {
        glm::vec2 g0(dxTemp(i).x, dyTemp(i).x);
        glm::vec2 g1(dxTemp(i).y, dyTemp(i).y);
        glm::vec2 g2(dxTemp(i).z, dyTemp(i).z);

        dx2(i) = (g0.x * g0.x) + (g1.x * g1.x) + (g2.x * g2.x);
        dy2(i) = (g0.y * g0.y) + (g1.y * g1.y) + (g2.y * g2.y);
        dxy(i) = (g0.x * g0.y) + (g1.x * g1.y) + (g2.x * g2.y);
    }

    // outer blur
    if (outerSigma > 0)
    {
        const int k_size = gaussKernelSizeFromSigma(outerSigma);
        cv::GaussianBlur(dx2, dx2, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);
        cv::GaussianBlur(dy2, dy2, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);
        cv::GaussianBlur(dxy, dxy, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);
    }

    // create tensors
    m_tensors.create(image.rows, image.cols);
    for (int i = 0; i < image.rows*image.cols; ++i)
    {
        const float xx = std::isnan(dx2(i)) ? 0.f : dx2(i); // isnan check
        const float xy = std::isnan(dxy(i)) ? 0.f : dxy(i); // isnan check
        const float yy = std::isnan(dy2(i)) ? 0.f : dy2(i); // isnan check
        m_tensors(i).set(xx, xy, yy);
    }

}

void StructureTensorField::computeStructureTensors(const cv::Mat_<glm::vec3> & image, const cv::Mat_<uchar> &mask,
                                                   const float innerSigma, const float outerSigma)
{
    rows = image.rows;
    cols = image.cols;


    cv::Mat_<glm::dvec3> image_cv;
    image.convertTo(image_cv, CV_64FC3);

    // compute derivation according to "Image and Video Abstraction by Coherence-Enhancing Filtering"
    // http://onlinelibrary.wiley.com/doi/10.1111/j.1467-8659.2011.01882.x/full
    cv::Mat_<glm::dvec3> dxTemp(image.size()), dyTemp(image.size());
    cv::Sobel(image, dxTemp, dxTemp.depth(), 1, 0, 3, 1.f/8.f);
    cv::Sobel(image, dyTemp, dyTemp.depth(), 0, 1, 3, 1.f/8.f);

    // inner blur
    if (innerSigma > 0)
    {
        const int k_size = gaussKernelSizeFromSigma(innerSigma);

        if (mask.data)
        {
            cv::Mat_<double> maskB(mask.size());
            for (int i = 0; i < dxTemp.cols*dxTemp.rows; i++)
            {
                maskB(i) = mask(i) > 0 ? 1. : 0.;
                if (mask(i))
                {
                    dxTemp(i) = dxTemp(i);
                    dyTemp(i) = dyTemp(i);
                } else
                {
                    dxTemp(i) = glm::dvec3(0.f);
                    dyTemp(i) = glm::dvec3(0.f);
                }
            }
            cv::GaussianBlur(maskB, maskB, cv::Size(k_size, k_size), innerSigma, 0.0, cv::BORDER_REFLECT);
            cv::GaussianBlur(dxTemp, dxTemp, cv::Size(k_size, k_size), innerSigma, 0.0, cv::BORDER_REFLECT);
            cv::GaussianBlur(dyTemp, dyTemp, cv::Size(k_size, k_size), innerSigma, 0.0, cv::BORDER_REFLECT);

            for (int i = 0; i < dxTemp.cols*dxTemp.rows; i++)
            {
                dxTemp(i) = maskB(i) ? dxTemp(i) / maskB(i) : dxTemp(i);
                dyTemp(i) = maskB(i) ? dyTemp(i) / maskB(i) : dyTemp(i);
            }
        }
        else
        {
            cv::GaussianBlur(dxTemp, dxTemp, cv::Size(k_size, k_size), innerSigma, 0.0, cv::BORDER_REFLECT);
            cv::GaussianBlur(dyTemp, dyTemp, cv::Size(k_size, k_size), innerSigma, 0.0, cv::BORDER_REFLECT);
        }
    }

    // second order tensors
    cv::Mat_<float> dx2(dxTemp.size()), dy2(dxTemp.size()), dxy(dxTemp.size());
    for (int i = 0; i < dxTemp.cols * dxTemp.rows; i++)
    {
        glm::vec2 g0(dxTemp(i).x, dyTemp(i).x);
        glm::vec2 g1(dxTemp(i).y, dyTemp(i).y);
        glm::vec2 g2(dxTemp(i).z, dyTemp(i).z);

        dx2(i) = (g0.x * g0.x) + (g1.x * g1.x) + (g2.x * g2.x);
        dy2(i) = (g0.y * g0.y) + (g1.y * g1.y) + (g2.y * g2.y);
        dxy(i) = (g0.x * g0.y) + (g1.x * g1.y) + (g2.x * g2.y);
    }

    // outer blur
    if (outerSigma > 0)
    {
        const int k_size = gaussKernelSizeFromSigma(outerSigma);
        if (mask.data)
        {
            cv::Mat_<double> maskB(mask.size());
            for (int i = 0; i < dxTemp.cols*dxTemp.rows; i++)
            {
                maskB(i) = mask(i) > 0 ? 1. : 0.;
                if (mask(i))
                {
                    dx2(i) = dx2(i);
                    dy2(i) = dy2(i);
                    dxy(i) = dxy(i);
                } else
                {
                    dx2(i) = 0.f;
                    dy2(i) = 0.f;
                    dxy(i) = 0.f;
                }
            }
            cv::GaussianBlur(maskB, maskB, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);
            cv::GaussianBlur(dx2, dx2, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);
            cv::GaussianBlur(dy2, dy2, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);
            cv::GaussianBlur(dxy, dxy, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);

            for (int i = 0; i < dxTemp.cols*dxTemp.rows; i++)
            {
                dx2(i) = maskB(i) ? dx2(i) / maskB(i) : dx2(i);
                dy2(i) = maskB(i) ? dy2(i) / maskB(i) : dy2(i);
                dxy(i) = maskB(i) ? dxy(i) / maskB(i) : dxy(i);
            }
        }
        else
        {
            cv::GaussianBlur(dx2, dx2, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);
            cv::GaussianBlur(dy2, dy2, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);
            cv::GaussianBlur(dxy, dxy, cv::Size(k_size, k_size), outerSigma, 0.0, cv::BORDER_REFLECT);
        }
    }

    // create tensors
    m_tensors.create(image.rows, image.cols);
    for (int i = 0; i < image.rows*image.cols; ++i)
    {
        const float xx = std::isnan(dx2(i)) ? 0.f : dx2(i); // isnan check
        const float xy = std::isnan(dxy(i)) ? 0.f : dxy(i); // isnan check
        const float yy = std::isnan(dy2(i)) ? 0.f : dy2(i); // isnan check
        m_tensors(i).set(xx, xy, yy);
    }

}


void StructureTensorField::clear(int i, int j)
{
    m_tensors(i, j).set(0.0f, 0.0f, 0.0f);
}

void StructureTensorField::lineIntegralConvolution(cv::Mat_<uchar> &vis) const
{
    cv::Mat_<uchar> saltPepper(rows, cols);
    vis.create(rows, cols);
    linde::Random rand(0.0, 1.0);
    for (int i = 0; i < rows*cols; i++)
    {
        saltPepper(i) = round(rand.next()) * 255;
    }
    const float l = 20;
    int x,y;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            glm::vec2 pos(j, i);
            int k;
            int color = 0;
            glm::vec2 lastDirection;
            // forward
            for (k = 0; k < l / 2; k++)
            {
                glm::vec2 direction;
                linde::StructureTensorField::RungeKutta4_MinEigenvector(*this, pos, direction, sqrt(2.f), true);

                float curvature = glm::dot(glm::normalize(direction), glm::normalize(lastDirection));

                if (curvature < 0) direction *= -1; // reverse direction

                lastDirection = direction;

                pos += 1.41f * direction;

                x = cv::borderInterpolate(pos.x, cols, cv::BORDER_REFLECT);
                y = cv::borderInterpolate(pos.y, rows, cv::BORDER_REFLECT);

                color += (saltPepper(y, x));
            }
            pos[0] = j;
            pos[1] = i;
            lastDirection[0] = 0.0f;
            lastDirection[1] = 0.0f;
            // backward
            for (; k < l; k++)
            {
                glm::vec2 direction;
                linde::StructureTensorField::RungeKutta4_MinEigenvector(*this, pos, direction, sqrt(2.f), true);

                float curvature = glm::dot(glm::normalize(direction), glm::normalize(lastDirection));

                if (curvature < 0) direction *= -1; // reverse direction

                lastDirection = direction;

                pos -= 1.41f * direction;

                x = cv::borderInterpolate(pos.x, cols, cv::BORDER_REFLECT);
                y = cv::borderInterpolate(pos.y, rows, cv::BORDER_REFLECT);

                color += (saltPepper(y, x));
            }
            vis(i, j) = color / l;
        }
    }
}


float StructureTensorField::getMinEigenvalue(int i, int j) const
{
    return m_tensors(i, j).getMinEigenvalue();
}

float StructureTensorField::getMaxEigenvalue(int i, int j) const
{
    return m_tensors(i, j).getMaxEigenvalue();
}

float StructureTensorField::getAnisotropy(int i, int j) const
{
    return m_tensors(i, j).getAnisotropy();
}

glm::vec2 StructureTensorField::getMinEigenvector(int i, int j) const
{

    return m_tensors(i, j).getMinEigenvector();
}

glm::vec2 StructureTensorField::getMinEigenvector(const glm::vec2 & pos) const
{
    StructureTensor2x2 t = interpolated(m_tensors, pos);
    return t.getMinEigenvector();
}

glm::vec2 StructureTensorField::getMaxEigenvector(int i, int j) const
{
    return m_tensors(i, j).getMaxEigenvector();
}

glm::vec2 StructureTensorField::getMaxEigenvector(const glm::vec2 & pos) const
{
	StructureTensor2x2 t = interpolated(m_tensors, pos);
	return t.getMaxEigenvector();
}

const StructureTensor2x2 & StructureTensorField::getTensor(int i, int j) const
{
    return m_tensors(i, j);
}

StructureTensor2x2 & StructureTensorField::getTensor(int i)
{
    return m_tensors(i);
}

const StructureTensor2x2 & StructureTensorField::getTensor(int i) const
{
    return m_tensors(i);
}

StructureTensor2x2 & StructureTensorField::getTensor(int i, int j)
{
    return m_tensors(i, j);
}

const cv::Mat_<StructureTensor2x2> & StructureTensorField::getTensors() const
{
    return m_tensors;
}

cv::Mat_<StructureTensor2x2> & StructureTensorField::getTensors()
{
    return m_tensors;
}

void StructureTensorField::save(const std::string & filename) const
{
    std::ofstream out;
    out.open(filename);
    out << m_tensors.rows << std::endl;
    out << m_tensors.cols << std::endl;

    for (const StructureTensor2x2 & t : m_tensors)
    {
        out << t.getA00() << ";";
        out << t.getA01() << ";";
        out << t.getA11() << ";";
        out << std::endl;
    }

    out.close();
}

bool StructureTensorField::load(const std::string & filename)
{
    std::ifstream in;
    in.open(filename);

    if (!in.good()) return false;

    std::string l;
    std::getline(in, l);
    rows = std::stoi(l.c_str());
    std::getline(in, l);
    cols = std::stoi(l.c_str());

    std::string delimiter = ";";

    m_tensors.create(rows, cols);
    for (StructureTensor2x2 & t : m_tensors)
    {
        std::string v1, v2, v3;
        std::getline(in, v1, ';');
        std::getline(in, v2, ';');
        std::getline(in, v3, ';');
        t.set(
                    (float)atof(v1.c_str()),
                    (float)atof(v2.c_str()),
                    (float)atof(v3.c_str()));
    }

    in.close();

    return true;
}

void StructureTensorField::normalize()
{
    float maxMag = 0.000001f;
    for (int i = 0; i < rows*cols; i++)
    {
        maxMag = glm::max(maxMag, m_tensors(i).getMagnitude());
    }
    float m = 1.f / maxMag;
    for (int i = 0; i < rows*cols; i++)
    {
        StructureTensor2x2 &tensor = m_tensors(i);
        tensor.E *= m;
        tensor.F *= m;
        tensor.G *= m;
    }
}

StructureTensorField StructureTensorField::clone() const
{
    StructureTensorField clone;

    clone.rows = rows;
    clone.cols = cols;
    clone.m_tensors.create(rows, cols);
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            const StructureTensor2x2 & t = m_tensors(i, j);
            clone.m_tensors(i, j) = t;
        }
    }

    return clone;
}



void StructureTensorField::smoothDiffusion(int iterations, float kappa, const cv::Mat_<uchar> & mask)
{

}

void StructureTensorField::smoothBilateral(int iterations, float sigmaSpatial, float sigmaColor, const cv::Mat_<glm::vec3> & colorSource, const cv::Mat_<uchar> & mask, GLWindow *window)
{
    if (window) // if OpenGL context available
    {
        ComputeShader * smoothingShader = window->createComputeShader("shaders/lindeLibShaders/TensorSmoothing.glsl");

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

        for (int it = 0; it < iterations; it++)
        {
            // upload tensors
            std::shared_ptr<Texture> tensorTex = window->createTexture(m_tensors.cols, m_tensors.rows, GL_RGB32F, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR,
                                                                       GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);

            cv::Mat_<StructureTensor2x2> tCpy;
            cv::flip(m_tensors, tCpy, 0);
            tensorTex->create(tCpy.data);

            // output tensors
            std::shared_ptr<ShaderStorageBufferObject> outputBuffer = window->createShaderStoragebufferObject();
            outputBuffer->create(nullptr, m_tensors.cols*m_tensors.rows *sizeof(glm::vec4));

            // upload color source and mask as alpha
            cv::Mat_<glm::vec4> colorSourceUpload(colorSource.size());
            for (int i = 0; i < colorSource.rows*colorSource.cols; i++)
            {
                colorSourceUpload(i).x = colorSource(i).x;
                colorSourceUpload(i).y = colorSource(i).y;
                colorSourceUpload(i).z = colorSource(i).z;
                colorSourceUpload(i).w = (mask.data) ? mask(i) : 255;
            }
            std::shared_ptr<Texture> colorTex = window->createTexture(colorSourceUpload.cols, colorSourceUpload.rows, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_LINEAR,
                                                                      GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
            cv::flip(colorSourceUpload, colorSourceUpload, 0);
            colorTex->create(colorSourceUpload.data);

            smoothingShader->bind(true);
            outputBuffer->bindBase(0);
            glActiveTexture(GL_TEXTURE0);
            tensorTex->bind();
            smoothingShader->seti("inputTensors", 0);
            glActiveTexture(GL_TEXTURE1);
            colorTex->bind();
            smoothingShader->seti("colorSource", 1);
            smoothingShader->setf("sigmaSpatial", sigmaSpatial);
            smoothingShader->seti("radius", gaussKernelRadiusFromSigma(sigmaSpatial));
            smoothingShader->setf("sigmaColor", sigmaColor);
            glm::vec3 workSize = smoothingShader->getWorkGroupSize();
            smoothingShader->dispatchCompute(tensorTex->width() / workSize.x + 1, tensorTex->height() / workSize.y + 1, 1);
            smoothingShader->memoryBarrier();
            smoothingShader->bind(false);

            cv::Mat_<glm::vec4> bufferTemp(m_tensors.size());
            outputBuffer->download(bufferTemp.data, m_tensors.cols*m_tensors.rows *sizeof(glm::vec4));
            cv::flip(bufferTemp, bufferTemp, 0);
            for (int i = 0; i < m_tensors.rows*m_tensors.cols; i++)
            {
                m_tensors(i).E = bufferTemp(i).x;
                m_tensors(i).F = bufferTemp(i).y;
                m_tensors(i).G = bufferTemp(i).z;
            }
        }

        glPopClientAttrib();
        glPopAttrib();

    }
    else
    {
        const int width = cols;
        const int height = rows;

        const int size = gaussKernelSizeFromSigma(sigmaSpatial);
        const int r = (size - 1) / 2;

        for (int it = 0; it < iterations; it++)
        {

            cv::Mat_<StructureTensor2x2> tCpy(height, width);

            // create gauss kernel as distance weights
            cv::Mat_<float> gaussKernel(size, size, 0.0);
            for (int k = -r; k <= r; ++k)
            {
                for (int l = -r; l <= r; ++l)
                {
                    gaussKernel(k + r, l + r) = gauss<float>(static_cast<float>(l), static_cast<float>(k), sigmaSpatial);
                }
            }

            // run the filter
            linde::parallel_for(0, height, [&](int y)
            {
                for (int x = 0; x < width; x++)
                {
                    float E = 0.0;
                    float F = 0.0;
                    float G = 0.0;
                    float Ew = 0.0;
                    float Fw = 0.0;
                    float Gw = 0.0;

                    int i_, j_;
                    float weight;
                    float colorW;

                    glm::vec3 c0 = colorSource(y, x);
                    glm::vec3 c1;

                    if (mask.data && mask(y, x) == 0) continue;

                    for (int j = -r; j <= r; ++j)
                    {
                        for (int i = -r; i <= r; ++i)
                        {
                            i_ = cv::borderInterpolate(y + i, height, cv::BORDER_REFLECT);
                            j_ = cv::borderInterpolate(x + j, width, cv::BORDER_REFLECT);

                            if (mask.data && mask(i_, j_) == 0) continue;

                            const StructureTensor2x2 & tensor = m_tensors(i_, j_);

                            c1 = colorSource(i_, j_);

                            colorW = gauss<float>(glm::distance(c0, c1), sigmaColor);

                            weight = gaussKernel(i + r, j + r) * colorW;

                            //std::cout << "t0: " << t0 << " t1:" << t1 << std::endl;
                            //std::cout << "tw: " << tensorW << " cw:" << colorW << std::endl;

                            E += weight * tensor.E;
                            F += weight * tensor.F;
                            G += weight * tensor.G;

                            Ew += weight;
                            Fw += weight;
                            Gw += weight;
                        }
                    }
                    tCpy(y, x).set(E / Ew, F / Fw, G / Gw);
                }
            });

            this->m_tensors = tCpy;
        }
    }
}


void StructureTensorField::interpolate(float minValidGradient, const std::shared_ptr<GLWindow> & gl)
{
    cv::Mat_<glm::vec4> psi(rows, cols);
    cv::Mat_<uchar> m(rows, cols);
    for (int i = 0; i < rows*cols; i++)
    {
        const StructureTensor2x2 &tensor = m_tensors(i);
        glm::vec4 &a = psi(i);

        if (tensor.getMagnitude() <= minValidGradient)
        {
            a.x = a.y = a.z = a.w = 0.f;
            m(i) = 0;
        } else
        {
            glm::vec4 &a = psi(i);
            a.x = tensor.E;
            a.y = tensor.F;
            a.z = tensor.G;
            a.w = 1.f;
            m(i) = 255;
        }
    }

    //    cv::imshow("interpol mask", m);
    //    cv::imshow("input", psi);

    GPU_MultiGridDiffusion diffusion(gl);
    diffusion.solve(psi);

    for (int i = 0; i < rows*cols; i++)
    {
        StructureTensor2x2 &tensor = m_tensors(i);
        const glm::vec4 &a = psi(i);

        if (tensor.getMagnitude() <= minValidGradient)
        {
            tensor.E = a.x;
            tensor.F = a.y;
            tensor.G = a.z;
        }
    }

    //    cv::imshow("Solved", m_tensors);
    //    cv::waitKey(0);
}

static inline glm::vec2 f(const StructureTensorField & field, const glm::vec2 & y, bool normalize)
{
    glm::vec2 v = field.getMinEigenvector(y);
    return (normalize) ? glm::normalize(v) : v;
}

void StructureTensorField::RungeKutta4_MinEigenvector(const StructureTensorField & field, const glm::vec2 & pos, glm::vec2 & dir, float h, bool normalize)
{
    glm::vec2 k1, k2, k3, k4;
    glm::vec2 y0 = pos;

    k1 = h * f(field, y0, normalize);
    k2 = h * f(field, y0 + k1 * 0.5f, normalize);
    k3 = h * f(field, y0 + k2 * 0.5f, normalize);
    k4 = h * f(field, y0 + k3, normalize);

    dir = (h / 6.f) * (k1 + 2.f * k2 + 2.f * k3 + k4);
}

static inline glm::vec2 F(const StructureTensorField & field, const glm::vec2 & y, bool normalize)
{
	glm::vec2 v = field.getMaxEigenvector(y);
	return (normalize) ? glm::normalize(v) : v;
}

void StructureTensorField::RungeKutta4_MaxEigenvector(const StructureTensorField & field, const glm::vec2 & pos, glm::vec2 & dir, float h, bool normalize)
{
	glm::vec2 k1, k2, k3, k4;
	glm::vec2 y0 = pos;

	k1 = h * F(field, y0, normalize);
	k2 = h * F(field, y0 + k1 * 0.5f, normalize);
	k3 = h * F(field, y0 + k2 * 0.5f, normalize);
	k4 = h * F(field, y0 + k3, normalize);

	dir = (h / 6.f) * (k1 + 2.f * k2 + 2.f * k3 + k4);
}


// interpolates values!!!! leq threshold
StructureTensorField StructureTensorField::computeStructureTensorField(const std::shared_ptr<GLWindow> & gl, const cv::Mat_<glm::vec3> & input_image, float inner_sigma, float outer_sigma, float threshold)
{
    StructureTensorField tf;

    // compute tensors
    tf.computeStructureTensors(input_image, inner_sigma, outer_sigma);
    tf.normalize();
    tf.interpolate(threshold, gl);

    return tf;
}


StructureTensorField StructureTensorField::computeStructureTensorField(const std::shared_ptr<GLWindow> & gl, const cv::Mat_<glm::vec3> & input_image, const cv::Mat_<uchar> & mask, float inner_sigma, float outer_sigma, float threshold)
{
    StructureTensorField tf;

    // compute tensors
    tf.computeStructureTensors(input_image, mask, inner_sigma, outer_sigma);
    tf.normalize();
    tf.interpolate(threshold, gl);

    return tf;
}

StructureTensorField StructureTensorField::createFixedStructureTensorField(const cv::Size &size, const StructureTensor2x2 &tensor)
{
    StructureTensorField tf;

    tf.m_tensors.create(size);
	tf.rows = tf.m_tensors.rows;
	tf.cols = tf.m_tensors.cols;

    for (StructureTensor2x2 & t : tf.m_tensors)
    {
        t = tensor;
    }

    return tf;
}




} // namespace linde
