#include "VideoReader.h"

#include <opencv/highgui.h>

#include <iostream>
#include <string>

#ifndef VIDEOFILEREADER_H
#define VIDEOFILEREADER_H

class VideoFileReader : public VideoReader
{
public:
    VideoFileReader();
    VideoFileReader(const std::string& fName);
    virtual bool open(const std::string& name);
    virtual bool readFrame(cv::Mat& frame);
     virtual bool retrieve(cv::Mat& mat);
    virtual bool isOpen();
    virtual double getWidth();
    virtual double getHeight();
    virtual double getNextFrameIndex();
    virtual double getFormat();
    virtual double getCount();
    virtual double getFPS();
    virtual bool setNextFrameIndex(double index);

private:
    std::string fileName;
    cv::VideoCapture cap;

};

#endif          //VIDEOFILEREADER_h
