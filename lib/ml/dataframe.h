#include "lib/header.h"

class DataFrame {
   public:
    using PDataFrame = shared_ptr<DataFrame>;
    static PDataFrame loadFromCsv(const string& filename);

   private:
    struct Column {
        Column(string name);

        string name_;
        StringVector data_;
    };
    using PColumn = shared_ptr<Column>;
    vector<PColumn> columns_;
};
