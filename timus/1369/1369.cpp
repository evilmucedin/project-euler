#define _CRT_SECURE_NO_WARNINGS

#ifndef _MSC_VER
#pragma GCC target("sse4.2")
#pragma GCC optimize("O3")
#include <emmintrin.h>
#else
#include <immintrin.h>
#include <intrin.h>
#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

#define M_PI 3.14159265358979323846

using namespace std;

static const double INF = 1e30;
static const double EPS = 2e-11;

using Integers = vector<int>;

class Exception : public std::exception {
   private:
    const char* _s;

   public:
    Exception(const char* s) : _s(s) {}

    const char* what() const throw() { return _s; }
};

template <typename T>
T Sqr(T x) {
    return x * x;
}

void Output(const Integers& vct) {
    char buffer[100];
    for (size_t i = 0; i < vct.size(); ++i) {
        int num = vct[i] + 1;
        char* pBuffer = buffer;
        while (num) {
            *pBuffer = (num % 10) + '0';
            num /= 10;
            ++pBuffer;
        }
        *pBuffer = 0;
        std::reverse(buffer, pBuffer);
        *pBuffer = ' ';
        ++pBuffer;
        *pBuffer = 0;
        fputs(buffer, stdout);
    }
    fputs("\n", stdout);
}

typedef double TBase;

struct Vector;

struct Point {
    TBase _x;
    TBase _y;
    int _index;

    Point() {}

    Point(TBase x, TBase y, int index) : _x(x), _y(y), _index(index) {}

    TBase Distance2(const Point& p) const { return Sqr(_x - p._x) + Sqr(_y - p._y); }

    bool operator==(const Point& rhs) const { return Distance2(rhs) < EPS; }
};

typedef vector<Point> Points;

struct Vector {
    TBase _dx;
    TBase _dy;

    Vector(TBase dx, TBase dy) : _dx(dx), _dy(dy) {}

    TBase Length2() const { return Sqr(_dx) + Sqr(_dy); }

    TBase Length() const { return sqrt(Length2()); }

    void Normalize() {
        double norm = Length();
        _dx /= norm;
        _dy /= norm;
    }
};

TBase Dot(const Point& p, const Vector& v) { return p._x * v._dx + p._y * v._dy; }

TBase Angle(const Point& p, const Point& p0) {
    /*
    if (p._x != p0._x)
        return (p._y - p0._y) / (p._x - p0._x);
    else if (p._y >= p0._y)
        return INF;
    else
        return -INF;
    */
    return atan2(p._y - p0._y, p._x - p0._x);
}

struct PointProjection {
    const Point* _p;
    TBase _projection;

    PointProjection() {}

    PointProjection(const Point* p, TBase projection) : _p(p), _projection(projection) {}

    bool operator<(const PointProjection& rhs) const { return _projection < rhs._projection; }
};
typedef vector<PointProjection> PointProjections;

template <typename T>
T Max(T a, T b) {
    return (a > b) ? a : b;
}

template <typename T>
T Min(T a, T b) {
    return (a < b) ? a : b;
}

union TVector {
    __m128d v;
    double f[2];
};

typedef vector<double> TFloats;

static void ConvertVector(const TFloats& floats, TVector** result, int* len) {
    const int m = static_cast<int>(floats.size());
    *len = m / 2;
    if (m % 2) {
        ++(*len);
    }
    *result = (TVector*)_mm_malloc(sizeof(TVector) * (*len), 32);
    for (int i = 0; i < *len; ++i) {
        double values[] = {INF, INF};
        for (int j = 0; j < 2; ++j) {
            if (2 * i + j < m) {
                values[j] = floats[2 * i + j];
            }
        }
        (*result)[i].v = _mm_load_pd(values);
    }
}

static double Median(const TFloats& floats) {
    TFloats temp(floats);
    size_t n = temp.size() / 2;
    nth_element(temp.begin(), temp.begin() + n, temp.end());
    return temp[n];
}

struct KDTree {
    int _len;
    int _depth;
    TVector* _x;
    TVector* _y;
    bool _isLeaf;
    KDTree* _left;
    KDTree* _right;
    Integers _indices;
    double _minX;
    double _maxX;
    double _minY;
    double _maxY;
    double _separator;

