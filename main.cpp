#include "DSString.h"
#include <iostream>
#include <vector>

int main() {
    DSString tweet("Hello, world! This is a test tweet.");

    std::vector<DSString> tokens = tweet.split();

    std::cout << "Tokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << token << std::endl;
    }

    return 0;
}
