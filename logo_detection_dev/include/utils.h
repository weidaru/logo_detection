#include <string>
#include <vector>

#include <opencv/cv.h>
#include <CImg.h>

#ifndef UTILS_H
#define ULILS_H

namespace utils
{
uchar* g_getPixel(const cv::Mat& m, int row, int col);
double g_calcGrayscale(uchar r, uchar g, uchar b);
void g_saveMat(const cv::Mat& mat, const char* file);
long g_matSum(const cv::Mat& mat);

std::string readStringFromPrivateProfile(const std::string& appName, const std::string& keyName, const std::string& fileName);
int readIntegerFromPrivateProfile(const std::string& appName, const std::string& keyName, const std::string& fileName);
double readDoubleFromPrivateProfile(const std::string& appName, const std::string& keyName, const std::string& fileName);
bool setPrivateProfileString(const std::string& appName, const std::string& keyName, const std::string& stringName, const std::string& fileName);

int getDir(const std::string& dirName, const std::string& extention, std::vector<std::string>& files);

int g_matToCimg(const cv::Mat& mat, cimg_library::CImg<unsigned char>& img);
}

#endif          //UTILS_H
