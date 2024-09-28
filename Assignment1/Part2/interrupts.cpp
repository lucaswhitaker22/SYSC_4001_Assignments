#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <unordered_map>
#include <iomanip>

struct Activity {
    std::string type;
    int duration;
};

struct VectorTableEntry {
    int syscall_number;
    int isr_address;
};

std::vector<VectorTableEntry> vectorTable = {
    {7, 14},
    {12, 22},
    {20, 40}
};

std::vector<Activity> readTrace(const std::string& filename) {
    std::vector<Activity> trace;
    std::ifstream file(filename);
    std::string line;
    
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open trace file: " + filename);
    }
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        int duration;
        
        if (std::getline(iss, type, ',') && iss >> duration) {
            trace.push_back({type, duration});
        }
    }
    
    return trace;
}

void simulateInterrupts(const std::vector<Activity>& trace, const std::string& outputFile) {
    std::ofstream execFile(outputFile);
    if (!execFile.is_open()) {
        throw std::runtime_error("Unable to open output file: " + outputFile);
    }

    int currentTime = 0;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> contextDist(1, 3);
    std::uniform_int_distribution<int> isrActivityDist(100, 400);

    int totalCPUTime = 0;
    int totalIOTime = 0;
    int totalOverhead = 0;

    for (const auto& activity : trace) {
        if (activity.type == "CPU") {
            currentTime += activity.duration;
            execFile << currentTime << ", " << activity.duration << ", CPU activity\n";
            totalCPUTime += activity.duration;
        } else if (activity.type.find("SYSCALL") != std::string::npos) {
            int syscall_num = std::stoi(activity.type.substr(8));
            
            // Switch to kernel mode
            currentTime += 1;
            execFile << currentTime << ", 1, switch to kernel mode\n";
            totalOverhead += 1;
            
            // Save context
            int saveTime = contextDist(rng);
            currentTime += saveTime;
            execFile << currentTime << ", " << saveTime << ", context saved\n";
            totalOverhead += saveTime;
            
            // Find vector
            currentTime += 1;
            execFile << currentTime << ", 1, find vector " << syscall_num << " in memory position " << (syscall_num * 2) << "\n";
            totalOverhead += 1;
            
            // Obtain ISR address
            currentTime += 1;
            execFile << currentTime << ", 1, obtain ISR address\n";
            totalOverhead += 1;
            
            // Execute ISR
            int isrDuration = activity.duration;
            currentTime += isrDuration;
            execFile << currentTime << ", " << isrDuration << ", call device driver\n";
            totalIOTime += isrDuration;
            
            // IRET
            currentTime += 1;
            execFile << currentTime << ", 1, IRET\n";
            totalOverhead += 1;
        } else if (activity.type.find("END_IO") != std::string::npos) {
            int io_num = std::stoi(activity.type.substr(7));
            currentTime += activity.duration;
            execFile << currentTime << ", " << activity.duration << ", end of I/O " << io_num << ": interrupt\n";
            totalIOTime += activity.duration;
        }
    }

    int totalTime = currentTime;
    execFile << "\nSummary:\n";
    execFile << "Total time: " << totalTime << " ms\n";
    execFile << "CPU time: " << totalCPUTime << " ms (" << std::fixed << std::setprecision(2) << (100.0 * totalCPUTime / totalTime) << "%)\n";
    execFile << "I/O time: " << totalIOTime << " ms (" << std::fixed << std::setprecision(2) << (100.0 * totalIOTime / totalTime) << "%)\n";
    execFile << "Overhead: " << totalOverhead << " ms (" << std::fixed << std::setprecision(2) << (100.0 * totalOverhead / totalTime) << "%)\n";

    execFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }

    try {
        std::vector<Activity> trace = readTrace(argv[1]);
        simulateInterrupts(trace, argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}