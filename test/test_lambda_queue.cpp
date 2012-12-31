#include <lambda_queue.h>
#include <iostream>
#include <functional>

using namespace std;

void test(void (*f)(), string name){
  cout<<"Testing "<<name<<endl;
  f();
  cout<<"Testing "<<name<<" finished."<<endl<<endl;
}

void test_basic_functionality(){
  lambda_queue queue(5);
  vector<function<void()>> pushed;

  for(int i=0;i<4000;i++){
    auto a=[=](){cout<<"Work item number "<<i<<endl;};
    queue.push(a);
  }

}

int main(void){

  test(&test_basic_functionality, "test_basic_functionality");

}
