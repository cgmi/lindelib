#include "../include/linde/Color.h"

#include <queue>

#include <opencv2/imgproc/imgproc.hpp>

#include <glm/gtc/reciprocal.hpp>
#include <glm/gtx/norm.hpp>

#include "../include/linde/Histogram.h"

namespace linde
{

// reference illuminant tristimulus values http://brucelindbloom.com/index.html?Eqn_ChromAdapt.html
const glm::vec3 IM_ILLUMINANT_A(1.09850f, 1.00000f, 0.35585f);
const glm::vec3 IM_ILLUMINANT_B(0.99072f, 1.00000f, 0.85223f);
const glm::vec3 IM_ILLUMINANT_C(0.98074f, 1.00000f, 1.18232f);
const glm::vec3 IM_ILLUMINANT_D50(0.96422f, 1.00000f, 0.82521f);
const glm::vec3 IM_ILLUMINANT_D55(0.95682f, 1.00000f, 0.92149f);
const glm::vec3 IM_ILLUMINANT_D65(0.95047f, 1.00000f, 1.08883f);
const glm::vec3 IM_ILLUMINANT_D75(0.94972f, 1.00000f, 1.22638f);
const glm::vec3 IM_ILLUMINANT_E(1.00000f, 1.00000f, 1.00000f);
const glm::vec3 IM_ILLUMINANT_F2(0.99186f, 1.00000f, 0.67393f);
const glm::vec3 IM_ILLUMINANT_F7(0.95041f, 1.00000f, 1.08747f);
const glm::vec3 IM_ILLUMINANT_F11(1.00962f, 1.00000f, 0.64350f);

// reference illuminant used for color conversions
glm::vec3 illuminant = IM_ILLUMINANT_D65;

//sRGB D65, http://brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
const float XYZ2RGB_MATRIX[3][3] =
{
    { 3.2404542f, -1.5371385f, -0.4985314f },
    { -0.9692660f, 1.8760108f, 0.0415560f },
    { 0.0556434f, -0.2040259f, 1.0572252f }
};

//sRGB D65, http://brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
const float RGB2XYZ_MATRIX[3][3] =
{
    { 0.4124564f, 0.3575761f, 0.1804375f },
    { 0.2126729f, 0.7151522f, 0.0721750f },
    { 0.0193339f, 0.1191920f, 0.9503041f }
};

// rgb to cmy
void convert_rgb2cmy(const glm::vec3 & rgb, glm::vec3 & cmy)
{
    cmy[0] = 1.f - rgb[0];
    cmy[1] = 1.f - rgb[1];
    cmy[2] = 1.f - rgb[2];
}

// cmy to rgb
void convert_cmy2rgb(const glm::vec3 & cmy, glm::vec3 & rgb)
{
    rgb[0] = 1.f - cmy[0];
    rgb[1] = 1.f - cmy[1];
    rgb[2] = 1.f - cmy[2];
}

//
void convert_rgb2cmyk(const glm::vec3 & rgb, glm::vec4 & cmyk)
{
    float k = std::min<float>(1.f - rgb[0], std::min<float>(1.f - rgb[1], 1.f - rgb[2]));
    cmyk[0] = (1.f - rgb[0] - k) / (1.f - k);
    cmyk[1] = (1.f - rgb[1] - k) / (1.f - k);
    cmyk[2] = (1.f - rgb[2] - k) / (1.f - k);
    cmyk[3] = k;
}

void convert_cmyk2rgb(const glm::vec4 & cmyk, glm::vec3 & rgb)
{
    rgb[0] = -((cmyk[0] * (1.f - cmyk[3])) + cmyk[3] - 1.f);
    rgb[1] = -((cmyk[1] * (1.f - cmyk[3])) + cmyk[3] - 1.f);
    rgb[2] = -((cmyk[2] * (1.f - cmyk[3])) + cmyk[3] - 1.f);
}

float color_difference_CIEDE2000(const glm::vec3 & lab1, const glm::vec3 & lab2)
{
    float Lstd = lab1[0];
    float astd = lab1[1];
    float bstd = lab1[2];

    float Lsample = lab2[0];
    float asample = lab2[1];
    float bsample = lab2[2];

    const float pi = PI<float>();

    float Cabstd = sqrt(astd*astd + bstd*bstd);
    float Cabsample = sqrt(asample*asample + bsample*bsample);

    float Cabarithmean = (Cabstd + Cabsample) / 2.f;

    float G = 0.5f*(1.f - sqrt(std::pow(Cabarithmean, 7.f) / (std::pow(Cabarithmean, 7.f) + pow(25.f, 7.f))));

    float apstd = (1.f + G)*astd; // aprime in paper
    float apsample = (1.f + G)*asample; // aprime in paper
    float Cpsample = sqrt(apsample*apsample + bsample*bsample);

    float Cpstd = sqrt(apstd*apstd + bstd*bstd);
    // Compute product of chromas
    float Cpprod = (Cpsample*Cpstd);


    // Ensure hue is between 0 and 2pi
    float hpstd = atan2(bstd, apstd);
    if (hpstd<0) hpstd += 2.f*pi;  // rollover ones that come -ve

    float hpsample = atan2(bsample, apsample);
    if (hpsample<0) hpsample += 2.f*pi;
    if ((fabs(apsample) + fabs(bsample)) == 0.f)  hpsample = 0.f;

    float dL = (Lsample - Lstd);
    float dC = (Cpsample - Cpstd);

    // Computation of hue difference
    float dhp = (hpsample - hpstd);
    if (dhp>pi)  dhp -= 2.f*pi;
    if (dhp<-pi) dhp += 2.f*pi;
    // set chroma difference to zero if the product of chromas is zero
    if (Cpprod == 0.f) dhp = 0.f;

    // Note that the defining equations actually need
    // signed Hue and chroma differences which is different
    // from prior color difference formulae

    float dH = 2.f*sqrt(Cpprod)*sin(dhp / 2.f);
    //%dH2 = 4*Cpprod.*(sin(dhp/2)).^2;

    // weighting functions
    float Lp = (Lsample + Lstd) / 2.f;
    float Cp = (Cpstd + Cpsample) / 2.f;

    // Average Hue Computation
    // This is equivalent to that in the paper but simpler programmatically.
    // Note average hue is computed in radians and converted to degrees only
    // where needed
    float hp = (hpstd + hpsample) / 2.f;
    // Identify positions for which abs hue diff exceeds 180 degrees
    if (fabs(hpstd - hpsample)  > pi) hp -= pi;
    // rollover ones that come -ve
    if (hp<0) hp += 2.f*pi;

    // Check if one of the chroma values is zero, in which case set
    // mean hue to the sum which is equivalent to other value
    if (Cpprod == 0.f) hp = hpsample + hpstd;

    float Lpm502 = (Lp - 50.f)*(Lp - 50.f);
    float Sl = 1.f + 0.015f*Lpm502 / std::sqrt(20.0f + Lpm502);
    float Sc = 1.f + 0.045f*Cp;
    float T = 1.f - 0.17f*std::cos(hp - pi / 6.f) + 0.24f*std::cos(2.f*hp) + 0.32f*std::cos(3.f*hp + pi / 30.f) - 0.20f*std::cos(4.f*hp - 63.f*pi / 180.f);
    float Sh = 1.f + 0.015f*Cp*T;
    float delthetarad = (30.f*pi / 180.f)*std::exp(-std::pow(((180.f / pi*hp - 275.f) / 25.f), 2.f));
    float Rc = 2.f*std::sqrt(std::pow(Cp, 7.f) / (std::pow(Cp, 7.f) + std::pow(25.f, 7.f)));
    float RT = -std::sin(2.0f*delthetarad)*Rc;

    // The CIE 00 color difference
    return sqrt(std::pow((dL / Sl), 2.f) + std::pow((dC / Sc), 2.f) + std::pow((dH / Sh), 2.f) + RT*(dC / Sc)*(dH / Sh));
}




// http://en.wikipedia.org/wiki/RYB_color_model
// http://threekings.tk/mirror/ryb_TR.pdf
void convert_ryb2rgb(const glm::vec3 & ryb, glm::vec3 & rgb)
{
    auto cubicInt = [](float t, float A, float B)
    {
        float weight = t*t*(3 - 2 * t);
        return A + weight*(B - A);
    };
    float x0, x1, x2, x3, y0, y1;
    //red
    x0 = cubicInt(ryb[2], 1.0f, 0.163f);
    x1 = cubicInt(ryb[2], 1.0f, 0.0f);
    x2 = cubicInt(ryb[2], 1.0f, 0.5f);
    x3 = cubicInt(ryb[2], 1.0f, 0.2f);
    y0 = cubicInt(ryb[1], x0, x1);
    y1 = cubicInt(ryb[1], x2, x3);
    rgb[0] = cubicInt(ryb[0], y0, y1);
    //green
    x0 = cubicInt(ryb[2], 1.0f, 0.373f);
    x1 = cubicInt(ryb[2], 1.0f, 0.66f);
    x2 = cubicInt(ryb[2], 0.0f, 0.0f);
    x3 = cubicInt(ryb[2], 0.5f, 0.094f);
    y0 = cubicInt(ryb[1], x0, x1);
    y1 = cubicInt(ryb[1], x2, x3);
    rgb[1] = cubicInt(ryb[0], y0, y1);
    //blue
    x0 = cubicInt(ryb[2], 1.0f, 0.6f);
    x1 = cubicInt(ryb[2], 0.0f, 0.2f);
    x2 = cubicInt(ryb[2], 0.0f, 0.5f);
    x3 = cubicInt(ryb[2], 0.0f, 0.0f);
    y0 = cubicInt(ryb[1], x0, x1);
    y1 = cubicInt(ryb[1], x2, x3);
    rgb[2] = cubicInt(ryb[0], y0, y1);

}

static inline float f(float t)
{
    return (t > std::pow<float>(6.f/29.f, 3.f)) ? std::pow<float>(t, 1.f/3.f) : (1.f/3.f) * std::pow<float>(29.f/6.f, 2.f) * t + (4.f/29.f);
}

static inline float fi(float t)
{
    return (t > 6.f/29.f) ? std::pow<float>(t, 3.f) : 3.f * std::pow<float>(6.f/29.f, 2.f) * (t - (4.f/29.f));
}

void convert_xyz2lab(const glm::vec3 & XYZ, glm::vec3 & Lab)
{
    Lab[0] = 116.f * f(XYZ[1]/ illuminant[1]) - 16.f;
    Lab[1] = 500.f * (f(XYZ[0]/ illuminant[0]) - f(XYZ[1]/ illuminant[1]));
    Lab[2] = 200.f * (f(XYZ[1]/ illuminant[1]) - f(XYZ[2]/ illuminant[2]));
}


void convert_lab2xyz(const glm::vec3 & Lab, glm::vec3 & XYZ)
{
    // chromatic adaption, reference white
    XYZ[1] = illuminant[1] * fi((1.f/116.f) * (Lab[0] + 16.f)); // Y
    XYZ[0] = illuminant[0] * fi((1.f/116.f) * (Lab[0] + 16.f) + (1.f/500.f) * Lab[1]); // X
    XYZ[2] = illuminant[2] * fi((1.f/116.f) * (Lab[0] + 16.f) - (1.f/200.f) * Lab[2]); // Z

}

void convert_lab2LCHab(const glm::vec3 & Lab, glm::vec3 & LCHab)
{
    LCHab[0] = Lab[0]; // [0,100]
    LCHab[1] = sqrt(Lab[1]*Lab[1] + Lab[2]*Lab[2]); // [0,100]

    LCHab[2] = atan2(Lab[2], Lab[1]);
    if (LCHab[2] < 0)
    {
        LCHab[2] += TWO_PI<float>(); // [0, 2pi]
    }

}

void convert_LCHab2lab(const glm::vec3 & LCHab, glm::vec3 & Lab)
{
    Lab[0] = LCHab[0];
    float h = LCHab[2];
    if (h > linde::PI<float>())
    {
        h -= linde::TWO_PI<float>(); // [0, 2pi]
    }
    Lab[1] = LCHab[1] * cos(h);
    Lab[2] = LCHab[1] * sin(h);
}



// uses sRGB chromatic adapted matrix
void convert_rgb2xyz(const glm::vec3 & rgb, glm::vec3 & XYZ)
{
    XYZ[0] = RGB2XYZ_MATRIX[0][0] * rgb[0] + RGB2XYZ_MATRIX[0][1] * rgb[1] + RGB2XYZ_MATRIX[0][2] * rgb[2];
    XYZ[1] = RGB2XYZ_MATRIX[1][0] * rgb[0] + RGB2XYZ_MATRIX[1][1] * rgb[1] + RGB2XYZ_MATRIX[1][2] * rgb[2];
    XYZ[2] = RGB2XYZ_MATRIX[2][0] * rgb[0] + RGB2XYZ_MATRIX[2][1] * rgb[1] + RGB2XYZ_MATRIX[2][2] * rgb[2];
}

// uses sRGB chromatic adapted matrix
void convert_xyz2rgb(const glm::vec3 & XYZ, glm::vec3 & rgb)
{
    rgb[0] = XYZ2RGB_MATRIX[0][0] * XYZ[0] + XYZ2RGB_MATRIX[0][1] * XYZ[1] + XYZ2RGB_MATRIX[0][2] * XYZ[2];
    rgb[1] = XYZ2RGB_MATRIX[1][0] * XYZ[0] + XYZ2RGB_MATRIX[1][1] * XYZ[1] + XYZ2RGB_MATRIX[1][2] * XYZ[2];
    rgb[2] = XYZ2RGB_MATRIX[2][0] * XYZ[0] + XYZ2RGB_MATRIX[2][1] * XYZ[1] + XYZ2RGB_MATRIX[2][2] * XYZ[2];
}

// make linear rgb, no chromatic adaption
void convert_srgb2rgb(const glm::vec3 & srgb, glm::vec3 & rgb)
{
    for (int i = 0; i < 3; i++)
    {
        if (srgb[i] <= 0.04045f)
            rgb[i] = srgb[i] / 12.92f;
        else
            rgb[i] = powf(((srgb[i] + 0.055f) / 1.055f), 2.4f);
    }
}

// make sRGB, with gamma
void convert_rgb2srgb(const glm::vec3 & rgb, glm::vec3 & srgb)
{
    for (int i = 0; i < 3; ++i)
    {
        if (rgb[i] <= 0.0031308f)
            srgb[i] = rgb[i] * 12.92f;
        else
            srgb[i] = 1.055f * powf(rgb[i], 1.0f / 2.4f) - 0.055f;
    }
}

// Lab (D50) -> XYZ -> rgb (D65) -> sRGB (D65)
void convert_lab2srgb(const glm::vec3 & Lab, glm::vec3 & srgb)
{
    glm::vec3 XYZ;
    convert_lab2xyz(Lab, XYZ);
    glm::vec3 rgb;
    convert_xyz2rgb(XYZ, rgb);
    convert_rgb2srgb(rgb, srgb);
}

// sRGB (D65) -> rgb (D65) -> XYZ -> Lab (D50)
void convert_srgb2lab(const glm::vec3 & srgb, glm::vec3 & Lab)
{
    glm::vec3 rgb;
    convert_srgb2rgb(srgb, rgb);
    glm::vec3 XYZ;
    convert_rgb2xyz(rgb, XYZ);
    convert_xyz2lab(XYZ, Lab);
}

// Lab (D50) -> XYZ -> rgb (D65)
void convert_lab2rgb(const glm::vec3 & Lab, glm::vec3 & rgb)
{
    glm::vec3 XYZ;
    convert_lab2xyz(Lab, XYZ);
    convert_xyz2rgb(XYZ, rgb);
}

// rgb (D65) -> XYZ -> Lab (D50)
void convert_rgb2lab(const glm::vec3 & rgb, glm::vec3 & Lab)
{
    glm::vec3 XYZ;
    convert_rgb2xyz(rgb, XYZ);
    convert_xyz2lab(XYZ, Lab);
}

void convert_rgb2lll(const glm::vec3 & rgb, glm::vec3 & lll)
{
    glm::vec3 XYZ;
    convert_rgb2xyz(rgb, XYZ);
    convert_xyz2lab(XYZ, lll);
    lll[0] /= 100.f;
    lll[1] = lll[2] = lll[0];
}


// XYZ -> rgb (D65) -> sRGB (D65)
void convert_xyz2srgb(const glm::vec3 & XYZ, glm::vec3 & srgb)
{
    glm::vec3 rgb;
    convert_xyz2rgb(XYZ, rgb);
    convert_rgb2srgb(rgb, srgb);
}

// [0..1] -> [0..1]
void convert_hsv2srgb(const glm::vec3 & hsv, glm::vec3 & srgb)
{
    const float h = (360.0f*hsv[0]) / 60.0f;
    float hi = std::floor(h);
    float f = (h - hi);

    float p = hsv[2] * (1 - hsv[1]);
    float q = hsv[2] * (1 - hsv[1] * f);
    float t = hsv[2] * (1 - hsv[1] * (1 - f));

    if (hi == 1)
    {
        srgb[0] = q;
        srgb[1] = hsv[2];
        srgb[2] = p;
    }
    else if (hi == 2)
    {
        srgb[0] = p;
        srgb[1] = hsv[2];
        srgb[2] = t;
    }
    else if (hi == 3)
    {
        srgb[0] = p;
        srgb[1] = q;
        srgb[2] = hsv[2];
    }
    else if (hi == 4)
    {
        srgb[0] = t;
        srgb[1] = p;
        srgb[2] = hsv[2];
    }
    else if (hi == 5)
    {
        srgb[0] = hsv[2];
        srgb[1] = p;
        srgb[2] = q;
    }
    else {
        srgb[0] = hsv[2];
        srgb[1] = t;
        srgb[2] = p;
    }

}

// [0..1] -> [0..1]
void convert_srgb2hsv(const glm::vec3 & srgb, glm::vec3 & hsv)
{
    float min;
    float max;
    float delMax;

    min = std::min<float>(std::min<float>(srgb[0], srgb[1]), srgb[2]);
    max = std::max<float>(std::max<float>(srgb[0], srgb[1]), srgb[2]);
    delMax = 1.0f / (max - min);

    const float fa = 1.0f / 360.0f;

    if (max == min)
        hsv[0] = 0;
    else if (max == srgb[0])
        hsv[0] = 60.0f * (0 + (srgb[1] - srgb[2]) * delMax);
    else if (max == srgb[1])
        hsv[0] = 60.0f * (2 + (srgb[2] - srgb[0]) * delMax);
    else if (max == srgb[2])
        hsv[0] = 60.0f * (4 + (srgb[0] - srgb[1]) * delMax);

    if (hsv[0] < 0)
        hsv[0] += 360;

    if (max == 0)
    {
        hsv[1] = 0;
    }
    else
    {
        hsv[1] = (max - min) / max;
    }
    hsv[2] = max;

    hsv[0] *= fa;
}

// sRGB (D65) -> XYZ
void convert_srgb2xyz(const glm::vec3 & srgb, glm::vec3 & XYZ)
{
    glm::vec3 rgb;
    convert_srgb2rgb(srgb, rgb);
    convert_rgb2xyz(rgb, XYZ);
}

void convert_xyz2xyY(const glm::vec3 & XYZ, glm::vec3 & xyY)
{
    xyY[0] = XYZ[0] / (XYZ[0] + XYZ[1] + XYZ[2]);
    xyY[1] = XYZ[1] / (XYZ[0] + XYZ[1] + XYZ[2]);
    xyY[2] = XYZ[1];
}

void convert_xyY2xyz(const glm::vec3 & xyY, glm::vec3 & XYZ)
{
    XYZ[1] = xyY[2];
    XYZ[0] = (xyY[2] / xyY[1]) * xyY[0];
    XYZ[2] = (xyY[2] / xyY[1]) * (1 - xyY[0] - xyY[1]);
}

void convert_Luv2XYZ(const glm::vec3 & Luv, glm::vec3 & XYZ)
{
    const float eps = 216.0f / 24389.0f;
    const float k = 24389.0f / 27.0f;
    const float keps = k*eps;

    XYZ[1] = (Luv[0] > keps) ? (std::pow<float>((Luv[0] + 16.0f) / 116.0f, 3.f)) : (Luv[1] / k);

    float Xr, Yr, Zr;
    Xr = illuminant[0];
    Yr = illuminant[1];
    Zr = illuminant[2];

    float u0, v0;
    u0 = (4.0f*Xr) / (Xr + 15.0f*Yr + 3.0f*Zr);
    v0 = (9.0f*Yr) / (Xr + 15.0f*Yr + 3.0f*Zr);

    float a, b, c, d;
    a = (1.0f / 3.0f) * (((52.0f*Luv[0]) / (Luv[1] + 13.0f*Luv[0] * u0)) - 1.0f);
    b = -5.0f * XYZ[1];
    c = -(1.0f / 3.0f);
    d = XYZ[1] * (((39.0f*Luv[0]) / (Luv[2] + 13.0f*Luv[0] * v0)) - 5.0f);

    XYZ[0] = (d - b) / (a - c);
    XYZ[2] = XYZ[0] * a + b;
}

void convert_Yuv2rgb(const glm::vec3 & Yuv, glm::vec3 & rgb)
{
    rgb[2] = 1.164f * (Yuv[0] - 16) + 2.018f * (Yuv[1] - 128.0f);
    rgb[1] = 1.164f * (Yuv[0] - 16) - 0.813f * (Yuv[2] - 128.0f) - 0.391f * (Yuv[1] - 128.0f);
    rgb[0] = 1.164f * (Yuv[0] - 16) + 1.596f * (Yuv[2] - 128.0f);
    rgb *= (1.0f / 255.0f);
}

void convert_rgb2Yuv(const glm::vec3 & rgb, glm::vec3 & Yuv)
{
    glm::vec3 rgb_scaled = rgb * 255.0f;
    Yuv[0] = (0.257f * rgb_scaled[0]) + (0.504f * rgb_scaled[1]) + (0.098f * rgb_scaled[2]) + 16;
    Yuv[2] = (0.439f * rgb_scaled[0]) - (0.368f * rgb_scaled[1]) - (0.071f * rgb_scaled[2]) + 128;
    Yuv[1] = -(0.148f * rgb_scaled[0]) - (0.291f * rgb_scaled[1]) + (0.439f * rgb_scaled[2]) + 128;
}

void convert_rgb2L_alpha_beta(const glm::vec3 & rgb, glm::vec3 & LAlphaBeta)
{
    // rgb 2 LMS
    glm::vec3 LMS;
    LMS[0] = 0.3811f*rgb[0] + 0.5783f*rgb[1] + 0.0402f*rgb[2];
    LMS[1] = 0.1967f*rgb[0] + 0.7244f*rgb[1] + 0.0782f*rgb[2];
    LMS[2] = 0.0241f*rgb[0] + 0.1288f*rgb[1] + 0.8444f*rgb[2];

    // scale logarithmic
    LMS[0] = log(LMS[0]);
    LMS[1] = log(LMS[1]);
    LMS[2] = log(LMS[2]);

    glm::vec3 temp;
    temp[0] = 1.f*LMS[0] + 1.f*LMS[1] + 1.f*LMS[2];
    temp[1] = 1.f*LMS[0] + 1.f*LMS[1] + (-2.f*LMS[2]);
    temp[2] = 1.f*LMS[0] + (-1.f*LMS[1]);

    LAlphaBeta[0] = (1.f / sqrt(3.f))*temp[0];
    LAlphaBeta[1] = (1.f / sqrt(6.f))*temp[1];
    LAlphaBeta[2] = (1.f / sqrt(2.f))*temp[2];
}

void convert_L_alpha_beta2rgb(const glm::vec3 & LAlphaBeta, glm::vec3 & rgb)
{
    glm::vec3 temp;
    temp[0] = (sqrt(3.f) / 3.f)*LAlphaBeta[0];
    temp[1] = (sqrt(6.f) / 6.f)*LAlphaBeta[1];
    temp[2] = (sqrt(2.f) / 2.f)*LAlphaBeta[2];

    temp[0] = 1.f*temp[0] + 1.f*temp[1] + 1.f*temp[2];
    temp[1] = 1.f*temp[0] + 1.f*temp[1] + (-1.f*temp[2]);
    temp[2] = 1.f*temp[0] + (-2.f*temp[1]);

    // remove logarithmic scale
    temp[0] = pow(10.0f, temp[0]);
    temp[1] = pow(10.0f, temp[1]);
    temp[2] = pow(10.0f, temp[2]);

    rgb[0] = 4.4679f*temp[0] + (-3.5873f*temp[1]) + 0.1193f*temp[2];
    rgb[1] = (-1.2186f*temp[0]) + 2.3809f*temp[1] + (-0.1624f*temp[2]);
    rgb[2] = 0.0497f*temp[0] + (-0.2439f*temp[1]) + 1.2405f*temp[2];
}

void convert_XYZ2Luv(const glm::vec3 & XYZ, glm::vec3 & Luv)
{
    const float eps = 216.0f / 24389.0f;
    const float k = 24389.0f / 27.0f;

    // chromatic adaption, reference white
    float Xr = illuminant[0];
    float Yr = illuminant[1];
    float Zr = illuminant[2];

    float yr = XYZ[1] / Yr;

    Luv[0] = (yr > eps) ? (116.0f * std::pow<float>(yr, 1.0f / 3.0f) - 16.0f) : k*yr;

    float nen = XYZ[0] + 15.0f*XYZ[1] + 3 * XYZ[2];
    float u_ = (4 * XYZ[0]) / (nen);
    float v_ = (9 * XYZ[1]) / (nen);
    nen = Xr + 15.0f*Yr + 3 * Zr;
    float ur_ = (4 * Xr) / (nen);
    float vr_ = (9 * Yr) / (nen);

    Luv[1] = 13.0f * Luv[0] * (u_ - ur_);
    Luv[2] = 13.0f * Luv[0] * (v_ - vr_);
}

void convert_Luv2LCHuv(const glm::vec3 & Luv, glm::vec3 & LCHuv)
{
    LCHuv[0] = Luv[0];
    LCHuv[1] = sqrt(sqr(Luv[1]) + sqr(Luv[2]));
    LCHuv[2] = atan2(Luv[2], Luv[1]);
}

void convert_LCHuv2Luv(const glm::vec3 & LCHuv, glm::vec3 & Luv)
{
    Luv[0] = LCHuv[0];
    Luv[1] = LCHuv[1] * cos(LCHuv[2]);
    Luv[2] = LCHuv[1] * sin(LCHuv[2]);
}

/*
Shiraishi, M., & Yamaguchi, Y. (2000).
An algorithm for automatic painterly rendering based on local source image approximation.
Non-Photorealistic Animation and Rendering, 53–58. Retrieved from http://dl.acm.org/citation.cfm?id=340923
http://www.visgraf.impa.br/Courses/ip00/papers/npar2000.pdf
*/
float ColorSimilarity(const glm::vec3 &lab0, const glm::vec3 &lab1)
{
    const float d0 = 150.f;
    float d = glm::distance(lab0, lab1);
    if (d >= 0 && d <= d0)
    {
        return glm::pow((1.f - glm::pow(d/d0, 2.f)), 2.f);
    } else
    {
        return 0.f;
    }
}

float ColorDifference(const glm::vec3 &lab0, const glm::vec3 &lab1)
{
    const float d0 = 150.f;
    float d = glm::distance(lab0, lab1);
    if (d >= 0 && d <= d0)
    {
        return d/d0;
    } else
    {
        return 0.f;
    }
}



/*
            #######################################################################
            #######################################################################
            #######################################################################
            #######################################################################
            #################### image conversions ################################
            #######################################################################
            #######################################################################
            #######################################################################
            #######################################################################
            #######################################################################
            */


void convert_image(
        const cv::Mat_<glm::vec3> & in, cv::Mat_<glm::vec3> & out,
        convert_color_call conversion)
{
    cv::Mat_<glm::vec3> temp(in.size());

    std::transform(in.begin(), in.end(), temp.begin(),
                   [&](const glm::vec3 & c)
    {
        glm::vec3 o;
        conversion(c, o);
        return o;
    });

    out = temp;
}




ColorTransferStatistic::ColorTransferStatistic()
{

}
ColorTransferStatistic::~ColorTransferStatistic()
{

}

void ColorTransferStatistic::setSource(const cv::Mat_<glm::vec3> & source)
{
    m_source = source;
}
void ColorTransferStatistic::setTarget(const cv::Mat_<glm::vec3> & target)
{
    m_target = target;
}

void ColorTransferStatistic::init()
{
    myassert(!m_source.data || !m_target.data);

    const float pixels = static_cast<float>(m_source.rows * m_source.cols);

    // compute mean
    m_meanS = glm::vec3(0.0f);
    for (glm::vec3 & p : m_source)
    {
        m_meanS += p;
    }
    m_meanS /= pixels;
    m_meanT = glm::vec3(0.0f);
    for (glm::vec3 & p : m_target)
    {
        m_meanT += p;
    }
    m_meanT /= pixels;

    // compute variance and standard deviation
    m_varianceS = glm::vec3(0.0f);
    for (glm::vec3 & p : m_source)
    {
        m_varianceS += (p - m_meanS) * (p - m_meanS);
    }
    m_varianceS /= pixels;
    m_varianceT = glm::vec3(0.0f);
    for (glm::vec3 & p : m_target)
    {
        m_varianceT += (p - m_meanT) * (p - m_meanT);
    }
    m_varianceT /= pixels;

    m_sigmaS = sqrt(m_varianceS);
    m_sigmaT = sqrt(m_varianceT);
}

cv::Mat_<glm::vec3> ColorTransferStatistic::transfer()
{
    const int rows = m_target.rows;
    const int cols = m_target.cols;

    glm::vec3 scale = m_sigmaT / m_sigmaS;

    cv::Mat_<glm::vec3> result(rows, cols);
    result.setTo(cv::Scalar(0.0f, 0.0f, 0.0f));

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result(i, j) = scale * (m_target(i, j) - m_meanT) + m_meanS;
        }
    }
    return result;
}

