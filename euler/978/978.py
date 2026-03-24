#!/usr/bin/env python

#!/usr/bin/env python3

from decimal import Decimal, ROUND_HALF_UP, getcontext


def skewness_at(t: int) -> Decimal:
    """
    Returns Skew(X_t) as a Decimal with current context precision.
    For this walk, E[X_t]=1 for all t>=1.
    """
    if t < 0:
        raise ValueError("t must be non-negative")

    # Raw moments:
    # a_t = E[X_t^2], with a_0=0, a_1=1, a_t = a_{t-1}+a_{t-2}
    # m_t = E[X_t^3], with m_0=0, m_1=1, m_t = m_{t-1}+3*m_{t-2}
    a0, a1 = 0, 1
    m0, m1 = 0, 1

    if t == 0:
        # X_0 is constant 0, skewness is undefined (sigma=0). Not needed.
        raise ValueError("Skewness at t=0 is undefined (variance 0).")
    if t == 1:
        # X_1 is constant 1, skewness is undefined (sigma=0). Not needed.
        raise ValueError("Skewness at t=1 is undefined (variance 0).")

    for _ in range(2, t + 1):
        a0, a1 = a1, a1 + a0
        m0, m1 = m1, m1 + 3 * m0

    a_t = a1
    m_t = m1

    mu = 1  # for all t>=1
    var = a_t - mu * mu
    if var <= 0:
        raise ValueError("Variance is non-positive; skewness undefined.")

    # Central third moment: E[(X-mu)^3] = E[X^3] - 3*mu*E[X^2] + 3*mu^2*E[X] - mu^3
    # Here mu=1 and E[X]=1, so this simplifies to: m_t - 3*a_t + 2
    central3 = m_t - 3 * a_t + 2

    varD = Decimal(var)
    sigma3 = (varD.sqrt()) ** 3
    return Decimal(central3) / sigma3


def main() -> None:
    # Use high precision so rounding to 8 decimals is stable.
    getcontext().prec = 100

    # Asserts from the statement:
    # For t=5, mean=1, std=2 -> skew = 0.75 exactly.
    sk5 = skewness_at(5)
    assert sk5.quantize(Decimal("0.00000001")) == Decimal("0.75000000")

    # Given: Skew(X_10) ≈ 2.50997097
    sk10 = skewness_at(10)
    assert abs(sk10 - Decimal("2.50997097")) < Decimal("0.00000001")

    # Required: Skew(X_50), rounded to 8 digits after the decimal point.
    sk50 = skewness_at(50)
    ans = sk50.quantize(Decimal("0.00000001"), rounding=ROUND_HALF_UP)
    print(ans)


if __name__ == "__main__":
    main()


