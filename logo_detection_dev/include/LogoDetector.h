#include "LogoletSimulator.h"

#include <tr1/memory>
#include <string>
#include <vector>

#include <opencv/cv.h>

#ifndef LOGODETECTOR_H
#define LOGODETECTOR_H
class VideoReader;

class LogoDetector
{
public:
    typedef LogoPosition Result;
    typedef std::tr1::shared_ptr<LogoPosition> t_result_ptr;
    typedef std::vector<t_result_ptr > t_result_vector;
public:
    LogoDetector(const std::string& fName);
    LogoDetector(std::tr1::shared_ptr<VideoReader> v_p);
    bool isOpen();
    void load(const std::string& fName);
    virtual bool process(cv::Mat& des) = 0;
    virtual void detect() = 0;

protected:
    std::tr1::shared_ptr<VideoReader> video_p;
    bool detectComplete;
};

#endif          //LOGODETECTOR_H
