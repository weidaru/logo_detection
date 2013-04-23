#include "BasicLogoDetector.h"
#include "VideoFileReader.h"
#include "LogoMatch.h"
#include "AdvancedLogoMatch.h"

#include "utils.h"
#include "globals.h"

#include <opencv/cv.h>

#include <log4cplus/logger.h>

#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace log4cplus;
using namespace utils;

BasicLogoDetector::BasicLogoDetector(const std::string& fName)
    : LogoDetector(fName), dfDetector(fName), featureDetector(fName), rectLineWidth(2), isFirstMatch(true),
      enableDfDetect(true), enableFeatureDetect(true), maxMatchIteration(100)
{
    dfDetector.setDfThreshold(readDoubleFromPrivateProfile("Detector", "dfThreshold", "./config/config.ini"));
    dfDetector.setMax_dfIteration(readIntegerFromPrivateProfile("Detector", "max_dfIteration", "./config/config.ini"));
    featureDetector.setMaxFrameSearch(readIntegerFromPrivateProfile("Detector", "maxFrameSearch", "./config/config.ini"));
}

bool BasicLogoDetector::process(cv::Mat& des)
{
    //Logo Detection
    if(!detectComplete)
    {
        detect();
        video_p->setNextFrameIndex(0);
    }

    //Poster video process
    cv::Mat frame;
    bool success = video_p->readFrame(frame);
    if(!success)
        return false;

    des = frame;
    //Draw selection rectangle
    if(dfDetector.getLogoNumber() > 0)
    {
        DfDetector::t_result_vector& results = dfDetector.getResult();
        for(int i = 0; i<(int)results.size(); i++)
        {
            DfDetector::t_result_ptr r_p = results[i];
            int x = r_p->x, y = r_p->y, width = r_p->width, height = r_p->height;
            cv::Rect rect(x, y, width, height);
            drawRect(des, rect);
        }
    }
    if(featureDetector.getLogoNumber() > 0 )
    {
        FeatureDetector::t_result_vector& results = featureDetector.getResult();
        for(int i = 0; i<(int)results.size(); i++)
        {
            FeatureDetector::t_result_ptr r_p = results[i];
            int x = r_p->x, y = r_p->y, width = r_p->width, height = r_p->height;
            cv::Rect rect(x, y, width, height);
            drawRect(des, rect);
        }
    }

    return success;
}

void BasicLogoDetector::drawRect(cv::Mat& mat, cv::Rect& rect)
{
    cv::Mat roi = mat(rect);
    for(int i = 0; i<roi.rows; i++)
    {
        for(int j = 0; j<roi.cols; j++)
        {
            if(i <= rectLineWidth-1 || i >= roi.rows-rectLineWidth||
                    j <= rectLineWidth-1 || j >= roi.cols-rectLineWidth)
            {
                uchar* pixel = g_getPixel(roi, i, j);
                pixel[0] = 0;
                pixel[1] = 255;
                pixel[2] = 0;
            }
        }
    }
}

void BasicLogoDetector::detect()
{
    LOG4CPLUS_INFO(Logger::getRoot(), "Start detecting...");
    if(enableDfDetect)
    {
        dfDetector.detect();
    }
    if(dfDetector.getLogoNumber() == 0 && enableFeatureDetect)
    {
        featureDetector.getBayersClassifier().loadNeuralNetwork(readStringFromPrivateProfile("Bayers", "neuralNetworkFile", "./config/config.ini"));
        featureDetector.detect();
    }
    if(dfDetector.getLogoNumber() > 0 && !globals::enableDebug)
    {
        doMatch(dfDetector.getResult());
    }
    if(featureDetector.getLogoNumber() > 0 && !globals::enableDebug)
    {
        doMatch(featureDetector.getResult());
    }
    detectComplete = true;

    video_p->setNextFrameIndex(0);
}

int BasicLogoDetector::getRectLineWidth()
{
    return rectLineWidth;
}

