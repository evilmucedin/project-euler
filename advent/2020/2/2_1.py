#!/usr/bin/env python

def is_password_valid(policy, password):
    # Parse the policy
    parts = policy.split()
    count_range = parts[0].split('-')
    min_count = int(count_range[0])
    max_count = int(count_range[1])
    target_char = parts[1][0]

    # Count occurrences of the target character in the password
    char_count = password.count(target_char)

    # Check if the count is within the specified range
    return min_count <= char_count <= max_count

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
print(f"Number of valid passwords: {valid_password_count}")
