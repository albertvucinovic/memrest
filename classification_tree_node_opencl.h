#ifndef CLASSIFICATION_NODE_OPENCL
#define CLASSIFICATION_NODE_OPENCL

#include <decision_tree_node.h>
#include <gini_opencl.h>

template<class T>
class ClassificationTreeNodeOpenCL:public DecisionTreeNode<T, ClassificationTreeNodeOpenCL<T>>{
  public:
    ClassificationTreeNodeOpenCL(
      int number_of_features,
      int number_of_decision_functions,
      int min_samples_to_split,
      int max_samples_to_hold,
      int max_tree_depth,
      shared_ptr<vector<shared_ptr<Sample<T>>>> initial_samples=new vector<shared_ptr<Sample<T>>>()
    ):
      DecisionTreeNode<T, ClassificationTreeNodeOpenCL<T>>(
        number_of_features, 
        number_of_decision_functions,
        min_samples_to_split,
        max_samples_to_hold,
        max_tree_depth,
        initial_samples)
    {
    }

    ~ClassificationTreeNodeOpenCL(){}

    virtual Split<T> find_best_split(){
      DEBUG1(cout<<"Finding best split OpenCL"<<endl);
      T best_split_score=0;
      T best_split_threshold=0.;
      int best_split_feature=-1;
      shared_ptr<vector<shared_ptr<Sample<T>>>> best_split_left;
      shared_ptr<vector<shared_ptr<Sample<T>>>> best_split_right;


    }


}

#endif
