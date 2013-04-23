#ifndef NNETWORK
#define NNETWORK

//standard libraries
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>

//custom includes
#include "dataEntry.h"

using namespace std;

//Constant Defaults!
#define LEARNING_RATE 0.001
#define MOMENTUM 0.9
#define MAX_EPOCHS 1500
#define DESIRED_ACCURACY 90

/*******************************************************************************************************************
 *	NEURAL NETWORK CLASS
 *	----------------------------------------------------------------------------------------------------------------
 *	Classic Back-propagation Neural Network ( makes use of gradient descent )
 *	Can toggle between stochastic and batch learning
 *	----------------------------------------------------------------------------------------------------------------
 *	Author: Bobby Anguelov (banguelov@cs.up.ac.za)
 *	Downloaded From: takinginitiative.wordpress.co.za
 *******************************************************************************************************************/

class neuralNetwork
{

//private members
//----------------------------------------------------------------------------------------------------------------
private:

    //learning parameters
    double learningRate;					// adjusts the step size of the weight update
    double momentum;						// improves performance of stochastic learning (don't use for batch)

    //number of neurons
    int nInput, nHidden, nOutput;

    //neurons
    double* inputNeurons;
    double* hiddenNeurons;
    double* outputNeurons;

    //weights
    double** wInputHidden;
    double** wHiddenOutput;

    //epoch counter
    long epoch;
    long maxEpochs;

    //accuracy required
    double desiredAccuracy;

    //change to weights
    double** deltaInputHidden;
    double** deltaHiddenOutput;

    //error gradients
    double* hiddenErrorGradients;
    double* outputErrorGradients;

    //accuracy stats per epoch
    double trainingSetAccuracy;
    double validationSetAccuracy;
    double generalizationSetAccuracy;
    double trainingSetMSE;
    double validationSetMSE;
    double generalizationSetMSE;

    //batch learning flag
    bool useBatch;

    //log file handle
    bool logResults;
    fstream logFile;
    int logResolution;
    int lastEpochLogged;


//public methods
//----------------------------------------------------------------------------------------------------------------
public:

    //constructor
    neuralNetwork(int in, int hidden, int out) :
        nInput(in), nHidden(hidden), nOutput(out), epoch(0), trainingSetAccuracy(0), validationSetAccuracy(0), generalizationSetAccuracy(0),
        trainingSetMSE(0), validationSetMSE(0), generalizationSetMSE(0),  logResults(false), logResolution(10), lastEpochLogged(-10)
    {
        //create neuron lists
        //--------------------------------------------------------------------------------------------------------
        inputNeurons = new( double[in + 1] );
        for ( int i=0; i < in; i++ ) inputNeurons[i] = 0;

        //create bias neuron
        inputNeurons[in] = 1;

        hiddenNeurons = new( double[hidden + 1] );
        for ( int i=0; i < hidden; i++ ) hiddenNeurons[i] = 0;

        //create bias neuron
        hiddenNeurons[hidden] = 1;

        outputNeurons = new( double[out] );
        for ( int i=0; i < out; i++ ) outputNeurons[i] = 0;

        //create weight lists (include bias neuron weights)
        //--------------------------------------------------------------------------------------------------------
        wInputHidden = new( double*[in + 1] );
        for ( int i=0; i <= in; i++ )
        {
            wInputHidden[i] = new (double[hidden]);
            for ( int j=0; j < hidden; j++ ) wInputHidden[i][j] = 0;
        }

        wHiddenOutput = new( double*[hidden + 1] );
        for ( int i=0; i <= hidden; i++ )
        {
            wHiddenOutput[i] = new (double[out]);
            for ( int j=0; j < out; j++ ) wHiddenOutput[i][j] = 0;
        }

        //create delta lists
        //--------------------------------------------------------------------------------------------------------
        deltaInputHidden = new( double*[in + 1] );
        for ( int i=0; i <= in; i++ )
        {
            deltaInputHidden[i] = new (double[hidden]);
            for ( int j=0; j < hidden; j++ ) deltaInputHidden[i][j] = 0;
        }

        deltaHiddenOutput = new( double*[hidden + 1] );
        for ( int i=0; i <= hidden; i++ )
        {
            deltaHiddenOutput[i] = new (double[out]);
            for ( int j=0; j < out; j++ ) deltaHiddenOutput[i][j] = 0;
        }

        //create error gradient storage
        //--------------------------------------------------------------------------------------------------------
        hiddenErrorGradients = new( double[hidden + 1] );
        for ( int i=0; i <= hidden; i++ ) hiddenErrorGradients[i] = 0;

        outputErrorGradients = new( double[out + 1] );
        for ( int i=0; i <= out; i++ ) outputErrorGradients[i] = 0;

        //initialize weights
        //--------------------------------------------------------------------------------------------------------
        initializeWeights();

        //default learning parameters
        //--------------------------------------------------------------------------------------------------------
        learningRate = LEARNING_RATE;
        momentum = MOMENTUM;

        //use stochastic learning by default
        useBatch = false;

        //stop conditions
        //--------------------------------------------------------------------------------------------------------
        maxEpochs = MAX_EPOCHS;
        desiredAccuracy = DESIRED_ACCURACY;
    }

