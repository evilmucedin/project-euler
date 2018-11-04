#include "blockStorage.h"

BlockFile::BlockFile(std::string filename, std::string mode) : filename_(std::move(filename)), f_(filename_, mode) {}

void BlockFile::writeString(const std::string& key) {
    f_.writeT<uint64_t>(key.size());
    f_.write(key.data(), key.size());
}

std::string BlockFile::readString(size_t begin, size_t end) {
    f_.seek(begin);
    auto length = f_.readT<uint64_t>();
    std::string result;
    result.resize(length);
    f_.read((char*)result.data(), length);
    ASSERTEQ(f_.tell(), end);
    return result;
}

BlockFile::Header BlockFile::readHeader() {
    f_.seek(0);
    return f_.readT<Header>();
}

void BlockFile::writeHeader(const BlockFile::Header& header) {
    f_.seek(0);
    f_.writeT<Header>(header);
}

BlockFileWriter::BlockFileWriter(string filename) : BlockFile(filename_, "wb") {
    Header h;
    f_.writeT<Header>(h);
}

BlockFileWriter::~BlockFileWriter() {
    ASSERTEQ(entries_.size(), keys_.size());
    uint64_t entriesStart = f_.tell();
    for (size_t i = 0; i < keys_.size(); ++i) {
        auto& entry = entries_[i];
        entry.keyStart_ = f_.tell();
        writeString(keys_[i]);
        entry.keyEnd_ = f_.tell();
    }
    auto header = readHeader();
    header.tableOffset_ = entriesStart;
    header.tableEntries_ = keys_.size();
    writeHeader(header);
}

std::shared_ptr<BlockFileWriter::Writer> BlockFileWriter::add(const std::string& key) {
    keys_.emplace_back(key);
    entries_.emplace_back();
    auto result = std::make_shared<Writer>(new Writer(this));
    return result;
}
