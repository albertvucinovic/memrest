#ifndef CLASSIFICATION_TREE_NODE_H
#define CLASSIFICATION_TREE_NODE_H

#include <decision_tree_node.h>
#include <utils.h>
#include <sample.h>
#include <memory>

template<class T>
class ClassificationTreeNode:public DecisionTreeNode<T>{
  public:
    ClassificationTreeNode(
      int number_of_features,
      int number_of_decision_functions,
      int min_samples_to_split,
      int max_samples_to_hold,
      int max_tree_depth,
      vector<shared_ptr<Sample<T>>> initial_samples=vector<shared_ptr<Sample<T>>>()
    ):
      DecisionTreeNode<T>(
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
      return my_error-1/total*(left_gini*left.size()+right_gini*right.size());
    }

    virtual void find_and_apply_best_split(){
      //find the best split
      T best_split_score=0;
      T best_split_threshold=0.;
      int best_split_feature=-1;
      vector<shared_ptr<Sample<T>>> best_split_left;
      vector<shared_ptr<Sample<T>>> best_split_right;
      for(auto s=this->samples->begin();s!=this->samples->end();s++){
        for(auto f=this->randomly_selected_features->begin();f!=this->randomly_selected_features->end();f++){
          vector<T> v=(*s)->features;
          T threshold=v[*f];
          vector<shared_ptr<Sample<T>>> left_samples;
          vector<T> left;
          vector<shared_ptr<Sample<T>>> right_samples;
          vector<T> right;
          for(auto s1=this->samples->begin();s1!=this->samples->end();s1++){
            if(((*s)->features[*f])>threshold){
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
        this->criterion_feature=best_split_feature;
        this->criterion_threshold=best_split_threshold;
        this->left.reset(
          new ClassificationTreeNode<T>(
            this->number_of_features,
            this->number_of_decision_functions,
            this->min_samples_to_split,
            this->max_samples_to_hold,
            this->max_tree_depth,
            best_split_left)
        );
        this->right.reset(
          new ClassificationTreeNode<T>(
            this->number_of_features,
            this->number_of_decision_functions,
            this->min_samples_to_split,
            this->max_samples_to_hold,
            this->max_tree_depth,
            best_split_right)
        );
        //free some memory if possible
        this->samples.reset();
        this->randomly_selected_features.reset();
      }
    }

    T node_prediction(){
      if(this->samples->size()>0){
        return utils::argmaxcount(this->samples_prediction_vector());
      }
      else if(this->prediction_frozen){
        return this->frozen_prediction;
      }
      else{
        return 0./0.;//a NaN
      }
    }

    virtual T predict(vector<T> sample){
      if(this->is_leaf()){
        return node_prediction();
      }
      else{
        if(this->criterion(sample)){
          return this->right->predict(sample);
        }
        else{
          return this->left->predict(sample);
        }
      }
    }

    virtual T update_oob(shared_ptr<Sample<T>> sample){
      //TODO
      return 0.;
    }

    virtual void freeze_prediction(){
      this->frozen_prediction=node_prediction();
      //TODO:implement probabilities
      this->frozen_prediction_probability=1.;
      this->prediction_frozen=true;
    }
};


#endif //CLASSIFICATION_TREE_NODE_H