MedianCutQuantization::MedianCutQuantization() {}

MedianCutQuantization::~MedianCutQuantization() {}

MedianCutQuantization::Cube::Cube(glm::vec3 * points, ptrdiff_t length) :
    m_minP(std::numeric_limits<float>::min()),
    m_maxP(std::numeric_limits<float>::max()),
    m_points(points),
    m_length(length)
{

}

glm::vec3 * MedianCutQuantization::Cube::getPoints()
{
    return m_points;
}

int MedianCutQuantization::Cube::numPoints() const
{
    return static_cast<int>(m_length);
}


int MedianCutQuantization::Cube::longestSideDim() const
{
    float m = m_maxP[0] - m_minP[0];
    int dim = 0;
    for (int i = 0; i < 3; ++i)
    {
        float d = m_maxP[i] - m_minP[i];
        if (d > m)
        {
            m = d;
            dim = i;
        }
    }
    return dim;
}

float MedianCutQuantization::Cube::longestSideLength() const
{
    int i = longestSideDim();
    return m_maxP[i] - m_minP[i];
}

bool MedianCutQuantization::Cube::operator<(const Cube & o) const
{
    return this->longestSideLength() < o.longestSideLength();
}


void MedianCutQuantization::Cube::shrink()
{
    int i, j;
    for (j = 0; j < 3; j++)
    {
        m_minP[j] = m_maxP[j] = m_points[0][j];
    }
    for (i = 1; i < m_length; i++)
    {
        for (j = 0; j < 3; j++)
        {
            m_minP[j] = std::min(m_minP[j], m_points[i][j]);
            m_maxP[j] = std::max(m_maxP[j], m_points[i][j]);
        }
    }
}


