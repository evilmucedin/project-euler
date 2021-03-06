#include "gbdt.h"

#include <sys/time.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "lib/exception.h"
#include "lib/header.h"
#include "lib/string.h"

static void inplaceTrimLeft(std::string& strValue) {
    size_t pos = 0;
    for (size_t i = 0; i < strValue.size(); ++i) {
        if (isspace((unsigned char)strValue[i])) {
            ++pos;
        } else {
            break;
        }
    }
    if (pos > 0) {
        strValue.erase(0, pos);
    }
}

static void inplaceTrimRight(std::string& strValue) {
    size_t n = 0;
    for (size_t i = 0; i < strValue.size(); ++i) {
        if (isspace((unsigned char)strValue[strValue.length() - i - 1]))
            ++n;
        else
            break;
    }
    if (n != 0) {
        strValue.erase(strValue.length() - n);
    }
}

static void inplaceTrim(std::string& strValue) {
    inplaceTrimRight(strValue);
    inplaceTrimLeft(strValue);
}

static void Split(const std::string& strMain, char chSpliter, std::vector<std::string>& strList,
                  bool bReserveNullString) {
    strList.clear();

    if (strMain.empty()) return;

    size_t nPrevPos = 0;
    size_t nPos;
    std::string strTemp;
    while ((nPos = strMain.find(chSpliter, nPrevPos)) != std::string::npos) {
        strTemp.assign(strMain, nPrevPos, nPos - nPrevPos);
        inplaceTrim(strTemp);
        if (bReserveNullString || !strTemp.empty()) strList.push_back(strTemp);
        nPrevPos = nPos + 1;
    }

    strTemp.assign(strMain, nPrevPos, strMain.length() - nPrevPos);
    inplaceTrim(strTemp);
    if (bReserveNullString || !strTemp.empty()) strList.push_back(strTemp);
}

bool DataReader::ReadDataFromCVS(const std::string& input_file, Data& data, bool verbose) {
    std::ifstream fs;
    fs.open(input_file.c_str(), std::ios_base::in);

    if (fs.fail()) {
        if (verbose) {
            std::cerr << " Sorry! The file isn't exist. " << input_file << std::endl;
        }
        return false;
    }

    std::string strLine;
    unsigned int line_num = 0;
    while (getline(fs, strLine)) {
        if (!strLine.empty()) {
            std::vector<std::string> vecResult;
            Split(strLine, ',', vecResult, true);
            if (vecResult.size() >= 2) {
                data.m_data.resize(line_num + 1);
                GBDTVectorType& fv = data.m_data[line_num];

                GBDTDType f_value = 0;
                for (size_t i = 0; i < vecResult.size() - 1; ++i) {
                    char** endptr = NULL;
                    f_value = strtof(vecResult[i].c_str(), endptr);
                    if (errno == ERANGE) {
                        std::cerr << " feature out of the range: " << vecResult[i] << std::endl;
                    }
                    if (endptr != NULL && endptr[0] == vecResult[i].c_str()) {
                        std::cerr << " feature format wrong: " << vecResult[i] << std::endl;
                    }

                    fv.push_back(f_value);
                }

                unsigned int target_index = vecResult.size() - 1;
                char** endptr = NULL;
                f_value = strtof(vecResult[target_index].c_str(), endptr);
                if (errno == ERANGE) {
                    std::cerr << " target out of the range: " << vecResult[target_index] << std::endl;
                }
                if (endptr != NULL && endptr[0] == vecResult[target_index].c_str()) {
                    std::cerr << " target format wrong: " << vecResult[target_index] << std::endl;
                }
                data.m_target.push_back(f_value);

                if (line_num == 0) {
                    data.m_dimension = data.m_data[0].size();
                }
                line_num++;
            } else {
                // TODO statistic wrong line
            }
        }
    }
    if (data.m_data.size() > 0) {
        data.m_num = data.m_data.size();
    } else {
        return false;
    }

    if (verbose) {
        std::cerr << "dimension: " << data.m_dimension << std::endl;
        std::cerr << "data num: " << data.m_num << std::endl;
    }

    return true;
}

