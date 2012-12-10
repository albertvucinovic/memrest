#ifndef SAMPLE_H
#define SAMPLE_H

#include <vector>
#include <iostream>
using namespace std;
template<class T> 
class Sample{
  public:
    vector<T> features;
    T prediction;
    Sample(vector<T> features, T prediction):
      features(features),
      prediction(prediction){}
    ~Sample(){
    }
};

#endif //SAMPLE_H
