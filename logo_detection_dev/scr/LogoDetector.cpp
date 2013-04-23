#include "LogoDetector.h"
#include "VideoFileReader.h"

#include <opencv/cv.h>

LogoDetector::LogoDetector(const std::string& fName) : video_p(new VideoFileReader(fName)), detectComplete(false)
{

}

LogoDetector::LogoDetector(std::tr1::shared_ptr<VideoReader> v_p) : video_p(v_p), detectComplete(false)
{

}

bool LogoDetector::isOpen()
{
    return video_p->isOpen();
}

void LogoDetector::load(const std::string& fName)
{
    video_p = std::tr1::shared_ptr<VideoReader>(new VideoFileReader(fName));
    detectComplete = false;
}
