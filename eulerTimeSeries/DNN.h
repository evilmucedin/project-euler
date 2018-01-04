#pragma once

#include "lib/header.h"

class DNNModel {
public:
    DNNModel();

private:
    class Impl;
    unique_ptr<Impl> impl_;
};

using PDNNModel = shared_ptr<DNNModel>;

struct DNNModelTrainer {
public:
    DNNModelTrainer();
    PDNNModel getModel();

private:
    class Impl;
    unique_ptr<Impl> impl_;
};
