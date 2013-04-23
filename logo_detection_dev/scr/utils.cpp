#include "utils.h"
#include "globals.h"

#include <log4cplus/logger.h>

#include <fstream>
#include <dirent.h>

using namespace log4cplus;
using namespace cimg_library;

namespace utils
{
long g_matSum(const cv::Mat& mat)
{
    long result  = 0;
    int rows = mat.rows;
    int cols = mat.cols;
    int chns = mat.channels();
    for(int i = 0; i<rows; i++)
    {
        for(int j = 0; j<cols; j++)
        {
            for(int k=0; k<chns; k++)
            {
                result+=long( ( (uchar*)mat.data + mat.step*i+j*chns)[k] );
            }
        }
    }
    return result;
}

//The returning color is arrganged as b g r
uchar* g_getPixel(const cv::Mat& m, int row, int col)
{
    return (uchar*)m.data + row*m.step + col*m.channels();
}

double g_calcGrayscale(uchar r, uchar g, uchar b)
{
    return 0.3*(double)r + 0.59*(double)g + 0.11*(double)b;
}

//File return is disabled when enableDebug is true
void g_saveMat(const cv::Mat& mat, const char* file)
{
    if(!globals::enableDebug)
        return;

    std::ofstream stream(file);
    int Rows=mat.rows;
    int Cols=mat.cols;
    int Chns=mat.channels();
    for(int i=0; i<Rows; i++)
    {
        for(int j=0; j<Cols; j++)
        {
            for(int k=0; k<Chns; k++)
            {
                stream<<int( ( (uchar*)mat.data + mat.step*i+j*Chns)[k] )<<" ";
            }
        }
        stream<<"\n";
    }
    stream<<"\n";
    stream.close();

    LOG4CPLUS_DEBUG(log4cplus::Logger::getRoot(), "Matrix data is saved to file " << file);
}

std::string readStringFromPrivateProfile(const std::string& appName, const std::string& keyName, const std::string& fileName)
{
    char cbuffer[256];
    GetPrivateProfileString(appName.c_str(), keyName.c_str(),
                                            NULL, cbuffer, sizeof(cbuffer), fileName.c_str() );
    std::string result(cbuffer);
    LOG4CPLUS_INFO(Logger::getRoot(), "Read ["<<keyName<< " = "<<result<<"] from"<<fileName);

    return result;
}

int readIntegerFromPrivateProfile(const std::string& appName, const std::string& keyName, const std::string& fileName)
{
    char cbuffer[256];
    GetPrivateProfileString(appName.c_str(), keyName.c_str(),
                                            NULL, cbuffer, sizeof(cbuffer), fileName.c_str() );
    int result = std::atoi(cbuffer);
    LOG4CPLUS_INFO(Logger::getRoot(), "Read ["<<keyName<< " = "<<result<<"] from"<<fileName);

    return result;
}

double readDoubleFromPrivateProfile(const std::string& appName, const std::string& keyName, const std::string& fileName)
{
    char cbuffer[256];
    GetPrivateProfileString(appName.c_str(), keyName.c_str(),
                                            NULL, cbuffer, sizeof(cbuffer), fileName.c_str() );
    double result = std::atof(cbuffer);
    LOG4CPLUS_INFO(Logger::getRoot(), "Read ["<<keyName<< " = "<<result<<"] from"<<fileName);

    return result;
}

bool setPrivateProfileString(const std::string& appName, const std::string& keyName, const std::string& stringName, const std::string& fileName)
{
    LOG4CPLUS_INFO(Logger::getRoot(), "Write ["<<keyName<< " = "<<stringName<<"] to "<<appName<< " of "<<fileName);
    return WritePrivateProfileString(appName.c_str(), keyName.c_str(), stringName.c_str(), fileName.c_str());
}

int getDir (const std::string& dir, const std::string& extention, std::vector<std::string>& files)
{
    std::string myext;
    if(extention.find(".") == std::string::npos)
        myext = "." + extention;
    else
        myext = extention;

    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        LOG4CPLUS_ERROR(Logger::getRoot(), "Error opening directory " << dir) ;
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL) {
        std::string file(dirp->d_name);
        if(file.find(myext) != std::string::npos)
            files.push_back(std::string(dirp->d_name));
    }
    closedir(dp);
    return 1;
}

int g_matToCimg(const cv::Mat& mat, CImg<unsigned char>& img)
{
    if(mat.channels() != 3)
        return -1;
    img.assign(mat.cols, mat.rows, 1, 3, 1);

    for(int i = 0; i<mat.rows; i++)
    {
        for(int j = 0; j<mat.cols; j++)
        {
            for(int k = 0; k<mat.channels(); k++)
            {
               img(j, i, k) = g_getPixel(mat, i, j)[2-k];
            }
        }
    }

    return 1;
}
}
