#include "geometry.h"

static constexpr double EPS = 1e-9;

ostream& operator<<(ostream& s, const Point2D& p) {
    s << "{" << p.x << ", " << p.y << "}";
    return s;
}

bool operator<(const Point2D& a, const Point2D& b) {
    if (a.x != b.x) {
        return a.x < b.x;
    }
    return a.y < b.y;
}

bool operator!=(const Point2D& a, const Point2D& b) { return (abs(a.x - b.x) > EPS) || (abs(a.y - b.y) > EPS); }

int orientation(const Point2D& p, const Point2D& q, const Point2D& r) {
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (abs(val) < EPS) {
        return 0;
    }

    return (val > 0) ? 1 : 2;
}

Points2 convexHull(const Points2& points) {
    if (points.size() < 3) {
        return points;
    }

    Points2 result;

    int l = 0;
    for (size_t i = 1; i < points.size(); i++) {
        if (points[i] < points[l]) {
            l = i;
        }
    }

    int p = l, q;
    do {
        result.push_back(points[p]);

        q = (p + 1) % points.size();
        for (size_t i = 0; i < points.size(); i++) {
            if (orientation(points[p], points[i], points[q]) == 2) {
                q = i;
            }
        }

        p = q;

    } while (p != l);

    return result;
}

Point3D mul(const Point3D& p, double a11, double a12, double a13, double a21, double a22, double a23, double a31,
            double a32, double a33) {
    Point3D res;
    res.x = a11 * p.x + a12 * p.y + a13 * p.z;
    res.y = a21 * p.x + a22 * p.y + a23 * p.z;
    res.z = a31 * p.x + a32 * p.y + a33 * p.z;
    return res;
}

Point3D rot(const Point3D& p, double a, double b, double c) {
    Point3D p3 = p;
    p3 = mul(p3, 1, 0, 0, 0, cos(a), -sin(a), 0, sin(a), cos(a));
    p3 = mul(p3, cos(b), 0, sin(b), 0, 1, 0, -sin(b), 0, cos(b));
    p3 = mul(p3, cos(c), -sin(c), 0, sin(c), cos(c), 0, 0, 0, 1);
    return p3;
}
