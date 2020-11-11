#include "lib/header.h"

#include "lib/string.h"

class DataFrame {
   public:
    DataFrame();
    DataFrame(const StringVector& names);

    using PDataFrame = shared_ptr<DataFrame>;
    static PDataFrame loadFromCsv(const string& filename);
    PDataFrame shallowCopy();

    size_t numLines() const;
    StringVector columnNames() const;
    StringVector line(size_t line) const;
    void pushBackLine(const StringVector& v) const;
    void emplaceBackLine(StringVector&& v) const;

    struct Column {
        Column(string name);

        template <typename T>
        vector<T> cast() const {
            vector<T> result(data_.size());
            for (size_t i = 0; i < data_.size(); ++i) {
                result[i] = stringCast<T>(data_[i]);
            }
            return result;
        }

        string name_;
        StringVector data_;
    };
    using PColumn = shared_ptr<Column>;
    vector<PColumn> columns_;
    unordered_map<string, size_t> name2index_;

    const PColumn getColumn(const string& name) const;
    void addColumn(PColumn column);
    void eraseColumn(const string& name);
    void reindex();
    pair<PDataFrame, PDataFrame> randomSplit(double ratio) const;
};

ostream& operator<<(ostream& s, const DataFrame& df);
