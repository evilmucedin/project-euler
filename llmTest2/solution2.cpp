// Compile: g++ -O2 -o solution2 solution2.cpp -lmpfr -lgmp
//
// Computes pi to arbitrary precision using the Chudnovsky series:
//   1/π = (12/C^(3/2)) * Σ_k (-1)^k (6k)!(B+Ak) / ((3k)!(k!)^3 C^(3k))
// where A=545140134, B=13591409, C=640320, C^3=262537412640768000.
// Rearranged: π = 426880·√10005 / S  (since C^(3/2)/12 = 426880·√10005).
// Convergence: ~14.18 correct decimal digits per term.

#include <iostream>
#include <string>
#include <mpfr.h>

using namespace std;

// Returns pi as a decimal string with 'len' digits after the decimal point.
// Works correctly for len < 1000.
string pi(int len) {
    if (len <= 0) return "3";

    // Extra guard digits to absorb internal rounding; 3.322 bits per decimal digit
    const int guard = 10;
    mpfr_prec_t prec = (mpfr_prec_t)((len + guard) * 3.322 + 64);

    // Number of series terms required
    int n_terms = (len + guard) / 14 + 2;

    // Chudnovsky linear-term constants
    const long long A = 545140134LL;
    const long long B = 13591409LL;

    mpfr_t sum, term;
    mpfr_init2(sum, prec);
    mpfr_init2(term, prec);

    // k=0: t_0 = B  (M_0=1, L_0=B, X_0=1)
    mpfr_set_si(term, (long)B, MPFR_RNDN);
    mpfr_set(sum, term, MPFR_RNDN);

    for (int k = 1; k <= n_terms; k++) {
        long long L_cur  = B + A * k;
        long long L_prev = B + A * (k - 1);

        // Iterative ratio: t_k / t_{k-1} =
        //   -(6k)(6k-1)(6k-2)(6k-3)(6k-4)(6k-5) * L_cur
        //   / ((3k)(3k-1)(3k-2) * k^3 * C^3 * L_prev)

        // Numerator (leading factor carries the sign flip)
        mpfr_mul_si(term, term, -(long)(6 * k), MPFR_RNDN);
        for (int j = 1; j <= 5; j++)
            mpfr_mul_ui(term, term, (unsigned long)(6 * k - j), MPFR_RNDN);
        mpfr_mul_si(term, term, (long)L_cur, MPFR_RNDN);

        // Denominator
        for (int j = 0; j <= 2; j++)
            mpfr_div_ui(term, term, (unsigned long)(3 * k - j), MPFR_RNDN);
        mpfr_div_ui(term, term, (unsigned long)k, MPFR_RNDN);  // k^3
        mpfr_div_ui(term, term, (unsigned long)k, MPFR_RNDN);
        mpfr_div_ui(term, term, (unsigned long)k, MPFR_RNDN);
        mpfr_div_ui(term, term, 262537412640768000UL, MPFR_RNDN);  // C^3
        mpfr_div_si(term, term, (long)L_prev, MPFR_RNDN);

        mpfr_add(sum, sum, term, MPFR_RNDN);
    }

    // Recover π:  π = 426880·√10005 / sum
    mpfr_t pi_val, factor;
    mpfr_init2(pi_val, prec);
    mpfr_init2(factor, prec);

    mpfr_set_ui(factor, 10005, MPFR_RNDN);
    mpfr_sqrt(factor, factor, MPFR_RNDN);
    mpfr_mul_ui(factor, factor, 426880UL, MPFR_RNDN);
    mpfr_div(pi_val, factor, sum, MPFR_RNDN);

    // Extract len+1 significant decimal digits (e.g. "314" for len=2)
    mpfr_exp_t exp_out;
    char* buf = mpfr_get_str(nullptr, &exp_out, 10, (size_t)(len + 1), pi_val, MPFR_RNDN);
    string digits(buf);
    mpfr_free_str(buf);

    mpfr_clear(sum);
    mpfr_clear(term);
    mpfr_clear(pi_val);
    mpfr_clear(factor);

    // exp_out==1: value = 0.digits × 10^1, so digits[0] is the integer part
    return digits.substr(0, 1) + "." + digits.substr(1, len);
}

int main() {
    cout << pi(50) << endl;
    return 0;
}
