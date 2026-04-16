#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

using namespace std;

// Function to calculate factorial
unsigned long long factorial(int n) {
if (n <= 1)
return 1;
return n * factorial(n - 1);
}
// Function to calculate binomial coefficient
unsigned long long binomial_coefficient(int n, int k) {
if (k > n)
return 0;
unsigned long long result = 1;
for (int i = 0; i < k; ++i) {
result *= (n - i);
result /= (i + 1);
}
return result;
}
// Function to calculate the nth term of Chudnovsky series
double chudnovsky_term(int n) {
double numerator = pow(-1, n) * factorial(6 * n) * (545140134 * n + 13591409);
double denominator = pow(factorial(3 * n), 3) * pow(640320, 3 * n + 3.0 / 2.0);
return numerator / denominator;
}
// Function to calculate Pi using Chudnovsky algorithm
string pi(int len) {
if (len < 1 || len >= 1000) {
return "Invalid length";
}
double sum = 0.0;
for (int n = 0; n <= len / 8; ++n) { // The series converges very fast
sum += chudnovsky_term(n);
}
double pi_value = 1.0 / (12.0 * sum);
char buffer[1000];
sprintf(buffer, "%.*f", len, pi_value);
return string(buffer).substr(0, len); // Return the first 'len' characters
}
int main() {
cout << pi(4) << endl; // Should output: "3.14"
cout << pi(10) << endl; // Example for more digits
return 0;
}
