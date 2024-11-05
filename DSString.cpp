//DSString.cpp

#include "DSString.h"
#include <vector>
#include <cctype> //for std::tolower

//helper function to compute the length of a C-string
size_t my_strlen(const char *str) {
    size_t i = 0;
    while (str[i] != '\0') {
        ++i;
    }
    return i;
}

//helper function to copy a C-string
void my_strcpy(char *dest, const char *src) {
    size_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

//helper function to concatenate two C-strings
void my_strcat(char *dest, const char *src) {
    size_t dest_len = my_strlen(dest);
    size_t i = 0;
    while (src[i] != '\0') {
        dest[dest_len + i] = src[i];
        ++i;
    }
    dest[dest_len + i] = '\0';
}

//helper function to compare two C-strings
int my_strcmp(const char *str1, const char *str2) {
    size_t i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return (unsigned char)str1[i] - (unsigned char)str2[i];
        }
        ++i;
    }
    return (unsigned char)str1[i] - (unsigned char)str2[i];
}

//helper function to copy 'n' characters
void my_strncpy(char *dest, const char *src, size_t n) {
    size_t i = 0;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0'; // Ensure null termination
}

//default constructor
DSString::DSString() {
    data = new char[1];
    data[0] = '\0';
    len = 0;
}

//constructor from C-string
DSString::DSString(const char *str) {
    if (str) {
        len = my_strlen(str);
        data = new char[len + 1];
        my_strcpy(data, str);
    } else {
        data = new char[1];
        data[0] = '\0';
        len = 0;
    }
}

//copy constructor
DSString::DSString(const DSString &other) {
    len = other.len;
    data = new char[len + 1];
    my_strcpy(data, other.data);
}

//copy assignment operator
DSString &DSString::operator=(const DSString &other) {
    if (this != &other) {
        delete[] data;
        len = other.len;
        data = new char[len + 1];
        my_strcpy(data, other.data);
    }
    return *this;
}

//assignment operator from C-string
DSString &DSString::operator=(const char *str) {
    delete[] data;
    if (str) {
        len = my_strlen(str);
        data = new char[len + 1];
        my_strcpy(data, str);
    } else {
        data = new char[1];
        data[0] = '\0';
        len = 0;
    }
    return *this;
}

//destructor
DSString::~DSString() {
    delete[] data;
}

//returns the length of the string
size_t DSString::length() const {
    return len;
}

//returns a reference to the character at the given index
char &DSString::operator[](size_t index) {
    return data[index];
}

//concatenation operator
DSString DSString::operator+(const DSString &other) const {
    size_t newLen = len + other.len;
    char *newData = new char[newLen + 1];
    my_strcpy(newData, data);
    my_strcat(newData, other.data);
    DSString result(newData);
    delete[] newData;
    return result;
}

//equality operator
bool DSString::operator==(const DSString &other) const noexcept {
    return my_strcmp(data, other.data) == 0;
}

//less than operator
bool DSString::operator<(const DSString &other) const noexcept{
    return my_strcmp(data, other.data) < 0;
}

//greater than operator
bool DSString::operator>(const DSString &other) const noexcept{
    return my_strcmp(data, other.data) > 0;
}

//not equal operator
bool DSString::operator!=(const DSString &other) const noexcept{
    return !(*this == other);
}

//less than or equal to operator
bool DSString::operator<=(const DSString &other) const noexcept{
    return !(*this > other);
}

//greater than or equal to operator
bool DSString::operator>=(const DSString &other) const noexcept{
    return !(*this < other);
}

//substring method
DSString DSString::substring(size_t start, size_t numChars) const {
    if (start >= len) {
        return DSString("");
    }
    if (start + numChars > len) {
        numChars = len - start;
    }
    char *subData = new char[numChars + 1];
    for (size_t i = 0; i < numChars; ++i) {
        subData[i] = data[start + i];
    }
    subData[numChars] = '\0';
    DSString result(subData);
    delete[] subData;
    return result;
}

//converts to lowercase
DSString DSString::toLower() const {
    DSString result(*this);
    for (size_t i = 0; i < len; ++i) {
        result.data[i] = std::tolower(result.data[i]);
    }
    return result;
}

//returns a C-string representation
const char *DSString::c_str() const noexcept{
    return data;
}

//overloaded stream insertion operator
std::ostream &operator<<(std::ostream &os, const DSString &str) {
    os << str.data;
    return os;
}
bool isDelimiter(char c) {
    return std::isspace(static_cast<unsigned char>(c)) || std::ispunct(static_cast<unsigned char>(c));
}

std::vector<DSString> DSString::split() const {
    std::vector<DSString> tokens;
    size_t i = 0;

    while (i < len) {
        //skip leading delimiters
        while (i < len && isDelimiter(data[i])) {
            ++i;
        }

        //start of a token
        size_t start = i;

        //find the end of the token
        while (i < len && !isDelimiter(data[i])) {
            ++i;
        }

        //extract the token if any
        if (start < i) {
            DSString token = substring(start, i - start);
            tokens.push_back(token);
        }
    }

    return tokens;
}