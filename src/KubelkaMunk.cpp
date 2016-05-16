#include "../include/linde/KubelkaMunk.h"

#include <glm/gtc/reciprocal.hpp>

namespace linde
{
/// Kubelka Munk
///
///
///
///
///
///
///
///
///


KMColor::KMColor() :
    K(0.f),S(0.f)
{}

KMColor::KMColor(const glm::vec3 & linearRgb) :
    K(0.f),S(0.f)
{
    const float t = 1.f / (float)std::numeric_limits<uchar>::max();

    glm::vec3 tv(t, t, t);

    glm::vec3 rgbWhite = glm::clamp(linearRgb, 2.f * tv, 1.f - tv);

    glm::vec3 rgbBlack = glm::clamp(rgbWhite - tv, tv, rgbWhite - tv);

    from_Rb_Rw(rgbBlack, rgbWhite, *this);
}

KMColor::KMColor(const glm::vec3 & K, const glm::vec3 & S) :
    K(K), S(S)
{}


glm::vec3 KMColor::getK() const
{
    return K;
}

glm::vec3 KMColor::getS() const
{
    return S;
}

glm::vec3 KMColor::getQuotient() const
{
    return K / S;
}


glm::vec3 KMColor::getTransmittance( const float d) const
{
    glm::vec3 K_S = K / S;

    glm::vec3 b = glm::sqrt(K_S * (K_S + 2.f));


    glm::vec3 bSd = b*S*d;

    glm::vec3 R =  1.f / (1.f + K_S + b * glm::coth(bSd));

    return b * R * glm::csch(bSd);
}


glm::vec3 KMColor::getReflectance(const float d) const
{
    glm::vec3 K_S = K / S;

    glm::vec3 b = glm::sqrt(K_S * (K_S + 2.f));

    glm::vec3 bSd = b*S*d;

    return  1.f / (1.f + K_S + b * glm::coth(bSd));

}

void KMColor::computeRT(glm::vec3 & R, glm::vec3 & T, const float d) const
{
    glm::vec3 K_S = K / S;

    glm::vec3 b = glm::sqrt(K_S * (K_S + 2.f));

    glm::vec3 bSd = b*S*d;

    R =  1.f / (1.f + K_S + b * glm::coth(bSd));
    T = b * R * glm::csch(bSd);

    //    if (glm::isnan(R) || glm::isnan(T)
    //            || glm::isinf(R) || glm::isinf(T))
    //    {
    //        std::cerr << "invalid color: " << std::endl;
    //    }
}

void KMColor::composeDry(const glm::vec3 &R0, const glm::vec3 &T0, const glm::vec3 &R1, const glm::vec3 &T1, glm::vec3 &R, glm::vec3 &T)
{
    R = R1 + (T1*T1*R0) / (1.f - R1*R0);
    T = (T1*T0) / (1.f - R1*R0);
}

void KMColor::composeDry(const glm::vec3 & R0,
                              const glm::vec3 & R1, const glm::vec3 & T1,
                              glm::vec3 & R)
{
    R = R1 + (T1*T1*R0) / (1.f - R1*R0);
}

void KMColor::from_Rb_Rw(const glm::vec3 & Rb_, const glm::vec3 & Rw_, KMColor & kmColor)
{
    glm::vec3 Rb = glm::clamp(Rb_, std::numeric_limits<float>::epsilon(), 1.f - std::numeric_limits<float>::epsilon());
    glm::vec3 Rw = glm::clamp(Rw_, std::numeric_limits<float>::epsilon(), 1.f - std::numeric_limits<float>::epsilon());

    if (Rb.x >= Rw.x && Rb.y >= Rw.y && Rb.y >= Rw.y) std::cerr << "invalid Rb Rw color pair" << std::endl;

    glm::vec3 a = 0.5f * (Rw + (Rb - Rw + 1.f) / Rb);
    glm::vec3 b = glm::sqrt(a*a - 1.f);

    glm::vec3 arg = (b*b - (a - Rw) * (a - 1.f)) / (b * (1.f - Rw));

    glm::vec3 acoth_r = glm::acoth(arg);

    kmColor.S = (1.f / b) * acoth_r;
    kmColor.K = kmColor.S * (a - 1.f);
}

KMColor KMColor::from_Rb_Rw(const glm::vec3 & Rb_, const glm::vec3 & Rw_)
{
    KMColor kmColor;

    from_Rb_Rw(Rb_, Rw_, kmColor);

    return kmColor;
}


Paint::Paint(const linde::KMColor &color, float thickness) :
    m_color(color),
    m_thickness(thickness)
{
    computeReflectanceAndTransmittance(R, T);
}


Paint::Paint() :
    m_color(),
    m_thickness(1.f)
{}


Paint::~Paint()
{

}

const KMColor & Paint::getColor() const
{
    return m_color;
}

void Paint::setColor(const KMColor &color)
{
    m_color = color;
    computeReflectanceAndTransmittance(R, T);
}

void Paint::computeReflectanceAndTransmittance(glm::vec3 &R, glm::vec3 &T) const
{
    m_color.computeRT(R, T, m_thickness);
}


float Paint::getThickness() const
{
    return m_thickness;
}

void Paint::setThickness(float thickness)
{
    m_thickness = thickness;
    computeReflectanceAndTransmittance(R, T);
}

void Paint::composeDry(const glm::vec3 &R0, glm::vec3 & Rnew) const
{
    KMColor::composeDry(R0, R, T, Rnew);
}

} // namespace linde