    KDTree(int depth, const TFloats& x, const TFloats& y, const Integers& indices) {
        _minX = INF;
        _maxX = -INF;
        _minY = INF;
        _maxY = -INF;
        for (size_t i = 0; i < x.size(); ++i) {
            _minX = Min(_minX, x[i]);
            _maxX = Max(_maxX, x[i]);
            _minY = Min(_minY, y[i]);
            _maxY = Max(_maxY, y[i]);
        }
        _depth = depth;

        if (9 == depth) {
            ConvertVector(x, &_x, &_len);
            ConvertVector(y, &_y, &_len);
            _indices = indices;
            _isLeaf = true;
        } else {
            _separator = (depth & 1) ? Median(x) : Median(y);

            TFloats xLeft;
            TFloats yLeft;
            Integers indicesLeft;
            TFloats xRight;
            TFloats yRight;
            Integers indicesRight;

            for (size_t i = 0; i < x.size(); ++i) {
                bool toLeft = (depth & 1) ? (x[i] < _separator) : (y[i] < _separator);
                if (toLeft) {
                    xLeft.push_back(x[i]);
                    yLeft.push_back(y[i]);
                    indicesLeft.push_back(indices[i]);
                } else {
                    xRight.push_back(x[i]);
                    yRight.push_back(y[i]);
                    indicesRight.push_back(indices[i]);
                }
            }

            if (xLeft.size()) {
                _left = new KDTree(depth + 1, xLeft, yLeft, indicesLeft);
            } else {
                _left = 0;
            }
            if (xRight.size()) {
                _right = new KDTree(depth + 1, xRight, yRight, indicesRight);
            } else {
                _right = 0;
            }

            _isLeaf = false;
        }
    }

#ifndef _MSC_VER
    __attribute__((force_align_arg_pointer))
#endif
    void
    Solve(double x, double y, const TVector& x2, const TVector& y2, double* minDist, double* minDistEps,
          TVector* minMax, Integers* result, int* limit) const {
        if (*limit < 0) {
            return;
        }

        double d2;
        if (x < _minX) {
            if (y < _minY) {
                d2 = Sqr(_minX - x) + Sqr(_minY - y);
            } else if (y > _maxY) {
                d2 = Sqr(_minX - x) + Sqr(y - _maxY);
            } else {
                d2 = Sqr(_minX - x);
            }
        } else if (x > _maxX) {
            if (y < _minY) {
                d2 = Sqr(x - _maxX) + Sqr(_minY - y);
            } else if (y > _maxY) {
                d2 = Sqr(x - _maxX) + Sqr(y - _maxY);
            } else {
                d2 = Sqr(x - _maxX);
            }
        } else {
            if (y < _minY) {
                d2 = Sqr(_minY - y);
            } else if (y > _maxY) {
                d2 = Sqr(y - _maxY);
            } else {
                d2 = 0;
            }
        }

        if (d2 > *minDist + EPS) {
            return;
        }

        if (_isLeaf) {
            *limit -= _len;
            for (int j = 0; j < _len; ++j) {
                TVector dx2;
                dx2.v = _mm_sub_pd(_x[j].v, x2.v);
                dx2.v = _mm_mul_pd(dx2.v, dx2.v);
                TVector dy2;
                dy2.v = _mm_sub_pd(_y[j].v, y2.v);
                dy2.v = _mm_mul_pd(dy2.v, dy2.v);
                dx2.v = _mm_add_pd(dx2.v, dy2.v);

                TVector cmpMax;
                cmpMax.v = _mm_cmple_pd(dx2.v, minMax->v);
                for (int k = 0; k < 2; ++k) {
                    if (cmpMax.f[k]) {
                        if (dx2.f[k] <= *minDistEps) {
                            if (dx2.f[k] < *minDist) {
                                if (dx2.f[k] + EPS < *minDist) {
                                    result->clear();
                                }
                                *minDist = dx2.f[k];
                                *minDistEps = *minDist + EPS;
                                minMax->v = _mm_set1_pd(*minDistEps);
                            }
                            result->push_back(_indices[2 * j + k]);
                        }
                    }
                }
            }
        } else {
            bool toLeft = (_depth & 1) ? (x < _separator) : (y < _separator);
            if (toLeft) {
                if (_left) {
                    _left->Solve(x, y, x2, y2, minDist, minDistEps, minMax, result, limit);
                }
                if (_right) {
                    _right->Solve(x, y, x2, y2, minDist, minDistEps, minMax, result, limit);
                }
            } else {
                if (_right) {
                    _right->Solve(x, y, x2, y2, minDist, minDistEps, minMax, result, limit);
                }
                if (_left) {
                    _left->Solve(x, y, x2, y2, minDist, minDistEps, minMax, result, limit);
                }
            }
        }
    }
};

