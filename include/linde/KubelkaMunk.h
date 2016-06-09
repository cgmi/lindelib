#ifndef KUBELKAMUNK_H
#define KUBELKAMUNK_H

#include "linde.h"
#include "Color.h"

/**
        * @author Thomas Lindemeier
        */
namespace linde
{
//Baxter, W., Wendt, J., & Lin, M. (2004).
//IMPaSTo: a realistic, interactive model for paint. … of the 3rd International Symposium on Non-
//…, 1(212), 45–57. Retrieved from http://dl.acm.org/citation.cfm?id=987665
class KMColor
{
public:
    glm::vec3 K; // absorption
    glm::vec3 S; // scattering

public:
    KMColor();
    KMColor(const glm::vec3 & linearRgb);
    KMColor(const glm::vec3 & K, const glm::vec3 & S);

    glm::vec3 getK() const;
    glm::vec3 getS() const;

    glm::vec3 getQuotient() const;

    glm::vec3 getTransmittance( const float d) const;

    glm::vec3 getReflectance( const float d) const;

    // d represents thickness of one layer of paint
    // Baxter, W., Wendt, J., & Lin, M. C. (2004).
    // IMPaSTo: A Realistic, Interactive Model for Paint.
    // Proceedings of the Third International Symposium on Non-Photorealistic Animation and Rendering
    // (NPAR 2004), 1(212), 45–56. http://doi.org/http://doi.acm.org/10.1145/987657.987665
    // with errata from http://gamma.cs.unc.edu/IMPASTO/#links
    void computeRT(glm::vec3 & R, glm::vec3 & T, const float d) const;

    static void composeDry(const glm::vec3 & R0, const glm::vec3 & T0,
                           const glm::vec3 & R1, const glm::vec3 & T1,
                           glm::vec3 & R, glm::vec3 & T);
    static void composeDry(const glm::vec3 & R0,
                           const glm::vec3 & R1, const glm::vec3 & T1,
                           glm::vec3 & R);


    // Curtis, C. J., Anderson, S. E., Seims, J. E., Fleischer, K. W., & Salesin, D. H. (1997).
    // Computer-generated watercolor.
    // Proceedings of the 24th Annual Conference on Computer Graphics and Interactive Techniques - SIGGRAPH ’97,
    // 421–430. http://doi.org/10.1145/258734.258896
    static KMColor from_Rb_Rw(const glm::vec3 & Rb_, const glm::vec3 & Rw_);
    static void from_Rb_Rw(const glm::vec3 & Rb_, const glm::vec3 & Rw_, KMColor & kmColor);

    KMColor& operator= ( const KMColor& v)
    {
        K = v.K;
        S = v.S;
        return *this;
    }

    KMColor& operator+= ( const KMColor& v)
    {
        K += v.K;
        S += v.S;
        return *this;
    }
    KMColor& operator-= ( const KMColor& v)
    {
        K -= v.K;
        S -= v.S;
        return *this;
    }
    KMColor& operator+= ( const float& f)
    {
        K += f;
        S += f;
        return *this;
    }
    KMColor& operator-= ( const float& f)
    {
        K -= f;
        S -= f;
        return *this;
    }

    KMColor& operator*= ( const float& f)
    {
        K *= f;
        S *= f;
        return *this;
    }

    KMColor& operator/= ( const float& f)
    {
        float inv = 1.0f / f;
        return( (*this) *= inv );
    }

private:

};

inline  KMColor operator* ( const float& c, const KMColor& v)
{
    return KMColor( c*v.K, c*v.S);
}
inline  KMColor operator* ( const KMColor& v, const float& c)
{
    return KMColor( c*v.K, c*v.S);
}

inline  KMColor operator+ ( const KMColor& c, const KMColor& v)
{
    return KMColor( c.K+v.K, c.S+v.S);
}

class Paint
{
    KMColor     m_color;
    glm::vec3   R;
    glm::vec3   T;

    float       m_thickness; // thickness of paint

public:
    Paint();
    Paint(const KMColor & color, float thickness);
    Paint(const KMColor & color);
    ~Paint();

    const KMColor & getColor() const;
    void setColor(const KMColor &color);

    void computeReflectanceAndTransmittance(glm::vec3 & R, glm::vec3 & T) const;

    float   getThickness() const;
    void    setThickness(float thickness);

    void composeDry(const glm::vec3 & R0, glm::vec3 &Rnew) const;

    void mixWith(const Paint & other, const float concentration);

    Paint& operator= ( const Paint& v)
    {
        m_color = v.m_color;
        R = v.R;
        T = v.T;
        m_thickness = v.m_thickness;
        return *this;
    }

};


} // namespace linde

#endif // KUBELKAMUNK_H
