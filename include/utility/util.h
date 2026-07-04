#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

class Util {
public:
    // Trims whitespace from the start and end of a string
    static std::string trim(const std::string& str);
    
    // Splits a string into a vector of strings based on a delimiter
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
};

#endif // UTIL_H