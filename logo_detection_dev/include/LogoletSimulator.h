#include <opencv/cv.h>

#include <vector>
#include <tr1/memory>

#ifndef LOGOLETSIMULATOR_H
#define LOGOLETSIMULATOR_H

class dataEntry;

struct Logolet
{
    dataEntry* toDataEntry(int size, double target);
    double* toArray(int size);
    int x, y ,width, height, rowIndex, colIndex;
    cv::Mat data;
};

struct LogoPosition
{
    LogoPosition(): x(0), y(0), width(0), height(0) {}
    int x, y, width, height;
};

class LogoletSimulator
{
public:
    typedef std::tr1::shared_ptr<Logolet> t_logolet_ptr;
    typedef std::vector<t_logolet_ptr > t_data_vector;

public:
    LogoletSimulator(cv::Mat m, int w, int h);

    LogoletSimulator& setSource(cv::Mat);
    cv::Mat& getSource();
    t_data_vector& getDataVector();
    int getWidth();
    int getHeight();
    int getRowCount();
    int getColCount();

private:
    t_data_vector datas;
    cv::Mat source;
    int width, height;
};

#endif          //LOGOLETSIMULATOR_H
