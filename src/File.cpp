#include "../include/linde/File.h"

#include <algorithm>


#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifdef OS_WIN
#include <windows.h>
#elif defined(OS_LINUX)
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#endif

#ifdef USE_TIFF_STORAGE
#include <tiffio.h>
#include <tiffio.hxx>
#include <tiff.h>
#endif

#include <ctime>

namespace linde
{

long int getFileChangedTimeStamp(const std::string & filename)
{
#ifdef OS_WIN
    HANDLE hFile;
    FILETIME ftCreate, ftAccess, ftWrite;

    long int time = 0;

    hFile = CreateFile(filename.c_str(), GENERIC_READ,
                       FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        //ERROR_FILE_NOT_FOUND, ERROR_SHARING_VIOLATION...
        std::cerr << "CreateFile failed for " << filename << std::endl;
        std::cerr << "with error code " << GetLastError() << std::endl;
    }
    else
    {
        if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        {
            ULARGE_INTEGER temp;
            temp.HighPart = ftWrite.dwHighDateTime;
            temp.LowPart = ftWrite.dwLowDateTime;
            /* now ready to use temp.QuadPart */
            time = static_cast<long int>(temp.QuadPart);
        }
        else
        {
            std::cerr << "GetFileTime failed for " << filename << std::endl;
            std::cerr << "with error code " << GetLastError() << std::endl;
        }
        CloseHandle(hFile);
    }


    return time;

#elif defined(OS_LINUX)

    struct stat attrib;			// create a file attribute structure
    stat(filename.c_str(), &attrib);		// get the attributes of afile.txt
    return static_cast<long int>(attrib.st_ctime);
#endif

}

std::string extractFilePath(const std::string & filename)
{
    std::string res(filename);
    size_t ls = res.find_last_of("/\\");
    return res.substr(0, ls+1);
}

std::string extractFilename(const std::string & filename, bool withFiletype)
{
    std::string res(filename);
    size_t ls = res.find_last_of("/\\");
    res = res.substr(ls + 1, res.size() - ls - 1);

    if (withFiletype) return res;

    size_t lp = res.find_last_of(".");
    res = res.substr(0, lp);
    return res;
}

std::string extractFiletype(const std::string & filename)
{
    std::string res(filename);
    size_t ls = res.find_last_of(".");
    res = res.substr(ls + 1, res.size() - ls - 1);
    return res;
}

cv::Mat_<glm::vec3> imLoad(const std::string & filenameOriginal)
{
    return imLoad(filenameOriginal, std::function<void(const glm::vec3&, glm::vec3&)>());
}


// opencv imRead wrapper
// retuns floating point RGB(A) image, 3 or 4 channel
// think about sRGB -> RGB conversion afterwards
cv::Mat_<glm::vec3> imLoad(const std::string & filenameOriginal, std::function<void(const glm::vec3&, glm::vec3&)> colorConversion)
{
    std::string filename = filenameOriginal;
    std::replace(filename.begin(), filename.end(), '\\', '/');

    cv::Mat cv_mat = cv::imread(filename, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

    // if not loaded succesfully
    if (!cv_mat.data)
    {
        std::cerr << "could not load image: " << filename << std::endl;
        return cv::Mat_<glm::vec3>();
    }

    if (cv_mat.channels() == 1)
    {
        cv::Mat in[] = { cv_mat, cv_mat, cv_mat };
        cv::merge(in, 3, cv_mat);
    }
    else if (cv_mat.channels() == 4)
    {
        cv::cvtColor(cv_mat, cv_mat, cv::COLOR_BGRA2BGR);
    }

    // data scale
    float scale = 1.0f;
    if (cv_mat.depth() == CV_16U)
        scale = 1.0f / 0xffff;
    else if (cv_mat.depth() == CV_32F)
        scale = 1.0f;
    else if (cv_mat.depth() == CV_8U)
        scale = 1.0f / 0xff;
    else if (cv_mat.depth() == CV_64F)
        scale = 1.0f / 0xffffffff;

    // convert to right type
    cv_mat.convertTo(cv_mat, CV_32FC3, scale);

    // OpenCV has BGR
    cv::cvtColor(cv_mat, cv_mat, cv::COLOR_BGR2RGB);

    cv::Mat_<glm::vec3> res = cv_mat;
    if (colorConversion)
    {
        convert_image(res, res, colorConversion);
    }

    return res;
}

cv::Mat_<float> imLoadSingleChannel(const std::string & filenameOriginal)
{
    std::string filename = filenameOriginal;
    std::replace(filename.begin(), filename.end(), '\\', '/');

    cv::Mat cv_mat = cv::imread(filename, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_GRAYSCALE);

    // if not loaded succesfully
    if (!cv_mat.data)
    {
        std::cerr << "could not decode image " << std::endl;
        return cv::Mat_<float>();
    }

    // data scale
    float scale = 1.0f;
    if (cv_mat.depth() == CV_16U)
        scale = 1.0f / 0xffff;
    else if (cv_mat.depth() == CV_32F)
        scale = 1.0f;
    else if (cv_mat.depth() == CV_8U)
        scale = 1.0f / 0xff;
    else if (cv_mat.depth() == CV_64F)
        scale = 1.0f / 0xffffffff;

    // convert to right type
    cv_mat.convertTo(cv_mat, CV_32FC1, scale);

    cv::Mat_<float> res = cv_mat;

    return res;
}

// opencv imRead wrapper
// retuns floating point RGB(A) image, 3 or 4 channel
// think about sRGB -> RGB conversion afterwards
cv::Mat_<glm::vec3> imLoad(const std::vector<uchar> &buffer)
{
    return imLoad(buffer, [](const glm::vec3 &a, glm::vec3&b){b=a;});
}

cv::Mat_<glm::vec3> imLoad(const std::vector<uchar> &buffer, std::function<void(const glm::vec3&, glm::vec3&)> colorConversion)
{

    cv::Mat cv_mat = cv::imdecode(buffer, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

    // if not loaded succesfully
    if (!cv_mat.data)
    {
        std::cerr << "could not decode image " << std::endl;
        return cv::Mat_<glm::vec3>();
    }

    if (cv_mat.channels() == 1)
    {
        cv::Mat in[] = { cv_mat, cv_mat, cv_mat };
        cv::merge(in, 3, cv_mat);
    }
    else if (cv_mat.channels() == 4)
    {
        cv::cvtColor(cv_mat, cv_mat, cv::COLOR_BGRA2BGR);
    }

    // data scale
    float scale = 1.0f;
    if (cv_mat.depth() == CV_16U)
        scale = 1.0f / 0xffff;
    else if (cv_mat.depth() == CV_32F)
        scale = 1.0f;
    else if (cv_mat.depth() == CV_8U)
        scale = 1.0f / 0xff;
    else if (cv_mat.depth() == CV_64F)
        scale = 1.0f / 0xffffffff;

    // convert to right type
    cv_mat.convertTo(cv_mat, CV_32FC3, scale);

    // OpenCV has BGR
    cv::cvtColor(cv_mat, cv_mat, cv::COLOR_BGR2RGB);

    cv::Mat_<glm::vec3> res = cv_mat;
    if (colorConversion)
    {
        convert_image(res, res, colorConversion);
    }

    return res;
}


// opencv imRead wrapper
// retuns floating point RGB(A) image, 3 or 4 channel
// think about sRGB -> RGB conversion afterwards
cv::Mat imLoadCV(const std::string & filenameOriginal)
{
    std::string filename = filenameOriginal;
    std::replace(filename.begin(), filename.end(), '\\', '/');

    cv::Mat cv_mat = cv::imread(filename, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

    // if not loaded succesfully
    if (!cv_mat.data) return cv_mat;

    if (cv_mat.channels() == 1)
    {
        cv::Mat in[] = { cv_mat, cv_mat, cv_mat };
        cv::merge(in, 3, cv_mat);
    }
    else if (cv_mat.channels() == 4)
    {
        cv::cvtColor(cv_mat, cv_mat, cv::COLOR_BGRA2BGR);
    }

    // data scale
    float scale = 1.0f;
    if (cv_mat.depth() == CV_16U)
        scale = 1.0f / 0xffff;
    else if (cv_mat.depth() == CV_32F)
        scale = 1.0f;
    else if (cv_mat.depth() == CV_8U)
        scale = 1.0f / 0xff;
    else if (cv_mat.depth() == CV_64F)
        scale = 1.0f / 0xffffffff;

    // convert to right type
    cv_mat.convertTo(cv_mat, CV_32FC3, scale);
    // OpenCV has BGR
    cv::cvtColor(cv_mat, cv_mat, cv::COLOR_BGR2RGB);

    return cv_mat;
}


// opencv wrapper for imRead
// automatically converts to right type
// expects image to be RGB(A)
// convert to sRGB! before saving
bool imSaveCV(const std::string & filenameOriginal, const cv::Mat & cv_mat, const std::vector<int>& params)
{
    std::string filename = filenameOriginal;
    std::replace(filename.begin(), filename.end(), '\\', '/');

    std::string filetype = extractFiletype(filename);

    cv::Mat m;

    if (filetype == "png")
    {
        if (cv_mat.depth() == CV_16U)
        {
            m = cv_mat.clone();
        }
        else if (cv_mat.depth() == CV_32F)
        {
            const float scale = (float)0xffff;
            cv_mat.convertTo(m, CV_MAKETYPE(CV_16U, cv_mat.channels()), scale);
        }
        else if (cv_mat.depth() == CV_8U)
        {
            m = cv_mat.clone();
        }
        else if (cv_mat.depth() == CV_64F)
        {
            const float scale = (float)0xffffffff;
            cv_mat.convertTo(m, CV_MAKETYPE(CV_16U, cv_mat.channels()), scale);
        }
        else
        {
            m = cv_mat.clone();
        }

    }
    else
    {
        if (cv_mat.depth() == CV_16U)
        {
            const float scale = (float)0xff;
            cv_mat.convertTo(m, CV_MAKETYPE(CV_8U, cv_mat.channels()), scale);
        }
        else if (cv_mat.depth() == CV_32F)
        {
            const float scale = (float)0xff;
            cv_mat.convertTo(m, CV_MAKETYPE(CV_8U, cv_mat.channels()), scale);
        }
        else if (cv_mat.depth() == CV_8U)
        {
            m = cv_mat.clone();
        }
        else if (cv_mat.depth() == CV_64F)
        {
            const float scale = (float)0xff;
            cv_mat.convertTo(m, CV_MAKETYPE(CV_8U, cv_mat.channels()), scale);
        }
        else
        {
            m = cv_mat.clone();
        }
    }
    if (m.channels() == 3)
        cv::cvtColor(m, m, cv::COLOR_RGB2BGR);
    return cv::imwrite(filename, m);
}


// opencv wrapper for imRead
// automatically converts to right type
// expects image to be RGB(A)
// convert to sRGB! before saving
bool imSave(const std::string & filenameOriginal, const cv::Mat_<glm::vec3> & mat)
{
    return imSave(filenameOriginal, mat, nullptr, std::vector<int>());
}

bool imSave(const std::string & filenameOriginal, const cv::Mat_<glm::vec3> & mat, std::function<void(const glm::vec3&, glm::vec3&)> colorConversion, const std::vector<int>& params)
{
    std::string filename = filenameOriginal;
    std::replace(filename.begin(), filename.end(), '\\', '/');

    std::string filetype = extractFiletype(filename);

    cv::Mat_<glm::vec3> output;
    if (colorConversion)
    {
        convert_image(mat, output, colorConversion);
    }
    else
    {
        output = mat;
    }

    cv::Mat m;

    if (filetype == "png")
    {
        const float scale = (float)0xffff;
        output.convertTo(m, CV_MAKETYPE(CV_16U, 3), scale);

    }
    else
    {
        const float scale = (float)0xff;
        output.convertTo(m, CV_MAKETYPE(CV_8U, 3), scale);
    }
    cv::cvtColor(m, m, cv::COLOR_RGB2BGR);
    return cv::imwrite(filename, m, params);
}

bool imSave(const std::string & filenameOriginal, const cv::Mat_<uchar> & mat)
{
    std::string filename = filenameOriginal;
    std::replace(filename.begin(), filename.end(), '\\', '/');

    return cv::imwrite(filename, mat);
}

bool imSave(const std::string & filenameOriginal, const cv::Mat_<float> & mat)
{
    return imSave(filenameOriginal, mat, std::vector<int>());
}

bool imSave(const std::string & filenameOriginal, const cv::Mat_<float> & mat, const std::vector<int>& params)
{
    std::string filename = filenameOriginal;
    std::replace(filename.begin(), filename.end(), '\\', '/');

    std::string filetype = extractFiletype(filename);

    cv::Mat m;

    if (filetype == "png")
    {
        const float scale = (float)0xffff;
        mat.convertTo(m, CV_MAKETYPE(CV_16U, 1), scale);

    }
    else
    {
        const float scale = (float)0xff;
        mat.convertTo(m, CV_MAKETYPE(CV_8U, 1), scale);
    }
    return cv::imwrite(filename, m);
}

#ifdef USE_TIFF_STORAGE

bool imSave_CIELab_tiff(const std::string & filename, const cv::Mat_<glm::vec3> & floatMat, int bps)
{
    TIFF *tiff= TIFFOpen(filename.data(), "w");

    time_t t = time(0);   // get time now
    tm * now = localtime( & t );
    std::stringstream datetimeStream;
    datetimeStream  << now->tm_year;
    datetimeStream  << ":";
    datetimeStream  << now->tm_mon;
    datetimeStream  << ":";
    datetimeStream  << now->tm_mday;
    datetimeStream  << " ";
    datetimeStream  << now->tm_hour;
    datetimeStream  << ":";
    datetimeStream  << now->tm_min;
    datetimeStream  << ":";
    datetimeStream  << now->tm_sec;
    datetimeStream  << ":";

    TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH, floatMat.cols); // image dimensions
    TIFFSetField (tiff, TIFFTAG_IMAGELENGTH, floatMat.rows); // image dimensions
    TIFFSetField (tiff, TIFFTAG_COMPRESSION, COMPRESSION_LZW); //
    TIFFSetField (tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_CIELAB); // store in CIELAB color space
    TIFFSetField (tiff, TIFFTAG_BITSPERSAMPLE, (bps == 16) ? 16 : 8); // 32 bits per channel
    TIFFSetField (tiff, TIFFTAG_SAMPLESPERPIXEL, 3); // 3 channels
    TIFFSetField (tiff, TIFFTAG_IMAGEDESCRIPTION, "written with 16 bit CIELAB writer");
    //TIFFSetField (tiff, TIFFTAG_COPYRIGHT, ..);
    //TIFFSetField (tiff, TIFFTAG_ARTIST, ..);
    TIFFSetField (tiff, TIFFTAG_DATETIME, datetimeStream.str().data());
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT); // origin is topleft
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); // LabLabLab
    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, floatMat.rows);
    float whitePoint[2] = {3127.f/10000.f, 3290.f/10000.f};
    TIFFSetField(tiff, TIFFTAG_WHITEPOINT, whitePoint);

