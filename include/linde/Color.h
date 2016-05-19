#ifndef LINDE_COLOR_H
#define LINDE_COLOR_H

#include "linde.h"


namespace linde
{
/**
        * @author Thomas Lindemeier
        * @date 07.12.2012
        *
        * All matrices, values and conversion formulas are from http://brucelindbloom.com/
        *
        * important:
        * -only sRGB as output and input color space is supported fom all rgb color spaces
        * -reference white for sRGB is D65
        * -reference white for L*a*b* is D50
        */

// reference illuminant tristimulus values http://brucelindbloom.com/index.html?Eqn_ChromAdapt.html
extern const glm::vec3 IM_ILLUMINANT_A;
extern const glm::vec3 IM_ILLUMINANT_B;
extern const glm::vec3 IM_ILLUMINANT_C;
extern const glm::vec3 IM_ILLUMINANT_D50;
extern const glm::vec3 IM_ILLUMINANT_D55;
extern const glm::vec3 IM_ILLUMINANT_D65;
extern const glm::vec3 IM_ILLUMINANT_D75;
extern const glm::vec3 IM_ILLUMINANT_E;
extern const glm::vec3 IM_ILLUMINANT_F2;
extern const glm::vec3 IM_ILLUMINANT_F7;
extern const glm::vec3 IM_ILLUMINANT_F11;


const float DELTA_LAB_JUST_NOTICEABLE = 2.3f;

// reference illuminant used for color conversions
extern glm::vec3 illuminant;

//sRGB D65, http://brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
extern const float XYZ2RGB_MATRIX[3][3];

//sRGB D65, http://brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
extern const float RGB2XYZ_MATRIX[3][3];


void convert_lab2xyz(const glm::vec3 & Lab, glm::vec3 & XYZ);

void convert_xyz2lab(const glm::vec3 & XYZ, glm::vec3 & Lab);
// CIE LCHab
void convert_lab2LCHab(const glm::vec3 & Lab, glm::vec3 & LCHab);
void convert_LCHab2lab(const glm::vec3 & LCHab, glm::vec3 & Lab);

// http://en.wikipedia.org/wiki/RYB_color_model
// http://threekings.tk/mirror/ryb_TR.pdf
void convert_ryb2rgb(const glm::vec3 & ryb, glm::vec3 & rgb);

// rgb to cmy
void convert_rgb2cmy(const glm::vec3 & rgb, glm::vec3 & cmy);
void convert_cmy2rgb(const glm::vec3 & cmy, glm::vec3 & rgb);

// rgb to cmyk
void convert_rgb2cmyk(const glm::vec3 & rgb, glm::vec4 & cmyk);
void convert_cmyk2rgb(const glm::vec4 & cmyk, glm::vec3 & rgb);

// uses sRGB chromatic adapted matrix
void convert_rgb2xyz(const glm::vec3 & rgb, glm::vec3 & XYZ);

// uses sRGB chromatic adapted matrix
void convert_xyz2rgb(const glm::vec3 & XYZ, glm::vec3 & rgb);

// make linear rgb, no chromatic adaption
void convert_srgb2rgb(const glm::vec3 & srgb, glm::vec3 & rgb);

// make sRGB, with gamma
void convert_rgb2srgb(const glm::vec3 & rgb, glm::vec3 & srgb);

// Lab (D50) -> XYZ -> rgb (D65) -> sRGB (D65)
void convert_lab2srgb(const glm::vec3 & Lab, glm::vec3 & srgb);

// sRGB (D65) -> rgb (D65) -> XYZ -> Lab (D50)
void convert_srgb2lab(const glm::vec3 & srgb, glm::vec3 & Lab);

// Lab (D50) -> XYZ -> rgb (D65)
void convert_lab2rgb(const glm::vec3 & Lab, glm::vec3 & rgb);

// rgb (D65) -> XYZ -> Lab (D50)
void convert_rgb2lab(const glm::vec3 & rgb, glm::vec3 & Lab);
void convert_rgb2lll(const glm::vec3 & rgb, glm::vec3 & lll); // 3 channel greyscale (all 3 channels contain luminance)

// XYZ -> rgb (D65) -> sRGB (D65)
void convert_xyz2srgb(const glm::vec3 & XYZ, glm::vec3 & srgb);

// [0..1] -> [0..1]
void convert_hsv2srgb(const glm::vec3 & hsv, glm::vec3 & srgb);

// [0..1] -> [0..1]
void convert_srgb2hsv(const glm::vec3 & srgb, glm::vec3 & hsv);

// sRGB (D65) -> XYZ
void convert_srgb2xyz(const glm::vec3 & srgb, glm::vec3 & XYZ);

void convert_xyz2xyY(const glm::vec3 & XYZ, glm::vec3 & xyY);
void convert_xyY2xyz(const glm::vec3 & xyY, glm::vec3 & XYZ);

void convert_Luv2XYZ(const glm::vec3 & Luv, glm::vec3 & XYZ);
void convert_XYZ2Luv(const glm::vec3 & XYZ, glm::vec3 & Luv);
// CIE LCHuv
void convert_Luv2LCHuv(const glm::vec3 & Luv, glm::vec3 & LCHuv);
void convert_LCHuv2Luv(const glm::vec3 & LCHuv, glm::vec3 & Luv);

// Y Cb Cr
void convert_Yuv2rgb(const glm::vec3 & Yuv, glm::vec3 & rgb);
void convert_rgb2Yuv(const glm::vec3 & rgb, glm::vec3 & Yuv);

// Ruderman et al.Statistics
// of Cone Responses to Natural Images; LAlphaBeta
void convert_rgb2L_alpha_beta(const glm::vec3 & rgb, glm::vec3 & LAlphaBeta);
void convert_L_alpha_beta2rgb(const glm::vec3 & LAlphaBeta, glm::vec3 & rgb);


// template for image conversions
typedef void(*convert_color_call)(const glm::vec3 &, glm::vec3 &);
void convert_image(const cv::Mat_<glm::vec3 > & in, cv::Mat_<glm::vec3 > & out, convert_color_call conversion);


// distances http://en.wikipedia.org/wiki/Color_difference
// http://cpansearch.perl.org/src/EWATERS/PDL-Graphics-ColorDistance-0.0.1/color_distance.c
float color_difference_CIEDE2000(const glm::vec3 & lab0, const glm::vec3 & lab1);

/**
 *  1 if colors are identical
*   0 if colors are maximal visual different
* */
float ColorSimilarity(const glm::vec3 & lab0, const glm::vec3 &lab1);
/**
*   1 if colors are maximal visual different
 *  0 if colors are identical
* */
float ColorDifference(const glm::vec3 & lab0, const glm::vec3 &lab1);


void AdjustContrast(const cv::Mat_<float> & source, cv::Mat_<float> & out, const float low, const float high, const float c);


void EqualizeLch(const cv::Mat_<glm::vec3> & Lch, cv::Mat_<glm::vec3> & out, const std::vector<uint> & channels, const cv::Mat_<uchar> &mask = cv::Mat_<uchar>());

void ContrastEdgeEnhance(const cv::Mat_<glm::vec3> & source, cv::Mat_<glm::vec3> & out, const std::vector<uint> & channels, const float sigma);

/**
    * @author Thomas Lindemeier
    * @date 02.12.2013
    */
class ColorTransferStatistic
{
    cv::Mat_<glm::vec3> m_source;
    cv::Mat_<glm::vec3> m_target;

