#include "test.h"
#include "utils.h"
#include <cassert>

int main(void){
  vector<int> a {1,2,5,2};
  a=utils::derivate<int>(a);
  utils::print(a);
  vector<int> b {1,3,-3};
  assert(a==b);

}
