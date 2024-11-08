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

//constructor
SentimentClassifier::SentimentClassifier() {
    //initialization if needed
}
//helper function to parse a CSV line into fields, handling quotes and commas
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
                //handle escaped double quote
                field += '"';
                ++i; //skip the next quote
            }
            else
            {
                inQuotes = !inQuotes;
            }
        }
        else if (c == ',' && !inQuotes)
        {
            //end of field
            fields.push_back(field);
            field.clear();
        }
        else
        {
            field += c;
        }
    }
    //add the last field
    fields.push_back(field);
}

//train the classifier using the training data file
void SentimentClassifier::train(const std::string &trainFile)
{
    //open the training data file
    std::ifstream infile(trainFile);
    if (!infile.is_open())
    {
        std::cerr << "Error opening training data file: " << trainFile << std::endl;
        return;
    }

    std::string line;
    size_t lineNumber = 0; //keep track of the line number for debugging

    //skip the header line
    if (std::getline(infile, line))
    {
        lineNumber++;
        //check if the line contains non-numeric sentiment
        std::vector<std::string> fields;
        parseCSVLine(line, fields);
        if (!fields.empty() && fields[0] == "Sentiment")
        {
             //skip the header line if present
        }
        else
        {
            //if no header, reset the stream to the beginning
            infile.clear();
            infile.seekg(0, std::ios::beg);
            lineNumber = 0;
        }
    }

    //read each line from the file
    while (std::getline(infile, line))
    {
        lineNumber++;
        std::vector<std::string> fields;
        parseCSVLine(line, fields);

        //ensure there are at least 6 fields
        if (fields.size() < 6)
        {
            std::cerr << "Skipping line " << lineNumber << ": Not enough fields." << std::endl;
            continue; //skip invalid lines
        }

        //debugging output: Print the contents of fields[0]
        std::string sentimentStr = fields[0];
        try
        {
            int sentiment = std::stoi(sentimentStr);
            //proceed only if sentiment is 0 or 4
            if (sentiment != 0 && sentiment != 4)
            {
                std::cerr << "Skipping line " << lineNumber << ": Invalid sentiment value (" << sentiment << ")." << std::endl;
                continue;
            }

            DSString tweetText(fields[5].c_str());

            //convert tweet text to lowercase
            tweetText = tweetText.toLower();

            //tokenize the tweet
            std::vector<DSString> words = tweetText.split();

            //update word frequencies
            for (const DSString &word : words)
            {
                if (sentiment == 4)
                {
                    wordFreq[word].first++; //increment positive count
                }
                else if (sentiment == 0)
                {
                    wordFreq[word].second++; //increment negative count
                }
            }
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Invalid argument on line " << lineNumber << ": Cannot convert sentiment '" << sentimentStr << "' to int." << std::endl;
            continue;
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "Out of range error on line " << lineNumber << ": Sentiment '" << sentimentStr << "' is out of int range." << std::endl;
            continue;
        }
    }

    infile.close();
}



//predict sentiments for the test data and write results to resultFile
void SentimentClassifier::predict(const std::string &testFile, const std::string &resultFile)
{
    //open the test data file
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
    size_t lineNumber = 0;

    //skip the header line if present
    if (std::getline(infile, line))
    {
        lineNumber++;
        std::vector<std::string> fields;
        parseCSVLine(line, fields);
        if (!fields.empty() && (fields[0] == "TweetID" || fields[0] == "Id" || fields[0] == "id"))
        {
            //header line detected and skipped
        }
        else
        {
            //if no header, reset the stream to the beginning
            infile.clear();
            infile.seekg(0, std::ios::beg);
            lineNumber = 0;
        }
    }

    //read each line from the file
    while (std::getline(infile, line))
    {
        lineNumber++;
        std::vector<std::string> fields;
        parseCSVLine(line, fields);

        //ensure there are at least 5 fields (test data has no sentiment column)
        if (fields.size() < 5)
        {
            continue; //skip invalid lines
        }

        //extract tweet ID and tweet text
        DSString tweetID(fields[0].c_str());
        DSString tweetText(fields[4].c_str());

        //convert tweet text to lowercase
        tweetText = tweetText.toLower();

        //tokenize the tweet
        std::vector<DSString> words = tweetText.split();

        //compute sentiment score for the tweet
        double tweetScore = 0.0;
        for (const DSString &word : words)
        {
            auto it = wordFreq.find(word);
            if (it != wordFreq.end())
            {
                int posCount = it->second.first;
                int negCount = it->second.second;
                //compute word sentiment score using log-likelihood ratio
                double wordScore = std::log((posCount + 1.0) / (negCount + 1.0));
                tweetScore += wordScore;
            }
            //if word not seen in training, ignore it
        }

        //predict sentiment based on tweet score
        int predictedSentiment = (tweetScore >= 0) ? 4 : 0;

        //store the prediction
        predictions[tweetID] = predictedSentiment;

        //write the prediction to the results file
        outfile << predictedSentiment << ", " << tweetID << std::endl;
    }

    infile.close();
    outfile.close();
}




//evaluate predictions against the ground truth and write accuracy and errors to accuracyFile
void SentimentClassifier::evaluatePredictions(const std::string &groundTruthFile, const std::string &accuracyFile)
{
    //read ground truth sentiments
    std::unordered_map<DSString, int> groundTruth;

    std::ifstream infile(groundTruthFile);
    if (!infile.is_open())
    {
        std::cerr << "Error opening ground truth file: " << groundTruthFile << std::endl;
        return;
    }

    std::string line;
    size_t lineNumber = 0;

    //skip the header line if present
    if (std::getline(infile, line))
    {
        lineNumber++;
        std::vector<std::string> fields;
        parseCSVLine(line, fields);
        if (!fields.empty() && fields[0] == "Sentiment")
        {
             //skip the header line if present
        }
        else
        {
            //if no header, reset the stream to the beginning
            infile.clear();
            infile.seekg(0, std::ios::beg);
            lineNumber = 0;
        }
    }

    while (std::getline(infile, line))
    {
        lineNumber++;
        std::vector<std::string> fields;
        parseCSVLine(line, fields);

        //ensure there are at least 2 fields
        if (fields.size() < 2)
        {
            std::cerr << "Skipping line " << lineNumber << ": Not enough fields." << std::endl;
            continue; //skip invalid lines
        }

        std::string sentimentStr = fields[0];
        try
        {
            int actualSentiment = std::stoi(sentimentStr);

            DSString tweetID(fields[1].c_str());

            groundTruth[tweetID] = actualSentiment;
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Invalid argument on line " << lineNumber << ": Cannot convert sentiment '" << sentimentStr << "' to int." << std::endl;
            continue;
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "Out of range error on line " << lineNumber << ": Sentiment '" << sentimentStr << "' is out of int range." << std::endl;
            continue;
        }
    }

    infile.close();

    //compare predictions to ground truth
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
                //store errors
                errors.emplace_back(predictedSentiment, actualSentiment, tweetID);
            }
            total++;
        }
    }

    //compute accuracy
    double accuracy = (total > 0) ? static_cast<double>(correct) / total : 0.0;

    //write accuracy and errors to the accuracy file
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

