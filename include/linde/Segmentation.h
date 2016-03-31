#ifndef POISSONSOLVER_H
#define POISSONSOLVER_H

#include "linde.h"
#include <set>

/**
    * @author Thomas Lindemeier
    * https://github.com/qiankanglai/opencv.meanshift
    *
    * University of Konstanz-
    * Department for Computergraphics
    */
namespace linde
{


// https://github.com/qiankanglai/opencv.meanshift
int
MeanShift(const cv::Mat_<glm::vec3> &rgb, cv::Mat_<int> &labels,
          const int hs = 10, const float hc = 6.5f, const int minRegion = 20);

int ConnectedComponents(const cv::Mat_<uchar> &binary, cv::Mat_<int> & labels);

void UnsharpMasking(const cv::Mat_<float> &depth, cv::Mat_<float> &spatialImportanceFunction, const float sigma);

// From Contours to Regions: An Empirical Evaluation, Arbelaez et al.
class UCMParser
{
    cv::Mat_<float> m_ucm;
    std::set<float> m_levels;
public:
    UCMParser();
    UCMParser(const cv::Mat_<float> &ucm);
    ~UCMParser();

    void parse(const cv::Mat_<float> &ucm);

    void getLevels(std::vector<float> &levels) const;

    int getLevel(const float level, cv::Mat_<int> &labels) const;

private:
    void parseLevels(std::set<float> & levels) const;
};

} // namespace linde


#endif // POISSONSOLVER_H
