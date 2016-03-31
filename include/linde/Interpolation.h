#ifndef SPLINE_H
#define SPLINE_H

#include <vector>
#include "linde.h"


#include <opencv2/imgproc/imgproc.hpp>

/**
* @author Thomas Lindemeier
*
* University of Konstanz-
* Department for Computergraphics
*/
namespace linde
{


//###############################################################################
//###############################################################################
//####################### Spline ################################################
//###############################################################################
//###############################################################################
//###############################################################################
const float BEZIER_MATRIX[4][4] = {
    {-1.0f  ,  3.0f  , -3.0f  , 1.0f  },
    { 3.0f  , -6.0f  ,  3.0f  , 0.0f  },
    {-3.0f  ,  3.0f  ,  0.0f  , 0.0f  },
    { 1.0f  ,  0.0f ,  0.0f  , 0.0f  }
};
const float BSPLINE_MATRIX[4][4] = {
    {-1.0f/6.0f ,  3.0f/6.0f , -3.0f/6.0f , 1.0f/6.0f },
    { 3.0f/6.0f , -6.0f/6.0f ,  3.0f/6.0f , 0.0f   },
    {-3.0f/6.0f ,  0.0f   ,  3.0f/6.0f , 0.0f   },
    { 1.0f/6.0f ,  4.0f/6.0f ,  1.0f/6.0f , 0.0f   }
};

template <class T>
T linearInterpolation(const T &p0, const T &p1, float t)
{
    return p0 * t + (p1 * (1-t));
}

// t=0 -> p0; t=1 -> p1
template <class T>
T catmullRom(const T & p_1, const T & p0, const T & p1, const T & p2, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    float b1 = 0.5f * (  -t3 + 2*t2 - t);
    float b2 = 0.5f * ( 3*t3 - 5*t2 + 2);
    float b3 = 0.5f * (-3*t3 + 4*t2 + t);
    float b4 = 0.5f * (   t3 -   t2    );

    return (p_1*b1 + p0*b2 + p1*b3 + p2*b4);
}

template <class T>
T cubic(const T &p_1, const T &p0, const T &p1, const T &p2, float t)
{
    float t2;
    T a0, a1, a2, a3;

    t2 = t*t;
    a0 = p2 - p1 - p_1 + p0;
    a1 = p_1 - p0 - a0;
    a2 = p1 - p_1;
    a3 = p0;

    return (a0*t*t2 + a1*t2+a2*t + a3);
}

template <class T>
T bSpline(const T &p_1, const T &p0, const T &p1, const T &p2, float t)
{
    T a, b, c, d;

    a += BSPLINE_MATRIX[0][0] * p_1;
    b += BSPLINE_MATRIX[1][0] * p_1;
    c += BSPLINE_MATRIX[2][0] * p_1;
    d += BSPLINE_MATRIX[3][0] * p_1;

    a += BSPLINE_MATRIX[0][1] * p0;
    b += BSPLINE_MATRIX[1][1] * p0;
    c += BSPLINE_MATRIX[2][1] * p0;
    d += BSPLINE_MATRIX[3][1] * p0;

    a += BSPLINE_MATRIX[0][2] * p1;
    b += BSPLINE_MATRIX[1][2] * p1;
    c += BSPLINE_MATRIX[2][2] * p1;
    d += BSPLINE_MATRIX[3][2] * p1;

    a += BSPLINE_MATRIX[0][3] * p2;
    b += BSPLINE_MATRIX[1][3] * p2;
    c += BSPLINE_MATRIX[2][3] * p2;
    d += BSPLINE_MATRIX[3][3] * p2;

    return t * (t * (t * a + b) + c) + d;
}

template <class T>
T hermite(const T &p0, const T &p1, const T &p2, const T &p3,
          float t, float tension = 0.0f, float bias = 0.0f)
{
    T m0, m1;
    float t2,t3;
    float a0, a1, a2, a3;

    t2 = t * t;
    t3 = t2 * t;

    m0  = (p1-p0)*(1.f+bias)*(1.f-tension)/2.f;
    m0 += (p2-p1)*(1.f-bias)*(1.f-tension)/2.f;
    m1  = (p2-p1)*(1.f+bias)*(1.f-tension)/2.f;
    m1 += (p3-p2)*(1.f-bias)*(1.f-tension)/2.f;

    a0 =  2.f*t3 - 3.f*t2 + 1.f;
    a1 =    t3 - 2.f*t2 + t;
    a2 =    t3 -   t2;
    a3 = -2.f*t3 + 3.f*t2;

    return (a0*p1 + a1*m0 + a2*m1 + a3*p2);
}

template <class T>
T piecewiseHermite(const T &a, const T &b, const T &startTangent,
                   const T &endTangent, float t)
{
    float t2, t3;
    float a0, a1, b0, b1;

    t2 = t*t;
    t3 = t2*t;

    a0 = (t3 * 2.0f) - (3.0f * t2) + 1.0f;
    a1 = (-2.0f * t3) + (3.0f * t2);
    b0 = t3 - (2.0f * t2) + t;
    b1 = t3 -   t2;

    return (a0*a + a1*b + b0*startTangent + b1*endTangent);
}

/**
        * Spline interpolation.
        * @author Thomas Lindemeier
        * @date 02.05.2013
        */
template <class T>
class Spline
{
    std::vector<T> m_control;

