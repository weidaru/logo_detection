#include "DfDetector.h"
#include "VideoFileReader.h"

#include <opencv/cv.h>

#include <log4cplus/logger.h>

#include <vector>
#include <sstream>

#include "utils.h"
#include "globals.h"
#include "LogoletSimulator.h"
#include "VideoReader.h"

using namespace log4cplus;
using namespace cv;
using namespace utils;
using namespace std::tr1;
using namespace globals;

DfDetector::DfDetector(const std::string& file) : LogoDetector(file), dfThreshold(60.0f), max_dfIteration(2000)
{

}

bool DfDetector::process(cv::Mat& des)
{
    return false;
}

void DfDetector::detect()
{
    LOG4CPLUS_INFO(Logger::getRoot(),
                   "Start frame difference based detection, it may take a couple of seconds, please be patient...");
    Mat frame, firstFrame;
    Mat r = cvCreateMat(video_p->getHeight(), video_p->getWidth(), CV_8UC1);
    for(int i = 0; i<r.rows; i++)
        for(int j = 0; j<r.cols; j++)
            *g_getPixel(r, i, j) = (uchar)1;

    video_p->setNextFrameIndex(0);
    video_p->readFrame(firstFrame);
    uchar firstFrameRGB[firstFrame.rows][firstFrame.cols][3];
    for(int i = 0; i<firstFrame.rows; i++)
    {
        for(int j = 0; j<firstFrame.cols; j++)
        {
            uchar* q = g_getPixel(firstFrame, i, j);
            firstFrameRGB[i][j][2] = q[2];
            firstFrameRGB[i][j][1] = q[1];
            firstFrameRGB[i][j][0] = q[0];
        }
    }
    int interval = video_p->getCount() / max_dfIteration +1;
    for(int k = 1; k<max_dfIteration ; k++)
    {
        video_p->setNextFrameIndex(k*interval);
        if(!video_p->readFrame(frame))
            break;

        for(int i = 0; i<frame.rows; i++)
        {
            for(int j = 0; j<frame.cols; j++)
            {
                uchar* p = g_getPixel(frame, i, j);

                if(p[2] + p[1] + p[0] < 10)
                    g_getPixel(r, i, j)[0] *= (uchar)0;
                if(std::abs(p[2]- firstFrameRGB[i][j][2])>= dfThreshold ||
                   std::abs(p[1]- firstFrameRGB[i][j][1])>= dfThreshold ||
                   std::abs(p[0]- firstFrameRGB[i][j][0])>= dfThreshold)
                    g_getPixel(r, i, j)[0] *= (uchar)0;
                else
                    g_getPixel(r, i, j)[0] *= (uchar)1;
            }
        }
    }
    g_saveMat(r, "./log/matrix/df_result.log");
    //poster check
    posterProcess(r);

    if((int)result.size() > 4 && !globals::enableDebug)
        result.clear();
    LOG4CPLUS_INFO(Logger::getRoot(),
                   "Frame difference based detection complete!" <<"Total "<<getLogoNumber()<<" logo detected.");
    for(int i = 0; i<(int)result.size(); i++)
    {
        Result& res = *result[i];
        LOG4CPLUS_DEBUG(Logger::getRoot(),
                        "Logo" << i <<": x="<<res.x<<"\ty="<<res.y<<"\twidth="<<res.width<<"\theight="<<res.height);
    }
}

void DfDetector::posterProcess(const cv::Mat& mat)
{
    LogoletSimulator ls(mat, logoletSize, logoletSize);
    LogoletSimulator::t_data_vector datas = ls.getDataVector();
    std::vector<int> logolets;

    for(int i = 0; i<(int)datas.size(); i++)
    {
        long sum = g_matSum(datas[i]->data);
        if(sum > 10)
        {
            logolets.push_back(i);
        }
    }
    LOG4CPLUS_DEBUG(Logger::getRoot(), "Total " << (int)logolets.size() << " logolets need processing.");
    if(!globals::enableDebug && logolets.size() > datas.size()/3)
        return;

    double distanceThreshold;
    int storageThreshold;
    if(globals::enableDebug)
    {
        distanceThreshold = 1;
        storageThreshold = 1;
    }
    else
    {
        distanceThreshold = 3.1;
        storageThreshold = 3;
    }
    while(!logolets.empty())
    {
        t_result_ptr res(new Result());
        std::vector<int> storage;
        int f = logolets[0];
        logolets.erase(logolets.begin());
        storage.push_back(f);

        for(int i = 0; i<(int)logolets.size(); i++)
        {
            int index = logolets[i];
            double minDistance = 10000.0;
            for(int j = 0; j<(int)storage.size(); j++)
            {
                int si= storage[j];
                double distance = sqrt(pow(datas[index]->rowIndex - datas[si]->rowIndex, 2) +
                                       pow(datas[index]->colIndex - datas[si]->colIndex, 2));
                if(minDistance > distance)
                    minDistance = distance;
            }
            if(minDistance < distanceThreshold)
            {
                storage.push_back(index);
                logolets.erase(logolets.begin() + i);
                i--;
            }
        }

        if((int)storage.size() >= storageThreshold)
        {
            int xMax = 0, yMax = 0, xMin = 10000, yMin = 10000;
            for(int i = 0; i<(int)storage.size(); i++)
            {
                int index = storage[i];
                if(xMax < datas[index]->x)
                    xMax = datas[index]->x;
                if(xMin > datas[index]->x)
                    xMin = datas[index]->x;
                if(yMax < datas[index]->y)
                    yMax = datas[index]->y;
                if(yMin > datas[index]->y)
                    yMin = datas[index]->y;
            }
            res->x = xMin;
            res->y = yMin;
            res->width = xMax - xMin + logoletSize;
            res->height = yMax - yMin + logoletSize;
            if(res->x+res->width > video_p->getWidth())
                res->width = video_p->getWidth() - res->x;
            if(res->y+res->height > video_p->getHeight())
                res->height = video_p->getHeight() - res->y;
            bool satisfied = true;
            if(!globals::enableDebug &&
                (res->width<2*logoletSize ||
                res->height<2*logoletSize))
                satisfied = false;
            if(res->width > video_p->getWidth()/3 ||
                res->height >video_p->getHeight()/3 )
                satisfied = false;
            if(satisfied)
                result.push_back(res);
        }
    }
}

double DfDetector::getDfThreshold()
{
    return dfThreshold;
}

DfDetector& DfDetector::setDfThreshold(double newValue)
{
    dfThreshold = newValue;
    LOG4CPLUS_TRACE(Logger::getRoot(), "dfThreshold set to new value: " <<newValue);
    return *this;
}

int DfDetector::getMax_dfIteration()
{
    return max_dfIteration;
}

DfDetector& DfDetector::setMax_dfIteration(int newValue)
{
    max_dfIteration = newValue;
    LOG4CPLUS_TRACE(Logger::getRoot(), "max_dfIteration set to new value: " << newValue);
    return *this;
}

int DfDetector::getLogoNumber()
{
    return (int)result.size();
}

DfDetector::t_result_vector& DfDetector::getResult()
{
    return result;
}

