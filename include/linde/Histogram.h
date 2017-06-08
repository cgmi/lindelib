#ifndef LINDE_HISTOGRAM_H
#define LINDE_HISTOGRAM_H

#include "linde.h"

namespace linde
{
/**
    *@author by Thomas Lindemeier
    * University of Konstanz -
    * Department of Computer Graphics and Media Design
    *
    * @date 06.01.2016
    *
    */
template <class T>
class Histogram
{
    T					m_minValue;
    T					m_maxValue;

    std::vector<T>      m_histData;
    uint				m_bins;

    bool				m_isCumulative;
    bool				m_isNormalized;

public:
    Histogram();
    ~Histogram();

    // elements > 0 in mask get counted
    void create(const cv::Mat_<T> & data, const T &minValue, const T &maxValue, const cv::Mat_<uchar> & mask = cv::Mat_<uchar>());
    void setBins(uint bins);

    //
    Histogram<T> cumulative();
    Histogram<T> normalized(T low, T high);

    void matchTo(cv::Mat_<T> & image, const cv::Mat_<uchar> & mask = cv::Mat_<uchar>());

    // list of most occuring values, descending
    void sortedValues(std::vector<T> & values) const;

    // drawing
    void draw(cv::Mat_<glm::vec4> &mat, glm::vec4 color);

    // statistics
    T getMean() const;
    T getStandardDevation() const;
    T getVariance() const;
    T getMaxBinValue() const;
    uint getNumberOfSamples() const;

    // comparison
    double compareHist(const Histogram<T> & other, int method = CV_COMP_CORREL);

    // access
    T get(uint bin) const;
    T get(T value) const;

    static void equalize(cv::Mat_<T> & image, const T &min, const T &max, uint bins = 1000);