    float m_deltaT;
    T m_pStart;
    T m_pEnd;

public:

    enum SplineType
    {
        CATMULL_ROM,
        CUBIC,
        HERMITE,
        BSPLINE
    };

    Spline(){}
    virtual ~Spline(){}

    void add(const T & v)
    {
        m_control.push_back(v);
        m_deltaT = (float)1 / ((float)m_control.size()-1);

        if(m_control.size() >= 3)
        {
            T sStart = m_control[0];
            T tStart = m_control[1];

            T nStart = sStart - tStart;
            float lenStart = glm::length(nStart);
            m_pStart = sStart +  glm::normalize(nStart) * lenStart * 0.5f;

            unsigned int size = m_control.size() - 1;
            T sEnd = m_control[size];
            T tEnd = m_control[size - 1];

            T nEnd = sEnd - tEnd;
            float lenEnd = glm::length(nEnd);
            m_pEnd = sEnd +  glm::normalize(nEnd) * lenEnd * 0.5f;
        }
    }

    T interpolate(float t, SplineType conf)
    {
        int p = (int)(t / m_deltaT);

        int p0 = p - 1;
        int p1 = p;
        int p2 = p + 1;
        int p3 = p + 2;

        T c1;
        if(p0 < 0)
            c1 = m_pStart;
        else
            c1 = m_control[p0];

        T v1 = m_control[p1];
        T v2;
        if((size_t)p2 >= m_control.size())
            v2 = m_control[m_control.size()-1];
        else
            v2 = m_control[p2];

        T c2;
        if((size_t)p3 >= m_control.size()-1)
            c2 = m_pEnd;
        else
            c2 = m_control[p3];

        float lt = (t - m_deltaT * (float)p) / m_deltaT;

        if(conf == CATMULL_ROM)
            return catmullRom(c1, v1, v2, c2, lt);
        if(conf == BSPLINE)
            return bSpline(c1, v1, v2, c2, lt);
        if(conf == CUBIC)
            return cubic(c1, v1, v2, c2, lt);
        if(conf == HERMITE)
            return hermite(c1, v1, v2, c2, lt);

        return linearInterpolation(v1, v2, lt);
    }


};

//############################################################################
//############################################################################
//############################################################################
//############################################################################
//############################################################################
//############################################################################
//############################################################################
//############################################################################
//############################################################################
//############################################################################
//############################################################################


/**
* Thin plate spline interpolation
* @author Thomas Lindemeier
* @date 01.12.2013
*/
template <class T>
class ThinPlateSpline
{
    cv::Mat_<glm::vec2>	m_points;
    cv::Mat_<T>		m_values;
    cv::Mat_<T>		m_coefficients;

public:
    ThinPlateSpline() :
        m_points(),
        m_values(),
        m_coefficients()
    {}

    virtual ~ThinPlateSpline(){}

    // input only as 1D col vector
    void buildCoefficients(cv::Mat_<glm::vec2> & points, cv::Mat_<T> & values)
    {
        m_points = points;
        m_values = values;

        myassert(m_points.rows == m_values.rows && m_values.rows >= 3);

        int n = m_points.rows;

        // matrix of distances, is symmetric
        // just compute half of the coeffs
        cv::Mat_<T> R(n, n);
        for (int i = 0; i < n; i++)
        {
            R(i,i) = 0.0f;
            for (int j = i+1; j < n; j++)
            {
                // distance to sample points
                T r = basisTPS(m_points(i, 0), m_points(j, 0));

                R(i,j) = R(j,i) = r;
            }
        }

        //
        std::vector<cv::Mat> R_;
        std::vector<cv::Mat> V_;
        cv::split(R, R_);
        cv::split(m_values, V_);
        std::vector<cv::Mat> C_(R_.size());
        for (int c = 0; c < R_.size(); c++)
        {
            cv::solve(R_[c], V_[c], C_[c], cv::DECOMP_SVD);
        }
        cv::merge(C_, m_coefficients);
    }

    T interpolate(const glm::vec2 & p)
    {
        T s = 0;
        for (int i = 0; i < m_coefficients.rows; ++i)
        {
            s += m_coefficients(i, 0) * basisTPS(p, m_points(i, 0));
        }
        return s;
    }


private:
    // thin plate spline
    T basisTPS(const glm::vec2 & p1, const glm::vec2 & p2)
    {
        T r = glm::length(p1-p2);
        if (r == T(0))
        {
            return T(0);
        } else
        {
            r = sqrt(r);
            return r * r * log(r);
        }
    }
};


} // namespace my

#endif // SPLINE_H


