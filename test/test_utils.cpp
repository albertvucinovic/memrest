#include <iostream>
#include <utils.h>
#include <vector>
#include <cassert>
#include <memory>
#include <sample.h>
#include <decision_tree_node.h>
#include <map>
using namespace std;

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


int main(void){

  cout<<"Ovo je proba!"<<endl;

  vector<float> a {3,1,4,1,5,9.2,2,6,5,4};
  vector<int> b {0,2,5};

  vector<float> c=subsample(a,b);
  vector<float> d {3,4,9.2};
  assert(d==c);

  shared_ptr<Sample<float>> s(new Sample<float>(new vector<float> {1,2,3}, 5));
  
  print(*(s->features));
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


  //DecisionTreeNode<float> n(100,10,10,20,10);

  shared_ptr<float> z(new float[5]);
  float* pf=z.get();
  *(pf+2)=3;
  for(int i=0;i<5;i++){
    cout<<pf[i]<<endl;
  }

  map<float,int> m;
  m[2.]++;
  cout<<"m[2.]="<<m[2.]<<endl;
  


  print(c);
  
}
