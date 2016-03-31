#ifndef STRUCTUREEXTRACTION_H
#define STRUCTUREEXTRACTION_H

#include "linde.h"

namespace linde
{

class StructureExtraction
{

    bool                        m_thresholdBin;
    int                         m_openingRadius;

    float                       m_thresholdFine;
    float                       m_thresholdCoarse;
    float                       m_minScale;

    cv::Mat_<float>             m_energy;

public:
    static const float THRESHOLD_MIN;

    StructureExtraction();
    ~StructureExtraction();

    void computeEnergy(const cv::Mat_<glm::vec3> LchImage, const std::vector<uint> &useChannels, const cv::Mat_<uchar> & mask = cv::Mat_<uchar>());

    void extractLayers(std::vector<cv::Mat_<float> > & layers, const cv::Mat_<uchar> & mask = cv::Mat_<uchar>()) const;

    void setThresholdBinary(bool thresholdBin);

    void setOpeningRadius(int openingRadius);

    float getThresholdFine() const;
    void setThresholdFine(float thresholdFine);

    float getThresholdCoarse() const;
    void setThresholdCoarse(float thresholdCoarse);

    float getMinScale() const;
    void setMinScale(float minScale);

    cv::Mat_<float> getEnergy() const;

private:
    void computeGaussEnergy(const cv::Mat_<glm::vec3> &image, cv::Mat_<float> & energy, const float minScale, const std::vector<uint> &useChannels = std::vector<uint>()) const;
};

} // namespace linde

#endif // STRUCTUREEXTRACTION_H
