#include <string>

#ifndef VIDEOREADER_H
#define VIDEOREADER_H

namespace cv
{
    class Mat;
}

class VideoReader
{
public:
    virtual bool open(const std::string& name) = 0;
    virtual bool readFrame(cv::Mat& frame) = 0;
    virtual bool retrieve(cv::Mat& mat) = 0;
    virtual bool isOpen() = 0;
    virtual double getWidth() = 0;
    virtual double getHeight() = 0;
    virtual double getNextFrameIndex() = 0;
    virtual double getFormat() = 0;
    virtual double getCount() = 0;
    virtual double getFPS() = 0;
    virtual bool setNextFrameIndex(double index) = 0;
};

#endif          //VIDEOREADER_H