bool DataReader::ReadDataFromL2R(const std::string& input_file, Data& data, unsigned int dimentions) {
    std::ifstream fs;
    fs.open(input_file.c_str(), std::ios_base::in);

    if (fs.fail()) {
        std::cerr << " Sorry ! The file isn't exist. " << input_file << std::endl;
        return false;
    }

    data.m_dimension = dimentions;
    std::string strLine;
    unsigned int line_num = 0;
    while (getline(fs, strLine)) {
        if (!strLine.empty()) {
            std::vector<std::string> vecResult;
            Split(strLine, '\t', vecResult, true);
            if (vecResult.size() >= 3) {
                data.m_data.resize(line_num + 1);
                GBDTVectorType& fv = data.m_data[line_num];
                fv.resize(dimentions);

                // read target
                unsigned int target_index = 0;
                char** endptr = NULL;
                GBDTDType target_value = strtof(vecResult[target_index].c_str(), endptr);
                if (errno == ERANGE) {
                    std::cerr << " target out of the range: " << vecResult[target_index] << std::endl;
                    continue;
                }
                if (endptr != NULL && endptr[0] == vecResult[target_index].c_str()) {
                    std::cerr << " target format wrong: " << vecResult[target_index] << std::endl;
                    continue;
                }
                data.m_target.push_back(target_value);

                for (size_t i = 2; i < vecResult.size(); ++i) {
                    int f_index = -1;
                    GBDTDType f_value = 0.0;
                    int ret = sscanf(vecResult[i].c_str(), "%d:%lf", &f_index, &f_value);
                    if (ret != 2 || f_index >= (int)dimentions) {
                        std::cerr << " feature format wrong: " << line_num << "\t" << vecResult[i] << std::endl;
                        continue;
                    }
                    fv[f_index] = f_value;
                    data.m_valid_id.insert(f_index);
                }

                line_num++;
            } else {
                // TODO statistic wrong line
            }
        }
    }
    if (data.m_data.size() > 0) {
        data.m_num = data.m_data.size();
    } else {
        return false;
    }

    std::cerr << "dimension: " << data.m_dimension << std::endl;
    std::cerr << "data num: " << data.m_num << std::endl;
    std::cerr << "valid feature size: " << data.m_valid_id.size() << std::endl;

    return true;
}

bool compareNodeReduced(GBDT::NodeReduced n0, GBDT::NodeReduced n1) { return n0.m_size < n1.m_size; }

static int64_t Milliseconds() {
    struct timeval t;
    ::gettimeofday(&t, NULL);
    int64_t curTime;
    curTime = t.tv_sec;
    curTime *= 1000;              // sec -> msec
    curTime += t.tv_usec / 1000;  // usec -> msec
    return curTime;
}

GBDT::GBDT() {
    m_max_epochs = 10;
    m_global_mean = 0.0;
    m_max_tree_leafes = 6;
    m_feature_subspace_size = 10;
    m_use_opt_splitpoint = true;
    m_lrate = 0.001;
    m_train_epoch = 0;
    m_data_sample_ratio = 0.8;
    m_min_samples = 50;
    m_norm_samples = 1000;
    verbose_ = true;
}

bool GBDT::LoadConfig(const std::string& conf_file) {
    std::ifstream fs;
    fs.open(conf_file.c_str(), std::ios_base::in);

    if (fs.fail()) {
        std::cerr << " Warning ! Conf File isn't exist. Use default setting!" << conf_file << std::endl;
        return false;
    }

    std::string line;
    while (getline(fs, line)) {
        string::size_type pos = line.find("=");
        if (pos != string::npos && pos != (line.length() - 1)) {
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);

            std::stringstream sstream;
            if (key == "max_epochs") {
                sstream << value;
                sstream >> m_max_epochs;
            } else if (key == "max_tree_leafes") {
                sstream << value;
                sstream >> m_max_tree_leafes;
            } else if (key == "feature_subspace_size") {
                sstream << value;
                sstream >> m_feature_subspace_size;
            } else if (key == "use_opt_splitpoint") {
                if (value == "false") m_use_opt_splitpoint = false;
            } else if (key == "learn_rate") {
                sstream << value;
                sstream >> m_lrate;
            } else if (key == "data_sample_ratio") {
                sstream << value;
                sstream >> m_data_sample_ratio;
                if (m_data_sample_ratio > 1) {
                    m_data_sample_ratio = 1;
                } else if (m_data_sample_ratio < 0) {
                    m_data_sample_ratio = 0.01;
                }
            }
        }
    }
    return true;
}

