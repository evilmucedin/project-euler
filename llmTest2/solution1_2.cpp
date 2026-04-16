#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

// Function to calculate factorial of a number n
unsigned long long factorial(int n) {
unsigned long long result = 1;
for (int i = 2; i <= n; ++i) {
result *= i;
}
return result;
}

// Function to implement the Chudnovsky algorithm to calculate pi
string pi(int len) {
const double C = 426880 * sqrt(10005);
double sum = 0.0;

for (int k = 0; k < 10; ++k) { // You can adjust the number of terms fo[2D[K
for precision
double M_k = factorial(6 * k) / (factorial(k) * pow(factorial(3), k[1D[K
k) * pow(factorial(2), 3 * k));
double X_k = pow(-1, k) * M_k * (13591409 + 545140134 * k);
double Y_k = factorial(3 * k) * pow(factorial(2), 3 * k) * pow(fact[8D[K
pow(factorial(6), 3);

sum += X_k / Y_k;
}

double pi_value = C / sum;

// Convert the calculated pi value to a string with the desired length
stringstream ss;
ss << setprecision(len - 1) << fixed << pi_value;
return ss.str();
}

int main() {
int len;
cout << "Enter the number of decimal places for Pi (max 1000): ";
cin >> len;

if (len < 1 || len > 1000) {
cerr << "Length must be between 1 and 1000." << endl;
return 1;
}

cout << pi(len) << endl;
return 0;
}
