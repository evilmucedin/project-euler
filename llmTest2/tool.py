#!/usr/bin/env python

import sys

def extract_and_format_cpp(input_file, output_file):
    with open(input_file, 'r') as f:
        lines = f.readlines()

    start_marker = "```cpp"
    end_marker = "```"

    # Find the start and end indices of the code block
    start_index = None
    end_index = None

    for i, line in enumerate(lines):
        if start_marker in line:
            start_index = i + 1  # Start from the next line after the marker
        elif end_marker in line and start_index is not None:
            end_index = i
            break

    if start_index is None or end_index is None:
        print("Error: Could not find code block markers.")
        return

    # Extract lines within the code block
    extracted_lines = lines[start_index:end_index]

    # Remove markdown formatting (e.g., backticks, emphasis)
    formatted_lines = []
    for line in extracted_lines:
        formatted_line = line.strip()
        if formatted_line.startswith('`') and formatted_line.endswith('`'):formatted_line.endswith('`'):
            formatted_line = formatted_line[1:-1]
        formatted_lines.append(formatted_line)

    # Write the formatted lines to the output file
    with open(output_file, 'w') as f:
        for line in formatted_lines:
            f.write(line + '\n')

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python extract_cpp.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    extract_and_format_cpp(input_file, output_file)
