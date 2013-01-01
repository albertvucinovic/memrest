#define BOOST_MEM

#include <serialization.h>
#include <test.h>
#include <sample.h>
#include <decision_tree_node.h>

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
#include <memory>
using std::unique_ptr;


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
  const char *fname="test_serialization.cpkl";

  std::ofstream ofs(fname);
  boost::archive::text_oarchive ar(ofs);

  unique_ptr<A> p(new A("Trying serialization with unique_ptr"));
 
  cout<<p->message<<endl;

  ar & p;
  ofs.close();
  
  unique_ptr<A> p_restored;

  std::ifstream ifs(fname);
  boost::archive::text_iarchive iar(ifs);

  iar & p_restored;

  cout<<p_restored->message<<endl;

}

void test_serialize_sample(){
  const char *fname="test_serialization.cpkl";

  std::ofstream ofs(fname);
  boost::archive::text_oarchive ar(ofs);

  
  vector<float> features {3,4,5,6};
  unique_ptr<Sample<float>> p(new Sample<float>(features, 9));
 
  cout<<p->features[0]<<endl;

  ar & p;
  ofs.close();
  
  unique_ptr<Sample<float>> p_restored;

  std::ifstream ifs(fname);
  boost::archive::text_iarchive iar(ifs);

  iar & p_restored;

  cout<<p->features[0]<<endl;

}

void test_serialize_classification_tree_node(){
   const char *fname="test_serialization.cpkl";

  std::ofstream ofs(fname);
  boost::archive::text_oarchive ar(ofs);

  shared_ptr<vector<shared_ptr<Sample<float>>>> initial_samples(new vector<shared_ptr<Sample<float>>>());
  ClassificationTreeNode<float> n(5,2,3,6,3,initial_samples);

  cout<<n.number_of_features<<endl;

  ar & n;
  ofs.close();
  
  ClassificationTreeNode<float> n_restored;

  std::ifstream ifs(fname);
  boost::archive::text_iarchive iar(ifs);

  iar & n_restored;

  cout<<n_restored.number_of_features<<endl;

}


int main(){
  cout<<"Here I am..."<<endl;
  test(&test_normal_serialize, "test_normal_serialize");
  test(&test_unique_ptr_serialize, "test_unique_ptr_serialize");
  test(&test_serialize_sample, "test_serialize_sample");
  test(&test_serialize_classification_tree_node, "test_serialize_classification_tree_node");
  
}
