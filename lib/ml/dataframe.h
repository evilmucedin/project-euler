#include "lib/header.h"

#include "lib/string.h"
#include "lib/exception.h"

class DataFrame {
   public:
    DataFrame();
    DataFrame(const StringVector& names);

    using PDataFrame = shared_ptr<DataFrame>;
    static PDataFrame loadFromCsv(const string& filename);
    void saveToCsv(const string& filename) const;
    PDataFrame shallowCopy();
    PDataFrame subDataFrame(const SizeTVector& indices);

    size_t numLines() const;
    StringVector columnNames() const;
    StringVector line(size_t line) const;
    void pushBackLine(const StringVector& v) const;
    void emplaceBackLine(StringVector&& v) const;
    void resizeLines(size_t lines);

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

        template <typename T = string>
        T as(size_t line) const {
            return stringCast<T>(data_[line]);
        }

        template <typename T>
        void set(size_t line, const T& value) {
            if (line > data_.size()) {
                THROW("Bad line index " << line);
            }
            data_[line] = to_string(value);
        }

        size_t size() const {
            return data_.size();
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

    template <typename T>
    vector<vector<T>> cast() const {
        vector<vector<T>> result(numLines(), vector<T>(columns_.size()));
        for (size_t j = 0; j < columns_.size(); ++j) {
            for (size_t i = 0; i < numLines(); ++i) {
                result[i][j] = stringCast<T>(columns_[j]->data_[i]);
            }
        }
        return result;
    }
};

ostream& operator<<(ostream& s, const DataFrame& df);
