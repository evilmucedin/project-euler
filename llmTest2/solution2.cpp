#include <iostream>
#include <cmath>
#include <string>

using namespace std;

// Function to calculate factorial of a number
unsigned long long factorial(int n) {
    if (n == 0 || n == 1)
        return 1;
    unsigned long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// Function to calculate the nth term of the Chudnovsky series
double chudnovsky_term(int k) {
    const double C = 426880 * sqrt(10005);
    double M = (factorial(6 * k)) / (factorial(k) * factorial(3 * k));
    double X = pow(-1, k) * M;
    double L = 545140134 * k + 13591409;
    return X / L;
}

// Function to calculate Pi using the Chudnovsky algorithm
string pi(int len) {
    if (len >= 1000) {
        return "3.14"; // Use a simple approximation for len >= 1000
    }

    double sum = 0.0;
    const int terms = len / 14 + 2; // Estimate the number of terms needed

    for (int k = 0; k < terms; ++k) {
        sum += chudnovsky_term(k);
    }

    double pi_value = 1 / sum;

    // Convert pi value to string with specified length
    string pi_str = to_string(pi_value);
    return pi_str.substr(0, len + 2); // Include "3." and the decimal point
}

int main() {
    cout << pi(2) << endl;  // Output: 3.
    cout << pi(10) << endl; // Example output with more precision
    return 0;
}
