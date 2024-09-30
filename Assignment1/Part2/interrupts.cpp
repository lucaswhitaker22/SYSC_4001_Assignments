#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <iomanip>

struct Activity {
    std::string type;
    int duration;
};

struct VectorTableEntry {
    int syscall_number;
    int isr_address;
};

std::vector<VectorTableEntry> readVectorTable(const std::string& filename) {
    std::vector<VectorTableEntry> vectorTable;
    std::ifstream file(filename);
    std::string line;
    int syscall_number = 0;

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open vector table file: " + filename);
    }

    while (std::getline(file, line)) {
        int isr_address;
        std::istringstream iss(line);
        if (iss >> std::hex >> isr_address) {
            vectorTable.push_back({syscall_number, isr_address});
            syscall_number++;
        }
    }

    return vectorTable;
}

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

void simulateInterrupts(const std::vector<Activity>& trace, const std::vector<VectorTableEntry>& vectorTable, const std::string& outputFile) {
    std::ofstream execFile(outputFile);
    if (!execFile.is_open()) {
        throw std::runtime_error("Unable to open output file: " + outputFile);
    }

    int currentTime = 0;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> contextDist(1, 3);

    for (const auto& activity : trace) {
        if (activity.type == "CPU") {
            execFile << currentTime << ", " << activity.duration << ", CPU execution\n";
            currentTime += activity.duration;
        } else if (activity.type.find("SYSCALL") != std::string::npos) {
            int syscall_num = std::stoi(activity.type.substr(8));
            
            execFile << currentTime << ", 1, switch to kernel mode\n";
            currentTime += 1;
            
            int contextSaveTime = contextDist(rng);
            execFile << currentTime << ", " << contextSaveTime << ", context saved\n";
            currentTime += contextSaveTime;
            
            execFile << currentTime << ", 1, find vector " << syscall_num << " in memory position 0x" 
                     << std::hex << std::setw(4) << std::setfill('0') << (syscall_num * 2) << std::dec << "\n";
            currentTime += 1;
            
            execFile << currentTime << ", 1, load address 0X" 
                     << std::hex << std::uppercase << std::setw(4) << std::setfill('0') 
                     << vectorTable[syscall_num].isr_address << std::dec << " into the PC\n";
            currentTime += 1;
            
            // Simulate ISR execution with three steps
            int totalIsrTime = activity.duration - 4 - contextSaveTime;
            int runIsrTime = totalIsrTime * 2 / 5;
            int transferDataTime = totalIsrTime * 2 / 5;
            int checkErrorsTime = totalIsrTime - runIsrTime - transferDataTime;
            
            execFile << currentTime << ", " << runIsrTime << ", SYSCALL: run the ISR\n";
            currentTime += runIsrTime;
            
            execFile << currentTime << ", " << transferDataTime << ", transfer data\n";
            currentTime += transferDataTime;
            
            execFile << currentTime << ", " << checkErrorsTime << ", check for errors\n";
            currentTime += checkErrorsTime;
            
            execFile << currentTime << ", 1, IRET\n";
            currentTime += 1;
            
        } else if (activity.type.find("END_IO") != std::string::npos) {
            int io_num = std::stoi(activity.type.substr(7));
            
            execFile << currentTime << ", 1, check priority of interrupt\n";
            currentTime += 1;
            execFile << currentTime << ", 1, check if masked\n";
            currentTime += 1;
            execFile << currentTime << ", 1, switch to kernel mode\n";
            currentTime += 1;
            
            int contextSaveTime = contextDist(rng);
            execFile << currentTime << ", " << contextSaveTime << ", context saved\n";
            currentTime += contextSaveTime;
            
            execFile << currentTime << ", 1, find vector " << io_num << " in memory position 0x" 
                     << std::hex << std::setw(4) << std::setfill('0') << (io_num * 2) << std::dec << "\n";
            currentTime += 1;
            
            execFile << currentTime << ", 1, load address 0X" 
                     << std::hex << std::uppercase << std::setw(4) << std::setfill('0') 
                     << vectorTable[io_num].isr_address << std::dec << " into the PC\n";
            currentTime += 1;
            
            execFile << currentTime << ", " << (activity.duration - 6 - contextSaveTime) << ", END_IO\n";
            currentTime += (activity.duration - 6 - contextSaveTime);
            
            execFile << currentTime << ", 1, IRET\n";
            currentTime += 1;
        }
    }

    execFile.close();
}
int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <vector_table_file> <output_file>\n";
        return 1;
    }
    try {
        std::vector<Activity> trace = readTrace(argv[1]);
        std::vector<VectorTableEntry> vectorTable = readVectorTable(argv[2]);
        simulateInterrupts(trace, vectorTable, argv[3]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}