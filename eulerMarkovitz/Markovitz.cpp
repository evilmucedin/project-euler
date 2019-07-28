#include "lib/header.h"
#include "lib/random.h"

#define CGAL_HEADER_ONLY 1

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>

int main() {
    static constexpr size_t N = 5000;

    using Program = CGAL::Quadratic_program<double>;
    // using Solution = CGAL::Quadratic_program_solution<double>;

    Program p;
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            p.set_d(i, j, randAB<double>(-0.01, 0.01));
            // p.set_a(i, j, randAB<double>(-1, 1));
        }
        // p.set_b(i, randAB<double>(0, 1));
        p.set_c(i, randAB<double>(-1, 1));
        p.set_l(i, true, -100.0);
        p.set_u(i, true, 100.0);
    }
    p.set_c0(randAB<double>(-1, 1));
    auto s = CGAL::solve_quadratic_program(p, 0.0);

    std::vector<CGAL::Quotient<double>> solution;
    std::copy(s.variable_values_begin(), s.variable_values_end(), std::back_inserter(solution));
    for (size_t i = 0; i < N; ++i) {
        std::cout << i << " " << CGAL::to_double(solution[i]) << std::endl;
    }

    return 0;
}
