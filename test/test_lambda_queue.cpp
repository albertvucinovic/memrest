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
  int num_tasks=4000;
  lambda_queue queue(5, num_tasks);
  vector<function<void()>> pushed;

  for(int i=0;i<num_tasks;i++){
    auto a=[=](){cout<<"Work item number "<<i<<endl;};
    queue.push(a);
  }

}

int main(void){

  test(&test_basic_functionality, "test_basic_functionality");

}
