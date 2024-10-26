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
#include <filesystem>

// Constants
const int ADDR_BASE = 0x0000;
const int VECTOR_SIZE = 2;

// Structures
struct MemoryPartition {
    int size;
    bool occupied;
    int processId;
};

struct PCB {
    int pid;
    std::string programName;
    int partitionNumber;
    int size;
    std::string state;
    int cpuTime;
    int ioTime;
    int remainingCpuTime;
    std::vector<std::string> instructions;
    size_t currentInstruction;
    bool isChild;
};

struct ExternalFile {
    std::string name;
    int size;
};

// Function declarations
void logActivity(int& currentTime, int duration, const std::string& activity);
std::vector<std::string> split_delim(const std::string& s, const std::string& delimiter);
void loadExternalFiles(const std::string& directory);
void loadVectorTable(const std::string& directory);
void saveSystemStatus(int currentTime);
int findBestFitPartition(int size);
void scheduler(int& currentTime);
void syscall(int syscallNumber, int& currentTime);
std::vector<std::string> readProgramFile(const std::string& directory, const std::string& programName);
void executeProcess(const std::string& directory, PCB& process, int& currentTime);
void initializeSystem();
void fork(int& currentTime);
void exec(const std::string& fileName, int& currentTime);
bool fileExists(const std::string& filename);
void printUsage(const char* programName);

#endif // INTERRUPTS_HPP