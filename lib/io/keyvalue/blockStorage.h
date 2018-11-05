#include "lib/header.h"
#include "lib/io/file.h"

class BlockFile {
   protected:
    BlockFile(std::string filename, std::string mode);

    static constexpr uint32_t MAGIC = 0x46425244;

#pragma pack(push, 8)
    struct Header {
        uint32_t magic_{MAGIC};
        uint32_t version_{1};
        uint64_t tableOffset_{0};
        uint64_t tableEntries_{0};
    };

    struct Entry {
        uint64_t keyBegin_;
        uint64_t keyEnd_;
        uint64_t valueBegin_;
        uint64_t valueEnd_;
    };
#pragma pack(pop)
    using Entries = vector<Entry>;

    string filename_;
    File f_;
    Entries entries_;
    StringVector keys_;

    void writeString(const std::string& key);
    std::string readString(size_t begin, size_t end);

    Header readHeader();
    void writeHeader(const Header& header);
};

class BlockFileWriter : protected BlockFile {
   public:
    BlockFileWriter(string filename);
    ~BlockFileWriter();

    class Writer {
       public:
        void write(const char* buffer, size_t size);
        void close();

       protected:
        Writer(BlockFileWriter* bf);

        BlockFileWriter* bfw_;

        friend class BlockFileWriter;
    };

    std::shared_ptr<Writer> add(const std::string& key);

   protected:
    void write(const char* buffer, size_t size);
    void onWriterClose();

   private:
    bool writerClosed_{true};
};

class BlockFileReader : protected BlockFile {
   public:
    BlockFileReader(std::string filename);

    class Reader {
       public:
        size_t read(char* buffer, size_t size);

       protected:
        Reader(BlockFileReader* bf, uint64_t startOffset, uint64_t endOffset);

        BlockFileReader* bfr_;
        uint64_t startOffset_;
        uint64_t endOffset_;

        friend class BlockFileReader;
    };

    std::unique_ptr<Reader> get(const std::string& key);

   protected:
    size_t read(uint64_t startOffset, uint64_t endOffset, char* buffer, size_t size);

   private:
    using KeyToEntry = unordered_map<string, uint64_t>;
    KeyToEntry keyToEntry_;
};
