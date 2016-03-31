#include "linde/Histogram.h"

namespace linde
{
Histogram3D::Histogram3D() :
	m_bins(100),
	m_isNormalized(false)
{
	m_minValue = std::vector<float>(3, 0.f);
	m_maxValue = std::vector<float>(3, 1.f);
	m_histData = std::vector<std::vector<std::vector<float> > >(m_bins, std::vector<std::vector<float>>(m_bins, std::vector<float>(m_bins, 0.f)));
}

Histogram3D::~Histogram3D()
{
}

void Histogram3D::reset()
{
	m_isNormalized = false;
	m_histData.clear();
	m_histData = std::vector<std::vector<std::vector<float> > >(m_bins, std::vector<std::vector<float>>(m_bins, std::vector<float>(m_bins, 0.f)));
}

void Histogram3D::add(glm::vec3 v)
{
	std::vector<uint> i = map(v);
	++m_histData[i[0]][i[1]][i[2]]; 
}

// min and max value of a sample to create the bins
void Histogram3D::create(const cv::Mat_<glm::vec3> & data, const std::vector<float> &minValue, const std::vector<float> &maxValue, const cv::Mat_<uchar> & mask)
{
	m_isNormalized = false;

	m_histData.clear();

	// resize the container	and set all bins to 0
	m_histData =  std::vector<std::vector<std::vector<float> > >(m_bins, std::vector<std::vector<float>>(m_bins, std::vector<float>(m_bins, 0.f)));	

	// find min max for the histogram borders
	if (minValue[0] == maxValue[0]) // 1st dim
	{
		m_minValue[0] = 0.f;
		m_maxValue[0] = 0.f;
		for (uint i = 0; i < data.total(); i++)
		{
			if (!mask.empty() && !mask(i)) continue;

			m_minValue[0] = glm::min<float>(m_minValue[0], data(i).x);
			m_maxValue[0] = glm::max<float>(m_maxValue[0], data(i).x);
		}
	}
	else
	{
		m_minValue[0] = minValue[0];
		m_maxValue[0] = maxValue[0];
	}
	if (minValue[1] == maxValue[1]) // 2nd dim
	{
		m_minValue[1] = 0.f;
		m_maxValue[1] = 0.f;
		for (uint i = 0; i < data.total(); i++)
		{
			if (!mask.empty() && !mask(i)) continue;

			m_minValue[1] = glm::min<float>(m_minValue[1], data(i).x);
			m_maxValue[1] = glm::max<float>(m_maxValue[1], data(i).x);
		}
	}
	else
	{
		m_minValue[1] = minValue[1];
		m_maxValue[1] = maxValue[1];
	}
	if (minValue[2] == maxValue[2]) // 3rd dim
	{
		m_minValue[2] = 0.f;
		m_maxValue[2] = 0.f;
		for (uint i = 0; i < data.total(); i++)
		{
			if (!mask.empty() && !mask(i)) continue;

			m_minValue[2] = glm::min<float>(m_minValue[2], data(i).x);
			m_maxValue[2] = glm::max<float>(m_maxValue[2], data(i).x);
		}
	}
	else
	{
		m_minValue[2] = minValue[2];
		m_maxValue[2] = maxValue[2];
	}

	// compute histogram
	for (uint i = 0; i < data.total(); i++)
	{
		if (!mask.empty() && !mask(i)) continue;

		std::vector<uint> index = map(data(i));
		if (index[0] < m_bins && index[0] >= 0 && index[1] < m_bins && index[1] >= 0 && index[2] < m_bins && index[2] >= 0)
			++m_histData[index[0]][index[1]][index[2]];
	}

}

std::vector<uint> Histogram3D::map(glm::vec3 value) const
{
	std::vector<uint> mapped(3);
	mapped[0] = ((value.x - m_minValue[0]) / (m_maxValue[0] - m_minValue[0])) * (m_bins - 1);
	mapped[1] = ((value.y - m_minValue[1]) / (m_maxValue[1] - m_minValue[1])) * (m_bins - 1);
	mapped[2] = ((value.z - m_minValue[2]) / (m_maxValue[2] - m_minValue[2])) * (m_bins - 1);
	return mapped;
}

void Histogram3D::setBins(uint bins)
{
	if (m_bins == bins)
		return;

	m_bins = bins;
	m_histData = std::vector<std::vector<std::vector<float> > >(m_bins, std::vector<std::vector<float>>(m_bins, std::vector<float>(m_bins, 0.f)));
}

float Histogram3D::getMaxBinValue() const
{
	float maxV = m_histData[0][0][0];
    for (uint x1 = 0; x1 < m_bins; ++x1)
	{
        for (uint x2 = 0; x2 < m_bins; ++x2)
		{
            for (uint x3 = 0; x3 < m_bins; ++x3)
			{
				maxV = glm::max(m_histData[x1][x2][x3], maxV);
			}
		}
	}
	return maxV;
}

float Histogram3D::getNumberOfSamples() const
{
	float sum = 0.f;
    for (uint x1 = 0; x1 < m_bins; ++x1)
	{
        for (uint x2 = 0; x2 < m_bins; ++x2)
		{
            for (uint x3 = 0; x3 < m_bins; ++x3)
			{
				sum += m_histData[x1][x2][x3];
			}
		}
	}
	return sum;
}

uint Histogram3D::getNumberOfDiscreteClasses() const
{
	uint count = 0;
    for (uint x1 = 0; x1 < m_bins; ++x1)
	{
        for (uint x2 = 0; x2 < m_bins; ++x2)
		{
            for (uint x3 = 0; x3 < m_bins; ++x3)
			{
				if (m_histData[x1][x2][x3] > 0.0f)
					++count;
			}
		}
	}
	return count;
}

uint Histogram3D::getNrBins() const
{
	return m_bins;
}

float Histogram3D::get(std::vector<uint> bin) const
{
	return m_histData[bin[0]][bin[1]][bin[2]];
}

float Histogram3D::get(glm::vec3 value) const
{
	return get(map(value));
}

glm::vec3 Histogram3D::remap(std::vector<uint> index) const
{
	glm::vec3 v;
	v.x = ((float)index[0] / ((m_bins - 1))) * m_maxValue[0];
	v.y = ((float)index[1] / ((m_bins - 1))) * m_maxValue[1];
	v.z = ((float)index[2] / ((m_bins - 1))) * m_maxValue[2];
	return v;
}

Histogram3D Histogram3D::normalized(float low, float high)
{
	Histogram3D h = *this;

	float minV = h.m_histData[0][0][0];
	float maxV = minV;

	// search min/max
    for (uint x1 = 0; x1 < m_bins; ++x1)
	{
        for (uint x2 = 0; x2 < m_bins; ++x2)
		{
            for (uint x3 = 0; x3 < m_bins; ++x3)
			{
				float v = h.m_histData[x1][x2][x3];
				minV = glm::min(v, minV);
				maxV = glm::max(v, maxV);
			}
		}
	}

    for (uint x1 = 0; x1 < m_bins; ++x1)
	{
        for (uint x2 = 0; x2 < m_bins; ++x2)
		{
            for (uint x3 = 0; x3 < m_bins; ++x3)
			{
				h.m_histData[x1][x2][x3] = ((h.m_histData[x1][x2][x3] - minV) / (maxV - minV) * (high - low) + low);
			}
		}
	}
	h.m_isNormalized = true;

	return h;
}

} // namespace linde
