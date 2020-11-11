#include "lib/header.h"

class DataFrame {
   public:
    using PDataFrame = shared_ptr<DataFrame>;
    static PDataFrame loadFromCsv(const string& filename);

    size_t numLines() const;
    StringVector columnNames() const;
    StringVector line(size_t line) const;

    struct Column {
        Column(string name);

        string name_;
        StringVector data_;
    };
    using PColumn = shared_ptr<Column>;
    vector<PColumn> columns_;
};

ostream& operator<<(ostream& s, const DataFrame& df);
