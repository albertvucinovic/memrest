#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <iostream>
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

#endif//UTILS_H