    glm::vec3 m_meanS;
    glm::vec3 m_meanT;
    glm::vec3 m_varianceS;
    glm::vec3 m_varianceT;
    glm::vec3 m_sigmaS;
    glm::vec3 m_sigmaT;

public:
    ColorTransferStatistic();
    ~ColorTransferStatistic();

    void setSource(const cv::Mat_<glm::vec3> & source_Lab);
    void setTarget(const cv::Mat_<glm::vec3> & target_Lab);

    void init();

    cv::Mat_<glm::vec3> transfer();

};

/* The authors of this work have released all rights to it and placed it
       in the public domain under the Creative Commons CC0 1.0 waiver
       (http://creativecommons.org/publicdomain/zero/1.0/).

       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
       EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
       MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
       IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
       CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
       TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
       SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

       Retrieved from: http://en.literateprograms.org/Median_cut_algorithm_(C_Plus_Plus)?oldid=19175
    */
class MedianCutQuantization
{
    class Cube
    {
        glm::vec3 m_minP;
        glm::vec3 m_maxP;

        glm::vec3* m_points;
        ptrdiff_t m_length;

    public:
        Cube(glm::vec3 * points, ptrdiff_t length);

        int longestSideDim() const;
        float longestSideLength() const;

        void shrink();

        bool operator<(const Cube & o) const;

        glm::vec3 * getPoints();
        int numPoints() const;
    };

    template <int index>
    class CoordinatePointComparator
    {
    public:
        bool operator()(const glm::vec3 & left, const glm::vec3 & right)
        {
            return left[index] < right[index];
        }
    };

public:
    MedianCutQuantization();
    ~MedianCutQuantization();

    void computeColors(const cv::Mat_<glm::vec3> & image, int nrColors, std::vector<glm::vec3> & palette) const;

};

void quantizeImage(const cv::Mat_<glm::vec3> & source, cv::Mat_<glm::vec3> & out, const std::vector<glm::vec3> & palette);

void floydSteinberg(const cv::Mat_<glm::vec3> & source, cv::Mat_<glm::vec3> & out, const std::vector<glm::vec3> & palette);



}

#endif // LINDE_COLOR_H
