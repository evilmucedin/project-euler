#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input_md_file> <output_cpp_file>"
    exit 1
fi

input_md_file="$1"
output_cpp_file="$2"

# Check if the input file exists
if [ ! -f "$input_md_file" ]; then
    echo "Input file '$input_md_file' not found!"
    exit 1
fi

# Initialize flags and variables
extracting=false
current_line=""
output=""

# Read the input Markdown file line by line
while IFS= read -r line; do
    if [[ "$line" == "```cpp" ]]; then
        # Start extracting content
        extracting=true
        continue
    elif [[ "$line" == "```" && "$extracting" == true ]]; then
        # Stop extracting content
        extracting=false
        break
    fi

    if [ "$extracting" == true ]; then
        # Append the line to the output, converting Markdown to plain text
        # Example: Remove markdown formatting like *bold* and _italic_
        current_line=$(echo "$line" | sed 's/\*\([^*]*\)\*/\1/g' | sed 's/_\([^_]*\)_/\\1/g')
        output+="$current_line\n"
    fi
done < "$input_md_file"

# Write the output to the C++ file, ensuring proper formatting
echo -e "#include <iostream>\n\nint main() {\n$output}\n" > "$output_cpp_file"

echo "Extraction and conversion completed. Output saved in '$output_cpp_file'."