    /*
For CIELab (PhotometricInterpretation = 8), the L* component is encoded in 8 bits as an unsigned integer
range [0,255], and encoded in 16 bits as an unsigned integer range [0,65535]. The a* and b* components
in 8 bits are signed integers range [-128,127], and in 16 bits are signed integers range [-32768,32767]. The
8 bit chrominance values are encoded exactly equal to the 1976 CIE a* and b* values, while the 16 bit
values are encoded as 256 times the 1976 CIE a* and b* values.
    */

    if (bps == 16)
    {
        cv::Mat_<cv::Vec<uint16, 3> > mat(floatMat.size());
        for (int i = 0; i < floatMat.cols*floatMat.rows; i++)
        {
            cv::Vec<uint16, 3>  & d = mat(i);
            const glm::vec3 & a = floatMat(i);

            d[0] = static_cast<uint16>(std::round((a[0] / 100.f) * std::numeric_limits<uint16>::max()));
            d[1] = static_cast<int16>(std::round(a[1] * 256.f));
            d[2] = static_cast<int16>(std::round(a[2] * 256.f));
        }
        TIFFWriteEncodedStrip(tiff, 0, mat.data, mat.rows * mat.cols * sizeof(cv::Vec<uint16, 3>));
    } else
    {
        cv::Mat_<cv::Vec<uchar, 3> > mat(floatMat.size());
        for (int i = 0; i < floatMat.cols*floatMat.rows; i++)
        {
            cv::Vec<uchar, 3>  & d = mat(i);
            const glm::vec3 & a = floatMat(i);

            d[0] = static_cast<uchar>(std::round((a[0] / 100.f) * std::numeric_limits<uchar>::max()));
            d[1] = static_cast<char>(std::round(a[1]));
            d[2] = static_cast<char>(std::round(a[2]));
        }
        TIFFWriteEncodedStrip(tiff, 0, mat.data, mat.rows * mat.cols * sizeof(cv::Vec<uchar, 3>));
    }

