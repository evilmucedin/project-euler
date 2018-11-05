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
    auto result = f_.readT<Header>();
    ASSERTEQ(result.magic_, MAGIC);
    return result;
}

void BlockFile::writeHeader(const BlockFile::Header& header) {
    f_.seek(0);
    f_.writeT<Header>(header);
    f_.flush();
}

BlockFileWriter::BlockFileWriter(string filename) : BlockFile(filename_, "wb") {
    Header h;
    f_.writeT<Header>(h);
}

BlockFileWriter::~BlockFileWriter() {
    ASSERT(writerClosed_);
    ASSERTEQ(entries_.size(), keys_.size());
    uint64_t entriesBegin = f_.tell();
    for (size_t i = 0; i < keys_.size(); ++i) {
        auto& entry = entries_[i];
        entry.keyBegin_ = f_.tell();
        writeString(keys_[i]);
        entry.keyEnd_ = f_.tell();
    }
    auto header = readHeader();
    header.tableOffset_ = entriesBegin;
    header.tableEntries_ = keys_.size();
    writeHeader(header);
}

std::shared_ptr<BlockFileWriter::Writer> BlockFileWriter::add(const std::string& key) {
    ASSERT(writerClosed_);
    keys_.emplace_back(key);
    entries_.emplace_back();
    entries_.back().valueBegin_ = f_.tell();
    std::shared_ptr<BlockFileWriter::Writer> result(new BlockFileWriter::Writer(this));
    writerClosed_ = false;
    return result;
}

void BlockFileWriter::write(const char* buffer, size_t size) {
    ASSERT(!writerClosed_);
    f_.write(buffer, size);
}

void BlockFileWriter::onWriterClose() {
    f_.flush();
    entries_.back().valueEnd_ = f_.tell();
    writerClosed_ = true;
}
