#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

using namespace std;

// Function to calculate factorial
unsigned long long factorial(int n) {
unsigned long long result = 1;
for (int i = 2; i <= n; ++i) {
result *= i;
}
return result;
}

// Function to compute Pi using the Chudnovsky algorithm
string pi(int len) {
if (len < 1) {
return "3";
}

// Constants for the Chudnovsky algorithm
const double C = 426880 * sqrt(10005);
double sum = 0.0;
int K = len / 14 + 1; // Adjust K based on the length required

for (int k = 0; k < K; ++k) {
unsigned long long M_k = factorial(6 * k) / (factorial(k) * factori[7D[K
factorial(3 * k));
double L_k = pow(-1, k);
double X_k = 545140134;
double Y_k = 13591409;
double Z_k = 640320;

sum += M_k * L_k * (X_k + Y_k * k) / pow(Z_k, 3 * k + 3/2);
}

// Calculate Pi
double pi_value = C / sum;

// Convert to string with the specified length
ostringstream out;
out << fixed << setprecision(len - 1) << pi_value;
return out.str();
}

int main() {
cout << pi(4) << endl; // Should print "3.14"
cout << pi(10) << endl; // Example for more digits
return 0;
}
