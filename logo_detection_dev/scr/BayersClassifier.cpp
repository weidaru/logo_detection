#include "BayersClassifier.h"
#include "LogoletSimulator.h"
#include "VideoFileReader.h"
#include "globals.h"
#include "neuralNetwork.h"
#include "dataEntry.h"
#include "utils.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <log4cplus/logger.h>

#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <iostream>

using namespace log4cplus;
using namespace cv;
using namespace std::tr1;
using namespace globals;

BayersClassifier::BayersClassifier() : vecSize(logoletSize*logoletSize*3), network(vecSize, 20, 1), loadData(false)
{

}

void BayersClassifier::saveInput(const std::string& file)
{
    LOG4CPLUS_INFO(Logger::getRoot(), "Save input to file "<<file);
    if(loadData)
    {
        inputData.loadData(inputBuffer, vecSize, 1);
        loadData = false;
    }
    inputData.saveDataFile(file.c_str());
}

double BayersClassifier::makeClassification(Logolet& input)
{
    double* inputArray = input.toArray(vecSize);
    double* result = network.feedInput(inputArray);
    delete inputArray;

    return result[0];
}

void BayersClassifier::addInput(cv::Mat& m, double target)
{
    LogoletSimulator ls(m, 12, 12);
    LogoletSimulator::t_data_vector& datas = ls.getDataVector();
    LOG4CPLUS_TRACE(Logger::getRoot(), ""<<(int)datas.size()<<" input vector added.");
    if((int)datas.size() > 0)
        loadData = true;
    int sumSize = 12*12*3;
    for(int i = 0; i<(int)datas.size(); i++)
    {
        LogoletSimulator::t_logolet_ptr logolet = datas[i];
        int meanValue = utils::g_matSum(logolet->data) / sumSize;
        if(meanValue< 5 || meanValue>250)
            continue;
        dataEntry* dEntry = datas[i]->toDataEntry(vecSize, target);
        inputBuffer.push_back(dEntry);
    }
}

void BayersClassifier::addInput(const std::string& file, double target)
{
    LOG4CPLUS_TRACE(Logger::getRoot(), "Reading file "<<file);
    if(file.find(".jpg") != std::string::npos || file.find(".JPG") != std::string::npos)
    {
        cv::Mat img = cv::imread(file);
        addInput(img, target);
    }
    else if(file.find(".avi") != std::string::npos || file.find(".AVI") != std::string::npos)
    {
        VideoFileReader video(file);
        if(!video.isOpen())
        {
            LOG4CPLUS_ERROR(Logger::getRoot(), "Cannot open file " << file);
        }

        int count = video.getCount();
        srand(time(NULL));
        video.setNextFrameIndex(0);
        for(int i = 0; i< 6; i++)
        {
            cv::Mat mat;
            int r = rand() % (count-1);
            if(!video.setNextFrameIndex(r))
            {
                i--;
                continue;
            }
            video.readFrame(mat);
            addInput(mat, target);
        }
    }
    else
        LOG4CPLUS_ERROR(Logger::getRoot(), "File " <<file<<"is neither a jpg file or avi file. Unable to process.");
}

void BayersClassifier::loadNeuralNetwork(const std::string& file)
{
    LOG4CPLUS_INFO(Logger::getRoot(), "Load neural network from file " << file);
    network.load(file.c_str());
}
