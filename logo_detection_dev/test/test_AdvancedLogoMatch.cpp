#ifdef UNIT_TEST

#include "UnitTest++.h"

#include "AdvancedLogoMatch.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <log4cplus/logger.h>
#include <utility>
#include <string>

using namespace log4cplus;

TEST(Test_AdvancedLogoMatch)
{
    AdvancedLogoMatch alm;
    cv::Mat mat;
    alm.setMatchThreshold(0.2);
    alm.matchDistanceThreshold = 30;
    mat = cv::imread("./test/data/logo1.jpg");
    alm.addInstance("Logo1", mat);

    mat = cv::imread("./test/data/logo2.jpg");
    alm.addInstance("Logo2", mat);

    mat = cv::imread("./test/data/logo3.jpg");
    alm.addInstance("Logo3", mat);

    mat = cv::imread("./test/data/testLogo.jpg");

    std::pair<std::string, double> result = alm.match(mat);
    LOG4CPLUS_INFO(Logger::getRoot(), "Best match is "<<result.first<<". Match value = "<<result.second);
    CHECK_EQUAL("Logo2", result.first);
}

#endif