    TIFFFlushData(tiff);

    TIFFClose(tiff);

    return true;
}


bool imSave_CIEL_tiff(const std::string & filename, const cv::Mat_<float> & floatMat, int bps)
{
    TIFF *tiff= TIFFOpen(filename.data(), "w");

    time_t t = time(0);   // get time now
    tm * now = localtime( & t );
    std::stringstream datetimeStream;
    datetimeStream  << now->tm_year;
    datetimeStream  << ":";
    datetimeStream  << now->tm_mon;
    datetimeStream  << ":";
    datetimeStream  << now->tm_mday;
    datetimeStream  << " ";
    datetimeStream  << now->tm_hour;
    datetimeStream  << ":";
    datetimeStream  << now->tm_min;
    datetimeStream  << ":";
    datetimeStream  << now->tm_sec;
    datetimeStream  << ":";

    TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH, floatMat.cols); // image dimensions
    TIFFSetField (tiff, TIFFTAG_IMAGELENGTH, floatMat.rows); // image dimensions
    TIFFSetField (tiff, TIFFTAG_COMPRESSION, COMPRESSION_LZW); //
    TIFFSetField (tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_CIELAB); // store in CIELAB color space
    TIFFSetField (tiff, TIFFTAG_BITSPERSAMPLE, (bps == 16) ? 16 : 8); //
    TIFFSetField (tiff, TIFFTAG_SAMPLESPERPIXEL, 1); // 3 channels
    TIFFSetField (tiff, TIFFTAG_IMAGEDESCRIPTION, "written with 16 bit CIELAB writer");
    //TIFFSetField (tiff, TIFFTAG_COPYRIGHT, ..);
    //TIFFSetField (tiff, TIFFTAG_ARTIST, ..);
    TIFFSetField (tiff, TIFFTAG_DATETIME, datetimeStream.str().data());
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT); // origin is topleft
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); // LabLabLab
    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, floatMat.rows);
    float whitePoint[2] = {3127.f/10000.f, 3290.f/10000.f};
    TIFFSetField(tiff, TIFFTAG_WHITEPOINT, whitePoint);

    /*
For CIELab (PhotometricInterpretation = 8), the L* component is encoded in 8 bits as an unsigned integer
range [0,255], and encoded in 16 bits as an unsigned integer range [0,65535]. The a* and b* components
in 8 bits are signed integers range [-128,127], and in 16 bits are signed integers range [-32768,32767]. The
8 bit chrominance values are encoded exactly equal to the 1976 CIE a* and b* values, while the 16 bit
values are encoded as 256 times the 1976 CIE a* and b* values.
    */

    if (bps == 16)
    {
        cv::Mat_<uint16> mat(floatMat.size());
        for (int i = 0; i < floatMat.cols*floatMat.rows; i++)
        {
            uint16  & d = mat(i);
            const float & a = floatMat(i);

            d= static_cast<uint16>(std::round((a / 100.f) * std::numeric_limits<uint16>::max()));
        }
        TIFFWriteEncodedStrip(tiff, 0, mat.data, mat.rows * mat.cols * sizeof(uint16));
    } else
    {
        cv::Mat_<uchar> mat(floatMat.size());
        for (int i = 0; i < floatMat.cols*floatMat.rows; i++)
        {
            uchar  & d = mat(i);
            const float & a = floatMat(i);

            d = static_cast<uchar>(std::round((a / 100.f) * std::numeric_limits<uchar>::max()));
        }
        TIFFWriteEncodedStrip(tiff, 0, mat.data, mat.rows * mat.cols * sizeof(uchar));
    }



    TIFFFlushData(tiff);

    TIFFClose(tiff);

    return true;
}

