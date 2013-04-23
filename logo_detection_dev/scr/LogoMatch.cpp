#include "LogoMatch.h"

#include <log4cplus/logger.h>

using namespace log4cplus;

LogoMatch::LogoMatch() : matchThreshold(0.3)
{

}

std::pair<std::string, double> LogoMatch::match(const cv::Mat &src)
{
    cv::MatND srcHis = getHistogram(src);
    std::map<std::string, cv::MatND>::iterator it;
    double bestMatch = -10.0;
    std::string bestMatchName;
    for(it = datas.begin(); it != datas.end(); it++)
    {
        cv::MatND matchHis = (*it).second;
        double match = cv::compareHist(srcHis, matchHis, CV_COMP_CORREL);
        if(bestMatch < match)
        {
            bestMatch = match;
            bestMatchName = (*it).first;
        }
    }

    if(bestMatch < matchThreshold)
        bestMatchName = "";
    std::pair<std::string, double> result(bestMatchName, bestMatch);
    return result;
}

void LogoMatch::addInstance(const std::string &srcName, const cv::Mat &src)
{
    datas[srcName] = getHistogram(src);
}

cv::MatND LogoMatch::getHistogram(const cv::Mat &src)
{
    cv::Mat hsv;
    cv::cvtColor(src, hsv, CV_BGR2HSV);

    int hbins = 30, sbins = 32;
    int histSize[] = {hbins, sbins};
    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    cv::MatND hist;
    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 1};

    cv::calcHist( &hsv, 1, channels, cv::Mat(), // do not use mask
                  hist, 2, histSize, ranges,
                  true, // the histogram is uniform
                  false );
    return hist;
}

double LogoMatch::getMatchThreshold()
{
    return matchThreshold;
}

LogoMatch& LogoMatch::setMatchThreshold(double newValue)
{
    LOG4CPLUS_TRACE(Logger::getRoot(), "MatchThreshold set to new value: " <<newValue);
    matchThreshold = newValue;
    return *this;
}
