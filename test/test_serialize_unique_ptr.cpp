#include <serialize_unique_ptr.h>
#include <test.h>

#include <boost/archive/tmpdir.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <fstream>
using std::ifstream;
using std::ofstream;


class A{
  public:
  string message;
  A(){}
  A(string s):message(s){ }
  ~A(){}
  private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive &ar, const unsigned int file_version){
    ar & message;
  }
};

void test_normal_serialize(){
  const char *fname="test_serialization.cpkl";
  A a("Trying serialization");

  std::ofstream ofs(fname);
  boost::archive::text_oarchive ar(ofs);
 
  cout<<a.message<<endl;

  ar & a;
  ofs.close();
  
  A restored;

  std::ifstream ifs(fname);
  boost::archive::text_iarchive iar(ifs);

  iar & restored;

  cout<<a.message<<endl;


}

void test_unique_ptr_serialize(){
}

int main(){
  cout<<"Here I am..."<<endl;
  test(&test_normal_serialize, "test_normal_serialize");
  test(&test_unique_ptr_serialize, "test_unique_ptr_serialize");
  
}
