#include "lib/header.h"

#include "lib/io/csv.h"
#include "lib/io/utils.h"

#include "lib/ml/ols.h"
#include "lib/ml/gbdt.h"

int main() {
    CsvParser csvParser(repoRoot() + "/eulerRegression/kc_house_data.csv");
    return 0;
}