    //destructor
    ~neuralNetwork()
    {
        //delete neurons
        delete[] inputNeurons;
        delete[] hiddenNeurons;
        delete[] outputNeurons;

        //delete weight storage
        for (int i=0; i <= nInput; i++) delete[] wInputHidden[i];
        delete[] wInputHidden;

        for (int j=0; j <= nHidden; j++) delete[] wHiddenOutput[j];
        delete[] wHiddenOutput;

        //delete delta storage
        for (int i=0; i <= nInput; i++) delete[] deltaInputHidden[i];
        delete[] deltaInputHidden;

        for (int j=0; j <= nHidden; j++) delete[] deltaHiddenOutput[j];
        delete[] deltaHiddenOutput;

        //delete error gradients
        delete[] hiddenErrorGradients;
        delete[] outputErrorGradients;

        //close log file
        if ( logFile.is_open() ) logFile.close();
    }

    //set learning parameters
    void setLearningParameters(double lr, double m)
    {
        learningRate = lr;
        momentum = m;
    }

    //set max epoch
    void setMaxEpochs(int max)
    {
        maxEpochs = max;
    }

    //set desired accuracy
    void setDesiredAccuracy(float d)
    {
        desiredAccuracy = d;
    }

    //enable batch learning
    void useBatchLearning()
    {
        useBatch = true;
    }

    //enable stochastic learning
    void useStochasticLearning()
    {
        useBatch = false;
    }

    //enable logging of training results
    void enableLogging(const char* filename, int resolution = 1)
    {
        //create log file
        if ( ! logFile.is_open() )
        {
            logFile.open(filename, ios::out);

            if ( logFile.is_open() )
            {
                //write log file header
                logFile << "Epoch,Training Set Accuracy, Generalization Set Accuracy,Training Set MSE, Generalization Set MSE" << endl;

                //enable logging
                logResults = true;

                //resolution setting;
                logResolution = resolution;
                lastEpochLogged = -resolution;
            }
        }
    }

    //resets the neural network
    void resetWeights()
    {
        //reinitialize weights
        initializeWeights();
    }

    //feed data through network
    double* feedInput( double* inputs)
    {
        //feed data into the network
        feedForward(inputs);

        //return results
        return outputNeurons;
    }

