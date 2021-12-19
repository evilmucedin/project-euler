#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

struct Node {
    using PNode = shared_ptr<Node>;

    PNode left;
    PNode right;
    PNode parent;
    int value{-1};

    Node() {}

    Node(int value) : value(value) {}

    bool isLeaf() const { return left == nullptr && right == nullptr; }

    static PNode prev(PNode node) {
        if (node->left != nullptr) {
            node = node->left;
            while (node->right != nullptr) {
                node = node->right;
            }
            return node;
        }

        if (node->parent != nullptr) {
            while (node->parent && node->parent->left == node) {
                node = node->parent;
            }
            return node->parent;
        }

        return nullptr;
    }

    static PNode next(PNode node) {
        if (node->right != nullptr) {
            node = node->right;
            while (node->left != nullptr) {
                node = node->left;
            }
            return node;
        }

        if (node->parent != nullptr) {
            while (node->parent && (node->parent->right == node)) {
                node = node->parent;
            }
            return node->parent;
        }

        return nullptr;
    }

    void print(ostream& str) const {
        if (left || right) {
            str << "[";
            left->print(str);
            str << ",";
            right->print(str);
            str << "]";
        } else {
            str << value;
        }
    }

    i64 magnitude() const {
        if (isLeaf()) {
            return value;
        }
        return 3 * left->magnitude() + 2 * right->magnitude();
    }

    PNode deepCopy() const {
        auto result = make_shared<Node>(value);
        if (left) {
            auto leftCopy = left->deepCopy();
            leftCopy->parent = result;
            result->left = leftCopy;
        }
        if (right) {
            auto rightCopy = right->deepCopy();
            rightCopy->parent = result;
            result->right = rightCopy;
        }
        return result;
    }
};

ostream& operator<<(ostream& s, const Node& node) {
    node.print(s);
    return s;
}

using PNode = Node::PNode;

void inOrder(PNode node, vector<PNode>& result) {
    if (node->left) {
        inOrder(node->left, result);
    }
    if (node->isLeaf()) {
        result.emplace_back(node);
    }
    if (node->right) {
        inOrder(node->right, result);
    }
}

PNode findSplitable(PNode node) {
    if (node->left) {
        auto left = findSplitable(node->left);
        if (left) {
            return left;
        }
    }
    if (node->isLeaf()) {
        if (node->value >= 10) {
            return node;
        }
    }
    if (node->right) {
        auto right = findSplitable(node->right);
        if (right) {
            return right;
        }
    }
    return nullptr;
}

PNode findExplodable(PNode node, int level) {
    if (node->left) {
        auto left = findExplodable(node->left, level + 1);
        if (left) {
            return left;
        }
    }
    if (level > 3) {
        if (node->left != nullptr && node->right != nullptr && node->left->isLeaf() && node->right->isLeaf()) {
            return node;
        }
    }
    if (node->right) {
        auto right = findExplodable(node->right, level + 1);
        if (right) {
            return right;
        }
    }
    return nullptr;
}

void split(PNode node) {
    node->left = make_shared<Node>(node->value / 2);
    node->left->parent = node;
    node->right = make_shared<Node>(node->value - node->left->value);
    node->right->parent = node;
    node->value = -1;
}

void explode(PNode node, PNode root) {
    vector<PNode> order;
    inOrder(root, order);
    auto pos = find(order.begin(), order.end(), node->left);
    if (pos != order.begin()) {
        auto left = *(pos - 1);
        left->value += node->left->value;
    }
    if (pos + 2 != order.end()) {
        auto right = *(pos + 2);
        right->value += node->right->value;
    }
    node->left = nullptr;
    node->right = nullptr;
    node->value = 0;
    /*
    cerr << "Explode '";
    node->print();
    cerr << "' into prev '";
    auto p = Node::prev(node->left);
    if (p) {
        while (p->left) {
            p = p->left;
        }
        p->print();
        p->value += node->left->value;
    }
    cerr << "' and next '";
    auto n = Node::next(node->right);
    if (n) {
        while (n->right) {
            n = n->right;
        }
        n->print();
        n->value += node->right->value;
    }
    cerr << "'" << endl;
    node->left = nullptr;
    node->right = nullptr;
    node->value = 0;
    */
}

bool isInt(const string& s) { return s.size() == 1 && isdigit(s[0]); }

PNode parse(const string& s, PNode parent) {
    // cerr << "Parse '" << s << "'" << endl;
    if (isInt(s)) {
        auto res = make_shared<Node>(stoi(s));
        res->parent = parent;
        return res;
    }
    ASSERTEQ(s.front(), '[');
    ASSERTEQ(s.back(), ']');
    int level = 0;
    string left;
    string right;
    bool inLeft = true;
    for (char ch : s) {
        if (ch == '[') {
            ++level;
        }
        if (ch == ']') {
            --level;
        }
        if (level == 1 && ch == ',') {
            inLeft = false;
        } else {
            if (inLeft) {
                left += ch;
            } else {
                right += ch;
            }
        }
    }
    // cerr << "Split '" << s << "' into '" << left << "' and '" << right << "'" << endl;
    auto result = make_shared<Node>();
    result->left = parse(left.substr(1), result);
    result->right = parse(right.substr(0, right.size() - 1), result);
    result->parent = parent;
    result->value = -1;
    return result;
}

PNode add(PNode left, PNode right) {
    auto result = make_shared<Node>();
    result->left = left;
    result->right = right;
    result->parent = nullptr;
    return result;
}

void normalize(PNode node) {
    bool change = true;
    while (change) {
        change = false;
        auto findExp = findExplodable(node, 0);
        if (findExp) {
            /*
            cerr << "Explode '";
            findExp->print();
            cerr << "' into '";
            */

            explode(findExp, node);

            /*
            node->print();
            cerr << "'" << endl;
            */

            change = true;
        } else {
            auto findSpl = findSplitable(node);
            if (findSpl) {
                split(findSpl);
                change = true;
            }
        }
    }
}

void first() {
    const auto input = readInputLines();

    auto sum = parse(input[0], nullptr);
    // sum->print();
    // cerr << endl;
    for (int i = 1; i < input.size(); ++i) {
        /*
        sum->print();
        cerr << endl;
        */

        sum = add(sum, parse(input[i], nullptr));
        normalize(sum);
    }

    /*
    sum->print();
    cerr << endl;
    */

    cout << sum->magnitude() << endl;
}

void second() {
    const auto input = readInputLines();

    vector<PNode> nodes;
    for (const auto& s : input) {
        nodes.emplace_back(parse(s, nullptr));
    }

    i64 mx = 0;
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = 0; j < nodes.size(); ++j) {
            if (i != j) {
                auto a = nodes[i]->deepCopy();
                auto b = nodes[j]->deepCopy();
                a = add(a, b);
                normalize(a);
                const auto m = a->magnitude();
                if (m > mx) {
                    mx = m;
                }
            }
        }
    }

    cout << mx << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
