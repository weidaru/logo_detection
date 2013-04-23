#include "VideoFileReader.h"

#include <log4cplus/logger.h>

#include <opencv/cv.h>

#include <string>

VideoFileReader::VideoFileReader()
{

}

VideoFileReader::VideoFileReader(const std::string& fName) : fileName(fName), cap(fName)
{

}

bool VideoFileReader::open(const std::string& name)
{
    return cap.open(name);
}

bool VideoFileReader::readFrame(cv::Mat& frame)
{
    cap>>frame;

    if(frame.empty())
    {
        LOG4CPLUS_INFO(log4cplus::Logger::getRoot(), "File " + fileName + " ends.");
        return false;
    }
    else
        return true;
}

bool VideoFileReader::retrieve(cv::Mat& mat)
 {
     return cap.retrieve(mat);
 }

bool VideoFileReader::isOpen()
{
    return cap.isOpened();
}

double VideoFileReader::getWidth()
{
    return cap.get(CV_CAP_PROP_FRAME_WIDTH );
}

double VideoFileReader::getHeight()
{
    return cap.get(CV_CAP_PROP_FRAME_HEIGHT );
}

double VideoFileReader::getNextFrameIndex()
{
    return cap.get(CV_CAP_PROP_POS_FRAMES);
}

double VideoFileReader::getFormat()
{
    return cap.get(CV_CAP_PROP_FORMAT);
}

double VideoFileReader::getCount()
{
    return cap.get(CV_CAP_PROP_FRAME_COUNT);
}

bool VideoFileReader::setNextFrameIndex(double index)
{
    return cap.set(CV_CAP_PROP_POS_FRAMES, index);
}

double VideoFileReader::getFPS()
{
    return cap.get(CV_CAP_PROP_FPS);
}
