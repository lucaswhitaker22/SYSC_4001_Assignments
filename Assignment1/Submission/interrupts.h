#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <random>
#include <chrono>
#include <map>
#include <numeric>

// Structures
struct Activity {
    std::string type;
    int duration;
};

struct VectorTableEntry {
    int syscall_number;
    int isr_address;
};

struct PerformanceMetrics {
    int totalExecutionTime = 0;
    int totalContextSwitches = 0;
    int totalISRExecutions = 0;
    int timeSpentOnVectorTableLookups = 0;
    int timeSpentInInterruptHandling = 0;
    int timeSpentInCPUOperations = 0;
    int timeSpentInIOOperations = 0;
    int totalContextSwitchTime = 0;
    int totalSyscallTime = 0;
    int totalISRExecutionTime = 0;
    std::vector<int> ioResponseTimes;
    std::map<int, int> syscallStartTimes;
};

// Function declarations
std::vector<VectorTableEntry> readVectorTable(const std::string &filename);
std::vector<Activity> readTrace(const std::string &filename);
void simulateInterrupts(const std::vector<Activity> &trace, 
                        const std::vector<VectorTableEntry> &vectorTable, 
                        const std::string &outputFile, 
                        int min_context_time = 1, 
                        int max_context_time = 3);

#endif // INTERRUPT_H