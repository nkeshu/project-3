// SentimentClassifier.h

#ifndef SENTIMENTCLASSIFIER_H
#define SENTIMENTCLASSIFIER_H

#include "DSString.h"
#include <unordered_map>
#include <vector>
#include <string>

class SentimentClassifier {
private:
    //map to store word frequencies in positive and negative tweets
    //key: word (DSString), Value: pair<positive count, negative count>
    std::unordered_map<DSString, std::pair<int, int>> wordFreq;

    //map to store predictions
    //key: tweet ID (DSString), Value: predicted sentiment (int)
    std::unordered_map<DSString, int> predictions;
    void parseCSVLine(const std::string& line, std::vector<std::string>& fields);

public:
    //constructor
    SentimentClassifier();

    //train the classifier using the training data file
    void train(const std::string& trainFile);

    //predict sentiments for the test data and write results to resultFile
    void predict(const std::string& testFile, const std::string& resultFile);

    //evaluate predictions against the ground truth and write accuracy and errors to accuracyFile
    void evaluatePredictions(const std::string& groundTruthFile, const std::string& accuracyFile);
    void testParseCSVLine();
};

#endif //SENTIMENTCLASSIFIER_H