void MedianCutQuantization::computeColors(const cv::Mat_<glm::vec3> & image, int nrColors, std::vector<glm::vec3> & palette) const
{
    std::priority_queue<Cube> cubeQueue;

    cv::Mat_<glm::vec3> copy = image.clone();
    Cube initCube((glm::vec3*)copy.data, copy.rows * copy.cols);
    initCube.shrink();

    cubeQueue.push(initCube);

    while (cubeQueue.size() < (size_t)nrColors)
    {
        Cube longestCube = cubeQueue.top();
        cubeQueue.pop();
        glm::vec3 * begin = longestCube.getPoints();
        glm::vec3 * median = longestCube.getPoints() + (longestCube.numPoints() + 1) / 2;
        glm::vec3 * end = longestCube.getPoints() + longestCube.numPoints();
        switch (longestCube.longestSideDim())
        {
        case 0: std::nth_element(begin, median, end, CoordinatePointComparator<0>()); break;
        case 1: std::nth_element(begin, median, end, CoordinatePointComparator<1>()); break;
        case 2: std::nth_element(begin, median, end, CoordinatePointComparator<2>()); break;
        }
        Cube block1(begin, median - begin), block2(median, end - median);
        block1.shrink();
        block2.shrink();
        cubeQueue.push(block1);
        cubeQueue.push(block2);
    }

    // find averaging colors of the cubes
    palette.clear();
    while (!cubeQueue.empty())
    {
        Cube block = cubeQueue.top();
        cubeQueue.pop();
        glm::vec3 * points = block.getPoints();

        glm::vec3 sum(0);
        for (int i = 0; i < block.numPoints(); i++)
        {
            sum += points[i];
        }

        glm::vec3 averagePoint = sum / static_cast<float>(block.numPoints());

        palette.push_back(averagePoint);
    }
}