GBDT& GBDT::setMaxEpochs(unsigned int max_epochs) {
    m_max_epochs = max_epochs;
    return *this;
}

GBDT& GBDT::setLRate(double lrate) {
    m_lrate = lrate;
    return *this;
}

GBDT& GBDT::setVerbose(bool verbose) {
    verbose_ = verbose;
    return *this;
}

bool GBDT::Init() {
    m_trees = new Node[m_max_epochs];
    for (unsigned int i = 0; i < m_max_epochs; ++i) {
        m_trees[i].m_featureNr = -1;
        m_trees[i].m_value = 1e10;
        m_trees[i].m_toSmallerEqual = 0;
        m_trees[i].m_toLarger = 0;
        m_trees[i].m_trainSamples = 0;
        m_trees[i].m_nSamples = -1;
    }

    srand(time(0));
    if (verbose_) {
        cerr << "configure--------" << endl;
        cerr << "  max_epochs: " << m_max_epochs << endl;
        cerr << "  max_tree_leafes: " << m_max_tree_leafes << endl;
        cerr << "  feature_subspace_size: " << m_feature_subspace_size << endl;
        cerr << "  use_opt_splitpoint: " << m_use_opt_splitpoint << endl;
        cerr << "  learn_rate: " << m_lrate << endl;
        cerr << "  data_sample_ratio: " << m_data_sample_ratio << endl;
        cerr << endl;
    }

    return true;
}

bool GBDT::Train(const Data& data) {
    m_tree_target.resize(data.m_target.size());
    m_feature_subspace_size = m_feature_subspace_size > data.m_dimension ? data.m_dimension : m_feature_subspace_size;

    double pre_rmse = -1;
    unsigned int train_epoch = 0;
    // TODO or rmse rise up
    for (; train_epoch < m_max_epochs; train_epoch++) {
        double rmse = 0.0;
        if (verbose_) {
            cerr << "epoch: " << train_epoch << endl;
        }

        ModelUpdate(data, train_epoch, rmse);

        // if (pre_rmse < rmse && pre_rmse != -1)
        // if (pre_rmse - rmse < ( m_lrate * 0.001 ) && pre_rmse != -1)
        if (pre_rmse < rmse && pre_rmse != -1) {
            if (verbose_) {
                cerr << "debug: rmse:" << rmse << " " << pre_rmse << " " << pre_rmse - rmse << std::endl;
            }
            break;
        }
        pre_rmse = rmse;
    }
    m_train_epoch = train_epoch - 1;

    return true;
}

void GBDT::fit(const DoubleMatrix& x, const DoubleVector& y) {
    Init();
    ASSERTEQ(x.size(), y.size());
    Data dt;
    dt.m_data = x;
    dt.m_target = y;
    dt.m_dimension = x[0].size();
    dt.m_num = x.size();
    for (size_t i = 0; i < x[0].size(); ++i) {
        dt.m_valid_id.emplace(i);
    }
    Train(dt);
}