    //train the network
    void trainNetwork( vector<dataEntry*> trainingSet, vector<dataEntry*> generalizationSet, vector<dataEntry*> validationSet )
    {
        cout<< endl << " Neural Network Training Starting: " << endl
            << "==========================================================================" << endl
            << " LR: " << learningRate << ", Momentum: " << momentum << ", Max Epochs: " << maxEpochs << endl
            << " " << nInput << " Input Neurons, " << nHidden << " Hidden Neurons, " << nOutput << " Output Neurons" << endl
            << "==========================================================================" << endl << endl;

        //reset epoch and log counters
        epoch = 0;
        lastEpochLogged = -logResolution;

        //train network using training dataset for training and generalization dataset for testing
        //--------------------------------------------------------------------------------------------------------
        double tAccuracy_max = trainingSetAccuracy;
        double gAccuracy_max = generalizationSetAccuracy;
        while (	( trainingSetAccuracy < desiredAccuracy || generalizationSetAccuracy < desiredAccuracy ) && epoch < maxEpochs )
        {
            //use training set to train network
            runTrainingEpoch( trainingSet );

            //get generalization set accuracy and MSE
            generalizationSetAccuracy = getSetAccuracy( generalizationSet );
            generalizationSetMSE = getSetMSE( generalizationSet );

            //Log Training results
            if (logResults && logFile.is_open() && ( epoch - lastEpochLogged == logResolution ) )
            {
                logFile << epoch << "," << trainingSetAccuracy << "," << generalizationSetAccuracy << "," << trainingSetMSE << "," << generalizationSetMSE << endl;
                lastEpochLogged = epoch;
            }

            //print out change in training /generalization accuracy (only if a change is greater than a percent)

            cout << "Epoch :" << epoch;
            cout << " TSet Acc:" << trainingSetAccuracy << "%, MSE: " << trainingSetMSE ;
            cout << " GSet Acc:" << generalizationSetAccuracy << "%, MSE: " << generalizationSetMSE << endl;

            if(tAccuracy_max < trainingSetAccuracy)
            {
                tAccuracy_max = trainingSetAccuracy;
                cout<<"New max training accuracy: "<<tAccuracy_max<<endl;
                save("max_tAccuracy.data");
            }

            if(gAccuracy_max < generalizationSetAccuracy)
            {
                gAccuracy_max = generalizationSetAccuracy;
                cout<<"New max generalization accuracy: "<<gAccuracy_max<<endl;
                save("max_gAccuracy.data");
            }



            //once training set is complete increment epoch
            epoch++;

        }//end while

        //get validation set accuracy and MSE
        validationSetAccuracy = getSetAccuracy(validationSet);
        validationSetMSE = getSetMSE(validationSet);

        //log end
        logFile << epoch << "," << trainingSetAccuracy << "," << generalizationSetAccuracy << "," << trainingSetMSE << "," << generalizationSetMSE << endl << endl;
        logFile << "Training Complete!!! - > Elapsed Epochs: " << epoch << " Validation Set Accuracy: " << validationSetAccuracy << " Validation Set MSE: " << validationSetMSE << endl;

        //out validation accuracy and MSE
        cout << endl << "Training Complete!!! - > Elapsed Epochs: " << epoch << endl;
        cout << " Validation Set Accuracy: " << validationSetAccuracy << endl;
        cout << " Validation Set MSE: " << validationSetMSE << endl << endl;
    }


//private methods
//----------------------------------------------------------------------------------------------------------------
private:

    //initialize weights and weight changes
    void initializeWeights()
    {
        //init random number generator
        srand( (unsigned int) time(0) );

        //set weights between input and hidden to a random value between -05 and 0.5
        //--------------------------------------------------------------------------------------------------------
        for(int i = 0; i <= nInput; i++)
        {
            for(int j = 0; j < nHidden; j++)
            {
                //set weights to random values
                wInputHidden[i][j] = (double)rand() / (RAND_MAX + 1) - 0.5;

                //create blank delta
                deltaInputHidden[i][j] = 0;
            }
        }

        //set weights between input and hidden to a random value between -05 and 0.5
        //--------------------------------------------------------------------------------------------------------
        for(int i = 0; i <= nHidden; i++)
        {
            for(int j = 0; j < nOutput; j++)
            {
                //set weights to random values
                wHiddenOutput[i][j] = (double)rand() / (RAND_MAX + 1) - 0.5;

                //create blank delta
                deltaHiddenOutput[i][j] = 0;
            }
        }
    }

