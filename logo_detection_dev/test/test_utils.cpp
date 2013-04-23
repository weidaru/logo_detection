#ifdef UNIT_TEST

#include "UnitTest++.h"

#include "utils.h"
#include "pHash.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include <log4cplus/logger.h>

using namespace utils;
using namespace cv;
using namespace log4cplus;
using namespace cimg_library;
using namespace std;

TEST(Test_g_getPixel)
{
    Mat img = imread("./test/data/hrhb.jpg");
    uchar* red = g_getPixel(img, 10,10);
    uchar* blue = g_getPixel(img, img.rows-10, img.cols-10);

    CHECK_CLOSE(255, (int)red[2], 5);
    CHECK_CLOSE(0, (int)red[1], 5);
    CHECK_CLOSE(0, (int)red[0], 5);
    CHECK_CLOSE(0, (int)blue[2], 5);
    CHECK_CLOSE(0, (int)blue[1], 5);
    CHECK_CLOSE(255, (int)blue[0], 5);
}

TEST(Test_g_matSum)
{
    Mat m;
    m.create(10, 10, CV_8UC1);
    for(int i = 0; i<10; i++)
    {
        for(int j = 0; j<10; j++)
        {
            *g_getPixel(m, i, j) = 2;
        }
    }
    *g_getPixel(m, 3,5) = 4;

    long result = g_matSum(m);
    CHECK_EQUAL(202, result);
}

TEST(Test_PrivateProfile)
{
    std::string str = readStringFromPrivateProfile("Test", "string", "./test/data/testINI.ini");
    int i = readIntegerFromPrivateProfile("Test", "int", "./test/data/testINI.ini");
    double d = readDoubleFromPrivateProfile("Test", "double", "./test/data/testINI.ini");

    CHECK_EQUAL("Hello", str);
    CHECK_EQUAL(10, i);
    CHECK_CLOSE(13.3, d, 0.00001);

    setPrivateProfileString("Test", "string", "World", "./test/data/testINI.ini");
    CHECK_EQUAL("World", readStringFromPrivateProfile("Test", "string", "./test/data/testINI.ini"));
    setPrivateProfileString("Test", "string", "Hello", "./test/data/testINI.ini");
}

TEST(Test_getDir)
{
    std::string dirName = "./test/data/dir/", extention = ".txt";
    std::vector<std::string> files;
    getDir(dirName, extention, files);
    for(int i = 0; i<(int)files.size(); i++)
    {
        LOG4CPLUS_DEBUG(Logger::getRoot(), "File " << files[i] <<" in "<<dirName);
    }

    CHECK_EQUAL(4, (int)files.size());
}

TEST(Test_g_matToCimgpHash)
{
    Mat mat = imread("./test/data/logo1.jpg");
    CImg<unsigned char> readImg("./test/data/logo1.jpg");
    CImg<unsigned char> convertImg;
    assert(g_matToCimg(mat, convertImg) != -1);
    double result = 0.0;
    _ph_compare_images(readImg, convertImg, result, 3.5, 1.0, 180, 0.9);
    CHECK_CLOSE(1.0, result, 0.001);
}

#endif          //UNIT_TEST
