#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>

const int ADDR_BASE = 0x0000;
const int VECTOR_SIZE = 2;

struct MemoryPartition {
    unsigned int number;
    unsigned int size;
    std::string status;
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

extern std::vector<MemoryPartition> memory_partitions;
extern std::vector<PCB> pcb_table;
extern std::vector<ExternalFile> external_files;
extern std::map<int, std::string> vector_table;

extern unsigned int current_pid;
extern int current_time;

extern std::string execution_log;

std::vector<std::string> split_delim(const std::string& s, const std::string& delimiter);

void load_external_files(const std::string& file_path);
void load_vector_table(const std::string& file_path);
void init_pcb();
void log_step(const std::string& step, int duration);
void simulate_syscall(int vector_num);
void save_system_status(const std::string& output_file_path);
void simulate_fork(const std::string& output_directory);
void simulate_exec(const std::string& program_name, const std::string& output_directory);
void simulate_cpu(int duration);
void simulate_syscall(int syscall_num, int duration);
void execute_program(const std::string& program_name, const std::string& output_directory, const std::string& input_directory);
void process_trace(const std::string& trace_file_path, const std::string& output_directory, const std::string& input_directory);

#endif