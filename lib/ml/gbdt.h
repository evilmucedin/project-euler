#pragma once

#include <deque>
#include <string>
#include <vector>
#include <set>

#include "lib/header.h"
#include "lib/ml/regressor.h"

using T_DTYPE = double;
using T_VECTOR = std::vector<T_DTYPE>;
using T_MATRIX = std::vector<T_VECTOR>;

struct Node {
    int m_featureNr;                 // decision on this feature
    T_DTYPE m_value;                 // the prediction value
    Node* m_toSmallerEqual;  // pointer to node, if:  feature[m_featureNr] <=  m_value
    Node* m_toLarger;        // pointer to node, if:  feature[m_featureNr] > m_value
    int* m_trainSamples;             // a list of indices of the training samples in this node
    int m_nSamples;                  // the length of m_trainSamples
};

struct NodeReduced {
    Node* m_node;
    uint m_size;
};

class Data {
   public:
    Data() {}

    ~Data() {}

   public:
    T_MATRIX m_data;
    T_VECTOR m_target;

    std::set<int> m_valid_id;

    unsigned int m_dimension;
    unsigned int m_num;

   private:
};  // end of class Data

class DataReader {
   public:
    DataReader() {}
    ~DataReader() {}

    bool ReadDataFromL2R(const std::string& input_file, Data& data, unsigned int dimentions);

    bool ReadDataFromCVS(const std::string& input_file, Data& data);

   private:
};  // end of class DataReader

class GBDT : public IRegressor {
   public:
    GBDT();

    ~GBDT() {}

    bool Init();

    bool Train(const Data& data);
    void fit(const DoubleMatrix& x, const DoubleVector& y) override;

    void PredictAllOutputs(const Data& data, T_VECTOR& predictions);
    DoubleVector regress(const DoubleMatrix& m) override;

    string explain(const StringVector& columnNames) const;

    void SaveWeights(const std::string& model_file);
    void LoadWeights(const std::string& model_file);
    bool LoadConfig(const std::string& conf_file);

    GBDT& setMaxEpochs(unsigned int max_epochs);
    GBDT& setLRate(double lrate);

   private:
    bool ModelUpdate(const Data& data, unsigned int train_epoch, double& rmse);

    void TrainSingleTree(Node* n, std::deque<NodeReduced>& largestNodes, const Data& data, bool* usedFeatures,
                         T_DTYPE* inputTmp, T_DTYPE* inputTargetsSort, int* sortIndex, const int* randFeatureIDs);

    T_DTYPE predictSingleTree(Node* n, const Data& data, int data_index);

    void cleanTree(Node* n);

    void SaveTreeRecursive(Node* n, std::fstream& f);
    void LoadTreeRecursive(Node* n, std::fstream& f, std::string prefix);
    string explainTreeRecursive(Node* n, const StringVector& columnNames, size_t indent) const;

   private:
    Node* m_trees;
    unsigned int m_max_epochs;
    unsigned int m_max_tree_leafes;
    unsigned int m_feature_subspace_size;
    bool m_use_opt_splitpoint;
    double m_lrate;
    unsigned int m_train_epoch;
    float m_data_sample_ratio;
    unsigned int m_min_samples;
    unsigned int m_norm_samples;

    T_VECTOR m_tree_target;

    T_DTYPE m_global_mean;

};  // end of class GBDT
