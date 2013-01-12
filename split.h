#ifndef SPLIT_H
#define SPLIT_H

#include <sample.h>
#include <memory>

#ifdef BOOST_MEM
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
#else
using std::shared_ptr;
#endif

using std::vector;

template<class T>
struct Split{
  T threshold;
  int feature;

  shared_ptr<vector<shared_ptr<Sample<T>>>> left, right;

  T score;

  Split(T threshold, int feature, 
    shared_ptr<vector<shared_ptr<Sample<T>>>> left, 
    shared_ptr<vector<shared_ptr<Sample<T>>>> right, 
    T score):
      threshold(threshold),
      feature(feature),
      left(left),
      right(right),
      score(score)
    {}
};

#endif
