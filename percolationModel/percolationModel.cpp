// Percolation model for IYPT 1998 problem 12 "Powder conductivity":
// measure and explain the conductivity of a mixture of metallic and
// dielectric powders with various proportions of the two components.
//
// The mixture is modeled as an n*n grid of cells; each cell is metallic
// with probability p (conductivity kSigmaMetal) and dielectric otherwise
// (conductivity kSigmaDielectric). Neighboring cells are joined by a bond
// whose conductance is the series combination of the two half-cells.
// A unit voltage is applied between the top and the bottom rows and the
// resulting Kirchhoff system is solved with a Jacobi-preconditioned
// conjugate gradient. For a square sample the effective conductivity
// equals the total current through the electrodes.

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

static constexpr double kSigmaMetal = 1.0;
static constexpr double kSigmaDielectric = 1e-6;

struct Grid {
    int n;
    vector<double> sigma;  // n*n cell conductivities

    double& at(int row, int col) {
        return sigma[row * n + col];
    }

    double bond(int row1, int col1, int row2, int col2) const {
        double a = sigma[row1 * n + col1];
        double b = sigma[row2 * n + col2];
        return 2.0 * a * b / (a + b);
    }
};

static Grid makeGrid(int n, double p, mt19937& rng) {
    Grid g{n, vector<double>(static_cast<size_t>(n) * n)};
    uniform_real_distribution<double> u(0.0, 1.0);
    for (auto& s : g.sigma) {
        s = (u(rng) < p) ? kSigmaMetal : kSigmaDielectric;
    }
    return g;
}

// Effective conductance of the sample with V=1 on row 0 and V=0 on row n-1.
// Unknowns are the cells of rows 1..n-2. If potentialOut is given, it is
// filled with the full n*n potential field including the electrode rows.
static double effectiveConductivity(const Grid& g, vector<double>* potentialOut = nullptr) {
    const int n = g.n;
    const int rows = n - 2;
    const size_t m = static_cast<size_t>(rows) * n;
    auto index = [n](int row, int col) { return static_cast<size_t>(row - 1) * n + col; };

    vector<double> diag(m, 0.0), b(m, 0.0);
    for (int row = 1; row + 1 < n; ++row) {
        for (int col = 0; col < n; ++col) {
            size_t i = index(row, col);
            const int dr[] = {-1, 1, 0, 0};
            const int dc[] = {0, 0, -1, 1};
            for (int k = 0; k < 4; ++k) {
                int row2 = row + dr[k];
                int col2 = col + dc[k];
                if (col2 < 0 || col2 >= n) {
                    continue;
                }
                double w = g.bond(row, col, row2, col2);
                diag[i] += w;
                if (row2 == 0) {
                    b[i] += w;  // top electrode at V=1
                }
            }
        }
    }

    // y = A*x for the interior Laplacian.
    auto applyA = [&](const vector<double>& x, vector<double>& y) {
        for (int row = 1; row + 1 < n; ++row) {
            for (int col = 0; col < n; ++col) {
                size_t i = index(row, col);
                double acc = diag[i] * x[i];
                if (row > 1) {
                    acc -= g.bond(row, col, row - 1, col) * x[index(row - 1, col)];
                }
                if (row + 2 < n) {
                    acc -= g.bond(row, col, row + 1, col) * x[index(row + 1, col)];
                }
                if (col > 0) {
                    acc -= g.bond(row, col, row, col - 1) * x[index(row, col - 1)];
                }
                if (col + 1 < n) {
                    acc -= g.bond(row, col, row, col + 1) * x[index(row, col + 1)];
                }
                y[i] = acc;
            }
        }
    };

    // Jacobi-preconditioned conjugate gradient.
    vector<double> x(m, 0.5), r(m), z(m), pDir(m), ap(m);
    applyA(x, r);
    double bNorm2 = 0.0;
    for (size_t i = 0; i < m; ++i) {
        r[i] = b[i] - r[i];
        bNorm2 += b[i] * b[i];
    }
    double rz = 0.0;
    for (size_t i = 0; i < m; ++i) {
        z[i] = r[i] / diag[i];
        pDir[i] = z[i];
        rz += r[i] * z[i];
    }
    const double tol2 = 1e-20 * bNorm2;
    const size_t maxIter = 200 * m;
    for (size_t iter = 0; iter < maxIter; ++iter) {
        double r2 = 0.0;
        for (size_t i = 0; i < m; ++i) {
            r2 += r[i] * r[i];
        }
        if (r2 <= tol2) {
            break;
        }
        applyA(pDir, ap);
        double pap = 0.0;
        for (size_t i = 0; i < m; ++i) {
            pap += pDir[i] * ap[i];
        }
        double alpha = rz / pap;
        for (size_t i = 0; i < m; ++i) {
            x[i] += alpha * pDir[i];
            r[i] -= alpha * ap[i];
        }
        double rzNew = 0.0;
        for (size_t i = 0; i < m; ++i) {
            z[i] = r[i] / diag[i];
            rzNew += r[i] * z[i];
        }
        double beta = rzNew / rz;
        rz = rzNew;
        for (size_t i = 0; i < m; ++i) {
            pDir[i] = z[i] + beta * pDir[i];
        }
    }

    if (potentialOut) {
        potentialOut->assign(static_cast<size_t>(n) * n, 0.0);
        for (int col = 0; col < n; ++col) {
            (*potentialOut)[col] = 1.0;
        }
        for (int row = 1; row + 1 < n; ++row) {
            for (int col = 0; col < n; ++col) {
                (*potentialOut)[static_cast<size_t>(row) * n + col] = x[index(row, col)];
            }
        }
    }

    // Total current out of the top electrode.
    double current = 0.0;
    for (int col = 0; col < n; ++col) {
        current += g.bond(0, col, 1, col) * (1.0 - x[index(1, col)]);
    }
    return current / n * (n - 1);  // normalize to conductivity of a unit square
}

