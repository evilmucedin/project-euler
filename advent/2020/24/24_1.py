#!/usr/bin/env python

def parse_directions(directions):
    i = 0
    moves = []
    while i < len(directions):
        if directions[i] == 'e':
            moves.append((2, 0))
            i += 1
        elif directions[i] == 'w':
            moves.append((-2, 0))
            i += 1
        elif directions[i:i+2] == 'se':
            moves.append((1, -1))
            i += 2
        elif directions[i:i+2] == 'sw':
            moves.append((-1, -1))
            i += 2
        elif directions[i:i+2] == 'nw':
            moves.append((-1, 1))
            i += 2
        elif directions[i:i+2] == 'ne':
            moves.append((1, 1))
            i += 2
    return moves

def flip_tiles(directions_list):
    tiles = {}
    for directions in directions_list:
        moves = parse_directions(directions)
        x, y = 0, 0
        for move in moves:
            x += move[0]
            y += move[1]
        if (x, y) in tiles:
            tiles[(x, y)] = not tiles[(x, y)]
        else:
            tiles[(x, y)] = True  # Black
    return tiles

def count_black_tiles(tiles):
    return sum(1 for is_black in tiles.values() if is_black)

def main():
    # Read the input from input.txt
    with open('input.txt', 'r') as file:
        directions_list = [line.strip() for line in file.readlines()]
    
    # Flip the tiles based on the directions
    tiles = flip_tiles(directions_list)
    
    # Count the number of black tiles
    black_tile_count = count_black_tiles(tiles)
    
    print(f"The number of black tiles is: {black_tile_count}")

if __name__ == "__main__":
    main()

