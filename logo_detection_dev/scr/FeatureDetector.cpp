#include "FeatureDetector.h"
#include "VideoFileReader.h"

#include "globals.h"

#include <log4cplus/logger.h>

using namespace log4cplus;
using namespace globals;

FeatureDetector::FeatureDetector(const std::string& file) : LogoDetector(file), maxFrameSearch(1000)
{

}

BayersClassifier& FeatureDetector::getBayersClassifier()
{
    return bClassifier;
}

void FeatureDetector::detect()
{
    LOG4CPLUS_INFO(Logger::getRoot(),
                   "Start feature based detection... ");
    cv::Mat frame;

    //Select the best frame for detecting
    video_p->setNextFrameIndex(0);
    int count = 0;
    std::map<int, int> logoletMap;
    int interval = video_p->getCount() / maxFrameSearch+1;
    for(int k =0;  count<maxFrameSearch; k++)
    {
        video_p->setNextFrameIndex(k*interval);
        if(!video_p->readFrame(frame))
            break;

        LogoletSimulator ls(frame, logoletSize, logoletSize);
        LogoletSimulator::t_data_vector& datas = ls.getDataVector();
        std::vector<int> logolets;

        for(int i = 0; i<(int)datas.size(); i++)
        {
            double p = bClassifier.makeClassification(*datas[i]);           //TODO: Consider only the color feature for now, position feature need future coding.
            int x = datas[i]->x, y = datas[i]->y;
            if(x > video_p->getWidth()/5 && x<video_p->getWidth()/5*4)
                p*= 0.3;
            if(x <= logoletSize || x>=video_p->getWidth()-logoletSize)
                p*= 0.3;
            if(y <= logoletSize || y>=video_p->getWidth()-logoletSize)
                p*= 0.3;
            if(y > video_p->getHeight()/6)
                p*= 0.3;
            if(p > 0.4)
            {
                if(logoletMap.find(i) == logoletMap.end())
                    logoletMap[i] = 1;
                else
                    logoletMap[i] += 1;
            }
        }
        count++;
    }

    video_p->setNextFrameIndex(0);
    video_p->readFrame(frame);
    LogoletSimulator ls(frame, logoletSize, logoletSize);
    LogoletSimulator::t_data_vector& datas = ls.getDataVector();
    std::vector<int> logolets;
    std::map<int, int>::iterator it;
    int meanCount = 0;
    for(it = logoletMap.begin(); it!=logoletMap.end(); it++)
    {
        meanCount += (*it).second;
    }
    if(!(int)logoletMap.empty())
        meanCount = meanCount / (int)logoletMap.size();
    for(it = logoletMap.begin(); it!=logoletMap.end(); it++)
    {
        if((*it).second >= (count+meanCount)/4)
            logolets.push_back((*it).first);
    }

    double distanceThreshold;
    int storageThreshold;
    if(globals::enableDebug)
    {
        distanceThreshold = 1;
        storageThreshold = 1;
    }
    else
    {
        distanceThreshold = 3.1;
        storageThreshold = 3;
    }
    //Combination of logolets TODO:: Improve the performance of combination
    LOG4CPLUS_DEBUG(Logger::getRoot(), "Total " << (int)logolets.size() << " logolets need processing.");
    while(!logolets.empty())
    {
        t_result_ptr res(new Result());
        std::vector<int> storage;
        int f = logolets[0];
        logolets.erase(logolets.begin());
        storage.push_back(f);

        for(int i = 0; i<(int)logolets.size(); i++)
        {
            int index = logolets[i];
            double minDistance = 10000.0;
            for(int j = 0; j<(int)storage.size(); j++)
            {
                int si= storage[j];
                double distance = sqrt(pow(datas[index]->rowIndex - datas[si]->rowIndex, 2) +
                                       pow(datas[index]->colIndex - datas[si]->colIndex, 2));
                if(minDistance > distance)
                    minDistance = distance;
            }
            if(minDistance < distanceThreshold)
            {
                storage.push_back(index);
                logolets.erase(logolets.begin() + i);
                i--;
            }
        }

        if((int)storage.size() >= storageThreshold)
        {
            int xMax = 0, yMax = 0, xMin = 10000, yMin = 10000;
            for(int i = 0; i<(int)storage.size(); i++)
            {
                int index = storage[i];
                if(xMax < datas[index]->x)
                    xMax = datas[index]->x;
                if(xMin > datas[index]->x)
                    xMin = datas[index]->x;
                if(yMax < datas[index]->y)
                    yMax = datas[index]->y;
                if(yMin > datas[index]->y)
                    yMin = datas[index]->y;
            }
            res->x = xMin;
            res->y = yMin;
            res->width = xMax - xMin + logoletSize;
            res->height = yMax - yMin + logoletSize;
            if(res->x+res->width > video_p->getWidth())
                res->width = video_p->getWidth() - res->x;
            if(res->y+res->height > video_p->getHeight())
                res->height = video_p->getHeight() - res->y;
            if(res->width < video_p->getWidth()/2 && res->height < video_p->getHeight()/5)
                result.push_back(res);
        }
    }
    if((int)result.size() > 4 && !globals::enableDebug)
        result.clear();
    LOG4CPLUS_INFO(Logger::getRoot(),
                   "Feature based detection complete!" <<"Total "<<getLogoNumber()<<" logo detected.");
    for(int i = 0; i<(int)result.size(); i++)
    {
        Result& res = *result[i];
        LOG4CPLUS_DEBUG(Logger::getRoot(),
                        "Logo" << i <<": x="<<res.x<<"\ty="<<res.y<<"\twidth="<<res.width<<"\theight="<<res.height);
    }
}

bool FeatureDetector::process(cv::Mat& mat)
{
    return false;
}

FeatureDetector::t_result_vector& FeatureDetector::getResult()
{
    return result;
}

int FeatureDetector::getLogoNumber()
{
    return (int)result.size();
}

int FeatureDetector::getMaxFrameSearch()
{
    return maxFrameSearch;
}

FeatureDetector& FeatureDetector::setMaxFrameSearch(int newValue)
{
    LOG4CPLUS_TRACE(Logger::getRoot(), "maxFrameSearch set to new value: " << newValue);
    maxFrameSearch = newValue;
    return *this;
}
