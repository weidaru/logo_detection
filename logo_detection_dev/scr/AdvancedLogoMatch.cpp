#include "AdvancedLogoMatch.h"
#include "utils.h"

using namespace cimg_library;

std::pair<std::string, double> AdvancedLogoMatch::match(const cv::Mat &src)
{
    CImg<unsigned char> img;
    utils::g_matToCimg(src, img);
    ulong64 hash;
    myph_dct_imagehash(img, hash);

    int bestMatch = 100000;
    std::string bestMatchName = "";
    std::map<std::string, ulong64>::iterator it;
    cv::MatND imgHistogram = getHistogram(src);
    for(it = hashDatas.begin(); it!=hashDatas.end(); it++)
    {
        int match = ph_hamming_distance(hash, it->second);
        double histogramMatch = cv::compareHist(imgHistogram,datas[it->first], CV_COMP_CORREL);
        if(bestMatch > match && histogramMatch > matchThreshold)
        {
            bestMatch = match;
            bestMatchName = it->first;
        }
    }


    if(bestMatch > matchDistanceThreshold )
        bestMatchName = "";
    std::pair<std::string, double> result(bestMatchName, (double)bestMatch);
    return result;
}

void AdvancedLogoMatch::addInstance(const std::string &srcName, const cv::Mat &src)
{
    LogoMatch::addInstance(srcName, src);
    CImg<unsigned char> img;
    utils::g_matToCimg(src, img);
    ulong64 hash;
    myph_dct_imagehash(img, hash);
    hashDatas[srcName] = hash;
}
