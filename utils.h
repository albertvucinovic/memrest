#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <iostream>
#include <map>
using namespace std;

template<class T>
vector<T> subsample(vector<T> v, vector<int> indices){
  vector<T> temp;
  for(vector<int>::iterator i=indices.begin();i!=indices.end();i++){
    temp.push_back(v[*i]);
  }
  return temp;
}

template<typename T>
void print(vector<T> v){
  cout<<"[";
  for(typename vector<T>::iterator i=v.begin();i!=v.end();i++){
    cout<<*i<<",";
  }
  cout<<"]"<<endl;
}

template<typename T>
T gini(vector<T> v){
  map<T,int> counts;
  float total=(float)v.size();
  for(auto i=v.begin();i!=v.end();i++){
    //we are presuming that the default constructor for int sets the int to 0
    counts[*i]++;
  }
  T res=1.;
  for(auto i=counts.begin();i!=counts.end();i++){
    T t=i->second/total;
    res-=t*t;
  }
  return res;
}

#endif//UTILS_H
