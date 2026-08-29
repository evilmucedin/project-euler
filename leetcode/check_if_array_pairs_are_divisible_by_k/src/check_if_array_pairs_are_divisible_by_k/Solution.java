class Solution {
    
    
    public static void main(String[] args) {
    	Solution s = new Solution();
    	System.out.println(s.canArrange(new int[] {1,2,3,4,5,10,6,7,8,9}, 5));
    }
	
	public boolean canArrange(int[] arr, int k) {
        int[] freq = new int[k];
        
        // Count remainder frequencies
        for (int num : arr) {
            int rem = ((num % k) + k) % k;
            freq[rem]++;
        }
        
        // Check pairing conditions
        for (int i = 0; i <= k / 2; i++) {
            if (i == 0) {
                // Remainder 0 elements must be even to pair with each other
                if (freq[0] % 2 != 0) return false;
            } else if (i * 2 == k) {
                // If k is even, the middle remainder must also be even
                if (freq[i] % 2 != 0) return false;
            } else {
                // Remainder i must match remainder k - i
                if (freq[i] != freq[k - i]) return false;
            }
        }
        
        return true;
    }
}