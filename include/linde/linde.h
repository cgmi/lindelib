#ifndef LINDE_H
#define LINDE_H



/**
* Global includes, typedefs and snippets
*
*   @author by Thomas Lindemeier
* University of Konstanz -
* Department of Computer Graphics and Media Design
*
* @date 24.07.2013
*
*/


// includes
#include <memory>
#include <iostream>
#include <random>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <glm/glm.hpp>

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#elif defined(__linux__) 
#define OS_LINUX
#endif

// use Microsoft's parallel patterns library if availabe
#if (_MSC_VER >= 1600)
#include <ppl.h>
#else
#include <thread>
#include <future>
#endif


// remove debug messages if _DEBUG is defined
// Visual studio defines that itself on debug conf
#ifdef DEBUG_BUILD
#define myDebug std::cout
#else
#define myDebug 0 && std::cout
#endif



// nice trick for static asserts at compile time
#define STATIC_ASSERT( x ) typedef char __STATIC_ASSERT__[( x )?1:-1]

// runtime assert just evaluated if debug conf
class debug
{
public:
    static void debug_assert(bool passed, const char* assert, const char* file, long line)
    {
        if (passed == false)
        {
            std::cerr << "failed assert " << assert << " in " << file << " at " << line << std::endl;
            std::cin.get();
            exit(EXIT_FAILURE);
        }
    }

#ifdef DEBUG_BUILD
#define myassert(x) debug::debug_assert(x, #x , __FILE__, __LINE__ )
#else
#define myassert(x) 0
#endif
};

// data types
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;


namespace linde
{



/*######################################################################
        *################## math  ##############################################
        *#######################################################################
        */
// constants
template <class T>
inline T PI() { return T(4) * atan(T(1)); }

template <class T>
inline T TWO_PI() { return T(2)*PI<T>(); }

template <class T>
inline T HALF_PI() { return PI<T>() / T(2); }

template <class T>
inline T RADIANS(const T & degrees)
{
    return degrees * (PI<T>() / T(180));
}

template <class T>
inline T DEGREES(const T & radians)
{
    return radians * (T(180) / PI<T>());
}

template <class T>
inline T sqr(const T & a)
{
    return a*a;
}



// maxV is maximum value v could have
// minV is minimum value v could have
// low is what the new minimum should be
// high is what the new maximum should be
template <class T>
inline T mapRange(const T & v, const T & minV, const T & maxV, const T & low, const T & high)
{
    return ((v - minV) / (maxV - minV)) * (high - low) + low;
}



/*######################################################################
    *################## mat access interpolation (linear)  #################
    *#######################################################################
    */
template<class T>
T interpolated(const cv::Mat_<T> & mat, const glm::vec2 & vec)
{
    int m = cv::borderInterpolate((int)vec[1], mat.rows, cv::BORDER_REFLECT);
    int n = cv::borderInterpolate((int)vec[0], mat.cols, cv::BORDER_REFLECT);
    float mf = vec[1] - m;
    float nf = vec[0] - n;

    int m1 = cv::borderInterpolate(m + 1, mat.rows, cv::BORDER_REFLECT);
    int n1 = cv::borderInterpolate(n + 1, mat.cols, cv::BORDER_REFLECT);

    return (1.f - nf)*(1.f - mf)*mat[m][n] + nf*(1.f - mf)*mat[m][n1]
            + (1.f - nf)*mf*mat[m1][n]
            + nf*mf*mat[m1][n1];
}

struct Transform
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};


class Random
{
public:
    Random(double low, double high)
    {
        rd = new std::random_device;
        gen = new std::mt19937((*rd)());
        dis = new std::uniform_real_distribution<double>(low, high);
    }
    ~Random()
    {
        delete dis;
        delete gen;
        delete rd;
    }

    double next() const { return (*dis)(*gen); }
    double operator()() const { return next(); }
private:
    std::random_device	*					rd;
    std::mt19937		*					gen;
    std::uniform_real_distribution<double>*	dis;
};

// parallel for
// only used if parallel pattern lib is enabled (only MSVC >= 2010)
template <class Iterator, class Function>
inline void parallel_for(Iterator start, Iterator end, Function func)
{
#ifdef _PPL_H
    concurrency::parallel_for(start, end, func);
#else
    Iterator len = end - start;

    // create the pool size
    const size_t nr_threads = (size_t)std::thread::hardware_concurrency();
    const size_t pool = std::max<size_t>(1, len / nr_threads);

    std::vector<std::future<void> > futures;
    futures.reserve(nr_threads);
    for (; start < end; start += pool)
    {
        std::future<void> future = std::async(std::launch::async,
                                              [=, &func]()
        {
            for (Iterator i = start; i < std::min<Iterator>(start + pool, end); i++)
            {
                func(i);
            }
        });
        futures.push_back(std::move(future));
    }

    // run like hell and wait till all is executed
    std::for_each(futures.begin(), futures.end(),
                  [](std::future<void> & f)
    {
        f.wait();
    });
#endif
}

} //namespace linde

