# SYSC_4001_Assignments

## Compile
#### Go to directory
```bash
cd Assignment1/Part2
```

#### Compilation
```bash
g++ -std=c++17 -o interrupts interrupts.cpp
```

## Running Tests

#### Running the Program

```bash
./interrupts <trace_file> <vector_table_file> <output_file> [min_context_time] [max_context_time]
```

Where:
- <trace_file> is the path to the input trace file
- <vector_table_file> is the path to the vector table file
- <output_file> is the path where the execution log will be written
- [min_context_time] (optional) is the minimum time for context switching (default: 1)
- [max_context_time] (optional) is the maximum time for context switching (default: 3)

#### Example Usage
```bash
./interrupts simulations/example/trace.txt simulations/example/vector_table.txt simulations/example/execution.txt
```

