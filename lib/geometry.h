#pragma once

#include "lib/header.h"

template <typename T>
struct Point3 {
    T x{};
    T y{};
    T z{};

    Point3() {}

    Point3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

    T norm() const { return sqrt(*this * *this); }

    Point3<T> normalize() const { return *this * (T(1) / norm()); }

    T operator[](size_t index) const {
        switch (index) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                assert(0);
                return 0;
        }
    }

    bool operator==(const Point3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }

    bool operator<(const Point3& rhs) const {
        if (x != rhs.x) {
            return x < rhs.x;
        }
        if (y != rhs.y) {
            return y < rhs.y;
        }
        return z < rhs.z;
    }
};

template <typename T>
Point3<T> operator*(const Point3<T>& p, T c) {
    return Point3<T>(p.x * c, p.y * c, p.z * c);
}

template <typename T>
Point3<T> operator+(const Point3<T>& p1, const Point3<T>& p2) {
    return Point3<T>(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
}

template <typename T>
Point3<T>& operator+=(Point3<T>& p1, const Point3<T>& p2) {
    p1.x += p2.x;
    p1.y += p2.y;
    p1.z += p2.z;
    return p1;
}

template <typename T>
Point3<T> operator-(const Point3<T>& p1, const Point3<T>& p2) {
    return Point3<T>(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}

template <typename T>
T operator*(const Point3<T>& p1, const Point3<T>& p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

template <typename T>
ostream& operator<<(ostream& s, const Point3<T>& p) {
    s << "{" << p.x << ", " << p.y << ", " << p.z << "}";
    return s;
}

using Point3D = Point3<double>;

using Points3 = vector<Point3D>;

struct Point2 {
    double x;
    double y;
};

ostream& operator<<(ostream& s, const Point2& p);

bool operator<(const Point2& a, const Point2& b);
bool operator!=(const Point2& a, const Point2& b);

using Points2 = vector<Point2>;

int orientation(const Point2& p, const Point2& q, const Point2& r);

Points2 convexHull(const Points2& points);

Point3D rot(const Point3D& p, double a, double b, double c);