cv::Mat_<glm::vec3> imLoad_CIELab_tiff(const std::string & filename)
{
    TIFF *tiff= TIFFOpen(filename.data(), "r");

    uint32 imageWidth, imageLength;
    uint32 photmetricType;
    short bitps, spp;

    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &imageWidth);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &imageLength);
    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &photmetricType);
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitps);
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &spp);

    if (photmetricType != PHOTOMETRIC_CIELAB)
    {
        std::cerr << "could not load " << filename << ": no CIELAB picture" << std::endl;
        return cv::Mat_<glm::vec3>();
    }

    cv::Mat_<glm::vec3> result(imageLength, imageWidth);

    if (bitps == 16)
    {
        cv::Mat_<cv::Vec<uint16, 3> > byteData(imageLength, imageWidth);

        for(uint32 strip = 0; strip < TIFFNumberOfStrips(tiff); strip++)
        {
            TIFFReadEncodedStrip(tiff, strip, byteData.data, (tsize_t)-1);
        }

        for (int i = 0; i < byteData.cols*byteData.rows; i++)
        {
            const cv::Vec<uint16, 3>  & d = byteData(i);
            glm::vec3 & a = result(i);

            a[0] = (d[0] / (float)std::numeric_limits<uint16>::max()) * 100.f;
            a[1] = static_cast<int16>(d[1]) / 256.f;
            a[2] = static_cast<int16>(d[2]) / 256.f;
        }
    } else if (bitps == 8)
    {
        cv::Mat_<cv::Vec<uchar, 3> > byteData(imageLength, imageWidth);

        for(uint32 strip = 0; strip < TIFFNumberOfStrips(tiff); strip++)
        {
            TIFFReadEncodedStrip(tiff, strip, byteData.data, (tsize_t)-1);
        }

        for (int i = 0; i < byteData.cols*byteData.rows; i++)
        {
            const cv::Vec<uchar, 3>  & d = byteData(i);
            glm::vec3 & a = result(i);

            a[0] = (d[0] / (float)std::numeric_limits<uchar>::max()) * 100.f;
            a[1] = static_cast<char>(d[1]);
            a[2] = static_cast<char>(d[2]);
        }
    }

    TIFFClose(tiff);

    return result;
}

