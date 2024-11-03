// test_DSString.cpp

#include "DSString.h"
#include <iostream>

int main() {
    DSString str1("Hello");
    DSString str2("World");
    DSString str3 = str1 + DSString(" ") + str2 + "!";
    std::cout << str3 << std::endl; // Should print "Hello World!"

    DSString str4 = str3.substring(6, 5);
    std::cout << str4 << std::endl; // Should print "World"

    if (str4.toLower() == "world") {
        std::cout << "Substring works and toLower is successful." << std::endl;
    }

    return 0;
}
