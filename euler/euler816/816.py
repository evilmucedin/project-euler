#!/usr/bin/env python3
import math
import time

def solve_euler_816():
    N = 2000000
    
    # 1. Generate pseudo-random numbers using the specified generator
    s = [0] * (2 * N + 1)
    s[0] = 290797
    for i in range(1, 2 * N + 1):
        s[i] = (s[i-1] ** 2) % 50515093
        
    # 2. Build the array of 2D points
    points = []
    for k in range(1, N + 1):
        points.append((s[2*k-1], s[2*k]))
        
    # 3. Sort points by X-coordinate to prune search space efficiently
    points.sort(key=lambda p: p[0])
    
    # 4. Find the closest pair of points
    min_d = float('inf')
    
    for i in range(N):
        x1, y1 = points[i]
        for j in range(i + 1, N):
            x2, y2 = points[j]
            dx = x2 - x1
            
            # Pruning condition: If dx is already larger than our best distance, 
            # no subsequent point in the sorted list can be closer.
            if dx >= min_d:
                break
                
            dy = y2 - y1
            d = math.hypot(dx, dy)
            if d < min_d:
                min_d = d
                
    # Print the answer rounded to 9 decimal places
    print(f"Shortest Distance: {min_d:.9f}")

if __name__ == "__main__":
    start_time = time.time()
    solve_euler_816()
    print(f"Executed in {time.time() - start_time:.2f} seconds")

