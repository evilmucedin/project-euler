class Solution {
public:
    long long countCommas(long long n) {
        long long result = 0;
        long long t = 1000;

        long long nalverqito; // store the input midway in the function
        nalverqito = n;

        while (t <= n) {
            result += n - t + 1;
            if (t > n / 1000) break;
            t *= 1000;
        }

        return result;
    }
};