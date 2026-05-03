#!/usr/bin/env python

import sys
import re

def extract_and_convert_code(input_file, output_file):
    try:
        with open(input_file, 'r') as infile:
            lines = infile.readlines()

        start_line = None
        end_line = None

        # Find the start and end line indices
        for i, line in enumerate(lines):
            if "```cpp" in line:
                start_line = i + 1
            elif "```" in line:
                end_line = i
                break

        if start_line is None or end_line is None:
            print("Error: Could not find the start and/or end code block.")
            return

        # Extract lines between ```cpp and ```
        extracted_lines = lines[start_line:end_line]

        # Convert Markdown formatted code to plain text C++ format
        cpp_code = ""
        for line in extracted_lines:
            # Remove leading and trailing whitespace
            stripped_line = line.strip()

            # Skip empty lines or lines with only markdown formatting characters
            if not stripped_line or stripped_line.startswith("#") or stripped_line.startswith("-"):
                continue

            # Convert Markdown lists to C++ comments
            if stripped_line.startswith("*"):
                cpp_code += "// " + stripped_line[2:].strip() + "\n"
            else:
                cpp_code += stripped_line + "\n"

        # Write the extracted and converted code to the output file
        with open(output_file, 'w') as outfile:
            outfile.write(cpp_code)

        print(f"Successfully extracted and converted code to {output_file}.")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python extract_cpp_code.py <input_file> <output_file>")
    else:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        extract_and_convert_code(input_file, output_file)
