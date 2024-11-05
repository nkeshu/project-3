//dSString.h

#ifndef DSSTRING_H
#define DSSTRING_H

#include <iostream>
#include <functional> //include this for std::hash
#include <cstddef>    //for std::size_t

class DSString
{
private:
    char *data; //pointer to a character array containing the string with a '\0' terminator
    size_t len; //the length of the string (without the terminator)

public:
    //constructors, Destructor, and Assignment Operator (Rule of Three)
    DSString();
    DSString(const char *);
    DSString(const DSString &);
    DSString &operator=(const DSString &);
    ~DSString();

    //additional Assignment Operator
    DSString &operator=(const char *);

    size_t length() const; //returns the length of the string
    char &operator[](size_t); //returns a reference to the character at the given index

    DSString operator+(const DSString &) const;

    bool operator==(const DSString &) const noexcept;
    bool operator<(const DSString &) const noexcept;
    bool operator>(const DSString &) const noexcept;
    bool operator!=(const DSString &) const noexcept;
    bool operator<=(const DSString &) const noexcept;
    bool operator>=(const DSString &) const noexcept;

    DSString substring(size_t start, size_t numChars) const;
    DSString toLower() const;

    const char *c_str() const noexcept;

    friend std::ostream &operator<<(std::ostream &, const DSString &);

    //friend declaration for std::hash
    friend struct std::hash<DSString>;
    std::vector<DSString> split() const;
};

//specialization of std::hash for DSString
namespace std
{
    template <>
    struct hash<DSString>
    {
        std::size_t operator()(const DSString &s) const noexcept
        {
            //simple hash function (djb2 algorithm)
            const char *str = s.c_str();
            std::size_t hash = 5381;
            int c;
            while ((c = *str++))
            {
                hash = ((hash << 5) + hash) + c; //hash * 33 + c
            }
            return hash;
        }
    };
}

#endif //DSSTRING_H