void GenBig() {
    FILE* fOut = fopen("big.txt", "w");
    static const int N = 100000;
    fprintf(fOut, "%d\n", N);
    for (int i = 0; i < N; ++i) {
        long double ldi = i;
        long double angle = ldi / N * 2.0 * M_PI;
        static const long double R = 10000.0;
        long double x = R * cos(angle);
        long double y = R * sin(angle);
        fprintf(fOut, "%Lf %Lf\n", x, y);
    }
    static const int M = 10000;
    fprintf(fOut, "%d\n", M);
    for (int i = 0; i < M; ++i) {
        long double ldi = i;
        long double angle = ldi / M * 2.0 * M_PI;
        static const long double R = 0.1;
        long double x = R * cos(angle);
        long double y = R * sin(angle);
        fprintf(fOut, "%Lf %Lf\n", x, y);
    }
    fclose(fOut);
}

void GenInt() {
    FILE* fOut = fopen("int.txt", "w");
    static const int N = 10000;
    fprintf(fOut, "%d\n", N);
    for (int i = 0; i < N; ++i) {
        fprintf(fOut, "%d %d\n", rand() % 10, rand() % 10);
    }
    static const int M = 10000;
    fprintf(fOut, "%d\n", M);
    for (int i = 0; i < M; ++i) {
        fprintf(fOut, "%lf %lf\n", static_cast<double>(rand() % 10), static_cast<double>(rand() % 10));
    }
    fclose(fOut);
}

double Rand() { return 2.0 * (static_cast<double>(rand()) / RAND_MAX - 0.5); }

#ifndef _MSC_VER
#define GET_ITEM2(var, index) (var)[(index)]
#else
#define GET_ITEM2(var, index) (var).m128d_f64[(index)]
#endif

Point CircumCenter(const Point& a, const Point& b, const Point& c) {
    TBase u = ((a._x - b._x) * (a._x + b._x) + (a._y - b._y) * (a._y + b._y));
    TBase v = ((b._x - c._x) * (b._x + c._x) + (b._y - c._y) * (b._y + c._y));
    TBase den = ((a._x - b._x) * (b._y - c._y) - (b._x - c._x) * (a._y - b._y)) * 2.0;
    return Point((u * (b._y - c._y) - v * (a._y - b._y)) / den, (v * (a._x - b._x) - u * (b._x - c._x)) / den, -1);
}

struct Circle {
    Point _center;
    TBase _r2;
    Integers _indices;

    void MinD(const Point& q, TBase* mind, Integers* result) const {
        if (_r2 < *mind + EPS) {
            if (q.Distance2(_center) < EPS) {
                if (_r2 < *mind) {
                    if (_r2 + EPS < *mind) {
                        result->clear();
                    }
                    *mind = _r2;
                }
                result->insert(result->end(), _indices.begin(), _indices.end());
            }
        }
    }

    bool operator!=(const Circle& rhs) const {
        if (_center.Distance2(rhs._center) > EPS) {
            return true;
        }
        if (abs(_r2 - rhs._r2) > EPS) {
            return true;
        }
        return false;
    }
};

