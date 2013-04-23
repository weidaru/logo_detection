#include "LogoDetector.h"
#include "BayersClassifier.h"
#include "DfDetector.h"
#include "FeatureDetector.h"

#include <opencv/cv.h>

#include <tr1/memory>


#ifndef BAISCLOGODETECTOR_H
#define BAISCLOGODETECTOR_H

class LogoMatch;

//Refactor df detection into module
class BasicLogoDetector : public LogoDetector
{
public:
    BasicLogoDetector(const std::string& fName);
    virtual bool process(cv::Mat& des);
    virtual void detect();

    int getRectLineWidth();
    BasicLogoDetector& setRectLineWidth(int newValue);

private:
    void drawRect(cv::Mat& mat, cv::Rect& rect);
    void doMatch(t_result_vector& datas);

private:
    DfDetector dfDetector;
    FeatureDetector featureDetector;
    std::tr1::shared_ptr<LogoMatch> logoMatch_ptr;
    int rectLineWidth;
    bool isFirstMatch;

public:
    bool enableDfDetect;
    bool enableFeatureDetect;
    bool useAdvancedLogoMatch;
    int maxMatchIteration;
};

#endif          //BAISCLOGODETECTOR_H