bool GBDT::ModelUpdate(const Data& data, unsigned int train_epoch, double& rmse) {
    int64_t t0 = Milliseconds();

    int nSamples = data.m_num;
    unsigned int nFeatures = data.m_dimension;

    bool* usedFeatures = new bool[data.m_dimension];
    GBDTDType* inputTmp = new GBDTDType[(nSamples + 1) * m_feature_subspace_size];
    GBDTDType* inputTargetsSort = new GBDTDType[(nSamples + 1) * m_feature_subspace_size];
    int* sortIndex = new int[nSamples];

    //----first epoch----
    if (train_epoch == 0) {
        double mean = 0.0;
        if (true) {
            for (unsigned int j = 0; j < data.m_num; j++) mean += data.m_target[j];
            mean /= (double)data.m_num;
        }
        m_global_mean = mean;
        if (verbose_) {
            std::cerr << "globalMean:" << mean << " " << std::flush;
        }

        // align by targets mean
        for (unsigned int j = 0; j < data.m_num; j++) {
            m_tree_target[j] = data.m_target[j] - mean;
        }
    }

    deque<NodeReduced> largestNodes;
    //----init feature mask----
    for (unsigned int j = 0; j < data.m_dimension; j++) {
        usedFeatures[j] = false;
    }

    //----data should be sampled !!!!----
    int data_sample_num = int(nSamples * m_data_sample_ratio);
    if (data_sample_num < 10) {
        data_sample_num = nSamples;
    }

    m_trees[train_epoch].m_trainSamples = new int[data_sample_num];
    m_trees[train_epoch].m_nSamples = data_sample_num;
    int* ptr = m_trees[train_epoch].m_trainSamples;

    set<int> used_data_ids;
    int sampled_count = 0;
    while (sampled_count < data_sample_num) {
        int id = rand() % nSamples;
        if (used_data_ids.find(id) == used_data_ids.end())  // can't find the id
        {
            ptr[sampled_count] = id;
            sampled_count++;
            used_data_ids.insert(id);
        }
    }
    ///////////////////

    //----init first node for split----
    NodeReduced firstNode;
    firstNode.m_node = &(m_trees[train_epoch]);
    firstNode.m_size = data_sample_num;
    largestNodes.push_back(firstNode);
    push_heap(largestNodes.begin(), largestNodes.end(), compareNodeReduced);  // heap for select largest num node

    //----sample feature----
    int randFeatureIDs[m_feature_subspace_size];
    // this tmp array is used to fast drawing a random subset
    if (m_feature_subspace_size < data.m_dimension)  // select a random feature subset
    {
        for (unsigned int i = 0; i < m_feature_subspace_size; i++) {
            unsigned int idx = rand() % nFeatures;                                             //
            while (usedFeatures[idx] || (data.m_valid_id.find(idx) == data.m_valid_id.end()))  // TODO check valid num;
                idx = rand() % nFeatures;
            randFeatureIDs[i] = idx;
            usedFeatures[idx] = true;
        }
    } else { // take all features
        for (unsigned int i = 0; i < m_feature_subspace_size; i++) {
            randFeatureIDs[i] = i;
        }
    }

    //---- train the tree loop wise----
    // call trainSingleTree recursive for the largest node
    for (unsigned int j = 0; j < m_max_tree_leafes; j++) {
        Node* largestNode = largestNodes[0].m_node;

        TrainSingleTree(largestNode, largestNodes, data, usedFeatures, inputTmp, inputTargetsSort, sortIndex,
                        randFeatureIDs);
    }
    // unmark the selected inputs
    for (unsigned int i = 0; i < nFeatures; i++) {
        usedFeatures[i] = false;
    }

    //----delete the train lists per node, they are not necessary for prediction----
    cleanTree(&(m_trees[train_epoch]));

    // update the targets/residuals and calc train error
    double trainRMSE = 0.0;
    // fstream f("tmp/a0.txt",ios::out);
    for (int j = 0; j < nSamples; j++) {
        GBDTDType p = predictSingleTree(&(m_trees[train_epoch]), data, j);

        // f<<p<<endl;
        m_tree_target[j] -= m_lrate * p;
        double err = m_tree_target[j];
        trainRMSE += err * err;
    }
    rmse = sqrt(trainRMSE / (double)nSamples);
    if (verbose_) {
        cerr << "RMSE:" << rmse << " " << trainRMSE << " " << std::flush;
        cerr << "cost: " << Milliseconds() - t0 << "[ms]" << endl;
    }

    delete[] usedFeatures;
    delete[] inputTmp;
    delete[] inputTargetsSort;
    delete[] sortIndex;
    usedFeatures = NULL;
    inputTmp = NULL;
    inputTargetsSort = NULL;
    sortIndex = NULL;

    return true;
}

