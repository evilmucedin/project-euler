#include "lib/header.h"

class DataFrame {
   public:
    using PDataFrame = shared_ptr<DataFrame>;
    static PDataFrame loadFromCsv(const string& filename);

   private:
};
