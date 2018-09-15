#pragma once

#include "lib/common.h"

struct Point3 {
    double x;
    double y;
    double z;

    Point3();
    Point3(double xx, double yy, double zz);
};

ostream& operator<<(ostream& s, const Point3& p);

using Points3 = vector<Point3>;

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

Point3 rot(const Point3& p, double a, double b, double c);