void quantizeImage(const cv::Mat_<glm::vec3> & source, cv::Mat_<glm::vec3> & out, const std::vector<glm::vec3> & palette)
{
    cv::Mat_<glm::vec3> temp(source.size());

    std::transform(source.begin(), source.end(), temp.begin(),
                   [&](const glm::vec3 & p)
    {
        glm::vec3 closest_color = palette[0];
        float dist = glm::length(closest_color - p);
        for (const glm::vec3 & color : palette)
        {
            float t_d = glm::length(color - p);
            if (dist > t_d)
            {
                dist = t_d;
                closest_color = color;
            }
        }
        return closest_color;
    });

    out = temp;
}

void floydSteinberg(const cv::Mat_<glm::vec3> & source, cv::Mat_<glm::vec3> & out, const std::vector<glm::vec3> & palette)
{
    cv::Mat_<glm::vec3> temp = source.clone();

    const int rows = source.rows;
    const int cols = source.cols;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            glm::vec3 old_pixel = temp(i, j);

            // find closest color
            glm::vec3 closest_color(0);
            float dist = std::numeric_limits<float>::max();
            for (const glm::vec3 & color : palette)
            {
                float t_d = glm::length(color - old_pixel);
                if (dist > t_d)
                {
                    dist = t_d;
                    closest_color = color;
                }
            } // end palette

            // set new pixel
            temp(i, j) = closest_color;

            // compute error
            glm::vec3 error = old_pixel - closest_color;

            int ni = cv::borderInterpolate(i + 1, rows, cv::BORDER_REFLECT);
            int nj = cv::borderInterpolate(j + 1, cols, cv::BORDER_REFLECT);
            int pj = cv::borderInterpolate(j - 1, cols, cv::BORDER_REFLECT);

            // diffuse error
            temp(i, nj) += (7.0f / 16.0f) * error;
            temp(ni, pj) += (3.0f / 16.0f) * error;
            temp(ni, j) += (5.0f / 16.0f) * error;
            temp(ni, nj) += (1.0f / 16.0f) * error;

        } // end y
    } // end x

    out = temp;
}

