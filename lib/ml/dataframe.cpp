#include "lib/ml/dataframe.h"

#include "lib/io/csv.h"

DataFrame::Column::Column(string name) : name_(std::move(name)) {}

PDataFrame DataFrame::loadFromCsv(const string& filename) {
    CsvReader reader(filename);
    if (!reader.readHeader()) {
        THROW("Header read failed");
    }
    const auto header = reader.getHeader();
    for (size_t i = 0; i < header.size(); ++i) {
        columns_.emplace_back(make_shared<Column>(header[i]));
    }
}
