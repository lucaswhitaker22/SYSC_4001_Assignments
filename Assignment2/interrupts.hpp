#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>
#include <queue>
#include <deque>

// Constants
const int ADDR_BASE = 0x0000;
const int VECTOR_SIZE = 2;

// Function declarations
std::vector<std::string> split_delim(const std::string& s, const std::string& delimiter);

#endif // INTERRUPTS_HPP