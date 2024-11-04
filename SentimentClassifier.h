// SentimentClassifier.h

#ifndef SENTIMENTCLASSIFIER_H
#define SENTIMENTCLASSIFIER_H

#include "DSString.h"
#include <unordered_map>
#include <vector>
#include <string>

class SentimentClassifier {
private:
    // Map to store word frequencies in positive and negative tweets
    // Key: word (DSString), Value: pair<positive count, negative count>
    std::unordered_map<DSString, std::pair<int, int>> wordFreq;

    // Map to store predictions
    // Key: tweet ID (DSString), Value: predicted sentiment (int)
    std::unordered_map<DSString, int> predictions;
    void parseCSVLine(const std::string& line, std::vector<std::string>& fields);

public:
    // Constructor
    SentimentClassifier();

    // Train the classifier using the training data file
    void train(const std::string& trainFile);

    // Predict sentiments for the test data and write results to resultFile
    void predict(const std::string& testFile, const std::string& resultFile);

    // Evaluate predictions against the ground truth and write accuracy and errors to accuracyFile
    void evaluatePredictions(const std::string& groundTruthFile, const std::string& accuracyFile);
    void testParseCSVLine();
};

#endif // SENTIMENTCLASSIFIER_H
