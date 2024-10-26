#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include <iomanip>

const int ADDR_BASE = 0x0000;
const int VECTOR_SIZE = 4;

struct MemoryPartition {
    unsigned int number;
    unsigned int size;
    std::string code;
};

struct PCB {
    unsigned int pid;
    std::string name;
    unsigned int partition;
    unsigned int size;
    unsigned int remaining_cpu_time;
    std::string state;
};

struct ExternalFile {
    std::string name;
    unsigned int size;
};

std::vector<MemoryPartition> memory_partitions = {
    {1, 40, "free"}, {2, 25, "free"}, {3, 15, "free"},
    {4, 10, "free"}, {5, 8, "free"}, {6, 2, "init"}
};

std::vector<PCB> pcb_table;
std::vector<ExternalFile> external_files;
std::map<int, std::string> vector_table;

unsigned int current_pid = 11;
int current_time = 0;

std::random_device rd;
std::mt19937 rng(rd());
std::uniform_int_distribution<> exec_time_distr(1, 10);

std::string execution_log;

std::vector<std::string> split_delim(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t last = 0;
    size_t next = 0;
    while ((next = s.find(delimiter, last)) != std::string::npos) {
        tokens.push_back(s.substr(last, next - last));
        last = next + 1;
    }
    tokens.push_back(s.substr(last));
    return tokens;
}

void load_external_files(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << " - " << strerror(errno) << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        auto parts = split_delim(line, ",");
        if (parts.size() != 2) {
            std::cerr << "Invalid line in external_files.txt: " << line << std::endl;
            continue;
        }
        try {
            external_files.push_back({parts[0], static_cast<unsigned int>(std::stoi(parts[1]))});
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line in external_files.txt: " << line << " - " << e.what() << std::endl;
        }
    }
}

void load_vector_table(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << " - " << strerror(errno) << std::endl;
        return;
    }
    std::string line;
    int vector_num = 0;
    while (std::getline(file, line)) {
        try {
            vector_table[vector_num] = line;
            vector_num++;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line in vector_table.txt: " << line << " - " << e.what() << std::endl;
        }
    }
}

void init_pcb() {
    pcb_table.push_back({current_pid++, "init", 6, 1, 0, "Ready"});
}

void log_step(const std::string& step, int duration) {
    execution_log += std::to_string(current_time) + ", " + std::to_string(duration) + ", " + step + "\n";
    current_time += duration;
}

void simulate_syscall(int vector_num) {
    log_step("switch to kernel mode", 1);
    log_step("context saved", 3);
    std::stringstream ss;
    ss << "find vector " << vector_num << " in memory position 0x" 
       << std::setfill('0') << std::setw(4) << std::hex << (ADDR_BASE + (vector_num * VECTOR_SIZE));
    log_step(ss.str(), 1);
    if (vector_table.find(vector_num) == vector_table.end()) {
        std::cerr << "Error: Vector " << vector_num << " not found in vector table" << std::endl;
        return;
    }
    log_step("load address " + vector_table[vector_num] + " into the PC", 1);
}

void save_system_status(const std::string& output_file_path) {
    std::ofstream output_file(output_file_path, std::ios::trunc);
    if (!output_file.is_open()) {
        std::cerr << "Error opening file for writing: " << output_file_path << " - " << strerror(errno) << std::endl;
        return;
    }
    output_file << "!-----------------------------------------------------------!\n";
    output_file << "Save Time: " << current_time << " ms\n";
    output_file << "+--------------------------------------------+\n";
    output_file << "| PID |Program Name |Partition Number | size |\n";
    output_file << "+--------------------------------------------+\n";
    for (const auto& pcb : pcb_table) {
        output_file << "| " << std::setw(3) << pcb.pid << " | " 
                    << std::setw(11) << std::left << pcb.name << " | " 
                    << std::setw(16) << std::left << pcb.partition << " | " 
                    << std::setw(4) << std::left << pcb.size << " |\n";
    }
    output_file << "+--------------------------------------------+\n";
    output_file << "!-----------------------------------------------------------!\n\n";
}

void simulate_fork(const std::string& output_directory) {
    simulate_syscall(2);
    int duration = exec_time_distr(rng);
    log_step("FORK: copy parent PCB to child PCB", duration);
    
    if (pcb_table.empty()) {
        std::cerr << "Error: PCB table is empty" << std::endl;
        return;
    }
    PCB parent = pcb_table.back();
    PCB child = parent;
    child.pid = current_pid++;
    child.state = "Ready";
    pcb_table.push_back(child);

    log_step("scheduler called", exec_time_distr(rng));
    log_step("IRET", 1);
    
    save_system_status(output_directory + "/system_status.txt");
}

