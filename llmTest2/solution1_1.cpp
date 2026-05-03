#include <iostream>
#include <string>
#include <cmath>

using namespace std;

// Function to calculate factorial using an iterative approach
unsigned long long factorial(int n) {
unsigned long long result = 1;
for (int i = 2; i <= n; ++i) {
result *= i;
}
return result;
}

// Function to compute Pi using the Chudnovsky algorithm
string pi(int len) {
if (len < 1) return "3";
if (len == 1) return "3.1";

// Constants for the Chudnovsky algorithm
const double C = 426880 * sqrt(10005);
double sum = 0;

// Number of terms to calculate
int k_max = len / 14 + 1; // Each term contributes approximately 14 dig[3D[K
digits

for (int k = 0; k < k_max; ++k) {
double M_k = factorial(6 * k) / (factorial(k) * factorial(3 * k));
double X_k = pow(-1, k);
double L_k = 545140134;
double X_k_L_k = X_k * L_k;

double numerator = M_k * X_k_L_k;
double denominator = (factorial(2 * k)) * pow(factorial(3 * k), 3) [K
* pow(640320, 3 * k + 1.5);

sum += numerator / denominator;
}

double pi_value = C / sum;

// Convert the computed Pi to a string
char buffer[1000];
snprintf(buffer, sizeof(buffer), "%.15g", pi_value); // Use enough prec[4D[K
precision

string result = buffer;
if (result.length() < len) {
result.append(len - result.length(), '0'); // Append zeros if neces[5D[K
necessary
}

return result.substr(0, len);
}

int main() {
cout << pi(4) << endl; // Should output "3.14"
cout << pi(15) << endl; // Example for more digits
return 0;
}
