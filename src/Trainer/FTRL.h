//
// Created by liyongbao on 15-7-22.
//

#ifndef FTRL_FTRL_H
#define FTRL_FTRL_H
#include <set>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <semaphore.h>
#include "../Utils/utils.h"
#include "../Unit/modelUnit.h"
#include "../DB/vectorDB.h"
#include "../Unit/entityUnit.h"
#include "../DB/mapDB.h"

class FTRL{
public:
    //  用作train时的初始化函数
    FTRL(double a, double b, double c, double d, int t_num, int model_size)
            :alpha(a),beta(b),lambda1(c),lambda2(d), threadNum(t_num){
        //WGSZN = new VectorDB(model_size);
        WGSZN = new MapDB();
    };
    // 用作test时的初始化函数
    FTRL(int t_num, std::string f_out):threadNum(t_num){
        //WGSZN = new VectorDB();
        WGSZN = new MapDB();
        fPredict.open(f_out,std::ofstream::out);
    };
    bool init(int type);
    void run(int type);
    void printW(std::ofstream& out);
    bool loadModel(std::ifstream& fModel);
    bool loadNonZeroWeight(std::ifstream& fModel);

private:
    double alpha;
    double beta;
    double lambda1;
    double lambda2;
    int threadNum;
    int type;
    std::ofstream fPredict;
    std::mutex queueMtx, outMtx;
    sem_t sem, semPro;
    std::queue<std::string> lineQueue;
    std::vector<std::thread> threadVec;
    VirtualDB* WGSZN;
    void parseLineToEntity(const std::string& line, EntityUnit *unit);
    double predict(const std::vector<std::pair<std::string, double> >& fea);
    void train(const std::vector<std::pair<std::string, double> >& fea, int label);
    void trainThread();
    void predictThread();
    void inputThread();
};


#endif //FTRL_FTRL_H