    uint getNrBins() const;

private:
    inline uint map(T value) const;
    inline T remap(uint index) const;

};


template <class T>
uint Histogram<T>::getNrBins() const
{
    return m_bins;
}

template <class T>
Histogram<T>::Histogram() :
    m_minValue(T()),
    m_maxValue(T()),
    m_bins(100),
    m_isCumulative(false),
    m_isNormalized(false)
{
}

template <class T>
Histogram<T>::~Histogram()
{
}


// comparison
// CV_COMP_CORREL Correlation
// CV_COMP_CHISQR Chi - Square
// CV_COMP_INTERSECT Intersection
// CV_COMP_BHATTACHARYYA Bhattacharyya distance
// CV_COMP_HELLINGER Synonym for CV_COMP_BHATTACHARYYA
template <class T>
double Histogram<T>::compareHist(const Histogram<T> & other, int method)
{
    return cv::compareHist(m_histData, other.m_histData, method);
}

// min and max value of a sample to create the bins
template <class T>
void Histogram<T>::create(const cv::Mat_<T> & data, const T &minValue, const T &maxValue, const cv::Mat_<uchar> & mask)
{
    m_isCumulative = false;
    m_isNormalized = false;

    // check the size of the container
    if (m_histData.size() != m_bins) m_histData.resize(m_bins);

    // set all bins to 0
    std::fill(m_histData.begin(), m_histData.end(), T(0));

    // find min max for the histogram borders
    if (minValue == maxValue)
    {
        m_minValue = data(0);
        m_maxValue = data(0);
        for (uint i = 0; i < data.total(); i++)
        {
            if (!mask.empty() && !mask(i)) continue;

            m_minValue = glm::min<float>(m_minValue, data(i));
            m_maxValue = glm::max<float>(m_maxValue, data(i));
        }
    }
    else
    {
        m_minValue = minValue;
        m_maxValue = maxValue;
    }

    // compute histogram
    for (uint i = 0; i < data.total(); i++)
    {
        if (!mask.empty() && !mask(i)) continue;

        if (std::isnan(data(i))) continue;

        uint index = map(data(i));
        if (index < m_bins && index >= 0)
            m_histData[index]++;
    }

}

template <class T>
T Histogram<T>::getMean() const
{
    T mean = 0;
    uint samples = 0;
    for (uint j = 0; j < m_bins; j++)
    {
        mean += m_histData[j] * remap(j);
        samples += m_histData[j];
    }
    return mean / (T)samples;
}

template <class T>
T Histogram<T>::getStandardDevation() const
{
    return sqrt(getVariance());
}

template <class T>
T Histogram<T>::getVariance() const
{
    T mean = getMean();
    T variance = 0;
    uint samples = 0;
    for (uint j = 0; j < m_bins; j++)
    {
        variance += m_histData[j] * sqr(remap(j) - mean);
        samples += m_histData[j];
    }
    return variance / samples;
}

template <class T>
T Histogram<T>::getMaxBinValue() const
{
    T	maxV = m_histData[0];
    for (const T & v : m_histData)
    {
        maxV = glm::max(v, maxV);
    }
    return maxV;
}

template <class T>
uint Histogram<T>::getNumberOfSamples() const
{
    return std::accumulate(m_histData.begin(), m_histData.end(), T(0));
}


//The cumulative histogram represents the cumulative intensity distribution of the pixels of an image,
//i.e. the number of pixels having at least a given luminous intensity.
template <class T>
Histogram<T> Histogram<T>::cumulative()
{
    Histogram<T> h = *this;
    for (uint j = 1; j < m_bins; j++)
    {
        h.m_histData[j] = h.m_histData[j - 1] + h.m_histData[j];
    }
    h.m_isCumulative = true;
    h.m_isNormalized = false;

    return h;
}

// normalizing always to 1
template <class T>
Histogram<T> Histogram<T>::normalized(T low, T high)
{
    Histogram<T> h = *this;

    T	minV = h.m_histData[0];
    T	maxV = minV;
    // search maximum and minimum of each channel
    for (const T & v : h.m_histData)
    {
        minV = glm::min(v, minV);
        maxV = glm::max(v, maxV);
    }

    for (T & v : h.m_histData)
    {
        v = ((v - minV) / (maxV - minV) * (high - low) + low);
    }
    h.m_isNormalized = true;

    return h;
}

template <class T>
void Histogram<T>::matchTo(cv::Mat_<T> & image, const cv::Mat_<uchar> & mask)
{
    if (!m_isNormalized)
    {
        Histogram<T> norm = this->normalized(m_minValue, m_maxValue);
        for (uint i = 0; i < image.total(); i++)
        {
            if (!mask.empty() && !mask(i)) continue;

            image(i) = norm.m_histData[map(image(i))];
        }
    } else
    {

        for (uint i = 0; i < image.total(); i++)
        {
            if (!mask.empty() && !mask(i)) continue;
            image(i) = m_histData[map(image(i))];
        }
    }

}

template <class T>
void Histogram<T>::sortedValues(std::vector<T> &values) const
{
    std::vector<std::pair<T, T> > elements(m_bins);
    for (uint i = 0; i < m_bins; i++)
    {
        elements[i].first = remap(i);
        elements[i].second = get(i);
    }
    std::sort(elements.begin(), elements.end(), [&](const std::pair<T, T> & e0, const std::pair<T, T> & e1)
    {
        return e0.second > e1.second;
    });

    values.resize(m_bins);
    for (uint i = 0; i < m_bins; i++)
    {
        values[i] = elements[i].first;
    }
}

template <class T>
void Histogram<T>::equalize(cv::Mat_<T> & image, const T &min, const T &max, uint bins)
{
    Histogram<T> hist;
    hist.setBins(bins);
    hist.create(image, min. max);

    hist.cumulative().matchTo(image);
}

template <class T>
void Histogram<T>::setBins(uint bins)
{
    m_bins = bins;
}

template <class T>
T Histogram<T>::get(uint bin) const
{
    return m_histData[bin];
}

template <class T>
T Histogram<T>::get(T value) const
{
    return get(map(value));
}

template <class T>
uint Histogram<T>::map(T value) const
{
    return ((value - m_minValue) / (m_maxValue - m_minValue)) * (m_bins - 1);
}

template <class T>
T Histogram<T>::remap(uint index) const
{
    return (static_cast<T>(index) / ((m_bins - 1))) * m_maxValue;
}

template <class T>
void Histogram<T>::draw(cv::Mat_<glm::vec4> &mat, glm::vec4 color)
{
    Histogram<T> normHisto = this->normalized(0.f, 1.0f);

    // draw rectangle around plot
    cv::rectangle(mat, cv::Point(0, 0), cv::Point(mat.cols - 1, mat.rows - 1), cv::Scalar(color.x, color.y, color.z, color.w));

    int dx = (int) round(mat.cols / (float)m_bins);
    cv::Point p1;
    cv::Point p2;
    uint nrBins = normHisto.getNrBins();

    for (uint i = 0; i < nrBins - 1; ++i)
    {
        p1 = cv::Point(i * dx, mat.rows - normHisto.get(i) * (mat.rows - 1));
        p2 = cv::Point((i + 1) * dx, mat.rows - normHisto.get(i + 1) * (mat.rows - 1));
        cv::line(mat, p1, p2, cv::Scalar(0, 0, 0, color.w));
    }
    // draw last segment
    p1 = cv::Point(mat.cols - 1, mat.rows - normHisto.get(nrBins - 1) * (mat.rows - 1));
    cv::line(mat, p2, p1, cv::Scalar(color.x, color.y, color.z, color.w));
}

/**
        3D Histogram
 */
class Histogram3D
{
    std::vector<float>									m_minValue;
    std::vector<float>									m_maxValue;

    std::vector<std::vector<std::vector<float> > >      m_histData;
    uint												m_bins;

    bool												m_isNormalized;

public:
    Histogram3D();
    ~Histogram3D();

    void reset();
    void add(glm::vec3 v);


    // elements > 0 in mask get counted
    void create(const cv::Mat_<glm::vec3> & data, const std::vector<float> &minValue, const std::vector<float> &maxValue, const cv::Mat_<uchar> & mask = cv::Mat_<uchar>());
    void setBins(uint bins);

    //
    Histogram3D normalized(float low, float high);

    // statistics
    float getMaxBinValue() const;
    float getNumberOfSamples() const;
    uint getNumberOfDiscreteClasses() const;

    // access
    float get(std::vector<uint> bin) const;
    float get(glm::vec3 value) const;

    uint getNrBins() const;

private:
    inline std::vector<uint> map(glm::vec3 value) const;
    inline glm::vec3 remap(std::vector<uint> index) const;

};

} // namespace linde

#endif // LINDE_HISTOGRAM_H
