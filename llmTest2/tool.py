#!/usr/bin/env python

import argparse

def extract_cpp_code(md_content):
    """
    Extracts C++ code blocks from markdown content.
    
    Args:
        md_content (str): Markdown content as a single string.
        
    Returns:
        str: A string containing the C++ code extracted from the markdown.
    """
    lines = md_content.splitlines()
    cpp_code = []
    in_cpp_block = False
    
    for line in lines:
        if "```cpp" in line:
            in_cpp_block = True
        elif "```" in line and in_cpp_block:
            break
        elif in_cpp_block:
            cpp_code.append(line)
    
    return "\n".join(cpp_code)

def main():
    # Create the argument parser
    parser = argparse.ArgumentParser(description='Extract C++ code from a Markdown file.')
    parser.add_argument('md_file', help='Path to the input Markdown file')
    parser.add_argument('cpp_file', help='Path to the output C++ file')

    # Parse arguments
    args = parser.parse_args()

    try:
        # Read the Markdown file
        with open(args.md_file, 'r') as md_file:
            md_content = md_file.read()

        # Extract C++ code
        cpp_code = extract_cpp_code(md_content)

        # Write C++ code to a new file
        with open(args.cpp_file, 'w') as cpp_file:
            cpp_file.write(cpp_code)

        print(f"C++ code successfully extracted and written to {args.cpp_file}")

    except FileNotFoundError:
        print(f"Error: The file {args.md_file} was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == '__main__':
    main()
