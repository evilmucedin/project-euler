#!/usr/bin/env python

def find_loop_size(subject_number, public_key):
    value = 1
    loop_size = 0
    modulus = 20201227
    
    while value != public_key:
        value = (value * subject_number) % modulus
        loop_size += 1
    
    return loop_size

def calculate_encryption_key(subject_number, loop_size):
    value = 1
    modulus = 20201227
    
    for _ in range(loop_size):
        value = (value * subject_number) % modulus
    
    return value

def main():
    # Read the input from input.txt
    with open('input.txt', 'r') as file:
        lines = file.readlines()
    
    # Extract the public keys from the input
    card_public_key = int(lines[0].strip())
    door_public_key = int(lines[1].strip())
    
    # Define the subject number
    subject_number = 7
    
    # Find the loop size for the card
    card_loop_size = find_loop_size(subject_number, card_public_key)
    
    # Calculate the encryption key using the card's loop size and the door's public key
    encryption_key = calculate_encryption_key(door_public_key, card_loop_size)
    
    print(f"The encryption key is: {encryption_key}")

if __name__ == "__main__":
    main()

