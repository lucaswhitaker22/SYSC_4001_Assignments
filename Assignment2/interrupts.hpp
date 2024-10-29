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

/**
 * @param file_path The path to the file containing the vector table.
 */
void load_vector_table(const std::string& file_path);

/**
 * Initializes the Process Control Block (PCB) with the initial process.
 */
void init_pcb();

/**
 * @param step The description of the step.
 * @param duration The duration of the step in milliseconds.
 */
void log_step(const std::string& step, int duration);

/**
 * @param vector_num The vector number of the system call.
 */
void simulate_syscall(int vector_num);

/**
 * @param output_file_path The path to the output file.
 */
void save_system_status(const std::string& output_file_path);

/**
 * @param output_directory The directory to save output files.
 */
void simulate_fork(const std::string& output_directory);

/**
 * @param program_name The name of the program to execute.
 * @param output_directory The directory to save output files.
 */
void simulate_exec(const std::string& program_name, const std::string& output_directory);

/**
 * @param duration The duration of CPU execution in milliseconds.
 */
void simulate_cpu(int duration);

/**
 * @param syscall_num The system call number.
 * @param duration The duration of the system call in milliseconds.
 */
void simulate_syscall(int syscall_num, int duration);

/**
 * @param program_name The name of the program to execute.
 * @param output_directory The directory to save output files.
 * @param input_directory The directory containing input files.
 */
void execute_program(const std::string& program_name, const std::string& output_directory, const std::string& input_directory);

/**
 * @param trace_file_path The path to the trace file.
 * @param output_directory The directory to save output files.
 * @param input_directory The directory containing input files.
 */
void process_trace(const std::string& trace_file_path, const std::string& output_directory, const std::string& input_directory);

#endif