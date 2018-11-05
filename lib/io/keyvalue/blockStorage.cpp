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

BlockFileWriter::BlockFileWriter(string filename) : BlockFile(std::move(filename), "wb") {
    Header h;
    f_.writeT<Header>(h);
}

BlockFileWriter::~BlockFileWriter() {
    ASSERT(writerClosed_);
    ASSERTEQ(entries_.size(), keys_.size());
    for (size_t i = 0; i < keys_.size(); ++i) {
        auto& entry = entries_[i];
        entry.keyBegin_ = f_.tell();
        writeString(keys_[i]);
        f_.flush();
        entry.keyEnd_ = f_.tell();
    }
    for (const auto& entry : entries_) {
        f_.writeT(entry);
    }
    f_.flush();
    auto header = readHeader();
    header.tableOffset_ = f_.tell();
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

BlockFileWriter::Writer::Writer(BlockFileWriter* bf) : bfw_(bf) {}

void BlockFileWriter::Writer::write(const char* buffer, size_t size) {
    ASSERT(bfw_);
    bfw_->write(buffer, size);
}

void BlockFileWriter::Writer::close() {
    bfw_->onWriterClose();
    bfw_ = nullptr;
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

BlockFileReader::BlockFileReader(std::string filename) : BlockFile(std::move(filename), "rb") {
    auto header = readHeader();
    f_.seek(header.tableOffset_);
    for (size_t i = 0; i < header.tableEntries_; ++i) {
        entries_.emplace_back(f_.readT<Entry>());
    }
    for (size_t i = 0; i < header.tableEntries_; ++i) {
        keys_.emplace_back(readString(entries_[i].keyBegin_, entries_[i].keyEnd_));
    }
    for (size_t i = 0; i < keys_.size(); ++i) {
        keyToEntry_.emplace(keys_[i], i);
    }
}

uint64_t BlockFileReader::read(uint64_t startOffset, uint64_t endOffset, char* buffer, size_t size) {
    auto pos = f_.tell();
    ASSERT(pos >= startOffset);
    ASSERT(pos <= endOffset);
    uint64_t toRead = std::min(size, endOffset - pos);
    return f_.read(buffer, toRead);
}

std::unique_ptr<BlockFileReader::Reader> BlockFileReader::get(const std::string& key) {
    auto toKey = keyToEntry_.find(key);
    if (toKey == keyToEntry_.end()) {
        return nullptr;
    }
    const auto& entry = entries_[toKey->second];
    f_.seek(entry.valueBegin_);
    return std::unique_ptr<Reader>(new Reader(this, entry.valueBegin_, entry.valueEnd_));
}

BlockFileReader::Reader::Reader(BlockFileReader* bf, uint64_t startOffset, uint64_t endOffset)
    : bfr_(bf), startOffset_(startOffset), endOffset_(endOffset) {}

size_t BlockFileReader::Reader::read(char* buffer, size_t size) {
    return bfr_->read(startOffset_, endOffset_, buffer, size);
}
