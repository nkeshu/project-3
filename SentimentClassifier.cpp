// SentimentClassifier.cpp

#include "SentimentClassifier.h"
#include "DSString.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <tuple>

// Constructor
SentimentClassifier::SentimentClassifier() {
    // Initialization if needed
}
// Helper function to parse a CSV line into fields, handling quotes and commas
void SentimentClassifier::parseCSVLine(const std::string& line, std::vector<std::string>& fields)
{
    fields.clear();
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];

        if (c == '"')
        {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"')
            {
                // Handle escaped double quote
                field += '"';
                ++i; // Skip the next quote
            }
            else
            {
                inQuotes = !inQuotes;
            }
        }
        else if (c == ',' && !inQuotes)
        {
            // End of field
            fields.push_back(field);
            field.clear();
        }
        else
        {
            field += c;
        }
    }
    // Add the last field
    fields.push_back(field);
}

// Train the classifier using the training data file
void SentimentClassifier::train(const std::string &trainFile)
{
    // Open the training data file
    std::ifstream infile(trainFile);
    if (!infile.is_open())
    {
        std::cerr << "Error opening training data file: " << trainFile << std::endl;
        return;
    }

    std::string line;
    // Read each line from the file
    while (std::getline(infile, line))
    {
        std::vector<std::string> fields;
        parseCSVLine(line, fields);

        // Ensure there are at least 6 fields
        if (fields.size() < 6)
        {
            continue; // Skip invalid lines
        }

        // Extract sentiment and tweet text
        int sentiment = std::stoi(fields[0]);
        DSString tweetText(fields[5].c_str());

        // Convert tweet text to lowercase
        tweetText = tweetText.toLower();

        // Tokenize the tweet
        std::vector<DSString> words = tweetText.split();

        // Update word frequencies
        for (const DSString &word : words)
        {
            if (sentiment == 4)
            {
                wordFreq[word].first++; // Increment positive count
            }
            else if (sentiment == 0)
            {
                wordFreq[word].second++; // Increment negative count
            }
        }
    }

    infile.close();
}


// Predict sentiments for the test data and write results to resultFile
void SentimentClassifier::predict(const std::string &testFile, const std::string &resultFile)
{
    // Open the test data file
    std::ifstream infile(testFile);
    if (!infile.is_open())
    {
        std::cerr << "Error opening test data file: " << testFile << std::endl;
        return;
    }

    // Open the results file
    std::ofstream outfile(resultFile);
    if (!outfile.is_open())
    {
        std::cerr << "Error opening results file: " << resultFile << std::endl;
        return;
    }

    std::string line;
    // Read each line from the file
    while (std::getline(infile, line))
    {
        std::vector<std::string> fields;
        parseCSVLine(line, fields);

        // Ensure there are at least 5 fields (test data has no sentiment column)
        if (fields.size() < 5)
        {
            continue; // Skip invalid lines
        }

        // Extract tweet ID and tweet text
        DSString tweetID(fields[0].c_str());
        DSString tweetText(fields[4].c_str());

        // Convert tweet text to lowercase
        tweetText = tweetText.toLower();

        // Tokenize the tweet
        std::vector<DSString> words = tweetText.split();

        // Compute sentiment score for the tweet
        double tweetScore = 0.0;
        for (const DSString &word : words)
        {
            auto it = wordFreq.find(word);
            if (it != wordFreq.end())
            {
                int posCount = it->second.first;
                int negCount = it->second.second;
                // Compute word sentiment score using log-likelihood ratio
                double wordScore = std::log((posCount + 1.0) / (negCount + 1.0));
                tweetScore += wordScore;
            }
            // If word not seen in training, ignore it
        }

        // Predict sentiment based on tweet score
        int predictedSentiment = (tweetScore >= 0) ? 4 : 0;

        // Store the prediction
        predictions[tweetID] = predictedSentiment;

        // Write the prediction to the results file
        outfile << predictedSentiment << ", " << tweetID << std::endl;
    }

    infile.close();
    outfile.close();
}


// Evaluate predictions against the ground truth and write accuracy and errors to accuracyFile
void SentimentClassifier::evaluatePredictions(const std::string &groundTruthFile, const std::string &accuracyFile)
{
    // Read ground truth sentiments
    std::unordered_map<DSString, int> groundTruth;

    std::ifstream infile(groundTruthFile);
    if (!infile.is_open())
    {
        std::cerr << "Error opening ground truth file: " << groundTruthFile << std::endl;
        return;
    }

    std::string line;
    while (std::getline(infile, line))
    {
        std::vector<std::string> fields;
        parseCSVLine(line, fields);

        // Ensure there are at least 2 fields
        if (fields.size() < 2)
        {
            continue; // Skip invalid lines
        }

        // Extract sentiment and tweet ID
        int actualSentiment = std::stoi(fields[0]);
        DSString tweetID(fields[1].c_str());

        groundTruth[tweetID] = actualSentiment;
    }

    infile.close();

    // Compare predictions to ground truth
    int correct = 0;
    int total = 0;
    std::vector<std::tuple<int, int, DSString>> errors; // (predicted, actual, tweetID)

    for (const auto &pred : predictions)
    {
        DSString tweetID = pred.first;
        int predictedSentiment = pred.second;

        auto it = groundTruth.find(tweetID);
        if (it != groundTruth.end())
        {
            int actualSentiment = it->second;
            if (predictedSentiment == actualSentiment)
            {
                correct++;
            }
            else
            {
                // Store errors
                errors.emplace_back(predictedSentiment, actualSentiment, tweetID);
            }
            total++;
        }
    }

    // Compute accuracy
    double accuracy = static_cast<double>(correct) / total;

    // Write accuracy and errors to the accuracy file
    std::ofstream outfile(accuracyFile);
    if (!outfile.is_open())
    {
        std::cerr << "Error opening accuracy file: " << accuracyFile << std::endl;
        return;
    }

    outfile << std::fixed;
    outfile.precision(3);
    outfile << accuracy << std::endl;

    for (const auto &err : errors)
    {
        int predicted = std::get<0>(err);
        int actual = std::get<1>(err);
        DSString tweetID = std::get<2>(err);

        outfile << predicted << ", " << actual << ", " << tweetID << std::endl;
    }

    outfile.close();
}
void SentimentClassifier::testParseCSVLine()
{
    std::string line = R"(4,1467811594,Mon Apr 06 22:20:03 PDT 2009,NO_QUERY,peruna_pony,"Beat TCU")";
    std::vector<std::string> fields;
    parseCSVLine(line, fields);

    std::cout << "Fields extracted:" << std::endl;
    for (size_t i = 0; i < fields.size(); ++i)
    {
        std::cout << i << ": [" << fields[i] << "]" << std::endl;
    }
}

