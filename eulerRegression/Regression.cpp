#include "lib/header.h"

#include "lib/io/utils.h"

#include "lib/ml/ols.h"
#include "lib/ml/gbdt.h"
#include "lib/ml/dataframe.h"

int main() {
    auto df = DataFrame::loadFromCsv(repoRoot() + "/eulerRegression/kc_house_data.csv");
    cout << *df << endl;
    cout << df->getColumn("price")->cast<double>() << endl;
    auto dfTrain = df->shallowCopy();
    dfTrain->eraseColumn("price");
    cout << dfTrain->columnNames() << endl;

    LinearRegressor lr(dfTrain->columnNames().size());
    lr.fit(dfTrain->cast<double>(), df->getColumn("price")->cast<double>());
    cout << lr.beta_ << endl;

    return 0;
}
