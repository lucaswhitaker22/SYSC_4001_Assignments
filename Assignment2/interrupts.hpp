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
#include <sys/stat.h>

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
void logActivity(int& currentTime, int duration, const std::string& activity, const std::string& outputDir);
std::vector<std::string> split_delim(const std::string& s, const std::string& delimiter);
void loadExternalFiles(const std::string& inputDir);
void loadVectorTable(const std::string& inputDir);
void saveSystemStatus(int currentTime, const std::string& outputDir);
int findBestFitPartition(int size);
void scheduler(int& currentTime, const std::string& outputDir);
void syscall(int syscallNumber, int& currentTime, const std::string& outputDir);
std::vector<std::string> readProgramFile(const std::string& inputDir, const std::string& programName);
void executeProcess(PCB& process, int& currentTime, const std::string& inputDir);
void initializeSystem();
void fork(int& currentTime, const std::string& outputDir);
void exec(const std::string& fileName, int& currentTime, const std::string& outputDir);
bool fileExists(const std::string& filename);
void printUsage(const char* programName);

// External declarations for global variables
extern std::vector<MemoryPartition> memoryPartitions;
extern std::vector<PCB> pcbTable;
extern std::deque<PCB*> readyQueue;
extern std::vector<ExternalFile> externalFiles;
extern std::map<int, std::string> vectorTable;
extern int currentPid;
extern std::mt19937 rng;
extern std::uniform_int_distribution<> isrTimeDist;

#endif // INTERRUPTS_HPP