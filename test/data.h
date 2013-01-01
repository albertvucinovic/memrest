#ifndef DATA_H
#define DATA_H

#include <sample.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>
using std::vector;
using std::ifstream;
using std::map;
using std::pair;
using std::ios;
using std::istringstream;

template<class T>
vector<shared_ptr<Sample<T>>> read_svm_data(string filename){
  ifstream f(filename.c_str(), ios::binary);
  string line;
  T prediction;
  vector<pair<map<T,T>,T>> rows;
  T max_feature=0;
  while(f){
    getline(f, line);
    istringstream iss(line);
    string temp;
    iss>>prediction;
    map<T,T> row;
    do{
      iss>>temp;
      istringstream iss2(temp);
      T feature, value;
      char column;
      iss2>>feature;
      iss2>>column;
      iss2>>value;
      if(feature>max_feature){
        max_feature=feature;
      }
      row[feature]=value;

    } while(iss);
    rows.push_back(pair<map<T,T>,T>(row, prediction));
  }
  vector<shared_ptr<Sample<T>>> result;
  for(auto i=rows.begin();i!=rows.end();i++){
    vector<T> features(max_feature+1);
    for(auto j=(*i).first.begin();j!=(*i).first.end();j++){
      features[(int)(*j).first]=(*j).second;
    }
    result.push_back(shared_ptr<Sample<T>>(new Sample<T>(features, (*i).second)));
  }
  return result;
}


#endif
