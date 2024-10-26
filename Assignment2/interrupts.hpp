#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

// Standard library includes
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <vector>

// Constants for interrupt vector table
const int ADDR_BASE = 1024;    // Base address for interrupt vector table
const int VECTOR_SIZE = 4;     // Size of each vector entry in bytes

/**
 * Splits a string by a delimiter and returns a vector of substrings
 * @param str The string to split
 * @param delimiter The delimiter to split by
 * @return Vector of substrings
 */
inline std::vector<std::string> split_delim(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    
    do {
        pos = str.find(delimiter, prev);
        if (pos == std::string::npos) pos = str.length();
        
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        
        prev = pos + delimiter.length();
    } while (pos < str.length() && prev < str.length());
    
    return tokens;
}

#endif // INTERRUPTS_HPP