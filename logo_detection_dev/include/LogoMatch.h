#include <map>
#include <string>
#include <utility>

#include <opencv/cv.h>

#ifndef LOGOMATCH_H
#define LOGOMATCH_H

class LogoMatch
{
public:
    LogoMatch();
    virtual std::pair<std::string, double> match(const cv::Mat &src);
    virtual void addInstance(const std::string &srcName, const cv::Mat &src);

    double getMatchThreshold();
    LogoMatch& setMatchThreshold(double newValue);

protected:
    cv::MatND getHistogram(const cv::Mat &src);

protected:
    std::map<std::string, cv::MatND> datas;
    double matchThreshold;
};

#endif          //LOGOMATCH_H
