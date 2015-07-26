//
// Created by liyongbao on 15-7-22.
//

#include "FTRL.h"
using std::pair;
const std::string spliter=" ";
const std::string innerSpliter =":";
const int cap = 5000;
const int log_num =200000;
bool FTRL::init(int type){
    sem_init(&sem,0,0);
    sem_init(&semPro,0,1);
    this->type = type;
    threadVec.clear();
    threadVec.push_back(std::thread(&FTRL::inputThread,this));
    for(int i=0;i< threadNum;i++){
        if(type == 0){
            threadVec.push_back(std::thread(&FTRL::predictThread,this));
        }
        else{
            threadVec.push_back(std::thread(&FTRL::trainThread,this));
        }
    }
    return true;
}


void FTRL::run(int type){
    if(init(type)){
        for(int i=0;i< threadVec.size();i++){
            threadVec[i].join();
        }
    }
}

void FTRL::inputThread(){
    std::string line;
    int line_num = 0;
    int i=0;
    bool finished_flag = false;
    while(true){
        sem_wait(&semPro);
        queueMtx.lock();
        for(i=0;i<cap;i++){
            if(!getline(std::cin,line)){
                finished_flag = true;
                break;
            }
            line_num++;
            lineQueue.push(line);
            if(line_num%log_num==0)
            {
                if(type == 1)
                    std::cout<<line_num<<" line has finished" <<std::endl;
            }
        }
        queueMtx.unlock();
        sem_post(&sem);
        if(finished_flag)
            break;
    }
}


void FTRL::trainThread(){
    bool finished_flag = false;
    EntityUnit* entity=new EntityUnit();
    std::vector<std::string> input_vec;
    input_vec.reserve(cap);
    while(true){
        input_vec.clear();
        sem_wait(&sem);
        queueMtx.lock();
        for(int i=0;i<cap;i++){
            if(lineQueue.empty()){
                finished_flag = true;
                break;
            }
            input_vec.push_back(lineQueue.front());
            lineQueue.pop();
        }
        queueMtx.unlock();
        sem_post(&semPro);
        for(int i=0;i<input_vec.size();i++){
            parseLineToEntity(input_vec[i], entity);
            train(entity->feature,entity->label);
        }
        if(finished_flag)
            break;
    }
    sem_post(&sem);
    queueMtx.unlock();
    delete entity;
}

void FTRL::predictThread(){
    bool finished_flag = false;
    EntityUnit* entity=new EntityUnit();
    std::vector<std::string> input_vec;
    std::vector<std::string> output_vec;
    input_vec.reserve(cap);
    output_vec.reserve(cap);
    while(true){
        input_vec.clear();
        output_vec.clear();
        sem_wait(&sem);
        queueMtx.lock();
        for(int i=0;i<cap;i++){
            if(lineQueue.empty()){
                finished_flag = true;
                break;
            }
            input_vec.push_back(lineQueue.front());
            lineQueue.pop();
        }
        queueMtx.unlock();
        sem_post(&semPro);
        for(int i=0;i<input_vec.size();i++){
            parseLineToEntity(input_vec[i], entity);
            double p = predict(entity->feature);
            output_vec.push_back(std::to_string(entity->label) + " " + std::to_string(p));
        }
        outMtx.lock();
        for(int i=0;i<output_vec.size();i++){
            fPredict << output_vec[i] <<std::endl;
        }
        outMtx.unlock();
        if(finished_flag)
            break;
    }
    sem_post(&sem);
    queueMtx.unlock();
    delete entity;
}

void FTRL::train(const std::vector<pair<std::string, double> >& fea, int label) {
    std::vector<ModelUnit*> tempvec(fea.size(),NULL);
    double p = 0.0;
    for (int i = 0; i < fea.size(); ++i) {
        const std::string& index = fea[i].first;
        tempvec[i] = WGSZN->getOrInitDB(index);
        ModelUnit& modelUnit = *(tempvec[i]);
        if(abs(modelUnit.z.load()) <= lambda1) {
            modelUnit.w.store(0.0);
        } else {
            modelUnit.w.store((-1) *
                              (1 / (lambda2 + (beta + sqrt(modelUnit.n.load())) / alpha)) *
                              (modelUnit.z.load() - utils::sgn(modelUnit.z.load()) * lambda1));
        }
        p += modelUnit.w.load() * fea[i].second;
    }
    for (int i = 0; i < fea.size(); ++i) {
        ModelUnit& modelUnit = *(tempvec[i]);
        modelUnit.g.store((p-label) * (fea[i].second));
        modelUnit.s.store(1 / alpha * (sqrt(modelUnit.n.load() + modelUnit.g.load() * modelUnit.g.load()) - sqrt(modelUnit.n.load())));
        modelUnit.z.store(modelUnit.z.load() + modelUnit.g.load() - modelUnit.s.load() * modelUnit.w.load());
        modelUnit.n.store(modelUnit.n.load() + modelUnit.g.load() * modelUnit.g.load());
    }
}


double FTRL::predict(const std::vector<pair<std::string, double> >& fea) {
    double res = 0.0;
    for (int i = 0; i != fea.size(); ++i) {
        res += fea[i].second * (WGSZN->get(fea[i].first));
        //res += fea[i].second * (WGSZN->getOrInitDB(fea[i].first)->w);
    }
    return utils::sigmoid(res);
}

void FTRL::parseLineToEntity(const std::string& line, EntityUnit *entity) {
    entity->feature.clear();
    std::size_t posb=line.find_first_not_of(spliter,0);
    std::size_t pose=line.find_first_of(spliter,posb);
    int label=atoi(line.substr(posb,pose-posb).c_str());
    entity->label=label>0?1:0;
    std::string key;
    double value;
    while(pose<line.size()){
        posb=line.find_first_not_of(spliter,pose);
        if(posb==std::string::npos)
            break;
        pose=line.find_first_of(innerSpliter,posb);
        if(pose==std::string::npos)
            throw 404;
        key=line.substr(posb,pose-posb);
        posb=pose+1;
        if(posb >= line.size())
            throw 404;
        pose=line.find_first_of(spliter,posb);
        value=std::stod(line.substr(posb,pose-posb));
        if(value!=0)
            entity->feature.push_back(std::make_pair(key,value));
    }
}

void FTRL::printW(std::ofstream& out) {
    WGSZN->print(out);
}

bool FTRL::loadModel(std::ifstream& fModel){
    return WGSZN->loadModel(fModel);
}
bool FTRL::loadNonZeroWeight(std::ifstream& fModel){
    return WGSZN->loadNonZeroWeight(fModel);
}