void AdjustContrast(const cv::Mat_<float> &source, cv::Mat_<float> &out, const float low, const float high, const float c)
{
    if (out.size() != source.size())
    {
        out.create(source.size());
    }

    const float f = (259.f * (c + 255.f)) / (255.f * (259.f - c));

    for (uint i = 0; i < out.total(); i++)
    {
        const float R = mapRange(source(i), low, high, 0.f, 255.f);
        const float R_ =  f * (R - 128.f) + 128.f;
        out(i) = mapRange(glm::clamp(R_, 0.f, 255.f), 0.f, 255.f, low, high);
    }
}

void EqualizeLch(const cv::Mat_<glm::vec3> & Lch, cv::Mat_<glm::vec3> & out, const std::vector<uint> & channels, const cv::Mat_<uchar> &mask)
{
    std::vector<cv::Mat_<float> > splitChannels;
    cv::split(Lch, splitChannels);

    for (const uint channel : channels)
    {
        Histogram<float> hist;
        if (channel == 0)
        {
            hist.setBins(1000.f);
            hist.create(splitChannels[channel], 0.f, 100.f);
        } else if (channel == 1)
        {
            hist.setBins(1000.f);
            hist.create(splitChannels[channel], 0.f, 100.f);
        } else if (channel == 2)
        {
            hist.setBins(1000.f);
            hist.create(splitChannels[channel], 0.f, TWO_PI<float>());
        }

        hist.cumulative().matchTo(splitChannels[channel]);
    }

    cv::merge(splitChannels, Lch);
}






} // namespace linde