void GBDT::cleanTree(Node* n) {
    if (n->m_trainSamples) {
        delete[] n->m_trainSamples;
        n->m_trainSamples = 0;
    }
    n->m_nSamples = 0;

    if (n->m_toSmallerEqual) {
        cleanTree(n->m_toSmallerEqual);
    }
    if (n->m_toLarger) {
        cleanTree(n->m_toLarger);
    }
}

void GBDT::TrainSingleTree(Node* n, std::deque<NodeReduced>& largestNodes, const Data& data, bool* usedFeatures,
                           GBDTDType* inputTmp, GBDTDType* inputTargetsSort, int* sortIndex, const int* randFeatureIDs) {
    unsigned int nFeatures = data.m_dimension;

    // break criteria: tree size limit or too less training samples
    unsigned int nS = largestNodes.size();
    if (nS >= m_max_tree_leafes || n->m_nSamples <= 1) {
        return;
    }

    // delete the current node (is currently the largest element in the heap)
    if (largestNodes.size() > 0) {
        // largestNodes.pop_front();
        pop_heap(largestNodes.begin(), largestNodes.end(), compareNodeReduced);
        largestNodes.pop_back();
    }

    // the number of training samples in this node
    int nNodeSamples = n->m_nSamples;

    // precalc sums and squared sums of targets
    double sumTarget = 0.0, sum2Target = 0.0;
    for (int j = 0; j < nNodeSamples; j++) {
        GBDTDType v = m_tree_target[n->m_trainSamples[j]];
        sumTarget += v;
        sum2Target += v * v;
    }

    static constexpr double INF = 1e12;

    int bestFeature = -1;
    // int bestFeaturePos = -1;
    double bestFeatureRMSE = INF;
    // GBDTDType bestFeatureLow = INF, bestFeatureHi = INF;
    GBDTDType optFeatureSplitValue = INF;

    // TODO check m_feature_subspace_size not larger than nFeatures!!
    // search optimal split point in all tmp input features
    for (unsigned int i = 0; i < m_feature_subspace_size; i++) {
        // search the optimal split value, which reduce the RMSE the most
        GBDTDType optimalSplitValue = 0.0;
        double rmseBest = 1e10;
        // GBDTDType meanLowBest = INF, meanHiBest = INF;
        int bestPos = -1;
        double sumLow = 0.0, sum2Low = 0.0, sumHi = sumTarget, sum2Hi = sum2Target, cntLow = 0.0, cntHi = nNodeSamples;
        GBDTDType* ptrInput = inputTmp + i * nNodeSamples;
        GBDTDType* ptrTarget = inputTargetsSort + i * nNodeSamples;

        //  copy current feature into preInput
        int nr = randFeatureIDs[i];
        for (int j = 0; j < nNodeSamples; j++)
            ptrInput[j] = data.m_data[n->m_trainSamples[j]][nr];  // line :n->m_trainSamples[j] , row:nr

        if (m_use_opt_splitpoint == false)  // random threshold value
        {
            for (int j = 0; j < nNodeSamples; j++) {
                ptrTarget[j] = m_tree_target[n->m_trainSamples[j]];
            }

            GBDTDType* ptrInput = inputTmp + i * nNodeSamples;  // TODO: del ????
            bestPos = rand() % nNodeSamples;
            optimalSplitValue = ptrInput[bestPos];
            sumLow = 0.0;
            sum2Low = 0.0;
            cntLow = 0.0;
            sumHi = 0.0;
            sum2Hi = 0.0;
            cntHi = 0.0;
            for (int j = 0; j < nNodeSamples; j++) {
                // GBDTDType v = ptrInput[j];
                GBDTDType t = ptrTarget[j];
                if (ptrInput[j] <= optimalSplitValue) {
                    sumLow += t;
                    sum2Low += t * t;
                    cntLow += 1.0;
                } else {
                    sumHi += t;
                    sum2Hi += t * t;
                    cntHi += 1.0;
                }
            }
            rmseBest = (sum2Low / cntLow - (sumLow / cntLow) * (sumLow / cntLow)) * cntLow;
            rmseBest += (sum2Hi / cntHi - (sumHi / cntHi) * (sumHi / cntHi)) * cntHi;
            rmseBest = sqrt(rmseBest / (cntLow + cntHi));
            // meanLowBest = sumLow / cntLow;
            // meanHiBest = sumHi / cntHi;
        } else  // search for the optimal threshold value, goal: best RMSE reduction split
        {
            // fast sort of the input dimension
            for (int j = 0; j < nNodeSamples; j++) {
                sortIndex[j] = j;
            }

            std::vector<std::pair<GBDTDType, int>> list(nNodeSamples);
            for (int j = 0; j < nNodeSamples; j++) {
                list[j].first = ptrInput[j];
                list[j].second = sortIndex[j];
            }
            sort(list.begin(), list.end());
            for (int j = 0; j < nNodeSamples; j++) {
                ptrInput[j] = list[j].first;
                sortIndex[j] = list[j].second;
            }
            for (int j = 0; j < nNodeSamples; j++) {
                ptrTarget[j] = m_tree_target[n->m_trainSamples[sortIndex[j]]];
            }

            int j = 0;
            while (j < nNodeSamples - 1) {
                GBDTDType t = ptrTarget[j];
                sumLow += t;
                sum2Low += t * t;
                sumHi -= t;
                sum2Hi -= t * t;
                cntLow += 1.0;
                cntHi -= 1.0;

                GBDTDType v0 = ptrInput[j], v1 = INF;
                if (j < nNodeSamples - 1) v1 = ptrInput[j + 1];
                if (v0 == v1)  // skip equal successors
                {
                    j++;
                    continue;
                }
                // rmse
                double rmse = (sum2Low / cntLow - (sumLow / cntLow) * (sumLow / cntLow)) * cntLow;
                rmse += (sum2Hi / cntHi - (sumHi / cntHi) * (sumHi / cntHi)) * cntHi;
                rmse = sqrt(rmse / (cntLow + cntHi));

                if (rmse < rmseBest) {
                    optimalSplitValue = v0;
                    rmseBest = rmse;
                    bestPos = j + 1;
                    // meanLowBest = sumLow / cntLow;
                    // meanHiBest = sumHi / cntHi;
                }

                j++;
            }
        }

        if (rmseBest < bestFeatureRMSE) {
            bestFeature = i;
            // bestFeaturePos = bestPos;
            bestFeatureRMSE = rmseBest;
            optFeatureSplitValue = optimalSplitValue;
            // bestFeatureLow = meanLowBest;
            // bestFeatureHi = meanHiBest;
        }
    }
    // cerr << bestFeature << " rmse: " << bestFeatureRMSE << endl;

    assert(bestFeature != -1);
    n->m_featureNr = randFeatureIDs[bestFeature];
    n->m_value = optFeatureSplitValue;

    if (n->m_featureNr < 0 || n->m_featureNr >= (int)nFeatures) {
        if (verbose_) {
            cerr << "f=" << n->m_featureNr << endl;
        }
        assert(false);
    }

    // count the samples of the low node
    unsigned int cnt = 0;
    for (int i = 0; i < nNodeSamples; i++) {
        int nr = n->m_featureNr;
        if (data.m_data[n->m_trainSamples[i]][nr] <= optFeatureSplitValue) {
            cnt++;
        }
    }

    int* lowList = new int[cnt];
    int* hiList = new int[nNodeSamples - cnt];
    if (cnt == 0) {
        lowList = 0;
    }
    if (nNodeSamples - cnt == 0) {
        hiList = 0;
    }

    unsigned int lowCnt = 0;
    unsigned int hiCnt = 0;
    double lowMean = 0.0, hiMean = 0.0;
    unsigned int acnt = 0;
    double amean = 0;
    for (int i = 0; i < nNodeSamples; i++) {
        int nr = n->m_featureNr;
        acnt++;
        amean += m_tree_target[n->m_trainSamples[i]];
        if (data.m_data[n->m_trainSamples[i]][nr] <= optFeatureSplitValue) {
            lowList[lowCnt] = n->m_trainSamples[i];
            lowMean += m_tree_target[n->m_trainSamples[i]];
            lowCnt++;
        } else {
            hiList[hiCnt] = n->m_trainSamples[i];
            hiMean += m_tree_target[n->m_trainSamples[i]];
            hiCnt++;
        }
    }
    lowMean /= lowCnt;
    hiMean /= hiCnt;
    amean /= acnt;

    if (static_cast<int>(hiCnt + lowCnt) != nNodeSamples || lowCnt != cnt) {
        assert(false);
    }
    ///////////////////////////

    // break, if too less samples
    if (lowCnt < m_min_samples || hiCnt < m_min_samples) {
        n->m_featureNr = -1;
        n->m_value = amean;
        double mul = sqrt(static_cast<double>(acnt) / (acnt + m_norm_samples));
        n->m_value *= mul;
        n->m_toSmallerEqual = 0;
        n->m_toLarger = 0;
        if (n->m_trainSamples) delete[] n->m_trainSamples;
        n->m_trainSamples = 0;
        n->m_nSamples = 0;

        NodeReduced currentNode;
        currentNode.m_node = n;
        currentNode.m_size = 0;
        largestNodes.push_back(currentNode);
        push_heap(largestNodes.begin(), largestNodes.end(), compareNodeReduced);

        return;
    }

    // prepare first new node
    n->m_toSmallerEqual = new Node();
    n->m_toSmallerEqual->m_featureNr = -1;
    n->m_toSmallerEqual->m_value = lowMean;
    n->m_toSmallerEqual->m_toSmallerEqual = 0;
    n->m_toSmallerEqual->m_toLarger = 0;
    n->m_toSmallerEqual->m_trainSamples = lowList;
    n->m_toSmallerEqual->m_nSamples = lowCnt;

    // prepare second new node
    n->m_toLarger = new Node();
    n->m_toLarger->m_featureNr = -1;
    n->m_toLarger->m_value = hiMean;
    n->m_toLarger->m_toSmallerEqual = 0;
    n->m_toLarger->m_toLarger = 0;
    n->m_toLarger->m_trainSamples = hiList;
    n->m_toLarger->m_nSamples = hiCnt;

    // add the new two nodes to the heap
    NodeReduced lowNode, hiNode;
    lowNode.m_node = n->m_toSmallerEqual;
    lowNode.m_size = lowCnt;
    hiNode.m_node = n->m_toLarger;
    hiNode.m_size = hiCnt;

    largestNodes.push_back(lowNode);
    push_heap(largestNodes.begin(), largestNodes.end(), compareNodeReduced);

    largestNodes.push_back(hiNode);
    push_heap(largestNodes.begin(), largestNodes.end(), compareNodeReduced);
}

