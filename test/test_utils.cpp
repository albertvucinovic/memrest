#include <iostream>
#include <utils.h>
#include <vector>
#include <cassert>
using namespace std;
int main(void){

  cout<<"Ovo je proba!"<<endl;

  vector<float> a {3,1,4,1,5,9.2,2,6,5,4};
  vector<int> b {0,2,5};

  vector<float> c=subsample(a,b);
  vector<float> d {3,4,9.2};
  assert(d==c);

  print(c);
  
}
