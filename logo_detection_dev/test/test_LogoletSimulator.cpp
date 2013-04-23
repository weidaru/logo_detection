#ifdef UNIT_TEST

#include "UnitTest++.h"

#include "LogoletSimulator.h"
#include "utils.h"
#include "dataEntry.h"

#include <opencv/highgui.h>

TEST(Test_LogoletSimulator)
{
    cv::Mat img = cv::imread("./test/data/hrhb.jpg");
    LogoletSimulator ls(img, 12, 12);

    CHECK_EQUAL(71, ls.getColCount());
    CHECK_EQUAL(39, ls.getRowCount());

    LogoletSimulator::t_data_vector& datas = ls.getDataVector();
    LogoletSimulator::t_logolet_ptr l;
    for(int i = 0; i < (int)datas.size(); i++)
    {
        l = datas[i];
        if(l->rowIndex == 0 && l->colIndex == 34)
            break;
    }
    uchar* red = utils::g_getPixel(l->data, 0, 0);
    uchar* blue = utils::g_getPixel(l->data, l->width-1, l->height-1);
    CHECK_CLOSE(255, (int)red[2], 5);
    CHECK_CLOSE(0, (int)red[1], 5);
    CHECK_CLOSE(0, (int)red[0], 5);
    CHECK_CLOSE(0, (int)blue[2], 5);
    CHECK_CLOSE(0, (int)blue[1], 5);
    CHECK_CLOSE(255, (int)blue[0], 5);

    l = datas[(int)datas.size()-1];
    CHECK_EQUAL(6, l->width);
    CHECK_EQUAL(8, l->height);
    CHECK_EQUAL(38, l->rowIndex);
    CHECK_EQUAL(70, l->colIndex);
}

TEST(Test_Logolet2DataEntry)
{
    cv::Mat img = cv::imread("./test/data/hrhb.jpg");

    LogoletSimulator ls(img, 12, 12);
    LogoletSimulator::t_data_vector& datas = ls.getDataVector();
    LogoletSimulator::t_logolet_ptr l_ptr = datas[0];
    int size = 12*12*3;
    dataEntry* dEntry = l_ptr->toDataEntry(size, 1);
    double* vector = dEntry->pattern;
    double target = *dEntry->target;
    CHECK_EQUAL(target, 1);
    CHECK_CLOSE(255, vector[2],5);
    CHECK_CLOSE(0, vector[1],5);
    CHECK_CLOSE(0, vector[0],5);

    delete dEntry;
}

#endif