cv::Mat_<float> imLoad_CIEL_tiff(const std::string & filename)
{
    TIFF *tiff= TIFFOpen(filename.data(), "r");

    uint32 imageWidth, imageLength;
    uint32 photmetricType;
    short bitps, spp;

    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &imageWidth);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &imageLength);
    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &photmetricType);
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitps);
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &spp);

    if (photmetricType != PHOTOMETRIC_CIELAB)
    {
        std::cerr << "could not load " << filename << ": no CIELAB picture" << std::endl;
        return cv::Mat_<float>();
    }

    if (spp != 1)
    {
        std::cerr << "no single channel image file: " << filename << std::endl;
        return cv::Mat_<float>();
    }

    cv::Mat_<float> result(imageLength, imageWidth);

    if (bitps == 16)
    {
        cv::Mat_<uint16> byteData(imageLength, imageWidth);

        for(uint32 strip = 0; strip < TIFFNumberOfStrips(tiff); strip++)
        {
            TIFFReadEncodedStrip(tiff, strip, byteData.data, (tsize_t)-1);
        }

        for (int i = 0; i < byteData.cols*byteData.rows; i++)
        {
            const uint16  & d = byteData(i);
            float & a = result(i);

            a= (d / (float)std::numeric_limits<uint16>::max()) * 100.f;
        }
    } else if (bitps == 8)
    {
        cv::Mat_<uchar> byteData(imageLength, imageWidth);

        for(uint32 strip = 0; strip < TIFFNumberOfStrips(tiff); strip++)
        {
            TIFFReadEncodedStrip(tiff, strip, byteData.data, (tsize_t)-1);
        }

        for (int i = 0; i < byteData.cols*byteData.rows; i++)
        {
            const uchar  & d = byteData(i);
            float & a = result(i);

            a = (d / (float)std::numeric_limits<uchar>::max()) * 100.f;
        }
    }

    TIFFClose(tiff);

    return result;
}


