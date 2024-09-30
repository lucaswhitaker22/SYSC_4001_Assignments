#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <iomanip>
#include <map>
struct Activity
{
    std::string type;
    int duration;
};

struct VectorTableEntry
{
    int syscall_number;
    int isr_address;
};

std::vector<VectorTableEntry> readVectorTable(const std::string &filename)
{
    std::vector<VectorTableEntry> vectorTable;
    std::ifstream file(filename);
    std::string line;
    int syscall_number = 0;

    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open vector table file: " + filename);
    }

    while (std::getline(file, line))
    {
        int isr_address;
        std::istringstream iss(line);
        if (iss >> std::hex >> isr_address)
        {
            vectorTable.push_back({syscall_number, isr_address});
            syscall_number++;
        }
    }

    return vectorTable;
}

std::vector<Activity> readTrace(const std::string &filename)
{
    std::vector<Activity> trace;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open trace file: " + filename);
    }

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string type;
        int duration;

        if (std::getline(iss, type, ',') && iss >> duration)
        {
            trace.push_back({type, duration});
        }
    }

    return trace;
}

struct PerformanceMetrics
{
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
void simulateInterrupts(const std::vector<Activity> &trace, const std::vector<VectorTableEntry> &vectorTable, const std::string &outputFile, int min_context_time = 1, int max_context_time = 3)
{
    // Open output file
    std::ofstream execFile(outputFile);
    if (!execFile.is_open())
    {
        throw std::runtime_error("Unable to open output file: " + outputFile);
    }

    int currentTime = 0;
    // Initialize random number generator for context switch times
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> contextDist(min_context_time, max_context_time);

    // Initialize performance metrics
    PerformanceMetrics metrics;

    // Iterate through each activity in the trace
    for (const auto &activity : trace)
    {
        if (activity.type == "CPU")
        {
            // Handle CPU operations
            execFile << currentTime << ", " << activity.duration << ", CPU execution\n";
            currentTime += activity.duration;
            metrics.totalExecutionTime += activity.duration;
            metrics.timeSpentInCPUOperations += activity.duration;
        }
        else if (activity.type.find("SYSCALL") != std::string::npos)
        {
            // Handle system calls
            int syscall_num = std::stoi(activity.type.substr(7));
            int syscallStartTime = currentTime;

            // Context switch to kernel mode
            int contextSwitchTime = contextDist(rng);
            execFile << currentTime << ", " << contextSwitchTime << ", switch to kernel mode and context saved\n";
            currentTime += contextSwitchTime;
            metrics.totalContextSwitches++;
            metrics.totalExecutionTime += contextSwitchTime;
            metrics.totalContextSwitchTime += contextSwitchTime;

            // Find vector in memory
            execFile << currentTime << ", 1, find vector " << syscall_num << " in memory position 0x"
                     << std::hex << std::setw(4) << std::setfill('0') << (syscall_num * 2) << std::dec << "\n";
            currentTime += 1;
            metrics.timeSpentOnVectorTableLookups += 1;
            metrics.totalExecutionTime += 1;

            // Load ISR address into PC
            execFile << currentTime << ", 1, load address 0X"
                     << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
                     << vectorTable[syscall_num].isr_address << std::dec << " into the PC\n";
            currentTime += 1;
            metrics.totalISRExecutions++;
            metrics.totalExecutionTime += 1;

            // Calculate ISR execution times
            int totalIsrTime = activity.duration - 2 - contextSwitchTime;
            int runIsrTime = totalIsrTime * 2 / 5;
            int transferDataTime = totalIsrTime * 2 / 5;
            int checkErrorsTime = totalIsrTime - runIsrTime - transferDataTime;

            // Run ISR
            execFile << currentTime << ", " << runIsrTime << ", SYSCALL: run the ISR\n";
            currentTime += runIsrTime;
            metrics.timeSpentInInterruptHandling += runIsrTime;
            metrics.totalExecutionTime += runIsrTime;

            // Transfer data
            execFile << currentTime << ", " << transferDataTime << ", transfer data\n";
            currentTime += transferDataTime;
            metrics.timeSpentInIOOperations += transferDataTime;
            metrics.totalExecutionTime += transferDataTime;

            // Check for errors
            execFile << currentTime << ", " << checkErrorsTime << ", check for errors\n";
            currentTime += checkErrorsTime;
            metrics.timeSpentInInterruptHandling += checkErrorsTime;
            metrics.totalExecutionTime += checkErrorsTime;

            // Return from interrupt
            int iretTime = contextDist(rng);
            execFile << currentTime << ", " << iretTime << ", IRET and context restored\n";
            currentTime += iretTime;
            metrics.totalExecutionTime += iretTime;
            metrics.totalContextSwitchTime += iretTime;

            metrics.syscallStartTimes[syscall_num] = currentTime;
            metrics.totalSyscallTime += (currentTime - syscallStartTime);
            metrics.totalISRExecutionTime += totalIsrTime;
        }
        else if (activity.type.find("END_IO") != std::string::npos)
        {
            // Handle I/O completion
            int io_num = std::stoi(activity.type.substr(6));

            // Check interrupt priority and mask
            execFile << currentTime << ", 1, check priority of interrupt\n";
            currentTime += 1;
            execFile << currentTime << ", 1, check if masked\n";
            currentTime += 1;

            // Context switch to kernel mode
            int contextSwitchTime = contextDist(rng);
            execFile << currentTime << ", " << contextSwitchTime << ", switch to kernel mode and context saved\n";
            currentTime += contextSwitchTime;
            metrics.totalContextSwitches++;
            metrics.totalExecutionTime += contextSwitchTime + 2;
            metrics.totalContextSwitchTime += contextSwitchTime;

            // Find vector in memory
            execFile << currentTime << ", 1, find vector " << io_num << " in memory position 0x"
                     << std::hex << std::setw(4) << std::setfill('0') << (io_num * 2) << std::dec << "\n";
            currentTime += 1;
            metrics.timeSpentOnVectorTableLookups += 1;
            metrics.totalExecutionTime += 1;

            // Load ISR address into PC
            execFile << currentTime << ", 1, load address 0X"
                     << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
                     << vectorTable[io_num].isr_address << std::dec << " into the PC\n";
            currentTime += 1;
            metrics.totalISRExecutions++;
            metrics.totalExecutionTime += 1;

            // Handle I/O completion
            int ioHandlingTime = activity.duration - 4 - contextSwitchTime;
            execFile << currentTime << ", " << ioHandlingTime << ", END_IO\n";
            currentTime += ioHandlingTime;
            metrics.timeSpentInInterruptHandling += ioHandlingTime;
            metrics.totalExecutionTime += ioHandlingTime;
            metrics.totalISRExecutionTime += ioHandlingTime;

            // Return from interrupt
            int iretTime = contextDist(rng);
            execFile << currentTime << ", " << iretTime << ", IRET and context restored\n";
            currentTime += iretTime;
            metrics.totalExecutionTime += iretTime;
            metrics.totalContextSwitchTime += iretTime;

            // Calculate I/O response time
            if (metrics.syscallStartTimes.find(io_num) != metrics.syscallStartTimes.end())
            {
                int responseTime = currentTime - metrics.syscallStartTimes[io_num];
                metrics.ioResponseTimes.push_back(responseTime);
                metrics.syscallStartTimes.erase(io_num);
            }
        }
    }

    // Calculate additional metrics
    int totalIOTime = metrics.timeSpentInIOOperations;
    int totalCPUTime = metrics.timeSpentInCPUOperations;
    double cpuUtilization = static_cast<double>(totalCPUTime) / metrics.totalExecutionTime * 100.0;
    double averageIOResponseTime = 0;
    if (!metrics.ioResponseTimes.empty())
    {
        averageIOResponseTime = static_cast<double>(std::accumulate(metrics.ioResponseTimes.begin(), metrics.ioResponseTimes.end(), 0)) / metrics.ioResponseTimes.size();
    }

    // Write all performance metrics to the output file
    execFile << "\nPerformance Metrics:\n";
    execFile << "Total Execution Time: " << metrics.totalExecutionTime << "\n";
    execFile << "Total Context Switches: " << metrics.totalContextSwitches << "\n";
    execFile << "Total ISR Executions: " << metrics.totalISRExecutions << "\n";
    execFile << "Time Spent on Vector Table Lookups: " << metrics.timeSpentOnVectorTableLookups << "\n";
    execFile << "Time Spent in Interrupt Handling: " << metrics.timeSpentInInterruptHandling << "\n";
    execFile << "Time Spent in CPU Operations: " << metrics.timeSpentInCPUOperations << "\n";
    execFile << "Time Spent in I/O Operations: " << metrics.timeSpentInIOOperations << "\n";
    execFile << "Total Context Switch Time: " << metrics.totalContextSwitchTime << "\n";
    execFile << "Total Syscall Time: " << metrics.totalSyscallTime << "\n";
    execFile << "Total ISR Execution Time: " << metrics.totalISRExecutionTime << "\n";
    execFile << "CPU Utilization: " << std::fixed << std::setprecision(2) << cpuUtilization << "%\n";
    execFile << "Average I/O Response Time: " << std::fixed << std::setprecision(2) << averageIOResponseTime << "\n";
    execFile << "Ratio of CPU to I/O Time: " << std::fixed << std::setprecision(2) << static_cast<double>(totalCPUTime) / totalIOTime << "\n";
    execFile << "System Throughput: " << std::fixed << std::setprecision(2) << static_cast<double>(metrics.totalISRExecutions) / metrics.totalExecutionTime << " ISRs per time unit\n";
    execFile << "Number of I/O Operations: " << metrics.ioResponseTimes.size() << "\n";
    execFile << "Average Syscall Time: " << std::fixed << std::setprecision(2) << static_cast<double>(metrics.totalSyscallTime) / metrics.totalISRExecutions << "\n";
    execFile << "Average ISR Execution Time: " << std::fixed << std::setprecision(2) << static_cast<double>(metrics.totalISRExecutionTime) / metrics.totalISRExecutions << "\n";
    execFile << "min_context_time: " << min_context_time << "\n";
    execFile << "max_context_time: " << max_context_time << "\n";

    execFile.close();
}
int main(int argc, char *argv[])
{
    if (argc < 4 || argc > 6)
    {
        std::cerr << "Usage: " << argv[0] << " <trace_file> <vector_table_file> <output_file> [min_context_time] [max_context_time]\n";
        return 1;
    }

    try
    {
        std::vector<Activity> trace = readTrace(argv[1]);
        std::vector<VectorTableEntry> vectorTable = readVectorTable(argv[2]);

        if (argc == 4)
        {
            simulateInterrupts(trace, vectorTable, argv[3]);
        }
        else if (argc == 6)
        {
            int min_context_time = std::stoi(argv[4]);
            int max_context_time = std::stoi(argv[5]);
            simulateInterrupts(trace, vectorTable, argv[3], min_context_time, max_context_time);
        }
        else
        {
            std::cerr << "Invalid number of arguments\n";
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}