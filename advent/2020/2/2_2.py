#!/usr/bin/env python

def is_password_valid(policy, password):
    # Parse the policy
    parts = policy.split()
    positions = parts[0].split('-')
    pos1 = int(positions[0]) - 1  # Convert to 0-based index
    pos2 = int(positions[1]) - 1  # Convert to 0-based index
    target_char = parts[1][0]
    
    # Check if exactly one of the specified positions contains the target character
    char_at_pos1 = (password[pos1] == target_char) if pos1 < len(password) else False
    char_at_pos2 = (password[pos2] == target_char) if pos2 < len(password) else False
    
    return char_at_pos1 != char_at_pos2

def count_valid_passwords(filename):
    valid_count = 0
    
    with open(filename, 'r') as file:
        for line in file:
            policy, password = line.strip().split(': ')
            if is_password_valid(policy, password):
                valid_count += 1
    
    return valid_count

# Assuming the input file is named 'input.txt'
filename = 'input.txt'
valid_password_count = count_valid_passwords(filename)
print(f"Number of valid passwords according to the new policy: {valid_password_count}")

