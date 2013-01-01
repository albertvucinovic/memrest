#ifndef SAMPLE_H
#define SAMPLE_H

#include <vector>
using std::vector;

template<class T> 
class Sample{
  public:
    vector<T> features;
    T prediction;
    Sample(){}
    Sample(vector<T> features, T prediction):
      features(features),
      prediction(prediction){}
    ~Sample(){
    }
};

#endif //SAMPLE_H
