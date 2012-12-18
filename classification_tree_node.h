#ifndef CLASSIFICATION_TREE_NODE_H
#define CLASSIFICATION_TREE_NODE_H

#include <decision_tree_node.h>
#include <utils.h>
#include <sample.h>
#include <split.h>

#include <utility>
#include <memory>
#include <iostream>
#include <cassert>

template<class T>
class ClassificationTreeNode:public DecisionTreeNode<T, ClassificationTreeNode<T>>{
  public:
    ClassificationTreeNode(
      int number_of_features,
      int number_of_decision_functions,
      int min_samples_to_split,
      int max_samples_to_hold,
      int max_tree_depth,
      vector<shared_ptr<Sample<T>>> initial_samples=vector<shared_ptr<Sample<T>>>()
    ):
      DecisionTreeNode<T, ClassificationTreeNode<T>>(
        number_of_features, 
        number_of_decision_functions,
        min_samples_to_split,
        max_samples_to_hold,
        max_tree_depth,
        initial_samples)
    {
    }

    ~ClassificationTreeNode(){
    }

    T calculate_split_score(vector<T> left, vector<T> right){
      vector<T> my_predictions;
      for(auto i=this->samples->begin();i!=this->samples->end();i++){
        my_predictions.push_back((*i)->prediction);
      }
      T my_error=utils::gini(my_predictions);
      T left_gini=utils::gini(left);
      T right_gini=utils::gini(right);
      float total=(float)my_predictions.size();
      T score= my_error-1/total*(left_gini*left.size()+right_gini*right.size());
      //cout<<"left size:"<<left.size()<<endl;
      //cout<<"left gini:"<<left_gini<<endl;
      //cout<<"right size:"<<right.size()<<endl;
      //cout<<"right gini:"<<right_gini<<endl;
      //cout<<"my error:"<<my_error<<endl;
      //cout<<"score:"<<score<<endl;
      return score;
    }

    virtual Split<T> find_best_split(){
      //find the best split
      DEBUG1(cout<<"Finding best split"<<endl);
      T best_split_score=0;
      T best_split_threshold=0.;
      int best_split_feature=-1;
      vector<shared_ptr<Sample<T>>> best_split_left;
      vector<shared_ptr<Sample<T>>> best_split_right;
      for(auto s=this->samples->begin();s!=this->samples->end();s++){
        //TODO: use just different values of the same feature
        for(auto f=this->randomly_selected_features->begin();f!=this->randomly_selected_features->end();f++){
          vector<T> v=(*s)->features;
          T threshold=v[*f];
          //cout<<"threshold:"<<threshold<<endl;
          vector<shared_ptr<Sample<T>>> left_samples;
          vector<T> left;
          vector<shared_ptr<Sample<T>>> right_samples;
          vector<T> right;
          for(auto s1=this->samples->begin();s1!=this->samples->end();s1++){
            if(((*s1)->features[*f])>threshold){
              right.push_back((*s1)->prediction);
              right_samples.push_back(*s1);
            }
            else{
              left.push_back((*s1)->prediction);
              left_samples.push_back(*s1);
            }
          }
          T split_score=calculate_split_score(left, right);
          if(split_score>best_split_score){
            best_split_score=split_score;
            best_split_threshold=threshold;
            best_split_left=left_samples;
            best_split_right=right_samples;
            best_split_feature=*f;
          }
        }
      }
      DEBUG1(cout<<"best split score:"<<best_split_score<<endl);
      return Split<T>(best_split_threshold, best_split_feature, best_split_left, best_split_right, best_split_score);
    }

    pair<T,T> node_prediction(){
      if(this->samples->size()>0){
        vector<T> spv=this->samples_prediction_vector();
        DEBUG1(assert(spv.size()>0));
        T prediction=utils::argmaxcount(spv);
        T probability=utils::count(spv, prediction)/((float)spv.size());
        return pair<T,T>(prediction, probability);
      }
      else{
        DEBUG1(cout<<"Returning a nan"<<endl);
        return pair<T,T>(0./0., 0.);//a NaN
      }
    }
    
};


#endif //CLASSIFICATION_TREE_NODE_H
