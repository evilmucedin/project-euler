#include "lib/header.h"

#include "lib/io/utils.h"

#include "lib/ml/ols.h"
#include "lib/ml/gbdt.h"
#include "lib/ml/dataframe.h"

int main() {
    auto df = DataFrame::loadFromCsv(repoRoot() + "/eulerRegression/kc_house_data.csv");
    cout << *df << endl;
    return 0;
}
