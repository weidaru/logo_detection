#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>

#include "globals.h"
#include "utils.h"

#ifdef UNIT_TEST
    #include <UnitTest++.h>
#else
    #include "BasicLogoDetector.h"
    #include <opencv/highgui.h>
#endif

using namespace log4cplus;


int main(int, char**)
{

    FILE* f =  fopen("README_CH.txt", "r");
    if(f == NULL)
    {
        int a = 0;
        a++;
    }


    /***************Configuration****************/
//configure log4cplus
    PropertyConfigurator::doConfigure("./config/log_config.properties");

    /************ini File Reader************/
    globals::enableDebug = utils::readIntegerFromPrivateProfile("Globals", "enableDebug", "./config/config.ini");

    /************Run UnitTest*************/
#ifdef UNIT_TEST
    return UnitTest::RunAllTests();
#else

    /*****************************ini File Reader Continue****************************/
    globals::logoletSize = utils::readIntegerFromPrivateProfile("Globals", "logoletSize", "./config/config.ini");
    globals::mode = utils::readIntegerFromPrivateProfile("Globals", "mode", "./config/config.ini");

    std::string logoimgs_directory = utils::readStringFromPrivateProfile("Directory", "logoimgs_directory", "./config/config.ini");
    std::string nonlogo_video_directory = utils::readStringFromPrivateProfile("Directory", "nonlogo_video_directory", "./config/config.ini");
    std::string nonlogo_image_directory = utils::readStringFromPrivateProfile("Directory", "nonlogo_image_directory", "./config/config.ini");
    std::string logobase_directory = utils::readStringFromPrivateProfile("Directory", "logobase_directory", "./config/config.ini");

    bool enableDfDetect = utils::readIntegerFromPrivateProfile("Detector", "enableDfDetect", "./config/config.ini");
    bool enableFeatureDetect = utils::readIntegerFromPrivateProfile("Detector", "enableFeatureDetect", "./config/config.ini");

    /***********GUI & Detector***********/
    if(globals::mode == globals::Detect)
    {
        BasicLogoDetector detector(utils::readStringFromPrivateProfile("Detector", "inputFile", "./config/config.ini"));
        detector.setRectLineWidth(utils::readIntegerFromPrivateProfile("Detector", "rectLineWidth", "./config/config.ini"));
        detector.enableDfDetect = enableDfDetect;
        detector.enableFeatureDetect = enableFeatureDetect;
        detector.maxMatchIteration = utils::readIntegerFromPrivateProfile("Detector", "maxMatchIteration", "./config/config.ini");
        detector.useAdvancedLogoMatch = utils::readIntegerFromPrivateProfile("Detector", "useAdvancedLogoMatch", "./config/config.ini");
        if(!detector.isOpen())
            return -1;
        detector.detect();

        cv::namedWindow("Logo Detection",1);
        for(;;)
        {
            cv::Mat frame;
            if(!detector.process(frame))
                break;
            cv::imshow("Logo Detection", frame);
            if(cv::waitKey(30) >= 0) break;
        }
    }
    else if(globals::mode == globals::DataFormat)
    {
        BayersClassifier bClassifier;
        std::string neuralNetworkFile = utils::readStringFromPrivateProfile("Bayers", "neuralNetworkFile ", "./config/config.ini");

        std::vector<std::string> logoImgFiles;
        std::vector<std::string> logobaseFiles;
        std::vector<std::string> nonlogoImageFiles;
        std::vector<std::string> nonlogoVideoFiles;

        utils::getDir(logoimgs_directory, ".jpg", logoImgFiles);
        utils::getDir(logoimgs_directory, ".JPG", logoImgFiles);
        utils::getDir(logobase_directory, ".jpg", logobaseFiles);
        utils::getDir(logobase_directory, ".JPG", logobaseFiles);

        utils::getDir(nonlogo_image_directory, ".jpg", nonlogoImageFiles);
        utils::getDir(nonlogo_image_directory, ".JPG", nonlogoImageFiles);
        utils::getDir(nonlogo_video_directory, ".avi", nonlogoVideoFiles);
        utils::getDir(nonlogo_video_directory, ".AVI", nonlogoVideoFiles);

        for(int i = 0; i<(int)logoImgFiles.size(); i++)
            bClassifier.addInput(logoimgs_directory + logoImgFiles[i], 1);
        for(int i = 0; i<(int)logobaseFiles.size(); i++)
            bClassifier.addInput(logobase_directory + logobaseFiles[i], 1);
        for(int i = 0; i<(int)nonlogoImageFiles.size(); i++)
            bClassifier.addInput(nonlogo_image_directory + nonlogoImageFiles[i], 0);
        for(int i = 0; i<(int)nonlogoVideoFiles.size(); i++)
            bClassifier.addInput(nonlogo_video_directory +nonlogoVideoFiles[i], 0);
        bClassifier.saveInput(utils::readStringFromPrivateProfile("Bayers", "inputDataFile", "./config/config.ini"));
    }

    return 0;
#endif
}
