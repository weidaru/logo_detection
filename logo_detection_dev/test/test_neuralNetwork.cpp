#ifdef UNIT_TEST

#include "UnitTest++.h"

#include "neuralNetwork.h"
#include "dataEntry.h"
#include "InputData.h"

#include <vector>

TEST(neuralNetwork_io)
{

    InputData input;

    std::vector<dataEntry*> data;
    double *p = new double[3];
    p[0] = 1.0;
    p[2] = 2.0;
    p[3] = 3.0;
    double *t = new double;
    *t = 0.0;
    dataEntry *de = new dataEntry(p, t);
    data.push_back(de);
    input.loadData(data, 3, 1);

    //create neural network
    neuralNetwork nn(3, 2, 1);
	nn.setLearningParameters(0.01, 0.8);
	nn.setDesiredAccuracy(100);
	nn.setMaxEpochs(10);

	//dataset
	dataSet* dset;

	for ( int i=0; i < input.nDataSets(); i++ )
	{
		dset = input.getDataSet();
		nn.trainNetwork( dset->trainingSet, dset->generalizationSet, dset->validationSet );
	}
	nn.save("./test/data/saved_neuralnetwork.data");
	double* testinput = new double[3];
	testinput[0] = 3.0;
	testinput[1] = 4.0;
	testinput[3] = 5.0;
    double correctAnswer= nn.feedInput(testinput)[0];

    neuralNetwork nn_load(3, 2, 1);
    nn_load.load("./test/data/saved_neuralnetwork.data");
    double answer =  nn_load.feedInput(testinput)[0];
    CHECK_CLOSE(correctAnswer, answer, 0.00001);
}

#endif
