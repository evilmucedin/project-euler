#include <set>

#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "lib/init.h"
#include "lib/random.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

struct Program {
    enum Op { Inp, Add, Mul, Div, Mod, Eql };

    using T = i64;

    struct Literal {
        bool isRef;
        T value;
        T* ref;
        string literal;
    };
    using Literals = vector<Literal>;

    struct Inst {
        Op op;
        Literals args;

        Inst() {}

        Inst(Op op, Literals args) : op(op), args(std::move(args)) {}
    };
    vector<Inst> ins;

    T w;
    T x;
    T y;
    T z;

    Literal toLiteral(const string& s) {
        Literal result;
        result.literal = s;
        if (s == "w" || s == "x" || s == "y" || s == "z") {
            result.isRef = true;
            result.ref = &var(s);
        } else {
            result.isRef = false;
            result.value = stoi(s);
        }
        return result;
    }

    Program(const StringVector& inp) {
        for (const auto& s : inp) {
            const auto parts = split(s, ' ');
            if (parts[0] == "inp") {
                ins.emplace_back(Inst(Inp, {toLiteral(parts[1])}));
            } else if (parts[0] == "add") {
                ins.emplace_back(Inst(Add, {toLiteral(parts[1]), toLiteral(parts[2])}));
            } else if (parts[0] == "mul") {
                ins.emplace_back(Inst(Mul, {toLiteral(parts[1]), toLiteral(parts[2])}));
            } else if (parts[0] == "div") {
                ins.emplace_back(Inst(Div, {toLiteral(parts[1]), toLiteral(parts[2])}));
            } else if (parts[0] == "mod") {
                ins.emplace_back(Inst(Mod, {toLiteral(parts[1]), toLiteral(parts[2])}));
            } else if (parts[0] == "eql") {
                ins.emplace_back(Inst(Eql, {toLiteral(parts[1]), toLiteral(parts[2])}));
            } else {
                throw Exception("unknown op");
            }
        }
        clear();
    }

    void clear() {
        w = 0;
        x = 0;
        y = 0;
        z = 0;
    }

    T& var(const string& s) {
        if (s == "w") {
            return w;
        }
        if (s == "x") {
            return x;
        }
        if (s == "y") {
            return y;
        }
        if (s == "z") {
            return z;
        }
        throw Exception("Unknown var");
    }

    static T eval(const Literal& l) {
        if (l.isRef) {
            return *(l.ref);
        } else {
            return l.value;
        }
    }

    static T& ref(const Literal& l) {
        if (!l.isRef) {
            throw Exception("not ref");
        }
        return *(l.ref);
    }

    T run(const IntVector& inp) {
        int inpIt = 0;
        for (const auto& in : ins) {
            switch (in.op) {
                case Inp:
                    if (inpIt >= inp.size()) {
                        throw Exception("Out of bounds");
                    }
                    ref(in.args[0]) = inp[inpIt++];
                    break;
                case Add:
                    ref(in.args[0]) += eval(in.args[1]);
                    break;
                case Mul:
                    ref(in.args[0]) *= eval(in.args[1]);
                    break;
                case Div: {
                    const auto b = eval(in.args[1]);
                    if (b == 0) {
                        throw Exception("bad div 1");
                    }
                    ref(in.args[0]) /= b;
                    break;
                }
                case Mod: {
                    const auto b = eval(in.args[1]);
                    if (b <= 0) {
                        throw Exception("bad mod 1");
                    }
                    if (eval(in.args[0]) < 0) {
                        throw Exception("bad mod 2");
                    }
                    ref(in.args[0]) %= b;
                    break;
                }
                case Eql:
                    ref(in.args[0]) = int(eval(in.args[0]) == eval(in.args[1]));
                    break;
                default:
                    throw Exception("unknown op");
            }
        }
        return z;
    }

    void codegen(ostream& o) const {
        int inpIt = 0;
        for (const auto& in : ins) {
            switch (in.op) {
                case Inp:
                    o << in.args[0].literal << " = inp[" << inpIt << "];" << endl;
                    ++inpIt;
                    break;
                case Add:
                    o << in.args[0].literal << " += " << in.args[1].literal << ";" << endl;
                    break;
                case Mul:
                    o << in.args[0].literal << " *= " << in.args[1].literal << ";" << endl;
                    break;
                case Div: {
                    o << in.args[0].literal << " /= " << in.args[1].literal << ";" << endl;
                    break;
                }
                case Mod: {
                    o << in.args[0].literal << " %= " << in.args[1].literal << ";" << endl;
                    break;
                }
                case Eql:
                    o << in.args[0].literal << " = " << in.args[0].literal << " == " << in.args[1].literal << ";" << endl;
                    break;
                default:
                    throw Exception("unknown op");
            }
        }
    }

    T runCodegen(const IntVector& inp){
#include "program.inc"
    }

    T runSubprogramCodegen(const IntVector& inp, int subprogram) {
#include "subPrograms.inc"
    }
};

set<pair<i64, int>> mem;

vector<i64> ans;

