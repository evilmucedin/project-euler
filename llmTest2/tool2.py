import sys

def extract_code_from_md(input_file, output_file):
    start_marker = "```cpp"
    end_marker = "```"

    with open(input_file, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    in_code_block = False
    extracted_lines = []

    for line in lines:
        if line.strip() == start_marker:
            in_code_block = True
            continue
        elif line.strip() == end_marker:
            in_code_block = False
            break
        if in_code_block:
            extracted_lines.append(line)

    # Remove leading and trailing whitespace from each line and join them into a single string
    code_content = ''.join(extracted_lines).strip()

    # Write the extracted code to the output file
    with open(output_file, 'w', encoding='utf-8') as file:
        file.write(code_content)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python extract_code.py <input_md_file> <output_cpp_file>")
        sys.exit(1)
    
    input_md_file = sys.argv[1]
    output_cpp_file = sys.argv[2]

    extract_code_from_md(input_md_file, output_cpp_file)
    print(f"Code extracted and saved to {output_cpp_file}")

