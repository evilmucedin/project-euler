#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

int versionSum = 0;

using TValue = i64;

TValue readInt(const string& s, size_t len, size_t& index) {
    i64 res = 0;
    REP(i, len) {
        if (index >= s.size()) {
            throw Exception("readInt");
        }
        res = 2 * res + (s[index] - '0');
        ++index;
    }
    return res;
}

TValue readLiteral(const string& s, size_t& index) {
    i64 result = 0;
    while (true) {
        auto val = readInt(s, 5, index);
        result = (result << 4) + (val & 15);
        if (val < 16) {
            break;
        }
    }
    cerr << "Literal: " << result << endl;
    return result;
}

TValue readPacket(const string& s, size_t& index);

TValue readOperator(const string& s, size_t& index, int id) {
    const auto mode = readInt(s, 1, index);
    vector<TValue> values;
    if (mode == 0) {
        const auto len = readInt(s, 15, index);
        cerr << "readOperator mode=0 len=" << len << ", index=" << index << ", size=" << s.size() << ", id=" << id
             << endl;
        const size_t limit = index + len;
        while (index < limit) {
            values.emplace_back(readPacket(s, index));
        }
        if (index != limit) {
            throw Exception("readOperator");
        }
    } else {
        const auto len = readInt(s, 11, index);
        cerr << "readOperator mode=1 len=" << len << ", id: " << id << endl;
        REP(i, len) {
            cerr << "readOperatorBeg i=" << i << ", index=" << index << endl;
            values.emplace_back(readPacket(s, index));
        }
    }

    switch (id) {
        case 0:
            return sum(values);
        case 1:
            return product(values);
        case 2:
            return minV(values);
        case 3:
            return maxV(values);
        case 5:
            return values[0] > values[1];
        case 6:
            return values[0] < values[1];
        case 7:
            return values[0] == values[1];
        default:
            throw Exception("unk id");
    }
}

TValue readPacket(const string& s, size_t& index) {
    size_t begin = index;
    const auto version = readInt(s, 3, index);
    versionSum += version;
    const auto id = readInt(s, 3, index);
    cerr << "ver: " << version << ", id=" << id << endl;
    if (id == 4) {
        return readLiteral(s, index);
    } else {
        return readOperator(s, index, id);
    }
    /*
    while ((index - begin) % 4) {
        ++index;
    }
    */
    cerr << "End of packet: " << index << ", begin=" << begin << ", size=" << s.size() << endl;
}

void first() {
    const auto input = readInputLines();

    string s;
    for (const auto ch : trim(input[0])) {
        switch (ch) {
            case '0':
                s += "0000";
                break;
            case '1':
                s += "0001";
                break;
            case '2':
                s += "0010";
                break;
            case '3':
                s += "0011";
                break;
            case '4':
                s += "0100";
                break;
            case '5':
                s += "0101";
                break;
            case '6':
                s += "0110";
                break;
            case '7':
                s += "0111";
                break;
            case '8':
                s += "1000";
                break;
            case '9':
                s += "1001";
                break;
            case 'A':
                s += "1010";
                break;
            case 'B':
                s += "1011";
                break;
            case 'C':
                s += "1100";
                break;
            case 'D':
                s += "1101";
                break;
            case 'E':
                s += "1110";
                break;
            case 'F':
                s += "1111";
                break;
            default:
                throw Exception("bad char");
        }
    }

    cerr << s << endl;

    size_t index = 0;
    auto res = readPacket(s, index);

    cerr << s.size() << " " << index << endl;

    cout << "vs: " << versionSum << endl;
    cout << "eval: " << res << endl;
}

void second() { first(); }

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
