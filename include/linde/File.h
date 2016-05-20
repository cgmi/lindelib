#ifndef LINDE_FILE_H
#define LINDE_FILE_H


#include "linde.h"
#include "Color.h"



namespace linde
{
    std::string extractFilePath(const std::string & filename);
	std::string extractFilename(const std::string & filename, bool withFiletype = true);
	std::string extractFiletype(const std::string & filename);

	long int getFileChangedTimeStamp(const std::string & filename);

	// opencv imRead wrapper
	// retuns floating point RGB(A) image, 3 or 4 channel
	// think about sRGB -> RGB conversion afterwards
	cv::Mat imLoadCV(const std::string & filename);

	// opencv wrapper for imWrite
	// automatically converts to right type
	// expects image to be RGB(A)
	// convert to sRGB! before saving
	bool imSaveCV(const std::string & filename, const cv::Mat & cv_mat, const std::vector<int>& params = std::vector<int>());


	// opencv imRead wrapper
	// retuns floating point RGB(A) image, 3 or 4 channel
	// think about sRGB -> RGB conversion afterwards
    cv::Mat_<glm::vec3> imLoad(const std::string & filename, convert_color_call colorConversion = nullptr);
    cv::Mat_<glm::vec3> imLoad(const std::vector<uchar> & buffer, convert_color_call colorConversion = nullptr);
    cv::Mat_<float>     imLoadSingleChannel(const std::string & filename);

	// opencv wrapper for imWrite
	// automatically converts to right type
	// expects image to be RGB(A)
	// convert to sRGB! before saving
    bool imSave(const std::string & filename, const cv::Mat_<uchar> & mat);
	bool imSave(const std::string & filename, const cv::Mat_<float> & mat, const std::vector<int>& params = std::vector<int>());
	bool imSave(const std::string & filename, const cv::Mat_<glm::vec3> & mat, convert_color_call colorConversion = nullptr, const std::vector<int>& params = std::vector<int>());

#ifdef USE_TIFF_STORAGE
    // image has to be in CIELAB color space
    bool imSave_CIELab_tiff(const std::string & filename, const cv::Mat_<glm::vec3> & mat, int bps = 16);
    cv::Mat_<glm::vec3> imLoad_CIELab_tiff(const std::string & filename);
    // only luminance channels
    bool imSave_CIEL_tiff(const std::string & filename, const cv::Mat_<float> & mat, int bps = 16);
    cv::Mat_<float> imLoad_CIEL_tiff(const std::string & filename);

    bool serialize(const std::string & filename,
                   const uchar * data,
                   const uint width, const uint height,
                   const short bitsPerChannel, const short channels, const bool compress = true);
    bool deserialize(const std::string & filename,
                     uchar * data,
                     const uint width, const uint height,
                     const short bitsPerChannel, const short channels);
#endif


}// namespace linde

#endif //LINDE_FILE_H
