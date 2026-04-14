#include <iostream>

using namespace std;

// Function prototypes
double add(double num1, double num2);
double subtract(double num1, double num2);
double multiply(double num1, double num2);
double divide(double num1, double num2);

int main() {
    char operation;
    double num1, num2;

    cout << "Simple Calculator" << endl;
    cout << "Enter an operator (+, -, *, /): ";
    cin >> operation;

    // Check if the entered operator is valid
    if (operation != '+' && operation != '-' && operation != '*' && operation != '/') {
        cout << "Invalid operator!" << endl;
        return 1; // Exit with an error code
    }

    cout << "Enter two numbers: ";
    cin >> num1 >> num2;

    switch (operation) {
        case '+':
            cout << "Result: " << add(num1, num2) << endl;
            break;
        case '-':
            cout << "Result: " << subtract(num1, num2) << endl;
            break;
        case '*':
            cout << "Result: " << multiply(num1, num2) << endl;
            break;
        case '/':
            if (num2 == 0) {
                cout << "Error! Division by zero." << endl;
            } else {
                cout << "Result: " << divide(num1, num2) << endl;
            }
            break;
    }

    return 0; // Program executed successfully
}

// Function definitions
double add(double num1, double num2) {
    return num1 + num2;
}

double subtract(double num1, double num2) {
    return num1 - num2;
}

double multiply(double num1, double num2) {
    return num1 * num2;
}

double divide(double num1, double num2) {
    return num1 / num2;
}

