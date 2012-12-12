#include <iostream>
#include <utils.h>
#include <vector>
#include <cassert>
#include <memory>
#include <sample.h>
#include <classification_tree_node.h>
#include <map>
using namespace std;

void test(void (*f)(), string name){
  cout<<"Testing "<<name<<endl;
  f();
  cout<<"Testing "<<name<<" finished."<<endl<<endl;
}


class Proba{
public:
  Proba(){}
  ~Proba(){
    cout<<"Unisten"<<endl;
  }
};

void f(){
  unique_ptr<Proba> p(new Proba);
  p.reset();
  p.reset();
}

void test_shared_pointer(){
  shared_ptr<Sample<float>> s(new Sample<float>(vector<float> {1,2,3}, 5));
  
  utils::print(s->features);
  cout<<"Prediction is: "<<s->prediction<<endl;


  shared_ptr<Sample<float>> sp(s);
  cout<<"1"<<endl;
  s.reset();

  f();

  shared_ptr<Sample<float>> sp1=sp;
  cout<<"3"<<endl;
  sp.reset();
  cout<<"4"<<endl;
  sp1.reset();
  cout<<"5"<<endl;


  unique_ptr<Proba> p;
  p.reset(new Proba());
  p.reset();



  shared_ptr<float> z(new float[5]);
  float* pf=z.get();
  *(pf+2)=3;
  for(int i=0;i<5;i++){
    cout<<pf[i]<<endl;
  }

}

void test_subsample(){
  vector<float> a {3,1,4,1,5,9.2,2,6,5,4};
  vector<int> b {0,2,5};

  vector<float> c=utils::subsample(a,b);
  vector<float> d {3,4,9.2};
  assert(d==c);

}

void test_map(){
  map<float,int> m;
  m[2.]++;
  cout<<"m[2.]="<<m[2.]<<endl;
}

void test_gini(){
  vector<float> v {1,2,2,2};
  float g=utils::gini(v);
  cout<<"Gini is "<<g<<endl;
  assert(g==0.375);
}

void test_argmaxcount(){
  vector<float> v{1,2,3,2};
  assert(utils::argmaxcount<float>(v)==2);
}

void test_classification_tree_node(){

  //DecisionTreeNode<float> n(100,10,10,20,10);
  vector<shared_ptr<Sample<float>>> initial_samples;
  ClassificationTreeNode<float> n(5,2,3,6,3,initial_samples);
  vector<float> t{1,2,3,4,5};
  float p=n.predict(t).first;
  cout<<p<<endl;
  assert(p!=p);//testing for nan

  shared_ptr<Sample<float>> s(new Sample<float>(t, 5));
  n.update(s);
  p=n.predict(t).first;
  cout<<p<<endl;
  assert(p==5);
  n.update(s);

  vector<float> t1{1,3,3,4,5};
  assert(n.predict(t1).first==5);
  shared_ptr<Sample<float>> s1(new Sample<float>(t, 3));
  n.update(s1);
  assert(n.predict(t1).first==5);
  n.update(s1);
  n.update(s1);
  assert(n.predict(t1).first==3);



}






int main(void){

  test(&test_subsample, "subsample");

  test(&test_shared_pointer, "shared pointer");

  test(&test_map, "map");

  test(&test_gini, "gini");

  test(&test_argmaxcount, "argmaxcount");

  test(&test_classification_tree_node, "classification tree node");
  
}