// Mean magnitude of the bond currents over all bonds of the sample.
static double averageCurrentDensity(const Grid& g, const vector<double>& potential) {
    const int n = g.n;
    auto v = [&](int row, int col) { return potential[static_cast<size_t>(row) * n + col]; };
    double sum = 0.0;
    size_t bonds = 0;
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            if (col + 1 < n) {
                sum += fabs(g.bond(row, col, row, col + 1) * (v(row, col) - v(row, col + 1)));
                ++bonds;
            }
            if (row + 1 < n) {
                sum += fabs(g.bond(row, col, row + 1, col) * (v(row, col) - v(row + 1, col)));
                ++bonds;
            }
        }
    }
    return sum / bonds;
}

static void writeMatrix(const string& filename, const vector<double>& values, int n) {
    ofstream out(filename);
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            out << values[static_cast<size_t>(row) * n + col] << (col + 1 < n ? "," : "\n");
        }
    }
}

// "field" mode: solve one random sample and dump the material grid and the
// potential field for visualization with drawField.py.
static int fieldMode(int argc, char* argv[]) {
    int n = (argc > 2) ? atoi(argv[2]) : 48;
    double p = (argc > 3) ? atof(argv[3]) : 0.6;
    uint32_t seed = (argc > 4) ? static_cast<uint32_t>(atoi(argv[4])) : 1;
    string prefix = (argc > 5) ? argv[5] : "field";

    mt19937 rng(seed);
    Grid g = makeGrid(n, p, rng);
    vector<double> potential;
    double sigma = effectiveConductivity(g, &potential);
    writeMatrix(prefix + "_grid.csv", g.sigma, n);
    writeMatrix(prefix + "_potential.csv", potential, n);
    cerr << "p=" << p << " sigma=" << sigma << " wrote " << prefix << "_{grid,potential}.csv\n";
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && string(argv[1]) == "field") {
        return fieldMode(argc, argv);
    }
    int n = (argc > 1) ? atoi(argv[1]) : 48;
    int trials = (argc > 2) ? atoi(argv[2]) : 20;
    double pStep = (argc > 3) ? atof(argv[3]) : 0.02;
    string outName = (argc > 4) ? argv[4] : "conductivity.csv";

    ofstream out(outName);
    out << "p,mean,std,meanCurrentDensity,stdCurrentDensity\n";
    for (int step = 0; step * pStep <= 1.0 + 1e-9; ++step) {
        double p = min(step * pStep, 1.0);
        double sum = 0.0;
        double sum2 = 0.0;
        double jSum = 0.0;
        double jSum2 = 0.0;
        for (int trial = 0; trial < trials; ++trial) {
            mt19937 rng(static_cast<uint32_t>(step * 7919 + trial + 1));
            Grid g = makeGrid(n, p, rng);
            vector<double> potential;
            double sigma = effectiveConductivity(g, &potential);
            sum += sigma;
            sum2 += sigma * sigma;
            double j = averageCurrentDensity(g, potential);
            jSum += j;
            jSum2 += j * j;
        }
        double mean = sum / trials;
        double var = max(0.0, sum2 / trials - mean * mean);
        double jMean = jSum / trials;
        double jVar = max(0.0, jSum2 / trials - jMean * jMean);
        out << p << "," << mean << "," << sqrt(var) << "," << jMean << "," << sqrt(jVar) << "\n";
        cerr << "p=" << p << " sigma=" << mean << " avgCurrentDensity=" << jMean << "\n";
    }
    cerr << "wrote " << outName << "\n";
    return 0;
}
