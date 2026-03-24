#!/usr/bin/env python

import math

# Function to find the minimal solution x for Pell's equation x^2 - D*y^2 = 1
# using the continued fraction expansion of sqrt(D)
def find_minimal_x(D):
    # Check if D is a perfect square, if so no solution in positive integers
    if int(math.isqrt(D))**2 == D:
        return 0

    m = 0
    d = 1
    a0 = math.isqrt(D)
    a = a0
    
    # Initialize the convergents
    h_prev = 1
    h = a0
    k_prev = 0
    k = 1

    while True:
        # Continued fraction algorithm steps
        m = d * a - m
        d = (D - m**2) // d
        a = (a0 + m) // d

        # Update convergents using recurrence relations
        h_next = a * h + h_prev
        k_next = a * k + k_prev
        
        # Check if the current convergent (h, k) is a solution
        if h_next**2 - D * k_next**2 == 1:
            return h_next
        
        # Update values for the next iteration
        h_prev = h
        h = h_next
        k_prev = k
        k = k_next

def solve_problem():
    limit = 1000
    max_x = 0
    result_D = 0

    for D in range(2, limit + 1):
        x = find_minimal_x(D)
        if x > max_x:
            max_x = x
            result_D = D
            
    return result_D

# To run the solution and get the answer
# print(solve_problem()) 

print(solve_problem())
