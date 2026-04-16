#!/usr/bin/env python

import sys
import ollama

def process_file(model_name, input_file, output_file):
    try:
        # Read prompt from input file
        with open(input_file, 'r') as f:
            prompt_text = f.read()

        print(f"Sending prompt to {model_name}...")

        # Generate response
        response = ollama.generate(model=model_name, prompt=prompt_text)
        result = response['response']

        # Write response to output file
        with open(output_file, 'w') as f:
            f.write(result)

        print(f"Done! Response saved to {output_file}")

    except FileNotFoundError:
        print(f"Error: The file '{input_file}' was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: ollama_cli.py <input_file> <output_file>")
    else:
        process_file("qwen3.5:27b", sys.argv[1], sys.argv[2])
