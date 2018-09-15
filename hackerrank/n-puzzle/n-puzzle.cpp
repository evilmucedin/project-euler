#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <memory>
#include <vector>
#include <queue>

using namespace std;

using IntVector = vector<int>;

struct State {
    IntVector field;
    int distance;
    int distanceEstimation;
    std::shared_ptr<State> parent;
    int move{-1};
};

struct CmpStates {
    bool operator()(const std::shared_ptr<State>& s1, const std::shared_ptr<State>& s2) {
        return s1->distanceEstimation > s2->distanceEstimation;
    }
};

using FieldToState = std::map<IntVector, std::shared_ptr<State>>;
using FieldsQueue = std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, CmpStates>;

int getEstimation(const IntVector& v, int k) {
    int result = 0;
    for (int i = 0; i < k*k; ++i) {
        if (v[i] == 0) {
            continue;
        }
        int x = i % k;
        int y = i / k;
        int fx = v[i] % k;
        int fy = v[i] / k;
        result += abs(x - fx) + abs(y - fy);
    }
    return result;
}

std::string format(const IntVector& f, int k) {
    std::ostringstream ss;
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            if (j) {
                ss << " ";
            }
            ss << f[k*i + j];
        }
        ss << endl;
    }
    return ss.str();
}

int main() {
    int k;
    cin >> k;
    IntVector start(k * k);
    for (int i = 0; i < k * k; ++i) {
        cin >> start[i];
    }

    auto s0 = std::make_shared<State>();
    s0->field = start;
    s0->distance = 0;
    s0->distanceEstimation = getEstimation(start, k);

    FieldToState f2s;
    f2s[s0->field] = s0;
    FieldsQueue q;
    q.push(s0);

    IntVector finalF(k * k);
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            finalF[i * k + j] = i * k + j;
        }
    }
    /*
    cout << format(start, k) << endl
         << format(finalF, k) << endl
         << " " << getEstimation(start, k) << " " << getEstimation(finalF, k) << endl;
    */

    while (!q.empty()) {
        auto now = q.top();
        q.pop();

        if (now->field == finalF) {
            break;
        }

        int x0;
        int y0;
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                if (now->field[i*k + j] == 0) {
                    x0 = i;
                    y0 = j;
                }
            }
        }
        static const int DIRS[][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
        for (int d = 0; d < 4; ++d) {
            int x = x0 + DIRS[d][0];
            int y = y0 + DIRS[d][1];
            if (x >= 0 && x < k && y >= 0 && y < k) {
                auto f = now->field;
                swap(f[x*k + y], f[x0*k + y0]);
                auto toS = f2s.find(f);
                if ((toS == f2s.end()) || (toS->second->distance > now->distance + 1)) {
                    auto s = std::make_shared<State>();
                    s->field = f;
                    s->distance = now->distance + 1;
                    s->distanceEstimation = s->distance + getEstimation(f, k);
                    s->parent = now;
                    s->move = d;
                    f2s[f] = s;
                    q.push(s);
                }
            }
        }
    }

    vector<string> solution;
    auto now = finalF;
    while (now != start) {
        auto parent = f2s[now]->move;
        switch (parent) {
            case 0:
                solution.emplace_back("UP");
                break;
            case 1:
                solution.emplace_back("RIGHT");
                break;
            case 2:
                solution.emplace_back("DOWN");
                break;
            case 3:
                solution.emplace_back("LEFT");
                break;
            default:
                assert(0);
        }
        now = f2s[now]->parent->field;
    }

    cout << solution.size() << endl;
    for (auto toS = solution.rbegin(); toS != solution.rend(); ++toS) {
        cout << *toS << endl;
    }

    return 0;
}