BasicLogoDetector& BasicLogoDetector::setRectLineWidth(int newValue)
{
    LOG4CPLUS_TRACE(Logger::getRoot(), "rectLineWidth set to new value: " <<newValue);
    rectLineWidth = newValue;
    return *this;
}

void BasicLogoDetector::doMatch(t_result_vector& datas)
{
    if(useAdvancedLogoMatch)
        LOG4CPLUS_INFO(Logger::getRoot(),"Start advanced matching...");
    else
        LOG4CPLUS_INFO(Logger::getRoot(),"Start normal matching...");
    if(isFirstMatch)
    {
        if(!useAdvancedLogoMatch)
            logoMatch_ptr = std::tr1::shared_ptr<LogoMatch>(new LogoMatch());
        else
            logoMatch_ptr = std::tr1::shared_ptr<LogoMatch>(new AdvancedLogoMatch());
        LogoMatch& logoMatch = *logoMatch_ptr;
        if(useAdvancedLogoMatch)
        {
            logoMatch.setMatchThreshold(readDoubleFromPrivateProfile("Detector", "matchThreshold", "./config/config.ini"));
            dynamic_cast<AdvancedLogoMatch&>(logoMatch).matchDistanceThreshold =
                                readDoubleFromPrivateProfile("Detector", "matchDistanceThreshold", "./config/config.ini");
        }
        else
            logoMatch.setMatchThreshold(readDoubleFromPrivateProfile("Detector", "matchThreshold", "./config/config.ini"));
        std::vector<std::string> logobaseFiles;
        std::string logobaseDirName = readStringFromPrivateProfile("Directory", "logobase_directory", "./config/config.ini");
        utils::getDir(logobaseDirName, ".jpg", logobaseFiles);
        utils::getDir(logobaseDirName, ".JPG", logobaseFiles);
        LOG4CPLUS_INFO(Logger::getRoot(), "Totally " << logobaseFiles.size() << " logos adding to LogoBase.");
        for(int i = 0; i<(int)logobaseFiles.size(); i++)
        {
            cv::Mat logo = cv::imread(logobaseDirName + logobaseFiles[i]);
            std::string logoName = logobaseFiles[i].substr(0, (int)logobaseFiles[i].length() - 4);
            logoMatch.addInstance(logoName, logo);
        }
        isFirstMatch = false;
    }

    cv::Mat frame;
    int interval = video_p->getCount() / maxMatchIteration + 1;
    std::map<std::string, double> resultMap;
    for(int i = 0; i<(int)datas.size(); i++)
    {
        DfDetector::t_result_ptr r_p = datas[i];
        int x = r_p->x, y = r_p->y, width = r_p->width, height = r_p->height;
        for(int j = 0; j<maxMatchIteration; j++)
        {
            video_p->setNextFrameIndex(j*interval);
            if(!video_p->readFrame(frame))
                break;
            cv::Mat src = frame(cv::Rect(x, y, width, height));
            std::pair<std::string, double> matchPair = logoMatch_ptr->match(src);
            if((int)matchPair.first.length() > 0)
            {
                if(resultMap.find(matchPair.first) == resultMap.end())
                    resultMap[matchPair.first] = matchPair.second;
                else if(resultMap[matchPair.first] > matchPair.second)
                    resultMap[matchPair.first] = matchPair.second;
            }
        }

        if((int)resultMap.size() > 0)
        {
            std::map<std::string, double>::iterator it;
            for(it = resultMap.begin(); it !=resultMap.end(); it++)
            {
                LOG4CPLUS_INFO(Logger::getRoot(),
                               "One match of logo locating at (" <<x<<", "<<y<<") is " <<it->first<<". Match distance = "<<it->second );
            }
        }
        else
            LOG4CPLUS_INFO(Logger::getRoot(),
                           "There is no match of logo locating at (" <<x<<", "<<y<<") in our logo base.");
    }
    LOG4CPLUS_INFO(Logger::getRoot(), "Match complete!");
}
