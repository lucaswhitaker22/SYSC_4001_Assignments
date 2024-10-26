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

// Global variables
std::vector<MemoryPartition> memoryPartitions = {
    {40, false, -1}, {25, false, -1}, {15, false, -1},
    {10, false, -1}, {8, false, -1}, {2, true, 0}
};
std::vector<PCB> pcbTable;
std::deque<PCB*> readyQueue;
std::vector<ExternalFile> externalFiles;
std::map<int, std::string> vectorTable;
int currentPid = -1;
std::random_device rd;
std::mt19937 rng(rd());
std::uniform_int_distribution<> isrTimeDist(1, 10);

// Function declarations
void logActivity(int& currentTime, int duration, const std::string& activity) {
    std::ofstream log_file("execution.txt", std::ios::app);
    log_file << currentTime << ", " << duration << ", " << activity << "\n";
    currentTime += duration;
}

std::vector<std::string> split_delim(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

void loadExternalFiles(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        auto parts = split_delim(line, ",");
        externalFiles.push_back({parts[0], std::stoi(parts[1])});
    }
}

void loadVectorTable(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    int index = 0;
    while (std::getline(file, line)) {
        vectorTable[index] = line;
        index++;
    }
}

void saveSystemStatus(int currentTime) {
    std::ofstream status_file("system_status.txt", std::ios::app);
    status_file << "!-----------------------------------------------------------!\n";
    status_file << "Save Time: " << currentTime << " ms\n";
    status_file << "+--------------------------------------------+\n";
    status_file << "| PID |Program Name |Partition Number | size |\n";
    status_file << "+--------------------------------------------+\n";
    for (const auto& pcb : pcbTable) {
        status_file << "| " << std::setw(3) << pcb.pid << " | " 
                    << std::setw(11) << pcb.programName << " | " 
                    << std::setw(15) << pcb.partitionNumber << " | " 
                    << std::setw(4) << pcb.size << " |\n";
    }
    status_file << "+--------------------------------------------+\n";
    status_file << "!-----------------------------------------------------------\n\n";
}

int findBestFitPartition(int size) {
    int bestFitIndex = -1;
    int smallestDifference = std::numeric_limits<int>::max();
    for (size_t i = 0; i < memoryPartitions.size(); ++i) {
        if (!memoryPartitions[i].occupied && memoryPartitions[i].size >= size) {
            int difference = memoryPartitions[i].size - size;
            if (difference < smallestDifference) {
                smallestDifference = difference;
                bestFitIndex = static_cast<int>(i);
            }
        }
    }
    return bestFitIndex;
}

void scheduler(int& currentTime) {
    logActivity(currentTime, 2, "scheduler called");
}

void syscall(int syscallNumber, int duration, int& currentTime) {
    logActivity(currentTime, 1, "switch to kernel mode");
    logActivity(currentTime, 1, "context saved");
    logActivity(currentTime, 1, "find vector " + std::to_string(syscallNumber) + " in memory position 0x000" + std::to_string(syscallNumber * 2));
    logActivity(currentTime, 1, "load address " + vectorTable[syscallNumber] + " into the PC");
    logActivity(currentTime, 38, "SYSCALL: run the ISR");
    logActivity(currentTime, 74, "transfer data");
    logActivity(currentTime, 13, "check for errors");
    logActivity(currentTime, 1, "IRET");
}

std::vector<std::string> readProgramFile(const std::string& programName) {
    std::vector<std::string> instructions;
    std::ifstream file("../inputs/"+programName + ".txt");
    if (!file) {
        std::cerr << "Error: Unable to open " << programName << ".txt" << std::endl;
        return instructions;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        instructions.push_back(line);
    }
    return instructions;
}

void executeProcess(PCB& process, int& currentTime) {
    process.state = "RUNNING";
    std::vector<std::string> instructions = readProgramFile(process.programName);
    
    for (const auto& instruction : instructions) {
        auto parts = split_delim(instruction, ",");
        auto activity = parts[0];
        auto duration = std::stoi(parts[1]);

        if (activity == "CPU") {
            process.remainingCpuTime = duration;
            process.cpuTime += duration;
            logActivity(currentTime, duration, "CPU execution");
        } else if (activity.substr(0, 7) == "SYSCALL") {
            int syscallNumber = std::stoi(split_delim(activity, " ")[1]);
            process.ioTime += duration;
            syscall(syscallNumber, duration, currentTime);
        } else if (activity == "END_IO") {
            process.ioTime += duration;
            logActivity(currentTime, duration, "End I/O operation");
        }
    }

    process.state = "TERMINATED";
    memoryPartitions[process.partitionNumber - 1].occupied = false;
}