    //run a single training epoch
    void runTrainingEpoch( vector<dataEntry*> trainingSet )
    {
        //incorrect patterns
        double incorrectPatterns = 0;
        double mse = 0;
        double** oldwInputHidden = new double*[nInput];
        double** oldwHiddenOutput = new double*[nOutput];

        oldwInputHidden = new( double*[nInput + 1] );
        for ( int i=0; i <= nInput; i++ )
        {
            oldwInputHidden[i] = new (double[nHidden]);
            for ( int j=0; j < nHidden; j++ ) oldwInputHidden[i][j] = wInputHidden[i][j];
        }

        oldwHiddenOutput = new( double*[nHidden + 1] );
        for ( int i=0; i <= nHidden; i++ )
        {
            oldwHiddenOutput[i] = new (double[nOutput]);
            for ( int j=0; j < nOutput; j++ ) oldwHiddenOutput[i][j] = wHiddenOutput[i][j];
        }

        //for every training pattern
        for ( int tp = 0; tp < (int) trainingSet.size(); tp++)
        {
            //feed inputs through network and backpropagate errors
            feedForward( trainingSet[tp]->pattern );
            backpropagate( trainingSet[tp]->target );

            //pattern correct flag
            bool patternCorrect = true;

            //check all outputs from neural network against desired values
            for ( int k = 0; k < nOutput; k++ )
            {
                //pattern incorrect if desired and output differ
                if ( getRoundedOutputValue( outputNeurons[k] ) != trainingSet[tp]->target[k] ) patternCorrect = false;

                //calculate MSE
                mse += pow((outputNeurons[k] - trainingSet[tp]->target[k]), 2);
            }

            //if pattern is incorrect add to incorrect count
            if ( !patternCorrect ) incorrectPatterns++;

        }//end for

        //if using batch learning - update the weights
        double oldMSE;
        if(epoch == 0)
            oldMSE= mse / ( nOutput * trainingSet.size() );
        else
            oldMSE = trainingSetMSE;
        double newMSE = mse / ( nOutput * trainingSet.size() );

        if ( !useBatch )
        {
            if(newMSE > oldMSE*1.04)
            {
                learningRate *= 0.7;
                cout<<"Learning rate adjust to new value: "<<learningRate<<". Discard weight update."<<endl;
                for(int j=0; j < nHidden; j++)
                {
                    for( int i=0; i <= nInput; i++ )
                        oldwInputHidden[i][j]=wInputHidden[i][j];
                }

                for(int k=0; k < nOutput; k++)
                {
                    for( int j=0; j <= nHidden; j++ )
                        oldwHiddenOutput[j][k] = wHiddenOutput[j][k];
                }
            }
            else if(newMSE < oldMSE)
            {
                learningRate *= 1.05;
                cout<<"Learning rate adjust to new value: "<<learningRate<<endl;
                 //update training accuracy and MSE
                trainingSetAccuracy = 100 - (incorrectPatterns/trainingSet.size() * 100);
                trainingSetMSE = mse / ( nOutput * trainingSet.size() );
            }
            else
            {
                //update training accuracy and MSE
                trainingSetAccuracy = 100 - (incorrectPatterns/trainingSet.size() * 100);
                trainingSetMSE = mse / ( nOutput * trainingSet.size() );
            }
        }
        //delete old paramter
        for (int i=0; i <= nInput; i++) delete[] oldwInputHidden[i];
            delete[] oldwInputHidden;

        for (int j=0; j <= nHidden; j++) delete[] oldwHiddenOutput[j];
            delete[] oldwHiddenOutput;
    }

