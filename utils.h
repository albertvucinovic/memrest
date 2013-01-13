#ifndef UTILS_H
#define UTILS_H

#define DEBUG_LEVEL 0
#define BOOST_MEM
#define NUMBER_OF_THREADS 2

#define TYPE "float"

#if DEBUG_LEVEL>0
#define DEBUG1(T) T;
#else
#define DEBUG1(T) ;
#endif


#include <vector>
#include <iostream>
#include <map>
#include <utility>
using std::vector;
using std::map;
using std::cout;
using std::endl;
using std::pair;
#include <string>
using std::string;

namespace utils{
template<class T>
vector<T> subsample(vector<T>& v, vector<int>& indices){
  vector<T> temp;
  for(vector<int>::iterator i=indices.begin();i!=indices.end();i++){
    temp.push_back(v[*i]);
  }
  return temp;
}

template<typename T>
void print(vector<T>& v){
  cout<<"[";
  for(typename vector<T>::iterator i=v.begin();i!=v.end();i++){
    cout<<*i<<",";
  }
  cout<<"]"<<endl;
}

template<typename T>
map<T,int> count_map(vector<T>& v){
  map<T, int> counts;
  for(auto i=v.begin();i!=v.end();i++){
    //we are presuming that the default constructor for int sets the int to 0
    counts[*i]++;
  }
  return counts;
}

template<typename T>
T gini(vector<T>& v){
  map<T,int> counts=count_map(v);
  float total=(float)v.size();
  T res=1.;
  for(auto i=counts.begin();i!=counts.end();i++){
    T t=i->second/total;
    res-=t*t;
  }
  return res;
}

template<typename T>
T argmax(map<T,int>& counts){
  int max_count=0;
  T max_element;
  for(auto i=counts.begin();i!=counts.end();i++){
    if(i->second>max_count){
      max_count=i->second;
      max_element=i->first;
    }
  }
  return max_element;
}

template<typename T>
pair<T,int> argmax(vector<T>& v){
  int i=0;
  T m=*(v.begin());
  int s=v.size();
  for(int k=0;k<s;k++){
    if(v[k]>m){
      m=v[k];
      i=k;
    }
  }
  return pair<T,int>(m, i);
}


template<typename T> 
T argmaxcount(vector<T>& v){
  map<T,int>counts=count_map(v);
  return argmax<T>(counts);
}


template<typename T>
T count(vector<T> &v, T el){
  T c=0;
  for(auto i=v.begin();i!=v.end();i++){
    if(*i==el) c+=1;
  }
  return c;
}


template<class T>
vector<T> derivate(vector<T> v){
  vector<T> temp;
  for(int i=1;i<v.size();i++){
    temp.push_back(v[i]-v[i-1]);
  }
  return temp;
}


void test(void (*f)(), string name){
  cout<<"Testing "<<name<<endl;
  f();
  cout<<"Testing "<<name<<" finished."<<endl<<endl;
}

}

#endif//UTILS_H
