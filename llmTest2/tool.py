#!/usr/bin/env python

import sys

def extract_cpp_code(input_file, output_file):
    try:
        with open(input_file, 'r') as md_file:
            lines = md_file.readlines()

        extracting = False
        cpp_code = []

        for line in lines:
            if "```cpp" in line:
                extracting = True
                continue
            elif "```" in line and extracting:
                extracting = False
                break

            if extracting:
                # Remove Markdown formatting from the line
                plain_text_line = line.strip() # line.replace("*", "").replace("#", "").strip()
                cpp_code.append(plain_text_line)

        with open(output_file, 'w') as cpp_file:
            for line in cpp_code:
                cpp_file.write(line + "\n")

        print(f"Successfully extracted and saved C++ code to {output_file}")

    except FileNotFoundError:
        print(f"Error: The file {input_file} does not exist.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python extract_cpp.py <input_md_file> <output_cpp_file>")
    else:
        input_md_file = sys.argv[1]
        output_cpp_file = sys.argv[2]
        extract_cpp_code(input_md_file, output_cpp_file)

