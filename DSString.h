// DSString.h

#ifndef DSSTRING_H
#define DSSTRING_H

#include <iostream>
#include <functional> // Include this for std::hash
#include <cstddef>    // For std::size_t

class DSString
{
private:
    char *data; // Pointer to a character array containing the string with a '\0' terminator
    size_t len; // The length of the string (without the terminator)

public:
    // Constructors, Destructor, and Assignment Operator (Rule of Three)
    DSString();
    DSString(const char *);
    DSString(const DSString &);
    DSString &operator=(const DSString &);
    ~DSString();

    // Additional Assignment Operator
    DSString &operator=(const char *);

    size_t length() const;               // Returns the length of the string
    char &operator[](size_t);            // Returns a reference to the character at the given index

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

    // Friend declaration for std::hash
    friend struct std::hash<DSString>;
    std::vector<DSString> split() const;
};

// Specialization of std::hash for DSString
namespace std
{
    template <>
    struct hash<DSString>
    {
        std::size_t operator()(const DSString &s) const noexcept
        {
            // Simple hash function (djb2 algorithm)
            const char *str = s.c_str();
            std::size_t hash = 5381;
            int c;
            while ((c = *str++))
            {
                hash = ((hash << 5) + hash) + c; // hash * 33 + c
            }
            return hash;
        }
    };
}

#endif // DSSTRING_H
