#include <opencv/cv.h>

#include <tr1/memory>
#include <vector>

#include "LogoDetector.h"

#ifndef DFDETECTOR_H
#define DFDETECTOR_H

namespace cv
{
    class Mat;
}

class DfDetector: public LogoDetector
{
public:
    DfDetector(const std::string& file);
    virtual void detect();
    virtual bool process(cv::Mat& des);

    double getDfThreshold();
    DfDetector& setDfThreshold(double newValue);
    int getMax_dfIteration();
    DfDetector& setMax_dfIteration(int newValue);
    int getLogoNumber();
    t_result_vector& getResult();

private:
    void posterProcess(const cv::Mat& mat);

private:
    double dfThreshold;
    int max_dfIteration;
    t_result_vector result;
};

#endif              //DFDETECTOR_H