GBDTDType GBDT::predictSingleTree(Node* n, const Data& data, int data_index) {
    int nFeatures = data.m_dimension;
    int nr = n->m_featureNr;
    if (nr < -1 || nr >= nFeatures) {
        if (verbose_) {
            cerr << "Feature nr:" << nr << endl;
        }
        assert(false);
    }

    // here, on a leaf: predict the constant value
    if (n->m_toSmallerEqual == 0 && n->m_toLarger == 0) {
        return n->m_value;
    }

    // TODO : del duplicate check
    if (nr < 0 || nr >= nFeatures) {
        if (verbose_) {
            cerr << endl << "Feature nr: " << nr << " (max:" << nFeatures << ")" << endl;
        }
        assert(false);
    }
    GBDTDType thresh = n->m_value;
    GBDTDType feature = data.m_data[data_index][nr];

    if (feature <= thresh) {
        return predictSingleTree(n->m_toSmallerEqual, data, data_index);
    }
    return predictSingleTree(n->m_toLarger, data, data_index);
}

void GBDT::PredictAllOutputs(const Data& data, GBDTVectorType& predictions) {
    unsigned int nSamples = data.m_num;
    predictions.resize(nSamples);

    // predict all values
    for (unsigned int i = 0; i < nSamples; i++) {
        double sum = m_global_mean;
        // for every boosting epoch : CORRECT, but slower
        for (unsigned int k = 0; k < m_train_epoch + 1; k++) {
            GBDTDType v = predictSingleTree(&(m_trees[k]), data, i);
            sum += m_lrate * v;  // this is gradient boosting
        }
        predictions[i] = sum;
    }
}

