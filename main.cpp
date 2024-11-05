// main.cpp
/*
Compiling: g++ -std=c++17 -o sentiment main.cpp SentimentClassifier.cpp DSString.cpp
./sentiment data/train_dataset_20k.csv data/test_dataset_10k.csv data/test_dataset_sentiment_10k.csv output
*/
#include "SentimentClassifier.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    //check for the correct number of command-line arguments
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <training_data> <test_data> <ground_truth> <output_prefix>" << std::endl;
        return 1;
    }

    //Parse command-line arguments
    std::string trainingDataFile = argv[1];
    std::string testDataFile = argv[2];
    std::string groundTruthFile = argv[3];
    std::string outputPrefix = argv[4];

    //output files
    std::string resultsFile = outputPrefix + "_results.csv";
    std::string accuracyFile = outputPrefix + "_accuracy.txt";

    //output the file names for debugging
    std::cout << "Training data file: " << trainingDataFile << std::endl;
    std::cout << "Test data file: " << testDataFile << std::endl;
    std::cout << "Ground truth file: " << groundTruthFile << std::endl;
    std::cout << "Results file: " << resultsFile << std::endl;
    std::cout << "Accuracy file: " << accuracyFile << std::endl;

    // create an instance of SentimentClassifier
    SentimentClassifier classifier;

    //train the classifier
    std::cout << "Training the classifier..." << std::endl;
    classifier.train(trainingDataFile);

    // predict sentiments
    std::cout << "Predicting sentiments..." << std::endl;
    classifier.predict(testDataFile, resultsFile);

    //evaluate predictions
    std::cout << "Evaluating predictions..." << std::endl;
    classifier.evaluatePredictions(groundTruthFile, accuracyFile);

    std::cout << "Results written to: " << resultsFile << std::endl;
    std::cout << "Accuracy and errors written to: " << accuracyFile << std::endl;

    return 0;
}
