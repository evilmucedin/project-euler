#include "lib/header.h"

#include "lib/io/utils.h"

#include "lib/ml/ols.h"
#include "lib/ml/gbdt.h"
#include "lib/ml/dataframe.h"
#include "lib/ml/metrics.h"

int main() {
    auto df = DataFrame::loadFromCsv(repoRoot() + "/eulerRegression/kc_house_data.csv");

    auto [dfTrain, dfTest] = df->randomSplit(0.8);

    auto dfTrainM = dfTrain->shallowCopy();
    dfTrainM->eraseColumn("price");
    cout << dfTrainM->columnNames() << endl;
    auto dfTestM = dfTest->shallowCopy();
    dfTestM->eraseColumn("price");

    LinearRegressor lr(dfTrainM->columnNames().size());
    lr.fit(dfTrainM->cast<double>(), dfTrain->getColumn("price")->cast<double>());
    cout << lr.beta_ << endl;
    cout << r2(dfTest->getColumn("price")->cast<double>(), lr.regress(dfTestM->cast<double>())) << endl;

    GBDT gbdt;
    gbdt.setMaxEpochs(100).setLRate(0.01).setVerbose(false);
    gbdt.fit(dfTrainM->cast<double>(), dfTrain->getColumn("price")->cast<double>());
    cout << gbdt.explain(dfTrainM->columnNames()) << endl;
    cout << r2(dfTest->getColumn("price")->cast<double>(), gbdt.regress(dfTestM->cast<double>())) << endl;

    return 0;
}
