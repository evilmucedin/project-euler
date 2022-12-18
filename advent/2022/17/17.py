#!/usr/bin/env python3

import sys

WIDTH = 7  # Width of the chamber
HEIGHT = 5000
NUM_ROCKS = 5  # Number of rock types

# Define the shapes of the rocks
rock_shapes = [
    [['#', '#', '#', '#']],
    [
    ['.', '#', '.'],
    ['#', '#', '#'],
    ['.', '#', ',']
    ],
    [
    ['#', '#', '#'],
    ['.', '.', '#'],
    ['.', '.', '#'],
    ],
    [
    ['#'],
    ['#'],
    ['#'],
    ['#']
    ],
    [
    ['#', '#'],
    ['#', '#'],
    ]
]

def parse_jet_pattern(input_str):
    """Parse the jet pattern from the input"""
    jet_pattern = ""
    for c in input_str:
        if c in ['<', '>']:
            jet_pattern += c
    return jet_pattern

def simulate(jet_pattern):
    """Simulate the falling of the rocks"""
    # Initialize the chamber to be empty
    chamber = [['.' for _ in range(WIDTH)] for _ in range(HEIGHT)]

    # Initialize variables for the simulation
    rock_index = 0
    jet_index = 0
    maxrow = 1

    # Simulate the falling of the rocks
    for it in range(2022):
        col = WIDTH // 2 - 1  # Start the first rock two units from the left wall
        falling = True
        row = maxrow + 3

        # Get the current rock shape
        rock = rock_shapes[rock_index]

        while falling:
            def inWalls(ncol):
                for i in range(len(rock)):
                    for j in range(len(rock[i])):
                        if rock[i][j] == '#':
                            if j + ncol >= WIDTH:
                                return False
                            elif j + ncol < 0:
                                return False
                            elif chamber[row + i - 1][j + ncol] == '#':
                                return False
                return True

            jet_dir = jet_pattern[jet_index]
            if jet_dir == '<':
                if inWalls(col - 1):
                    col -= 1
            elif jet_dir == '>':
                if inWalls(col + 1):
                    col += 1
            jet_index = (jet_index + 1) % len(jet_pattern)

            for i in range(len(rock)):
                for j in range(len(rock[i])):
                    if rock[i][j] == '#' and ((row == 1) or (chamber[row + i - 1][j + col] == '#')):
                        falling = False

            # If the rock is falling, move it down one unit
            if falling:
                row -= 1
            else:
                for i in range(len(rock)):
                    for j in range(len(rock[i])):
                        if rock[i][j] == '#':
                            chamber[row + i][j + col] = '#'
                            maxrow = max(maxrow, row + i)

        # Move to the next rock
        rock_index = (rock_index + 1) % NUM_ROCKS

        if it < 10:
            for i in range(maxrow + 1, -1, -1):
                print(chamber[i])
            print()

    print("final row: ", maxrow)

input_str = sys.stdin.readline().strip()

# Parse the jet pattern from the input
jet_pattern = parse_jet_pattern(input_str)

# Simulate the falling of the rocks
simulate(jet_pattern)