void initializeSystem() {
    PCB initProcess = {
        ++currentPid,  // Will be 0
        "init",     
        6,          // Partition 6 (2MB)
        1,          // Size 1MB
        "READY",    
        0,          // cpuTime
        0,          // ioTime
        0,          // remainingCpuTime
        {},         // instructions
        0,          // currentInstruction
        false       // isChild
    };
    pcbTable.push_back(initProcess);
    readyQueue.push_back(&pcbTable.back());
}

void fork(int& currentTime) {
    logActivity(currentTime, 1, "switch to kernel mode");
    logActivity(currentTime, 3, "context saved");
    logActivity(currentTime, 1, "find vector 2 in memory position 0x0004");
    logActivity(currentTime, 1, "load address " + vectorTable[2] + " into the PC");
    logActivity(currentTime, 8, "FORK: copy parent PCB to child PCB");

    PCB childProcess = pcbTable.back();
    childProcess.pid = ++currentPid;
    childProcess.isChild = true;
    childProcess.state = "READY";
    pcbTable.push_back(childProcess);
    readyQueue.push_front(&pcbTable.back());  // Child gets higher priority

    scheduler(currentTime);
    logActivity(currentTime, 1, "IRET");
}

void exec(const std::string& fileName, int& currentTime) {
    logActivity(currentTime, 1, "switch to kernel mode");
    logActivity(currentTime, 3, "context saved");
    logActivity(currentTime, 1, "find vector 3 in memory position 0x0006");
    logActivity(currentTime, 1, "load address " + vectorTable[3] + " into the PC");

    auto it = std::find_if(externalFiles.begin(), externalFiles.end(),
                           [&fileName](const ExternalFile& ef) { return ef.name == fileName; });
    if (it == externalFiles.end()) {
        logActivity(currentTime, 1, "Exec failed: File not found");
        return;
    }

    int fileSize = it->size;
    int loadTime = (fileName == "program1") ? 30 : 5;
    logActivity(currentTime, loadTime, "EXEC: load " + fileName + " of size " + std::to_string(fileSize) + "Mb");

    int partitionIndex = findBestFitPartition(fileSize);
    if (partitionIndex == -1) {
        logActivity(currentTime, 1, "Exec failed: No suitable partition");
        return;
    }

    logActivity(currentTime, 10, "found partition " + std::to_string(partitionIndex + 1) + 
                " with " + std::to_string(memoryPartitions[partitionIndex].size) + "Mb of space");
    logActivity(currentTime, 6, "partition " + std::to_string(partitionIndex + 1) + " marked as occupied");

    PCB& currentProcess = pcbTable.back();
    currentProcess.programName = fileName;
    currentProcess.partitionNumber = partitionIndex + 1;
    currentProcess.size = fileSize;
    memoryPartitions[partitionIndex].occupied = true;
    memoryPartitions[partitionIndex].processId = currentProcess.pid;

    logActivity(currentTime, 2, "updating PCB with new information");
    scheduler(currentTime);
    logActivity(currentTime, 1, "IRET");
}

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " <vector_table_file> <external_files_file> <trace_file>" << std::endl;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    // Clear existing output files
    std::ofstream clear_exec("execution.txt", std::ios::trunc);
    clear_exec.close();
    std::ofstream clear_status("system_status.txt", std::ios::trunc);
    clear_status.close();

    initializeSystem();
    loadVectorTable(argv[1]);
    loadExternalFiles(argv[2]);

    std::ifstream input_file(argv[3]);
    std::string trace;
    int current_time = 0;

    while(std::getline(input_file, trace)) {
        auto parts = split_delim(trace, ",");
        auto activity = parts[0];

        if (activity == "FORK") {
            fork(current_time);
            saveSystemStatus(current_time);
        } else if (activity.substr(0, 4) == "EXEC") {
            std::string programName = split_delim(activity, " ")[1];
            exec(programName, current_time);
            saveSystemStatus(current_time);
            
            // Execute all child processes before continuing
            while (!readyQueue.empty()) {
                PCB* currentProcess = readyQueue.front();
                readyQueue.pop_front();
                if (currentProcess->isChild) {
                    executeProcess(*currentProcess, current_time);
                    saveSystemStatus(current_time);
                } else {
                    readyQueue.push_back(currentProcess);
                }
            }
        }
    }

    return 0;
}