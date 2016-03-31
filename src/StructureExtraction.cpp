#include "../include/linde/StructureExtraction.h"
#include "../include/linde/Convolution.h"
#include "../include/linde/Color.h"
#include "../include/linde/File.h"
#include "../include/linde/Morphology.h"

#include <opencv2/highgui/highgui.hpp>

namespace linde
{
const float StructureExtraction::THRESHOLD_MIN = 0.f;


cv::Mat_<float> StructureExtraction::getEnergy() const
{
    return m_energy;
}

StructureExtraction::StructureExtraction() :
    m_thresholdBin(false),
    m_openingRadius(0),
    m_thresholdFine(210.f/255.f),
    m_thresholdCoarse(50.f/255.f),
    m_minScale(1.f)
{

}

StructureExtraction::~StructureExtraction()
{

}

void StructureExtraction::computeEnergy(const cv::Mat_<glm::vec3> LchImage, const std::vector<uint> &useChannels, const cv::Mat_<uchar> &mask)
{
    const cv::Size size = LchImage.size();
    const int nrPixels = size.width*size.height;

    computeGaussEnergy(LchImage, m_energy, m_minScale, useChannels);
    if (mask.data)
    {
        for (int i = 0; i < nrPixels; i++)
        {
            if (!mask(i))
            {
                m_energy(i) = 0.f;
            }
        }
    }
}

void StructureExtraction::setThresholdBinary(bool thresholdBin)
{
    m_thresholdBin = thresholdBin;
}

void StructureExtraction::setOpeningRadius(int openingRadius)
{
    m_openingRadius = openingRadius;
}

void StructureExtraction::computeGaussEnergy(const cv::Mat_<glm::vec3> & image, cv::Mat_<float> &energy, const float minScale, const std::vector<uint> & useChannels) const
{
    const int nrOrientations = 6;

    std::vector<cv::Mat_<float> > gaussEnergies;
    std::vector<cv::Mat_<float> > channels;
    cv::split(image, channels);
    for (const uint channel : useChannels)
    {
        gaussEnergies.push_back(cv::Mat_<float>());
        for (uint i = 0; i < gaussEnergies.size(); i++)
        {
            linde::ComputeGaussDerivativeEnergy(channels[channel], gaussEnergies.back(), minScale, nrOrientations);
        }
    }

    energy.create(image.size());
    energy.setTo(0.f);
    for (uint i = 0; i < gaussEnergies.size(); i++)
    {
        energy += gaussEnergies[i];
    }
    energy *= 1.f/gaussEnergies.size();
}

void StructureExtraction::extractLayers(std::vector<cv::Mat_<float> > & layers, const cv::Mat_<uchar> & mask) const
{
    const cv::Size size = m_energy.size();
    const int nrPixels = size.width*size.height;


    // layer coarse:
    cv::Mat_<float> coarseLayer(m_energy.size());
    coarseLayer.setTo(0.f);
    for (int l = 0; l < nrPixels; l++)
    {
        if (mask.data && !mask(l))
        {
            coarseLayer(l) = 0.f;
            continue;
        }
        if (m_energy(l) < m_thresholdCoarse)
        {
            const float v = (m_thresholdBin) ? 1.f : m_thresholdCoarse - m_energy(l);
            coarseLayer(l) = v;
        } else
        {
            coarseLayer(l) = THRESHOLD_MIN;
        }
    }
    layers.push_back(coarseLayer);

    // layer medium:
    cv::Mat_<float> mediumLayer(m_energy.size());
    mediumLayer.setTo(0.f);
    for (int l = 0; l < nrPixels; l++)
    {
        if (mask.data && !mask(l))
        {
            mediumLayer(l) = 0.f;
            continue;
        }
        if (m_energy(l) < m_thresholdFine && m_energy(l) >= m_thresholdCoarse)
        {
            const float v = (m_thresholdBin) ? 1.f : mapRange(m_energy(l), m_thresholdCoarse, m_thresholdFine, m_thresholdFine, 0.f);
            mediumLayer(l) = v;
        } else
        {
            mediumLayer(l) = THRESHOLD_MIN;
        }
    }
    layers.push_back(mediumLayer);

    // layer fine:
    float maxFine = 0.f;
    cv::Mat_<float> fineLayer(m_energy.size());
    fineLayer.setTo(0.f);
    for (int l = 0; l < nrPixels; l++)
    {
        if (mask.data && !mask(l))
        {
            fineLayer(l) = 0.f;
            continue;
        }
        if (m_energy(l) >= m_thresholdFine)
        {
            const float v = (m_thresholdBin) ? 1.f : m_energy(l);
            fineLayer(l) = v;
            maxFine = glm::max(maxFine, v);
        } else
        {
            fineLayer(l) = THRESHOLD_MIN;
        }
    }
    layers.push_back(fineLayer);

    if (!m_thresholdBin)
    {
        for (int l = 0; l < nrPixels; l++)
        {
            layers[0](l) = mapRange(layers[0](l), THRESHOLD_MIN, m_thresholdCoarse, THRESHOLD_MIN, 1.f);
            layers[1](l) = mapRange(layers[1](l), THRESHOLD_MIN, m_thresholdFine, THRESHOLD_MIN, 1.f);
            layers[2](l) = mapRange(layers[2](l), THRESHOLD_MIN, maxFine, THRESHOLD_MIN, 1.f);
        }

    }

    if (m_openingRadius > 0)
    {
        linde::Morphology<float> morph;
        morph.createStructuringCircle(m_openingRadius, 1);
        for (cv::Mat_<float> & l : layers)
        {
            morph.open(l, l);
        }

    }
}




float StructureExtraction::getThresholdFine() const
{
    return m_thresholdFine;
}

void StructureExtraction::setThresholdFine(float thresholdFine)
{
    m_thresholdFine = thresholdFine;
}

float StructureExtraction::getThresholdCoarse() const
{
    return m_thresholdCoarse;
}

void StructureExtraction::setThresholdCoarse(float thresholdCoarse)
{
    m_thresholdCoarse = thresholdCoarse;
}

float StructureExtraction::getMinScale() const
{
    return m_minScale;
}

void StructureExtraction::setMinScale(float minScale)
{
    m_minScale = minScale;
}


} // namespace linde

