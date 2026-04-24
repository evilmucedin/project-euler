#include <iostream>
#include <cmath>

using namespace std;

// Function to calculate factorial
unsigned long long factorial(int n) {
    if (n == 0 || n == 1)
        return 1;
    unsigned long long result = 1;
    for (int i = 2; i <= n; ++i)
        result *= i;
    return result;
}

// Function to calculate pi using the Chudnovsky algorithm
string pi(int len) {
    if (len < 0)
        return "Invalid length";

    const int iterations = len / 14 + 1; // Estimate number of iterations needed

    double sum = 0.0;
    for (int k = 0; k <= iterations; ++k) {
        double numerator = factorial(6 * k) * (545140134 * k + 13591409);
        double denominator = pow(factorial(3 * k), 3) * pow(262537412640768000, k);
        sum += numerator / denominator;
    }

    double piValue = 1.0 / (12 * sum);

    // Convert to string with desired precision
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%.15f", piValue); // Adjust the format specifier as needed

    return buffer;
}

int main() {
    int len;
    cout << "Enter the number of digits for π: ";
    cin >> len;

    if (len >= 1000) {
        cout << "This program can only generate up to 999 digits of π." << endl;
        return 1;
    }

    string piValue = pi(len);
    cout << "π with " << len << " digits: " << piValue.substr(0, len + 2) << endl; // Include "3."

    return 0;
}

