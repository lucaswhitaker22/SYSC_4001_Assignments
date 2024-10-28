
#include "interrupts.hpp"

std::vector<MemoryPartition> memory_partitions = {
    {1, 40, "free"}, {2, 25, "free"}, {3, 15, "free"},
    {4, 10, "free"}, {5, 8, "free"}, {6, 2, "occupied"}
};
std::vector<PCB> pcb_table;
std::vector<ExternalFile> external_files;
std::map<int, std::string> vector_table;

unsigned int current_pid = 11;
int current_time = 0;

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
        if (parts.size() == 2) {
            external_files.push_back({parts[0], static_cast<unsigned int>(std::stoi(parts[1]))});
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
        vector_table[vector_num] = line;
        vector_num++;
    }
}

void init_pcb() {
    pcb_table.push_back({current_pid, "init", 6, 1, 0, "Ready"});
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
    log_step("load address " + vector_table[vector_num] + " into the PC", 1);
}

void save_system_status(const std::string& output_file_path) {
    std::ofstream output_file(output_file_path, std::ios::app);
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
    log_step("FORK: copy parent PCB to child PCB", 8);
    
    PCB parent = pcb_table.back();
    PCB child = parent;
    child.pid = ++current_pid;
    child.state = "Ready";
    pcb_table.push_back(child);

    log_step("scheduler called", 2);
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
    log_step("EXEC: load " + program_name + " of size " + std::to_string(program_size) + "Mb", 30);

    auto partition_it = std::min_element(memory_partitions.begin(), memory_partitions.end(),
                                         [&](const MemoryPartition& a, const MemoryPartition& b) {
                                             return (a.status == "free" && a.size >= program_size && a.size < b.size) ||
                                                    (b.status != "free" || b.size < program_size);
                                         });
    if (partition_it == memory_partitions.end() || partition_it->status != "free" || partition_it->size < program_size) {
        std::cerr << "No suitable partition found for " << program_name << std::endl;
        return;
    }

    log_step("found partition " + std::to_string(partition_it->number) + " with " + std::to_string(partition_it->size) + "Mb of space", 10);
    log_step("partition " + std::to_string(partition_it->number) + " marked as occupied", 6);

    partition_it->status = "occupied";
    
    PCB& current_pcb = pcb_table.back();
    current_pcb.name = program_name;
    current_pcb.partition = partition_it->number;
    current_pcb.size = program_size;

    log_step("updating PCB with new information", 2);
    log_step("scheduler called", 2);
    log_step("IRET", 1);
    
    save_system_status(output_directory + "/system_status.txt");
}

void simulate_cpu(int duration) {
    log_step("CPU", duration);
}

void simulate_syscall(int syscall_num, int duration) {
    simulate_syscall(syscall_num);
    log_step("SYSCALL: run the ISR", duration / 3);
    log_step("transfer data", (duration * 2) / 3);
    log_step("check for errors", 13);
    log_step("IRET", 1);
}

void execute_program(const std::string& program_name, const std::string& output_directory, const std::string& input_directory) {
    std::string program_file = input_directory + "/" + program_name + ".txt";
    std::ifstream program(program_file);
    if (!program.is_open()) {
        std::cerr << "Error opening program file: " << program_file << " - " << strerror(errno) << std::endl;
        return;
    }

    std::string line;
    while (std::getline(program, line)) {
        auto parts = split_delim(line, ",");
        if (parts.empty()) continue;

        if (parts[0] == "FORK") {
            simulate_fork(output_directory);
        } else if (parts[0].substr(0, 4) == "EXEC") {
            auto exec_parts = split_delim(parts[0], " ");
            if (exec_parts.size() >= 2) {
                simulate_exec(exec_parts[1], output_directory);
                execute_program(exec_parts[1], output_directory, input_directory);
            }
        } else if (parts[0] == "CPU") {
            if (parts.size() >= 2) {
                simulate_cpu(std::stoi(parts[1]));
            }
        } else if (parts[0].substr(0, 7) == "SYSCALL") {
            auto syscall_parts = split_delim(parts[0], " ");
            if (syscall_parts.size() >= 2 && parts.size() >= 2) {
                simulate_syscall(std::stoi(syscall_parts[1]), std::stoi(parts[1]));
            }
        }
    }
}

void process_trace(const std::string& trace_file_path, const std::string& output_directory,  const std::string& input_directory) {
    std::ifstream trace_file(trace_file_path);
    if (!trace_file.is_open()) {
        std::cerr << "Error opening file: " << trace_file_path << " - " << strerror(errno) << std::endl;
        return;
    }
    std::string line;
    while (std::getline(trace_file, line)) {
        auto parts = split_delim(line, ",");
        if (parts.empty()) continue;
        
        if (parts[0] == "FORK") {
            simulate_fork(output_directory);
        } else if (parts[0].substr(0, 4) == "EXEC") {
            auto exec_parts = split_delim(parts[0], " ");
            if (exec_parts.size() >= 2) {
                simulate_exec(exec_parts[1], output_directory);
                execute_program(exec_parts[1], output_directory, input_directory);
            }
        } else if (parts[0] == "CPU") {
            if (parts.size() >= 2) {
                simulate_cpu(std::stoi(parts[1]));
            }
        } else if (parts[0].substr(0, 7) == "SYSCALL") {
            auto syscall_parts = split_delim(parts[0], " ");
            if (syscall_parts.size() >= 2 && parts.size() >= 2) {
                simulate_syscall(std::stoi(syscall_parts[1]), std::stoi(parts[1]));
            }
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
    mkdir(output_directory.c_str(), 0777);

    // Delete existing output files if they exist
    std::string system_status_file = output_directory + "/system_status.txt";
    std::string execution_file = output_directory + "/execution.txt";
    std::remove(system_status_file.c_str());
    std::remove(execution_file.c_str());

    load_external_files(input_directory + "/external_files.txt");
    load_vector_table(input_directory + "/vector_table.txt");
    init_pcb();

    save_system_status(system_status_file);
    process_trace(input_directory + "/trace.txt", output_directory, input_directory);

    std::ofstream execution_output(execution_file);
    if (execution_output.is_open()) {
        execution_output << execution_log;
        execution_output.close();
    } else {
        std::cerr << "Error opening file for writing: " << execution_file << std::endl;
    }

    std::cout << "Output generated in " << output_directory << std::endl;
    return 0;
}