    //feed input forward
    void feedForward( double *inputs )
    {
        //set input neurons to input values
        for(int i = 0; i < nInput; i++) inputNeurons[i] = scale(inputs[i]);

        //Calculate Hidden Layer values - include bias neuron
        //--------------------------------------------------------------------------------------------------------
        for(int j=0; j < nHidden; j++)
        {
            //clear value
            hiddenNeurons[j] = 0;

            //get weighted sum of inputs and bias neuron
            for( int i=0; i <= nInput; i++ )
                hiddenNeurons[j] += inputNeurons[i] * wInputHidden[i][j];

            //set to result of sigmoid
            hiddenNeurons[j] = activationFunction( hiddenNeurons[j] );
        }

        //Calculating Output Layer values - include bias neuron
        //--------------------------------------------------------------------------------------------------------
        for(int k=0; k < nOutput; k++)
        {
            //clear value
            outputNeurons[k] = 0;

            //get weighted sum of inputs and bias neuron
            for( int j=0; j <= nHidden; j++ )
                outputNeurons[k] += hiddenNeurons[j] * wHiddenOutput[j][k];

            //set to result of sigmoid
           // outputNeurons[k] = activationFunction( outputNeurons[k] );
           outputNeurons[k] = unscale(outputNeurons[k]);
        }
    }

    double scale(double x)
    {
        return -1.0 + x*2/255;
    }

    double unscale(double y)
    {
        return (y+1)*1/2;
    }

    //modify weights according to ouput
    void backpropagate( double* desiredValues )
    {
        //modify deltas between hidden and output layers
        //--------------------------------------------------------------------------------------------------------
        for (int k = 0; k < nOutput; k++)
        {
            //get error gradient for every output node
            outputErrorGradients[k] = getOutputErrorGradient( desiredValues[k], outputNeurons[k] );

            //for all nodes in hidden layer and bias neuron
            for (int j = 0; j <= nHidden; j++)
            {
                //calculate change in weight
                if ( !useBatch )
                    deltaHiddenOutput[j][k] = learningRate * hiddenNeurons[j] * outputErrorGradients[k] + momentum * deltaHiddenOutput[j][k];
                else
                    deltaHiddenOutput[j][k] += learningRate * hiddenNeurons[j] * outputErrorGradients[k];
            }
        }

        //modify deltas between input and hidden layers
        //--------------------------------------------------------------------------------------------------------
        for (int j = 0; j < nHidden; j++)
        {
            //get error gradient for every hidden node
            hiddenErrorGradients[j] = getHiddenErrorGradient( j );

            //for all nodes in input layer and bias neuron
            for (int i = 0; i <= nInput; i++)
            {
                //calculate change in weight
                if ( !useBatch ) deltaInputHidden[i][j] = learningRate * inputNeurons[i] * hiddenErrorGradients[j] + momentum * deltaInputHidden[i][j];
                else deltaInputHidden[i][j] += learningRate * inputNeurons[i] * hiddenErrorGradients[j];

            }
        }

        //if using stochastic learning update the weights immediately
        if ( !useBatch ) updateWeights();
    }

    //update weights
    void updateWeights()
    {
        //input -> hidden weights
        //--------------------------------------------------------------------------------------------------------
        for (int i = 0; i <= nInput; i++)
        {
            for (int j = 0; j < nHidden; j++)
            {
                //update weight
                wInputHidden[i][j] += deltaInputHidden[i][j];

                //clear delta only if using batch (previous delta is needed for momentum
                if (useBatch) deltaInputHidden[i][j] = 0;
            }
        }

        //hidden -> output weights
        //--------------------------------------------------------------------------------------------------------
        for (int j = 0; j <= nHidden; j++)
        {
            for (int k = 0; k < nOutput; k++)
            {
                //update weight
                wHiddenOutput[j][k] += deltaHiddenOutput[j][k];

                //clear delta only if using batch (previous delta is needed for momentum)
                if (useBatch)deltaHiddenOutput[j][k] = 0;
            }
        }
    }

