#include "lib/geometry.h"

#include <stack>

template <typename T>
Point2<T> nextToTop(stack<Point2<T>>& s)
{
    Point2<T> p = s.top();
    s.pop();
    Point2<T> res = s.top();
    s.push(p);
    return res;
}

template <typename T>
void swap(Point2<T>& p1, Point2<T>& p2) {
    Point2<T> temp = p1;
    p1 = p2;
    p2 = temp;
}

template <typename T>
int orientation(const Point2<T>& p, const Point2<T>& q, const Point2<T>& r) {
    T val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0) {
        return 0;
    }
    return (val > 0) ? 1 : 2;
}

template <typename T>
T dist(const Point2<T>& p1, const Point2<T>& p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

template <typename It, typename T>
constexpr bool is = std::is_same<typename std::iterator_traits<It>::iterator_category, T>::value;

template <typename BidIt, typename Pred>
void qSort(BidIt first, BidIt last, Pred compare) noexcept {
    static_assert(is<BidIt, std::bidirectional_iterator_tag> || is<BidIt, std::random_access_iterator_tag>,
                  "bidirectional iterator required");

    auto size = std::distance(first, last);

    if (size > 1) {
        using content_type = typename std::iterator_traits<BidIt>::value_type;

        content_type pivot = *first;
        std::vector<content_type> left(size), right(size);
        auto left_end = left.begin();
        auto right_end = right.begin();

        for (BidIt i = std::next(first); i != last; ++i) {
            compare(*i, pivot) ? * left_end++ = *i : * right_end++ = *i;
        }

        qSort(left.begin(), left_end, compare);
        qSort(right.begin(), right_end, compare);

        std::copy(left.begin(), left_end, first);
        *std::next(first, std::distance(left.begin(), left_end)) = pivot;
        std::copy(right.begin(), right_end, std::next(first, std::distance(left.begin(), left_end) + 1));
    }
}

template <typename T>
vector<Point2<T>> convexHull(vector<Point2<T>> points) {
    T ymin = points[0].y;
    int min = 0;
    for (int i = 1; i < points.size(); i++)
    {
        T y = points[i].y;

        if ((y < ymin) || (ymin == y && points[i].x < points[min].x)) {
            ymin = points[i].y;
            min = i;
        }
    }

    swap(points[0], points[min]);

    Point2<T> p0 = points[0];

    auto compare = [p0](const Point2<T>& p1, const Point2<T>& p2) -> bool {
        // Find orientation
        int o = orientation(p0, p1, p2);
        if (o == 0) {
            return (dist(p0, p2) >= dist(p0, p1)) ? true : false;
        }

        return (o == 2) ? true : false;
    };

    sort(points.begin() + 1, points.end(), compare);

    // Create an empty stack and push first three points to it.
    stack<Point2<T>> s;
    s.push(points[0]);
    s.push(points[1]);
    s.push(points[2]);

    // Process remaining n-3 points
    for (int i = 3; i < points.size(); i++)
    {
        // Keep removing top while the angle formed by points next-to-top,
        // top, and points[i] makes a non-left turn
        while (orientation(nextToTop(s), s.top(), points[i]) != 2) {
            s.pop();
        }
        s.push(points[i]);
    }

    vector<Point2<T>> result;

    // Now stack has the output points, print contents of stack
    while (!s.empty())
    {
        Point2<T> p = s.top();
        result.emplace_back(p);
        s.pop();
    }

    return result;
}
