// DSString.h

#ifndef DSSTRING_H
#define DSSTRING_H

#include <iostream>
#include <cstddef> // For size_t

class DSString
{
private:
    char *data;  // Pointer to a character array containing the string with a '\0' terminator
    size_t len;  // The length of the string (without the terminator)

public:
    /**
     * Constructors, Destructor, and Assignment Operator (Rule of Three)
     **/
    DSString();                           // Default constructor
    DSString(const char *);               // Constructor that converts a C-string
    DSString(const DSString &);           // Copy constructor
    DSString &operator=(const DSString &); // Copy assignment operator
    ~DSString();                          // Destructor

    // Additional Assignment Operator
    DSString &operator=(const char *);

    size_t length() const;                 // Returns the length of the string
    char &operator[](size_t);              // Returns a reference to the character at the given index

    /**
     * Overloaded operator+ which appends the string in the argument to this string
     */
    DSString operator+(const DSString &) const;

    /**
     * Standard relational operators to compare and order your strings.
     **/
    bool operator==(const DSString &) const;
    bool operator<(const DSString &) const;
    bool operator>(const DSString &) const;
    bool operator!=(const DSString &) const;
    bool operator<=(const DSString &) const;
    bool operator>=(const DSString &) const;

    /**
     * The substring method returns a new string object that contains a
     * sequence of characters from this string object.
     *
     * @param start - the index of where to start
     * @param numChars - the number (count) of characters to copy into
     *    the substring
     * @return a DSString object containing the requested substring
     **/
    DSString substring(size_t start, size_t numChars) const;

    /**
     * Returns a new string object with all characters in lowercase
     *
     * @return DSString
     */
    DSString toLower() const;

    /**
     * The c_str function returns a pointer to a null-terminated C-string holding the
     * contents of this object.
     **/
    const char *c_str() const;

    /**
     * Overloaded stream insertion operator to print the contents of this
     * string to the output stream in the first argument.
     **/
    friend std::ostream &operator<<(std::ostream &, const DSString &);
};

#endif
