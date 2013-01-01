#ifndef CLASSIFICATION_NODE_OPENCL
#define CLASSIFICATION_NODE_OPENCL

#include <decision_tree_node.h>
#include <opencl_gini_calculator.h>
#include <utils.h>
#include <iostream>
#include <memory>

#ifdef BOOST_MEM
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
#else
using std::shared_ptr;
#endif

using std::vector;
using std::pair;

template<class T>
class ClassificationTreeNodeOpenCL:public DecisionTreeNode<T, ClassificationTreeNodeOpenCL<T>>{
  public:
    OpenCLGiniCalculator<T> *gini_calc;
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
      gini_calc=OpenCLGiniCalculator<T>::get_instance();
    }

    ~ClassificationTreeNodeOpenCL(){}

    virtual Split<T> find_best_split(){
      int num_samples=this->samples->size();
      int num_features=this->number_of_decision_functions;
      shared_ptr<vector<T>> sm=this->samples_matrix();
      shared_ptr<vector<T>> predictions=this->samples_prediction_vector();

      //the key step
      shared_ptr<vector<T>> calculated_gini=gini_calc->opencl_gini_matrix(
        pair<int,int>(num_features, num_samples), 
        sm,
        predictions
        );

      //finding best split parameters
      pair<T,int> m=utils::argmax(*calculated_gini);
      T best_split_score=m.first;
      int best_score_index=m.second;
      T threshold=(*sm)[best_score_index];
      int best_split_feature=best_score_index%num_features;

      //splitting the samples
      shared_ptr<vector<shared_ptr<Sample<T>>>> left_samples(new vector<shared_ptr<Sample<T>>>());
      shared_ptr<vector<shared_ptr<Sample<T>>>> right_samples(new vector<shared_ptr<Sample<T>>>());
      for(auto s1=this->samples->begin();s1!=this->samples->end();s1++){
        if(((*s1)->features[best_split_feature])>threshold){
          right_samples->push_back(*s1);
        }
        else{
          left_samples->push_back(*s1);
        }
      }

      //DEBUG1(cout<<"best split score:"<<best_split_score<<endl);
      return Split<T>(
        threshold, 
        best_split_feature, 
        left_samples, 
        right_samples, 
        best_split_score);
    }

    pair<T,T> node_prediction(){
      if(this->samples->size()>0){
        shared_ptr<vector<T>> spv=this->samples_prediction_vector();
        T prediction=utils::argmaxcount(*spv);
        T probability=utils::count(*spv, prediction)/((float)spv->size());
        return pair<T,T>(prediction, probability);
      }
      else{
        return pair<T,T>(0./0., 0.);//a NaN
      }
    }

};

#endif
