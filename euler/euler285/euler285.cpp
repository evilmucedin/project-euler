#include <cstdio>

#define _USE_MATH_DEFINES
#include <cmath>

struct TPoint {
	double X;
	double Y;

	TPoint() {
	}

	TPoint(double x, double y)
		: X(x)
		, Y(y)
	{
	}
};

struct TVec {
	double DX;
	double DY;

	TVec() {
	}

	TVec(const TPoint& a, const TPoint& b)
		: DX(b.X - a.X)
		, DY(b.Y - a.Y)
	{
	}

	double Len() const {
		return sqrt(DX*DX + DY*DY);
	}
};

static double Angle(const TVec& a, const TVec& b) {
	const double scalarProduct = a.DX*b.DX + a.DY*b.DY;
	return acos(scalarProduct/a.Len()/b.Len());
}

static double Square(const TVec& a, const TVec& b) {
	return fabs(a.DX*b.DY - a.DY*b.DX)/2.;
}

static double SectorSquare(double x0, double y0, double r) {
	if (r >= fabs(x0)) {
		TPoint center(x0, y0);
		TPoint zero(0., 0.);
		const double y = y0 + sqrt(r*r - x0*x0);
		TPoint v(0., y);
		TVec a(center, v);
		TVec b(center, zero);
		const double angle = Angle(a, b);
		const double sectorSquare = angle*r*r/2;
		const double triangleSquare = Square(a, b);
		return (sectorSquare - triangleSquare)*2.;
	} else {
		return 0.;
	}
}

int main() {
	double result = 0.;
	for (size_t iK = 1; iK <= 100000; ++iK) {
		double k = iK;
		double plus = SectorSquare(-1./k, -1./k, 1 + 1./2/k);
		double minus = SectorSquare(-1./k, -1./k, 1 - 1./2/k);
		result += k*(plus - minus);
	}
	printf("%.5lf\n", result);
	return 0;
}