DoubleVector GBDT::regress(const DoubleMatrix& m) {
    Data dt;
    dt.m_data = m;
    dt.m_dimension = m[0].size();
    dt.m_num = m.size();
    DoubleVector result;
    PredictAllOutputs(dt, result);
    return result;
}

string GBDT::explain(const StringVector& columnNames) const {
    string result;

    for (unsigned int j = 0; j < m_train_epoch + 1; j++) {
        result += explainTreeRecursive(&(m_trees[j]), columnNames, 0);
        result += "\n";
    }

    return result;
}

void GBDT::SaveWeights(const std::string& model_file) {
    if (verbose_) {
        cerr << "Save:" << model_file << endl;
    }
    std::fstream f(model_file.c_str(), std::ios::out);

    // save learnrate
    f.write((char*)&m_lrate, sizeof(m_lrate));

    // save number of epochs
    f.write((char*)&m_train_epoch, sizeof(m_train_epoch));

    // save global means
    f.write((char*)&m_global_mean, sizeof(m_global_mean));

    // save trees
    for (unsigned int j = 0; j < m_train_epoch + 1; j++) {
        SaveTreeRecursive(&(m_trees[j]), f);
    }

    if (verbose_) {
        cerr << "debug: train_epoch: " << m_train_epoch << endl;
    }
    f.close();
}

