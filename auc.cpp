//
// Created by liyongbao on 15-8-5.
//

#ifndef FTRL_TRAIN_AUC_H
#define FTRL_TRAIN_AUC_H
#include "fstream"
#include "src/Utils/utils.h"
#include "algorithm"
#include "iostream"


void readFile(std::vector<int>& labels, std::vector<double>& probs){
    std::string line;
    std::vector<std::string> sp;
    while(std::getline(std::cin, line)){
        sp.clear();
        utils::splitString(line, ' ', &sp);
        if(std::stoi(sp[0]) <= 0)
            labels.push_back(0);
        else
            labels.push_back(1);
        probs.push_back(std::stof(sp[1]));
    }
}

double scoreAuc(std::vector<int>& labels, std::vector<double>& probs){
    std::vector<int> range;
    range.reserve(probs.size());
    for(int i=0;i< probs.size();i++){
        range.push_back(i);
    }
    std::sort(range.begin(),range.end(),[&probs](int i1, int i2){return probs[i1] > probs[i2];});
    long double aucTemp = 0.0, tp = 0.0, tpPre = 0.0, fp = 0.0, fpPre = 0.0;
    unsigned long long p = 0, n = 0;
    double last_prob = probs[range[0]] + 1.0;
    for(int i =0; i < probs.size(); i++){
        if(last_prob != probs[range[i]]){
            aucTemp += (tp + tpPre) * (fp - fpPre) / 2.0;
            tpPre = tp;
            fpPre = fp;
            last_prob = probs[range[i]];
        }
        if(labels[range[i]] == 1)
            tp++;
        else
            fp ++;
    }

    aucTemp += (tp + tpPre) * (fp - fpPre) / 2.0;
    double auc = aucTemp / tp / fp;
    return auc;
}

int main(int argc,char* argv[]) {
    std::vector<int> labels;
    std::vector<double> probs;
    readFile(labels, probs);
    double auc = scoreAuc(labels, probs);
    std::cout << "auc:" << auc << std::endl;
}
#endif //FTRL_TRAIN_AUC_H
