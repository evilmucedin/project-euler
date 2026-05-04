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

    static const Point3<T> ZERO;
};

template <typename T>
const Point3<T> Point3<T>::ZERO(0, 0, 0);

namespace std {
template <typename T>
struct hash<Point3<T>> {
    size_t operator()(const Point3<T>& p) const {
        size_t result = 0;
        hashCombine(result, p.x, p.y, p.z);
        return result;
    }
};
}  // namespace std

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

template <typename T>
struct Point2 {
    T x{};
    T y{};

    Point2() {}

    Point2(T xx, T yy) : x(xx), y(yy) {}

    T norm() const { return sqrt(*this * *this); }

    Point2<T> normalize() const { return *this * (T(1) / norm()); }

    T operator[](size_t index) const {
        switch (index) {
            case 0:
                return x;
            case 1:
                return y;
            default:
                assert(0);
                return 0;
        }
    }

    bool operator==(const Point2& rhs) const { return x == rhs.x && y == rhs.y; }
    bool operator!=(const Point2& rhs) const { return x != rhs.x || y != rhs.y; }

    bool operator<(const Point2& rhs) const {
        if (x != rhs.x) {
            return x < rhs.x;
        }
        return y < rhs.y;
    }

    static const Point2<T> ZERO;
};

template <typename T>
const Point2<T> Point2<T>::ZERO(0, 0);

using Point2D = Point2<double>;

ostream& operator<<(ostream& s, const Point2D& p);

using Points2 = vector<Point2D>;

int orientation(const Point2D& p, const Point2D& q, const Point2D& r);

Points2 convexHull(const Points2& points);

Point3D rot(const Point3D& p, double a, double b, double c);

template <typename T>
struct Line2 {
    T a;
    T b;
    T c;

    bool on(const Point2<T>& p) const { return v(p) == 0; }

    T v(const Point2<T>& p) const { return a * p.x + b * p.y + c; }

    static Line2<T> fromPoints(const Point2<T>& a, const Point2<T>& b) {
        Line2<T> result;
        result.a = a.y - b.y;
        result.b = b.x - a.x;
        result.c = -result.a * a.x - result.b * a.y;
        return result;
    }
};

template <typename T>
struct Interval2 {
    Line2<T> l;
    Point2<T> start;
    Point2<T> finish;

    Interval2(const Line2<T>& l, const Point2<T>& start, const Point2<T>& finish)
        : l(l), start(start), finish(finish) {}

    bool on(const Point2<T>& p) const {
        if (l.on(p)) {
            if ((p.x - start.x) * (p.x - finish.x) > 0) {
                return false;
            }
            if ((p.y - start.y) * (p.y - finish.y) > 0) {
                return false;
            }
            return true;
        }
        return false;
    }
};