void find(IntVector& v, i64 targetZ, Program& program, int index) {
    if (index < 0) {
        cerr << v << endl;
        ans.emplace_back(digitsToNum<i64>(v));
        return;
    }

    pair<i64, int> key = make_pair(targetZ, index);
    if (mem.count(key)) {
        return;
    }
    mem.emplace(key);

    // cerr << targetZ << " " << index << endl;
    // LOG_EVERY_MS(INFO, 1000) << OUT(targetZ) << OUT(index) << OUT(v);
    I64Vector cands;
    static constexpr int LIMIT = 35;
    /*
    for (int z = 0; z < LIMIT; ++z) {
        cands.emplace_back(z);
    }
    */
    for (int z = -LIMIT; z < LIMIT; ++z) {
        cands.emplace_back(targetZ + z);
    }
    /*
     */
    for (int z = -LIMIT; z < LIMIT; ++z) {
        cands.emplace_back(26 * targetZ + z);
    }
    for (int z = -LIMIT; z < LIMIT; ++z) {
        cands.emplace_back(targetZ / 26 + z);
    }
    sortAndUnique(cands);
    IntVector inp(1);
    for (int i = 1; i <= 9; ++i) {
        v[index] = i;
        inp[0] = i;
        // for (int z = 26 * targetZ; z < 26 * targetZ + 100; ++z) {
        for (auto z : cands) {
            if (z < 0) {
                continue;
            }
            program.clear();
            program.z = z;
            program.runSubprogramCodegen(inp, index);
            if (program.z == targetZ) {
                /*
                cerr << "Found: " << index << " " << z << " " << targetZ << " " << ((targetZ) ? (z / targetZ) : z)
                     << " " << ((targetZ) ? (z % targetZ) : z) << endl;
                */
                find(v, z, program, index - 1);
            }
        }
    }
}

vector<shared_ptr<Program>> parsePrograms(const StringVector& input) {
    vector<shared_ptr<Program>> programs;
    StringVector buffer;
    for (const auto& s : input) {
        if (s[0] == 'i') {
            if (buffer.size()) {
                programs.emplace_back(make_shared<Program>(buffer));
                buffer.clear();
            }
            buffer.emplace_back(s);
        } else {
            buffer.emplace_back(s);
        }
    }
    programs.emplace_back(make_shared<Program>(buffer));
    return programs;
}

void codegen() {
    const auto input = readInputLines();

    {
        Program p(input);
        ofstream fOut("program.inc");
        p.codegen(fOut);
        fOut << "return z;" << endl;
    }

    {
        auto subPrograms = parsePrograms(input);
        ofstream fOut("subPrograms.inc");
        fOut << "switch (subprogram) {" << endl;
        for (size_t i = 0; i < subPrograms.size(); ++i) {
            fOut << "case " << i << ": {" << endl;
            subPrograms[i]->codegen(fOut);
            fOut << "}" << endl;
            fOut << "break;" << endl;
        }
        fOut << "}" << endl;
        fOut << "return z;" << endl;
    }
}

void first() {
    const auto input = readInputLines();
    Program p(input);
    const auto programs = parsePrograms(input);

    IntVector v(14, 9);
    find(v, 0, p, programs.size() - 1);

    if (ans.size()) {
        auto res = maxV(ans);
        cout << "1: " << res << endl;

        p.clear();
        cerr << p.run(numToDigits(res)) << endl;

        auto res2 = minV(ans);
        cout << "2: " << res2 << endl;

        p.clear();
        cerr << p.run(numToDigits(res2)) << endl;
    }

    /*
    Program p(input);
    cerr << p.ins.size() << endl;

    IntVector inp(14, 9);
    REP(k, 1000) {
        REP (i, 14) {
            inp[i] = (randInt() % 9) + 1;
        }
        IntVector indices(14);
        REP (i, 14) {
            indices[i] = i;
        }
        shuffle(indices);
        REP(i, 14) {
            i64 min = 99999999999999;
            int minJ = 9;
            for (int j = 9; j > 0; --j) {
                p.clear();
                inp[indices[i]] = j;
                auto val = p.run(inp);
                // cerr << indices[i] << " " << j << " " << val << endl;
                if (val < min) {
                    min = val;
                    minJ = j;
                }
            }
            inp[indices[i]] = minJ;
        }
        p.clear();
        auto val = p.run(inp);
        cerr << k << " " << val << " " << inp << endl;
    }
    p.clear();
    cout << inp << " " << p.run(inp) << endl;
    */
    /*
    i64 n = 99999999999999;
    while (true) {
        string s = to_string(n);
        if (s.find('0') == string::npos) {
            IntVector v;
            for (char ch: s) {
                v.emplace_back(ch - '0');
            }
            p.clear();
            auto res = p.run(v);
            if (res == 0) {
                break;
            }
            LOG_EVERY_MS(INFO, 1000) << n << " " << res;
        }
        --n;
    }
    cerr << n << endl;
    */
}

void second() {
    const auto input = readInputLines();
    cout << endl;
}

void helper() {
    const auto input = readInputLines();
    Program p(input);
    IntVector v(1);
    for (int z = 0; z < 1000; ++z) {
        for (int i = 1; i < 10; ++i) {
            v[0] = i;
            p.clear();
            p.z = z;
            p.run(v);
            if (p.z == 0) {
                cerr << z << " " << i << " " << p.z << " " << p.w << " " << p.x << " " << p.y << " " << p.z << endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    /*
    helper();
    return 0;
    */

    if (FLAGS_test == 0) {
        codegen();
    } else if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