    //activation function
    inline double activationFunction( double x )
    {
        //sigmoid function
        return 2/(1+exp(-2*x))-1;
    }

    //get error gradient for ouput layer
    inline double getOutputErrorGradient(double desiredValue, double outputValue)
    {
        //return error gradient
        return outputValue * ( 1 - outputValue ) * ( desiredValue - outputValue );
    }

    //get error gradient for hidden layer
    double getHiddenErrorGradient( int j )
    {
        //get sum of hidden->output weights * output error gradients
        double weightedSum = 0;
        for( int k = 0; k < nOutput; k++ ) weightedSum += wHiddenOutput[j][k] * outputErrorGradients[k];

        //return error gradient
        return hiddenNeurons[j] * ( 1 - hiddenNeurons[j] ) * weightedSum;
    }

    //round up value to get a boolean result
    int getRoundedOutputValue( double x )
    {
        if ( x < 0.1 ) return 0;
        else if ( x > 0.9 ) return 1;
        else return -1;
    }
    //feed forward set of patterns and return error
    double getSetAccuracy( vector<dataEntry*> set )
    {
        double incorrectResults = 0;

        //for every training input array
        for ( int tp = 0; tp < (int) set.size(); tp++)
        {
            //feed inputs through network and backpropagate errors
            feedForward( set[tp]->pattern );

            //correct pattern flag
            bool correctResult = true;

            //check all outputs against desired output values
            for ( int k = 0; k < nOutput; k++ )
            {
                //set flag to false if desired and output differ
                if ( getRoundedOutputValue(outputNeurons[k]) != set[tp]->target[k] ) correctResult = false;
            }

            //inc training error for a incorrect result
            if ( !correctResult ) incorrectResults++;

        }//end for

        //calculate error and return as percentage
        return 100 - (incorrectResults/set.size() * 100);
    }

    //feed forward set of patterns and return MSE
    double getSetMSE ( vector<dataEntry*> set )
    {
        double mse = 0;

        //for every training input array
        for ( int tp = 0; tp < (int) set.size(); tp++)
        {
            //feed inputs through network and backpropagate errors
            feedForward( set[tp]->pattern );

            //check all outputs against desired output values
            for ( int k = 0; k < nOutput; k++ )
            {
                //sum all the MSEs together
                mse += pow((outputNeurons[k] - set[tp]->target[k]), 2);
            }

        }//end for

        //calculate error and return as percentage
        return mse/(nOutput * set.size());
    }

public:
    void load(const char* file)
    {
        fstream in;
        in.open(file);
        if(!in.is_open())
            std::cerr<<"Cannot open file "<<file<<std::endl;

        in>>nInput>>nHidden>>nOutput;

        for(int j=0; j < nHidden; j++)
        {
            for( int i=0; i <= nInput; i++ )
                in>>wInputHidden[i][j];
        }

        for(int k=0; k < nOutput; k++)
        {
            for( int j=0; j <= nHidden; j++ )
                in>>wHiddenOutput[j][k];
        }
    }

    void save(const char* file)
    {
        ofstream out;
        out.open(file);
        if(!out.is_open())
            std::cerr<<"Cannot open file "<<file<<std::endl;
        out<<nInput<<'\t'<<nHidden<<'\t'<<nOutput<<std::endl;
        for(int j=0; j < nHidden; j++)
        {
            for( int i=0; i <= nInput; i++ )
                out<<wInputHidden[i][j]<<' ';
            out<<std::endl;
        }
        out<<std::endl;

        for(int k=0; k < nOutput; k++)
        {
            for( int j=0; j <= nHidden; j++ )
                out<<wHiddenOutput[j][k]<<' ';
            out<<std::endl;
        }
    }
};

#endif
