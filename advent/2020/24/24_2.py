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

def get_neighbors(x, y):
    return [
        (x + 2, y),    # e
        (x + 1, y - 1), # se
        (x - 1, y - 1), # sw
        (x - 2, y),    # w
        (x - 1, y + 1), # nw
        (x + 1, y + 1)  # ne
    ]


def simulate_days(tiles, days):
    for _ in range(days):
        new_tiles = tiles.copy()
        checked_tiles = set()
        
        # Determine which tiles need to be checked
        for x, y in tiles.keys():
            checked_tiles.add((x, y))
            for nx, ny in get_neighbors(x, y):
                checked_tiles.add((nx, ny))
        
        # Apply the rules to each tile
        for x, y in checked_tiles:
            black_neighbors = sum(1 for nx, ny in get_neighbors(x, y) if tiles.get((nx, ny), False))
            if tiles.get((x, y), False):  # Black tile
                if black_neighbors == 0 or black_neighbors > 2:
                    new_tiles[(x, y)] = False  # Flip to white
            else:  # White tile
                if black_neighbors == 2:
                    new_tiles[(x, y)] = True  # Flip to black
        
        tiles = new_tiles
    
    return tiles

def count_black_tiles(tiles):
    return sum(1 for is_black in tiles.values() if is_black)

def main():
    # Read the input from input.txt
    with open('input.txt', 'r') as file:
        directions_list = [line.strip() for line in file.readlines()]
    
    # Flip the tiles based on the directions
    tiles = flip_tiles(directions_list)
    
    # Simulate 100 days
    tiles = simulate_days(tiles, 100)
    
    # Count the number of black tiles
    black_tile_count = count_black_tiles(tiles)
    
    print(f"The number of black tiles after 100 days is: {black_tile_count}")

if __name__ == "__main__":
    main()