void GBDT::SaveTreeRecursive(Node* n, std::fstream& f) {
    // cerr << "debug_save: " << n->m_value << " " << n->m_featureNr << endl;
    f.write((char*)n, sizeof(Node));
    if (n->m_toSmallerEqual) SaveTreeRecursive(n->m_toSmallerEqual, f);
    if (n->m_toLarger) SaveTreeRecursive(n->m_toLarger, f);
}

string GBDT::explainTreeRecursive(Node* n, const StringVector& columnNames, size_t indent) const {
    if (!n) {
        return "";
    }

    string sep = rep(" ", indent);
    if (n->m_toSmallerEqual && n->m_toLarger) {
        string result = stringSprintf("%s[%s < %lf]:\n", sep.c_str(), columnNames[n->m_featureNr].c_str(), n->m_value);
        result += explainTreeRecursive(n->m_toSmallerEqual, columnNames, indent + 2);
        result += sep;
        result += "else:\n";
        result += explainTreeRecursive(n->m_toLarger, columnNames, indent + 2);
        return result;
    } else {
        return stringSprintf("%s%lf\n", sep.c_str(), n->m_value);
    }
}

void GBDT::LoadWeights(const std::string& model_file) {
    if (verbose_) {
        cerr << "Load:" << model_file << endl;
    }
    std::fstream f(model_file.c_str(), std::ios::in);
    if (!f.is_open()) {
        if (verbose_) {
            cerr << "Load " << model_file << "failed!" << endl;
        }
        THROW("Cannot load weights from: " << model_file);
    }

    // load learnrate
    f.read((char*)&m_lrate, sizeof(m_lrate));

    // load number of epochs
    f.read((char*)&m_train_epoch, sizeof(m_train_epoch));

    // load global means
    f.read((char*)&m_global_mean, sizeof(m_global_mean));

    // allocate and load the trees
    m_trees = new Node[m_train_epoch + 1];
    for (unsigned int j = 0; j < m_train_epoch + 1; j++) {
        std::string prefix = "";
        LoadTreeRecursive(&(m_trees[j]), f, prefix);
    }

    f.close();
}

void GBDT::LoadTreeRecursive(Node* n, std::fstream& f, std::string prefix) {
    f.read((char*)n, sizeof(Node));

    // cerr << prefix;
    // cerr << "debug_load: " << n->m_value << " " << n->m_featureNr << endl;
    if (n->m_toLarger == 0 && n->m_toSmallerEqual == 0) {
        assert(n->m_featureNr == -1);
    }
    prefix += "    ";
    if (n->m_toSmallerEqual) {
        n->m_toSmallerEqual = new Node();
        LoadTreeRecursive(n->m_toSmallerEqual, f, prefix);
    }
    if (n->m_toLarger) {
        n->m_toLarger = new Node();
        LoadTreeRecursive(n->m_toLarger, f, prefix);
    }
}
