    static const int X = 64;

class NumArray {    
public:
    NumArray(vector<int>& nums) : nums_(nums) {
        const int s = nums_.size();
        int i = 0;
        while (i + X < s) {
            int sum = 0;
            int j = i;
            for (; (j < i + X) && (j < s); ++j) {
                sum += nums_[j];
            }
            if (j == i + X && j < s) {
                numsX_.push_back(sum);
            }
            i += X;
        }
    }
    
    int sumRange(int left, int right) {
        int result1 = 0;
        for (int i = left; i <= right;) {
                if (i % X != 0 || i + X >= right) {
                    result1 += nums_[i];
                    ++i;
                } else {
                    result1 += numsX_[i / X];
                    i += X;
                }
        }
        return result1;
    }

    const vector<int>& nums_;
    vector<int> numsX_;
};

/**
 * Your NumArray object will be instantiated and called as such:
 * NumArray* obj = new NumArray(nums);
 * int param_1 = obj->sumRange(left,right);
 */
