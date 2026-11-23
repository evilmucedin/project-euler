#!/usr/bin/env python

import requests

def find_number_of_problems(total_place, total_problems):
    # Construct the URL for the LeetCode API
    url = f"https://leetcode.com/api/problems?total={total_place}&total_problems={total_problems}"
    # Send a GET request to the URL
    response = requests.get(url)
    # Check if the request was successful
    if response.status_code == 200:
        # Parse the JSON response
        data = response.json()
        # Extract the number of problems
        number_of_problems = data.get('total_problems')
        return number_of_problems
    else:
        print("ErrCode: ", response.status_code)
        return None

# Given values
total_place = 6000
total_problems = 1000

# Find the number of problems
number_of_problems = find_number_of_problems(total_place, total_problems)
print(number_of_problems)