bool serialize(const std::string & filename,
               const uchar * data,
               const uint width, const uint height,
               const short bitsPerChannel, const short channels, const bool compress)
{
    TIFF *tiff= TIFFOpen(filename.data(), "w");

    time_t t = time(0);   // get time now
    tm * now = localtime( & t );
    std::stringstream datetimeStream;
    datetimeStream  << now->tm_year;
    datetimeStream  << ":";
    datetimeStream  << now->tm_mon;
    datetimeStream  << ":";
    datetimeStream  << now->tm_mday;
    datetimeStream  << " ";
    datetimeStream  << now->tm_hour;
    datetimeStream  << ":";
    datetimeStream  << now->tm_min;
    datetimeStream  << ":";
    datetimeStream  << now->tm_sec;
    datetimeStream  << ":";

    TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH, width); // image dimensions
    TIFFSetField (tiff, TIFFTAG_IMAGELENGTH, height); // image dimensions
    TIFFSetField (tiff, TIFFTAG_COMPRESSION, compress ? COMPRESSION_LZW : COMPRESSION_NONE); //
    // TIFFSetField (tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_CIELAB); // store in CIELAB color space
    TIFFSetField (tiff, TIFFTAG_BITSPERSAMPLE, bitsPerChannel);
    TIFFSetField (tiff, TIFFTAG_SAMPLESPERPIXEL, channels); // 3 channels
    TIFFSetField (tiff, TIFFTAG_IMAGEDESCRIPTION, "written with tiff writer");
    //TIFFSetField (tiff, TIFFTAG_COPYRIGHT, ..);
    //TIFFSetField (tiff, TIFFTAG_ARTIST, ..);
    TIFFSetField (tiff, TIFFTAG_DATETIME, datetimeStream.str().data());
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT); // origin is topleft
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); // LabLabLab
    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, height);
    // float whitePoint[2] = {3127.f/10000.f, 3290.f/10000.f};
    // TIFFSetField(tiff, TIFFTAG_WHITEPOINT, whitePoint);

    TIFFWriteEncodedStrip(tiff, 0, const_cast<uchar*>(data), width * height * (bitsPerChannel / 8) * channels);

    TIFFFlushData(tiff);

    TIFFClose(tiff);

    return true;
}


bool deserialize(const std::string & filename,
                 uchar * data,
                 const uint width, const uint height,
                 const short bitsPerChannel, const short channels)
{
    TIFF *tiff= TIFFOpen(filename.data(), "r");

    uint32 imageWidth, imageLength;
    short bitps, spp;

    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &imageWidth);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &imageLength);
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitps);
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &spp);

    std::cout << imageWidth << " " << imageLength << " " << bitps << " " << spp << std::endl;

    if (imageWidth == width && imageLength == height && bitsPerChannel == bitps && channels == spp)
    {

        for(uint32 strip = 0; strip < TIFFNumberOfStrips(tiff); strip++)
        {
            TIFFReadEncodedStrip(tiff, strip, data, (tsize_t)-1);
        }

        TIFFClose(tiff);
        return true;
    } else
    {
        std::cerr << "format does not equal the arguments" << std::endl;

        TIFFClose(tiff);
        return false;
    }
}


#endif

} // namepsace linde



