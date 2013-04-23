#include <string>
#include <tr1/memory>

#include "neuralNetwork.h"
#include "InputData.h"


#ifndef BAYERSCLASSIFIER_H
#define BAYERSCLASSIFIER_H

class Logolet;
class dataEntry;

namespace cv
{
    class Mat;
}

class BayersClassifier
{
public:
    BayersClassifier();

    double makeClassification(Logolet& input);
    void addInput(cv::Mat& m, double target);
    void addInput(const std::string& file, double target);
    void saveInput(const std::string& file);
    void loadNeuralNetwork(const std::string& file);

private:
    int vecSize;
    neuralNetwork network;
    InputData inputData;
    std::vector<dataEntry*> inputBuffer;
    bool loadData;
    bool needTraining;
};

#endif      //BAYERSCLASSIFIER_H