int main() {
#ifndef ONLINE_JUDGE
    // GenBig();
    // freopen("big.txt", "r", stdin);
    freopen("input.txt", "r", stdin);
#endif

    int m;
    scanf("%d", &m);
    __m128d* xyd = (__m128d*)_mm_malloc(sizeof(__m128d) * (m), 32);
    double x0 = 0.0;
    double y0 = 0.0;
    Points points(m);
    Points spoints(m);
    for (int i = 0; i < m; ++i) {
        double x;
        double y;
        scanf("%lf%lf", &x, &y);

        x0 += x;
        y0 += y;

        xyd[i] = _mm_set_pd(x, y);
        points[i]._x = x;
        points[i]._y = y;
        points[i]._index = i;
    }
    x0 /= m;
    y0 /= m;

    Integers indices(m);
    for (int i = 0; i < m; ++i) {
        indices[i] = i;
    }
    for (int i = 0; i < m; ++i) {
        swap(indices[i], indices[i + (rand() % (m - i))]);
    }

    static const size_t NCENTERS = 3;
    Points centers;
    vector<PointProjections> angles;

    vector<Circle> circles;

    {
        for (int i = 0; i < m; ++i) {
            spoints[i] = points[indices[i]];
        }

        centers.resize(NCENTERS);
        angles.resize(NCENTERS);
        for (int it = 0; it < NCENTERS; ++it) {
            Point cc;
            if (it > 0) {
                cc = CircumCenter(spoints[rand() % m], spoints[rand() % m], spoints[rand() % m]);
                int iit = 0;
                while (iit < 100 && find(centers.begin(), centers.end(), cc) != centers.end()) {
                    cc = CircumCenter(spoints[rand() % m], spoints[rand() % m], spoints[rand() % m]);
                    ++iit;
                }
            } else {
                cc = Point(x0, y0, -1);
            }
            const __m128d ccxy = _mm_set_pd(cc._x, cc._y);
            centers[it] = cc;
            angles[it].resize(m);

            for (int i = 0; i < m; ++i) {
                angles[it][i]._p = &points[i];

                __m128d xyd2 = _mm_sub_pd(xyd[i], ccxy);
                xyd2 = _mm_mul_pd(xyd2, xyd2);
                long double d = GET_ITEM2(xyd2, 0) + GET_ITEM2(xyd2, 1);

                angles[it][i]._projection = d;
            }
            sort(angles[it].begin(), angles[it].end());

            int index = 0;
            while (index < angles[it].size()) {
                int endindex = index + 50;
                if (endindex < angles[it].size() &&
                    angles[it][endindex]._projection <= angles[it][index]._projection + EPS) {
                    while (endindex < angles[it].size() &&
                           angles[it][endindex]._projection <= angles[it][index]._projection + EPS) {
                        ++endindex;
                    }
                    Circle c;
                    c._center = centers[it];
                    c._r2 = angles[it][index]._projection;
                    int i = 0;
                    while (i < circles.size() && c != circles[i]) {
                        ++i;
                    }
                    if (i == circles.size()) {
                        c._indices.reserve(endindex - index);
                        for (int j = index; j < endindex; ++j) {
                            c._indices.push_back(angles[it][j]._p->_index);
                        }
                        circles.push_back(c);
                    }
                    index = endindex;
                } else {
                    ++index;
                }
            }

            for (int i = 0; i < m; ++i) {
                angles[it][i]._p = &points[i];
                angles[it][i]._projection = Angle(points[i], centers[it]);
            }
            sort(angles[it].begin(), angles[it].end());
        }
    }

    KDTree* kdTree;
    {
        TFloats xc(m);
        TFloats yc(m);
        for (int i = 0; i < m; ++i) {
            xc[i] = spoints[i]._x;
            yc[i] = spoints[i]._y;
        }

        kdTree = new KDTree(0, xc, yc, indices);
    }

    int n;
    scanf("%d", &n);
    Integers dindices;

    for (int i = 0; i < n; ++i) {
        double x;
        double y;
        scanf("%lf%lf", &x, &y);

        Point pq(x, y, -1);
        __m128d xy = _mm_set_pd(x, y);

        dindices.clear();

        TBase mind = INF;
        TBase mindEps = INF;

        for (int k = 0; k < angles.size(); ++k) {
            PointProjection qpp;
            qpp._p = &pq;
            qpp._projection = Angle(pq, centers[k]);
            vector<PointProjection>::const_iterator toAngle = lower_bound(angles[k].begin(), angles[k].end(), qpp);
            vector<PointProjection>::const_iterator bgn = angles[k].begin();
            static const int IT = 2150;
            vector<PointProjection>::const_iterator toCand = toAngle - IT / 2;
            while (toCand < bgn) {
                toCand += angles[k].size();
            }
            for (int j = 0; j < IT; ++j) {
                if (toCand == angles[k].end()) {
                    toCand -= angles[k].size();
                }

                const int index = toCand->_p->_index;
                __m128d xyd2 = _mm_sub_pd(xyd[index], xy);
                xyd2 = _mm_mul_pd(xyd2, xyd2);
                double d = GET_ITEM2(xyd2, 0) + GET_ITEM2(xyd2, 1);

                if (d <= mindEps) {
                    if (d < mind) {
                        if (d + EPS < mind) {
                            dindices.clear();
                        }
                        mind = d;
                        mindEps = mind + EPS;
                    }
                    dindices.push_back(index);
                }

                ++toCand;
            }
        }

        for (int k = 0; k < circles.size(); ++k) {
            circles[k].MinD(pq, &mind, &dindices);
        }

        TVector x2;
        x2.v = _mm_set1_pd(x);
        TVector y2;
        y2.v = _mm_set1_pd(y);

        TVector minMax;
        minMax.v = _mm_set1_pd(mindEps);
        int limit = 3150;
        kdTree->Solve(x, y, x2, y2, &mind, &mindEps, &minMax, &dindices, &limit);

        sort(dindices.begin(), dindices.end());
        dindices.erase(unique(dindices.begin(), dindices.end()), dindices.end());

        Output(dindices);
    }

    return 0;
}
