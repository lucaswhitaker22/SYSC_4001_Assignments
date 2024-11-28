#!/bin/bash
cd /workspaces/SYSC_4001_Assignments/Assignment3/tests

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
    cat "$file" >> "$output_file"
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