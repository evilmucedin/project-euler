
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// Function to calculate factorial
unsigned long long factorial(int n) {
    unsigned long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// Function to generate Pi using the Chudnovsky algorithm
string pi(int len) {
    if (len < 0) {
        return "Invalid length";
    }

    // Constants used in the Chudnovsky formula
    const double C = 426880 * sqrt(10005);
    const int M = 1;
    const int K = 6;

    // Variables to store the result and intermediate values
    long double sum = 0.0;

    // Chudnovsky series calculation
    for (int k = 0; k < len / 14 + 1; ++k) {
        long double term numerator = factorial(6 * k) * (545140134 * k + 13[2D[K
13591409);
        long double term denominator = pow(factorial(3 * k), 3) * pow(facto[9D[K
pow(factorial(k), 3) * pow(-262537412640768000, k);
        sum += term numerator / term denominator;
    }

    // Calculate Pi using the Chudnovsky formula
    long double pi_value = C / sum;

    // Convert the result to a string with the desired length
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%.*Lf", len, pi_value);
    return buffer;
}

int main() {
    cout << pi(4) << endl;  // Should output "3.14"
    cout << pi(50) << endl; // Example of more precision
    return 0;
}
