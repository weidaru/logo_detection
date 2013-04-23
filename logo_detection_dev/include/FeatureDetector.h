#include "BayersClassifier.h"
#include "LogoDetector.h"

#include <string>

#ifndef FEATUREDETECTOR_H
#define FEATUREDETECTOR_H

class FeatureDetector : public LogoDetector
{
public:
    FeatureDetector(const std::string& file);

    virtual void detect();
    virtual bool process(cv::Mat& mat);

    BayersClassifier& getBayersClassifier();
    t_result_vector& getResult();
    int getLogoNumber();
    int getMaxFrameSearch();
    FeatureDetector& setMaxFrameSearch(int newValue);

private:
    BayersClassifier bClassifier;
    t_result_vector result;
    int maxFrameSearch;
};

#endif              //FEATUREDETECTOR_H