void simulate_exec(const std::string& program_name, const std::string& output_directory) {
    simulate_syscall(3);
    
    auto it = std::find_if(external_files.begin(), external_files.end(),
                           [&](const ExternalFile& ef) { return ef.name == program_name; });
    if (it == external_files.end()) {
        std::cerr << "Program not found: " << program_name << std::endl;
        return;
    }

    unsigned int program_size = it->size;
    log_step("EXEC: load " + program_name + " of size " + std::to_string(program_size) + "Mb", exec_time_distr(rng));

    auto partition_it = std::min_element(memory_partitions.begin(), memory_partitions.end(),
                                         [&](const MemoryPartition& a, const MemoryPartition& b) {
                                             return (a.code == "free" && a.size >= program_size && a.size < b.size) ||
                                                    (b.code != "free" || b.size < program_size);
                                         });
    if (partition_it == memory_partitions.end() || partition_it->code != "free" || partition_it->size < program_size) {
        std::cerr << "No suitable partition found for " << program_name << std::endl;
        return;
    }

    log_step("found partition " + std::to_string(partition_it->number) + " with " + std::to_string(partition_it->size) + "Mb of space", exec_time_distr(rng));
    log_step("partition " + std::to_string(partition_it->number) + " marked as occupied", exec_time_distr(rng));

    partition_it->code = program_name;
    
    if (pcb_table.empty()) {
        std::cerr << "Error: PCB table is empty" << std::endl;
        return;
    }
    PCB& current_pcb = pcb_table.back();
    current_pcb.name = program_name;
    current_pcb.partition = partition_it->number;
    current_pcb.size = program_size;

    log_step("updating PCB with new information", exec_time_distr(rng));
    log_step("scheduler called", exec_time_distr(rng));
    log_step("IRET", 1);
    
    save_system_status(output_directory + "/system_status.txt");
}

void simulate_cpu(int duration) {
    log_step("CPU", duration);
}

void simulate_syscall(int syscall_num, int duration) {
    simulate_syscall(syscall_num);
    log_step(std::to_string(syscall_num) + " ISR execution", duration / 3);
    log_step("transfer data", (duration * 2) / 3);
    log_step("check for errors", exec_time_distr(rng));
    log_step("IRET", 1);
}

void process_trace(const std::string& trace_file_path, const std::string& output_directory) {
    std::ifstream trace_file(trace_file_path);
    if (!trace_file.is_open()) {
        std::cerr << "Error opening file: " << trace_file_path << " - " << strerror(errno) << std::endl;
        return;
    }
    std::string line;
    while (std::getline(trace_file, line)) {
        auto parts = split_delim(line, ",");
        if (parts.empty()) {
            std::cerr << "Invalid line in trace file: " << line << std::endl;
            continue;
        }
        if (parts[0] == "FORK") {
            simulate_fork(output_directory);
        } else if (parts[0].substr(0, 4) == "EXEC") {
            auto exec_parts = split_delim(parts[0], " ");
            if (exec_parts.size() < 2) {
                std::cerr << "Invalid EXEC command: " << parts[0] << std::endl;
                continue;
            }
            simulate_exec(exec_parts[1], output_directory);
        } else if (parts[0] == "CPU") {
            if (parts.size() < 2) {
                std::cerr << "Invalid CPU command: " << line << std::endl;
                continue;
            }
            try {
                simulate_cpu(std::stoi(parts[1]));
            } catch (const std::exception& e) {
                std::cerr << "Error parsing CPU duration: " << parts[1] << " - " << e.what() << std::endl;
            }
        } else if (parts[0].substr(0, 7) == "SYSCALL") {
            auto syscall_parts = split_delim(parts[0], " ");
            if (syscall_parts.size() < 2 || parts.size() < 2) {
                std::cerr << "Invalid SYSCALL command: " << line << std::endl;
                continue;
            }
            try {
                simulate_syscall(std::stoi(syscall_parts[1]), std::stoi(parts[1]));
            } catch (const std::exception& e) {
                std::cerr << "Error parsing SYSCALL: " << line << " - " << e.what() << std::endl;
            }
        } else {
            std::cerr << "Unknown command in trace file: " << parts[0] << std::endl;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <PATH TO INPUT DIRECTORY>" << std::endl;
        return 1;
    }

    std::string input_directory(argv[1]);
    std::string output_directory = input_directory + "/outputs";
    if (mkdir(output_directory.c_str(), 0777) == -1) {
        if (errno != EEXIST) {
            std::cerr << "Failed to create output directory: " << output_directory << " - " << strerror(errno) << std::endl;
            return 1;
        }
    }

    load_external_files(input_directory + "/external_files.txt");
    load_vector_table(input_directory + "/vector_table.txt");
    init_pcb();

    save_system_status(output_directory + "/system_status.txt");
    process_trace(input_directory + "/trace.txt", output_directory);

    std::ofstream execution_file(output_directory + "/execution.txt", std::ios::out);
    if (!execution_file.is_open()) {
        std::cerr << "Error opening file for writing: " << output_directory + "/execution.txt" << " - " << strerror(errno) << std::endl;
        return 1;
    }
    execution_file << execution_log;
    execution_file.close();

    save_system_status(output_directory + "/system_status.txt");

    std::cout << "Output generated in " << output_directory << std::endl;
    return 0;
}