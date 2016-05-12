#ifndef TENSOR_FIELD_H
#define TENSOR_FIELD_H

#include "linde.h"




// TODO adaptive smoothing with gaussian according to anisotropy
namespace linde
{
/*
    ###################################################################################
    ###################################################################################
    ############################### Tensor Field ######################################
    ###################################################################################
    ###################################################################################
    */
/**
    *@author Thomas Lindemeier
    * University of Konstanz -
    * Department of Computer Graphics and Media Design
    *
    * @date 16.07.2013
    *
    */
// Kyprianidis, JE Kang, Henry D�llner, J
// Image and video abstraction by anisotropic Kuwahara filtering
// http://onlinelibrary.wiley.com/doi/10.1111/j.1467-8659.2009.01574.x/full
// |E F|
// |F G|
class StructureTensor2x2
{
public:
    float E;
    float F;
    float G;

public:
    StructureTensor2x2();
    StructureTensor2x2(float E, float F, float G);

    float getA00() const {return E;}
    float getA01() const {return F;}
    float getA11() const {return G;}

    void set(float E, float F, float G);

    glm::vec2 getMinEigenvector() const;
    glm::vec2 getMaxEigenvector() const;

    float getMinEigenvalue() const;
    float getMaxEigenvalue() const;

    float getAnisotropy() const;

    float getOrientation() const;

    float getMagnitude() const;

    StructureTensor2x2 &operator =  (const StructureTensor2x2 &a);
    StructureTensor2x2 &operator += (const StructureTensor2x2 &a);
    StructureTensor2x2 &operator += (float s);
    StructureTensor2x2 &operator -= (const StructureTensor2x2 &a);
    StructureTensor2x2 &operator -= (float s);
    StructureTensor2x2 &operator *= (float s);
    StructureTensor2x2 &operator /= (float s);

    friend StructureTensor2x2 operator + (const StructureTensor2x2 &a, const StructureTensor2x2 &b);
    friend StructureTensor2x2 operator + (const StructureTensor2x2 &a, float s);
    friend StructureTensor2x2 operator + (float s, const StructureTensor2x2 &a);
    friend StructureTensor2x2 operator - (const StructureTensor2x2 &a, const StructureTensor2x2 &b);
    friend StructureTensor2x2 operator - (const StructureTensor2x2 &a, float s);
    friend StructureTensor2x2 operator * (const StructureTensor2x2 &a, float s);
    friend StructureTensor2x2 operator * (float s, const StructureTensor2x2 &a);
    friend StructureTensor2x2 operator / (const StructureTensor2x2 &a, float s);
    friend StructureTensor2x2 operator / (const StructureTensor2x2 &a, const StructureTensor2x2 &b);

    //Comparison
    friend bool operator == (const StructureTensor2x2 &a, const StructureTensor2x2 &b);
    friend bool operator != (const StructureTensor2x2 &a, const StructureTensor2x2 &b);

    friend std::ostream & operator<< (std::ostream & output, const StructureTensor2x2 & v);

    static StructureTensor2x2 fromAngle(float rad);
};
} // namespace linde

// opencv access data traits
namespace cv
{
template<> class DataType<linde::StructureTensor2x2>
{
public:
    typedef linde::StructureTensor2x2 value_type;
    typedef linde::StructureTensor2x2 work_type;
    typedef float channel_type;
    typedef value_type vec_type;
    enum { generic_type = 0, depth = DataDepth<float>::value, channels = 3,
           fmt = ((channels-1)<<8) + DataDepth<float>::fmt,
           type = CV_MAKETYPE(depth, channels) };
};

} //namespace cv

namespace linde
{
class GLWindow;


class StructureTensorField
{
public:
    int rows;
    int cols;

    StructureTensorField(void);
    StructureTensorField(int rows, int cols);
    ~StructureTensorField(void);

    void create(int rows, int cols);
    const StructureTensor2x2 & operator()(int i, int j) const;
    StructureTensor2x2 & operator()(int i, int j);
    const StructureTensor2x2 & operator()(int i) const;
    StructureTensor2x2 & operator()(int i);

    cv::Size size() const {return m_tensors.size();}

    float getMinEigenvalue(int i, int j) const;
    float getMaxEigenvalue(int i, int j) const;
    float getAnisotropy(int i, int j) const;

    glm::vec2 getMinEigenvector(int i, int j) const;
    glm::vec2 getMinEigenvector(const glm::vec2 & pos) const;
    glm::vec2 getMaxEigenvector(int i, int j) const;
	glm::vec2 getMaxEigenvector(const glm::vec2 & pos) const;

    const StructureTensor2x2 & getTensor(int i, int j) const;
    StructureTensor2x2 & getTensor(int i, int j);
    StructureTensor2x2 & getTensor(int i);
    const StructureTensor2x2 &getTensor(int i) const;

    const cv::Mat_<StructureTensor2x2> & getTensors() const;
    cv::Mat_<StructureTensor2x2> & getTensors();

    void computeStructureTensors(const cv::Mat_<glm::vec3> & image,
                                 const float innerSigma = 0.0f, const float outerSigma = 0.0f);   
    void computeStructureTensors(const cv::Mat_<glm::vec3> & image, const cv::Mat_<uchar> &mask,
                                 const float innerSigma, const float outerSigma);

    void clear(int i, int j);

    void lineIntegralConvolution(cv::Mat_<uchar> & vis) const;

    void save(const std::string & filename) const;
    bool load(const std::string & filename);

    void normalize();

    StructureTensorField clone() const;

    // smooth according to radius
    void smoothDiffusion(int iterations, float kappa, const cv::Mat_<uchar> & mask = cv::Mat_<uchar>()); // anisotropic diffusion
    void smoothBilateral(int iterations, float sigmaSpatial, float sigmaColor, const cv::Mat_<glm::vec3> & colorSource, const cv::Mat_<uchar> & mask = cv::Mat_<uchar>(), GLWindow *window = nullptr);

    // interpolate all values smaller equal to minValidGradient (values bertween 0...1)
    void interpolate(float minValidGradient, const std::shared_ptr<GLWindow> &gl);

    static
    StructureTensorField computeStructureTensorField(const std::shared_ptr<GLWindow> &gl, const cv::Mat_<glm::vec3> & input_image, float inner_sigma, float outer_sigma, float threshold);

    static
    StructureTensorField computeStructureTensorField(const std::shared_ptr<GLWindow> & gl, const cv::Mat_<glm::vec3> &input_image, const cv::Mat_<uchar> &mask, float inner_sigma, float outer_sigma, float threshold);
    static
    StructureTensorField createFixedStructureTensorField(const cv::Size & size, const StructureTensor2x2 & tensor);

    static
    void RungeKutta4_MinEigenvector(const StructureTensorField & field, const glm::vec2 & pos, glm::vec2 & dir, float stepSize = sqrt(2.f), bool normalize = true);
	static
	void RungeKutta4_MaxEigenvector(const StructureTensorField & field, const glm::vec2 & pos, glm::vec2 & dir, float stepSize = sqrt(2.f), bool normalize = true);

private:

    cv::Mat_<StructureTensor2x2> m_tensors;

};
} // namespace linde

#endif // TENSOR_FIELD_H
