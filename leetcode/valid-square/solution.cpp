To solve this problem efficiently in C++, we can follow these steps:

1. Calculate the distances between all pairs of points.
2. Determine if there are exactly four distinct distances: two equal and two different.
3. Check if one of the equal distances is twice the other, which would be necessary for it to form a square.

Here's the implementation of the `validSquare` function:

```cpp
class Solution {
public:
    bool validSquare(vector<int>& p1, vector<int>& p2, vector<int>& p3, vector<int>& p4) {
        // Calculate all pairwise distances
        vector<double> dists;
        
        dists.push_back(dist(p1, p2));
        dists.push_back(dist(p1, p3));
        dists.push_back(dist(p1, p4));
        dists.push_back(dist(p2, p3));
        dists.push_back(dist(p2, p4));
        dists.push_back(dist(p3, p4));
        
        // Sort the distances
        sort(dists.begin(), dists.end());
        
        // Check if there are exactly four distinct distances
        if (dists.size() != 6 || dists[0] == 0) {
            return false;
        }
        
        // Check if there are two equal and two different distances
        if (dists[0] == dists[1] && dists[2] == dists[3] && dists[4] == dists[5]) {
            return true;
        } else if (dists[0] == dists[1] && dists[2] == dists[3] && dists[0] * 2 == dists[4]) {
            return true;
        }
        
        // Check if there are two equal and two different distances
        if (dists[0] == dists[1] && dists[2] == dists[4] && dists[3] == dists[5]) {
            return true;
        } else if (dists[0] == dists[1] && dists[3] == dists[4] && dists[2] * 2 == dists[5]) {
            return true;
        }
        
        // Check if there are two equal and two different distances
        if (dists[0] == dists[2] && dists[1] == dists[3] && dists[4] == dists[5]) {
            return true;
        } else if (dists[0] == dists[2] && dists[3] == dists[4] && dists[1] * 2 == dists[5]) {
            return true;
        }
        
        // Check if there are two equal and two different distances
        if (dists[0] == dists[2] && dists[1] == dists[4] && dists[3] == dists[5]) {
            return true;
        } else if (dists[0] == dists[2] && dists[4] == dists[5] && dists[1] * 2 == dists[3]) {
            return true;
        }
        
        // If none of the above conditions are met, it's not a square
        return false;
    }
    
private:
    double dist(vector<int>& p1, vector<int>& p2) {
        return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2));
    }
};
```

### Explanation:

- **Distance Calculation**: We calculate the distance between each pair of points using the Euclidean distance formula.
- **Sorting**: We sort the distances to easily identify if there are two equal and two different values.
- **Condition Checks**: We check all possible combinations of these distances to see if they match the properties of a square (two equal sides and two different sides, where one is twice the other).

This solution efficiently checks for a valid square by leveraging sorting and distance calculations.