#include "lib/ml/dataframe.h"

#include "lib/io/csv.h"
#include "lib/exception.h"

DataFrame::Column::Column(string name) : name_(std::move(name)) {}

DataFrame::DataFrame() {
}

DataFrame::DataFrame(const StringVector& names) {
    for (const auto& name: names) {
        addColumn(make_shared<Column>(name));
    }
}

DataFrame::PDataFrame DataFrame::shallowCopy() {
    auto result = make_shared<DataFrame>();
    for (auto& c : columns_) {
        result->addColumn(c);
    }
    return result;
}

DataFrame::PDataFrame DataFrame::loadFromCsv(const string& filename) {
    auto result = make_shared<DataFrame>();
    CsvParser reader(filename);
    if (!reader.readHeader()) {
        THROW("Header read failed");
    }
    const auto header = reader.header();
    for (size_t i = 0; i < header.size(); ++i) {
        result->addColumn(make_shared<Column>(header[i]));
    }

    while (reader.readLine()) {
        for (size_t i = 0; i < header.size(); ++i) {
            result->columns_[i]->data_.emplace_back(reader.get(i));
        }
    }
    return result;
}

size_t DataFrame::numLines() const { return columns_[0]->data_.size(); }

StringVector DataFrame::columnNames() const {
    StringVector result(columns_.size());
    for (size_t i = 0; i < columns_.size(); ++i) {
        result[i] = columns_[i]->name_;
    }
    return result;
}

StringVector DataFrame::line(size_t line) const {
    StringVector result(columns_.size());
    for (size_t i = 0; i < columns_.size(); ++i) {
        result[i] = columns_[i]->data_[line];
    }
    return result;
}

void DataFrame::pushBackLine(const StringVector& v) const {
    for (size_t i = 0; i < columns_.size(); ++i) {
        columns_[i]->data_.emplace_back(v[i]);
    }
}

void DataFrame::emplaceBackLine(StringVector&& v) const {
    for (size_t i = 0; i < columns_.size(); ++i) {
        columns_[i]->data_.emplace_back(std::move(v[i]));
    }
}

const DataFrame::PColumn DataFrame::getColumn(const string& name) const {
    auto toColumn = name2index_.find(name);
    if (toColumn == name2index_.end()) {
        THROW("Column '" << name << "' not found");
    }
    return columns_[toColumn->second];
}

void DataFrame::addColumn(PColumn column) {
    columns_.emplace_back(column);
    name2index_[column->name_] = columns_.size() - 1;
}

void DataFrame::eraseColumn(const string& name) {
    auto toColumn = name2index_.find(name);
    if (toColumn == name2index_.end()) {
        THROW("Column '" << name << "' not found");
    }
    columns_.erase(columns_.begin() + toColumn->second);
    reindex();
}

void DataFrame::reindex() {
    name2index_.clear();
    for (size_t i = 0; i < columns_.size(); ++i) {
        name2index_.emplace(columns_[i]->name_, i);
    }
}

pair<DataFrame::PDataFrame, DataFrame::PDataFrame> DataFrame::randomSplit(double ratio) const {
    auto first = make_shared<DataFrame>(columnNames());
    auto second = make_shared<DataFrame>(columnNames());
    for (size_t i = 0; i < numLines(); ++i) {
        double intPart;
        const double p = modf(i * M_PI, &intPart);
        if (p < ratio) {
            first->emplaceBackLine(line(i));
        } else {
            second->emplaceBackLine(line(i));
        }
    }
    return make_pair(first, second);
}

ostream& operator<<(ostream& s, const DataFrame& df) {
    s << df.columnNames() << endl;
    for (size_t i = 0; i < df.numLines(); ++i) {
        s << df.line(i) << endl;
    }
    return s;
}
