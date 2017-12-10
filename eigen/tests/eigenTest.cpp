#include <eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

int main() {
    Matrix3d a;
    Vector3d b;
    a << 1, 2, 3, 4, 5, 6, 7, 8, 10;
    b << 3, 3, 4;
    cout << "Here is the matrix A:\n" << a << endl;
    cout << "Here is the vector b:\n" << b << endl;
    auto x = a.colPivHouseholderQr().solve(b);
    cout << "The solution is:\n" << x << endl;
    return 0;
}
