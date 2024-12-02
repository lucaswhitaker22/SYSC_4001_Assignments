#!/bin/bash
cd C:/Users/lwhitaker/Personal/SYSC4001/SYSC_4001_Assignments/Assignment3/tests

output_file="concatenated_output.txt"

# Function to process each file
process_file() {
    local file="$1"
    local filepath=$(dirname "$file")
    local filename=$(basename "$file")
    
    echo "----------" >> "$output_file"
    echo "File path: $filepath" >> "$output_file"
    echo "----------" >> "$output_file"
    echo "File Name: $filename" >> "$output_file"
    echo "<Content>" >> "$output_file"
    
    if [[ $filename == execution_log_* ]]; then
        # For execution log files, only include the last 6 lines
        tail -n 6 "$file" >> "$output_file"
    else
        # For other files, include the entire content
        cat "$file" >> "$output_file"
    fi
    
    echo "</Content>" >> "$output_file"
    echo "" >> "$output_file"
}

# Clear the output file if it exists
> "$output_file"

# Find all .txt files and process them
find . -type f -name "*.txt" | while read -r file; do
    process_file "$file"
done

echo "Concatenation complete. Output saved to $output_file"