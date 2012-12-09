#ifndef CLASSIFICATION_TREE_NODE_H
#define CLASSIFICATION_TREE_NODE_H

#include <decision_tree_node.h>
#include <utils.h>

template<classT>
class ClassificationTreeNode{
  public:
    T calculate_split_score(vector<T> left, vector<T> right){
      vector<T> my_predictions;
      for(auto i=samples->begin();i!=samples.end();i++){
        my_predictions.push_back((*i)->prediction);
      }
      T my_error=gini<T>(my_prediction);
      left_gini=gini<t>(left);
      right_gini=gini<T>(right);
      float total=(float)my_predictions.size();
      return my_error-1/total*(left_gini*left.size()+right_gini*right.size());
    }

    virtual void find_and_apply_best_split(){
      //find the best split
      T best_split_score=0;
      T best_split_threshold=0.;
      int best_split_feature=-1;
      vector<shared_ptr<Sample<T>> best_split_left;
      vector<shared_ptr<Sample<T>> best_split_right;
      for(auto s=samples->begin();s!=samples->end();s++){
        for(auto f=randomly_selected_features->begin();f!=randomly_selected_features->end();f++){
          T threshold=(*s)->features[*f];
          vector<shared_ptr<Sample<T>> left_samples;
          vector<T> left;
          vector<shared_ptr<Sample<T>> right_samples;
          vector<T> right;
          for(auto s1=samples->begin();s1!=samples->end();s1++){
            if((*s)->features[*f]>threshold){
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
      //apply the best split
      if(best_split_score>0.){
        criterion_feature=best_split_feature;
        criterion_threshold=best_split_threshold;
        left=reset(
          new ClassificationTreeNode<T>(
            number_of_features,
            number_of_decision_functions,
            min_samples_to_split,
            max_samples_to_hold,
            max_tree_depth,
            best_split_left)
        )
        right=reset(
          new ClassificationTreeNode<T>(
            number_of_features,
            number_of_decision_functions,
            min_samples_to_split,
            max_samples_to_hold,
            max_tree_depth,
            best_split_right)
        )
        //free some memory if possible
        samples.reset();
        randomly_selected_features.reset();
      }
    }
}


#endif //CLASSIFICATION_TREE_NODE_H
