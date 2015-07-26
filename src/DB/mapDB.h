//
// Created by liyongbao on 15-7-22.
//

#ifndef FTRL_MAPDB_H
#define FTRL_MAPDB_H
#include<vector>
#include<string>
#include<unordered_map>
#include "../Unit/modelUnit.h"
#include "virtualDB.h"
#include "../Utils/utils.h"
class MapDB: public VirtualDB{
public:
    ModelUnit* getOrInitDB(std::string k){
        std::unordered_map<std::string, ModelUnit*>::iterator iterator = vMap.find(k);
        if (iterator == vMap.end()) {
            mtx.lock();
            ModelUnit* modelUnit = new ModelUnit();
            vMap.insert(make_pair(k, modelUnit));
            mtx.unlock();
            return modelUnit;
        }
        else {
            return iterator->second;
        }
    }

    double get(std::string k){
        std::unordered_map<std::string, double>::iterator iterator = vWeight.find(k);
        if (iterator == vWeight.end())
            return 0;
        else
            return iterator->second;
    }

    void print(std::ofstream& out){
        for (std::unordered_map<std::string, ModelUnit*>::iterator it = vMap.begin(); it != vMap.end(); ++it) {
            out << it->first << "\t" << it->second->w <<"\t"<<it->second->g <<"\t"<<it->second->s <<"\t"<< it->second->z <<"\t"<< it->second->n <<std::endl;
        }
    }
    bool loadModel(std::ifstream& in){
        std::string line;
        double fp=0;
        std::vector<std::string> r;
        std::vector<double> dList;
        while(getline(in,line)){
            r.clear();
            dList.clear();
            utils::splitString(line, '\t', &r);
            if (r.size() != 6) // key,W,G,S,Z,N
            {
                std::cout<<line<<std::endl;
                return false;
            }
            std::string key=r[0];
            for(int i=1;i<6;i++){
                dList.push_back(std::stod(r[i]));
            }
            ModelUnit *modelUnit= new ModelUnit(dList[0], dList[1], dList[2], dList[3], dList[4]);
            vMap.insert(std::make_pair(key,modelUnit));
        }
        return true;
    }

    bool loadNonZeroWeight(std::ifstream& in){
        std::string line;
        double fp=0;
        std::vector<std::string> r;
        while(getline(in,line)){
            r.clear();
            utils::splitString(line, '\t', &r);
            if (r.size() < 2) // key,W,G,S,Z,N
            {
                std::cout<<line<<std::endl;
                return false;
            }
            if(fabs(std::stod(r[1])) > smallDouble)
                vWeight.insert(std::make_pair(r[0], std::stod(r[1])));
        }
        return true;
    }

    MapDB(){}
private:
    std::unordered_map<std::string, ModelUnit*> vMap;
    std::unordered_map<std::string, double> vWeight;
    std::mutex mtx;
};
#endif //FTRL_MAPDB_H
