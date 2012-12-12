#ifndef SPLIT_H
#define SPLIT_H

#include <sample.h>

#include <memory>

template<class T>
struct Split{
  T threshold;
  int feature;

  vector<shared_ptr<Sample<T>>> left, right;

  T score;

  Split(T threshold, int feature, 
    vector<shared_ptr<Sample<T>>> left, 
    vector<shared_ptr<Sample<T>>> right, 
    T score):
      threshold(threshold),
      feature(feature),
      left(left),
      right(right),
      score(score)
    {}
};

#endif