// glm vec stream operators
namespace glm
{
// glm vec stream operators
template <typename T, glm::precision P>
std::ostream & operator<< (std::ostream & output, const glm::tvec2<T, P> & v)
{
    output << "(";
    output << v[0] << ", ";
    output << v[1] << ")";

    return output;
}

template <typename T, glm::precision P>
std::istream & operator>> (std::istream & input, glm::tvec2<T, P> & a)
{
    std::string v;
    std::getline(input, v);
    int k = 1;
    for (int i = 0; i < 2; ++i)
    {
        std::string w;
        while (v[k] != ',' && v[k] != ')')
        {
            w.push_back(v[k]);
            k++;
        }
        k++;
        a[i] = std::stof(w);
        if (k >= v.size()) return input;
    }

    return input;
}


template <typename T, glm::precision P>
std::ostream & operator<< (std::ostream & output, const glm::tvec3<T, P> & v)
{
    output << "(";
    for (int i = 0; i < 2; ++i)
    {
        output << v[i] << ", ";
    }
    output << v[2] << ")";

    return output;
}

template <typename T, glm::precision P>
std::istream & operator>> (std::istream & input, glm::tvec3<T, P> & a)
{
    std::string v;
    std::getline(input, v);
    int k = 1;
    for (int i = 0; i < 3; ++i)
    {
        std::string w;
        while (v[k] != ',' && v[k] != ')')
        {
            w.push_back(v[k]);
            k++;
        }
        k++;
        a[i] = std::stof(w);
        if (k >= v.size()) return input;
    }

    return input;
}

// glm vec stream operators
template <typename T, glm::precision P>
std::ostream & operator<< (std::ostream & output, const glm::tvec4<T, P> & v)
{
    output << "(";
    for (int i = 0; i < 3; ++i)
    {
        output << v[i] << ", ";
    }
    output << v[3] << ")";

    return output;
}

template <typename T, glm::precision P>
std::istream & operator>> (std::istream & input, glm::tvec4<T, P> & a)
{
    std::string v;
    std::getline(input, v);
    int k = 1;
    for (int i = 0; i < 4; ++i)
    {
        std::string w;
        while (v[k] != ',' && v[k] != ')')
        {
            w.push_back(v[k]);
            k++;
        }
        k++;
        a[i] = std::stof(w);
        if (k >= v.size()) return input;
    }

    return input;
}
}



// opencv access data traits for glm vector
namespace cv
{
template <typename T, glm::precision P> class DataType < glm::tvec2<T, P> >
{
public:
    typedef glm::tvec2<T, P> value_type;
    typedef glm::tvec2<T, P> work_type;
    typedef T channel_type;
    typedef value_type vec_type;
    enum {
        generic_type = 0,
        depth = DataDepth<channel_type>::value,
        channels = 2,
        fmt = ((channels - 1) << 8) + DataDepth<channel_type>::fmt,
        type = CV_MAKETYPE(depth, channels)
    };
};

template <typename T, glm::precision P> class DataType < glm::tvec3<T, P> >
{
public:
    typedef glm::tvec3<T, P> value_type;
    typedef glm::tvec3<T, P> work_type;
    typedef T channel_type;
    typedef value_type vec_type;
    enum {
        generic_type = 0,
        depth = DataDepth<channel_type>::value,
        channels = 3,
        fmt = ((channels - 1) << 8) + DataDepth<channel_type>::fmt,
        type = CV_MAKETYPE(depth, channels)
    };
};

template <typename T, glm::precision P> class DataType < glm::tvec4<T, P> >
{
public:
    typedef glm::tvec4<T, P> value_type;
    typedef glm::tvec4<T, P> work_type;
    typedef T channel_type;
    typedef value_type vec_type;
    enum {
        generic_type = 0,
        depth = DataDepth<channel_type>::value,
        channels = 4,
        fmt = ((channels - 1) << 8) + DataDepth<channel_type>::fmt,
        type = CV_MAKETYPE(depth, channels)
    };
};

} // namespace cv

#endif //LINDE_H
