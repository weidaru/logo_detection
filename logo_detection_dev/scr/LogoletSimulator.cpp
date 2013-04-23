#include "LogoletSimulator.h"
#include "dataEntry.h"

#include "utils.h"

#include <log4cplus/logger.h>
#include <iostream>

using namespace std::tr1;
using namespace utils;
using namespace log4cplus;

double* Logolet::toArray(int size)
{

    if(size%3 != 0)
    {
        LOG4CPLUS_ERROR(Logger::getRoot(), "Bad size "<<size<<" in Logolet::toArray.");
        return NULL;
    }
    double* result = new double[size];
    int count = 0;
    for(int i = 0; i<height; i++)
    {
        for(int j = 0; j<width; j++)
        {
            result[3*count] = (double)g_getPixel(data, i, j)[0];
            result[3*count+1] = (double)g_getPixel(data, i, j)[1];
            result[3*count+2] = (double)g_getPixel(data, i, j)[2];
            count++;
        }
    }
    while(count < size/3)
    {
        result[3*count] = 0.0;
        result[3*count+1] = 0.0;
        result[3*count+2] = 0.0;
        count++;
    }
    return result;
}

dataEntry* Logolet::toDataEntry(int size, double target)
{
    double *pattern = toArray(size), *tar = new double;
    *tar = target;
    dataEntry* result = new dataEntry(pattern, tar);
    return result;
}

LogoletSimulator::LogoletSimulator(cv::Mat m, int w, int h)
    : width(w), height(h)
{
    setSource(m);
}

LogoletSimulator::t_data_vector& LogoletSimulator::getDataVector()
{
    return datas;
}

LogoletSimulator& LogoletSimulator::setSource(cv::Mat m)
{
    datas.clear();
    source = m;
    int sWidth= source.cols, sHeight = source.rows;
    int rc = 0, cc = 0;
    bool endLoop = false;

    while(!endLoop)
    {
        if(sWidth - width * cc<=width && sHeight - height * rc<=height)
            endLoop = true;
        shared_ptr<Logolet> l(new Logolet());

        l->x = cc*width;
        l->y = rc*height;
        l->rowIndex = rc;
        l->colIndex = cc;
        if(sHeight - height * rc<height)
            l->height = sHeight- height*rc;
        else
            l->height = height;
        if(sWidth - width * cc<width)
        {
            l->width = sWidth - width * cc;
            cc = 0;
            rc ++;
        }
        else
            l->width = width;

        cc++;

        l->data = cv::Mat(source, cv::Rect(l->x, l->y ,l->width, l->height));

        datas.push_back(l);
    }
    return *this;
}

cv::Mat& LogoletSimulator::getSource()
{
    return source;
}

int LogoletSimulator::getWidth()
{
    return width;
}

int LogoletSimulator::getHeight()
{
    return height;
}

int LogoletSimulator::getRowCount()
{
    if(source.rows % height == 0)
        return source.rows / height;
    else
        return source.rows / height +1;
}

int LogoletSimulator::getColCount()
{
    if(source.cols % width == 0)
        return source.cols / width;
    else
        return source.cols / width +1;
}
