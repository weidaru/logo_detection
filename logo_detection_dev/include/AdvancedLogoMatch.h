#include <map>
#include <string>
#include <utility>

#include "pHash.h"
#include "LogoMatch.h"

#ifndef ADVANCEDLOGOMATCH_H
#define ADVANCEDLOGOMATCH_H

class AdvancedLogoMatch : public LogoMatch
{
public:
    virtual std::pair<std::string, double> match(const cv::Mat &src);
    virtual void addInstance(const std::string &srcName, const cv::Mat &src);

public:
    int matchDistanceThreshold;
private:
    std::map<std::string, ulong64> hashDatas;
};

#endif              //ADVANCEDLOGOMATCH_H
