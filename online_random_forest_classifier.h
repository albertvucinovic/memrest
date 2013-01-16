#ifndef ONLINE_RANDOM_FOREST_CLASSIFIER_H
#define ONLINE_RANDOM_FOREST_CLASSIFIER_H

#include <classification_tree_node.h>
#include <sample.h>
#include <oob_error.h>

#include <memory>
#include <vector>
#include <cstdlib>
#include <utility>
#include <iostream>

#include <lambda_queue.h>



template <class T, class NodeType>
class OnlineRandomForestClassifier{
  typedef pair<shared_ptr<NodeType>,shared_ptr<oob_error<T>>> TreeNodeOOB;

  public:
  
  int number_of_trees;
  int number_of_features;
  int number_of_decision_functions;
  int min_samples_to_split;
  int max_samples_to_hold;
  int max_tree_depth;

  shared_ptr<vector<TreeNodeOOB>> trees;

  lambda_queue *parallel_queue;

  OnlineRandomForestClassifier(){}

  OnlineRandomForestClassifier(
    int number_of_trees,
    int number_of_features,
    int number_of_decision_functions,
    int min_samples_to_split,
    int max_samples_to_hold,
    int max_tree_depth
  ):
    number_of_trees(number_of_trees),
    number_of_features(number_of_features),
    number_of_decision_functions(number_of_decision_functions),
    min_samples_to_split(min_samples_to_split),
    max_samples_to_hold(max_samples_to_hold),
    max_tree_depth(max_tree_depth)
  {
    trees.reset(new vector<TreeNodeOOB>());
    for(int i=0;i<number_of_trees;i++){
      shared_ptr<vector<shared_ptr<Sample<T>>>> initial_samples(new vector<shared_ptr<Sample<T>>>());
      trees->push_back(
        TreeNodeOOB(
          shared_ptr<NodeType>(new NodeType(
            number_of_features,
            number_of_decision_functions,
            min_samples_to_split,
            max_samples_to_hold,
            max_tree_depth,
            initial_samples
          )),
          shared_ptr<oob_error<T>>(new oob_error<T>(0.,0.))
        ));
    }
    parallel_queue=new lambda_queue(NUMBER_OF_THREADS,number_of_trees);
  }

  ~OnlineRandomForestClassifier(){}

  void update_oob_error(TreeNodeOOB& tree, shared_ptr<Sample<T>> sample){
    pair<T,T> prediction=tree.first->predict(sample->features);
    if(prediction.first!=sample->prediction){
      tree.second->oob_absolute_error+=1;
    }
    tree.second->oob_samples_count+=1;
  }


  void update_tree(TreeNodeOOB& tree, shared_ptr<Sample<T>> sample){
    int k=rand()%2;
    for(int i=0;i<k;i++){
      tree.first->update(sample);
    }
    if(k==0){
      update_oob_error(tree, sample);
    }
  }

  void update(shared_ptr<Sample<T>> sample){
    for(auto i=trees->begin();i!=trees->end();i++){
      auto f=[this, i, sample](){ this->update_tree(*i, sample); };
      parallel_queue->push(f);
    }
    parallel_queue->sync();
  }

  pair<T,T> predict(vector<T>& sample){
    pair<T,T> temp;
    map<T,int> predictions;
    map<T,T> predictions_probabilities;
    float total_predictions=trees->size();
    for(auto i=trees->begin();i!=trees->end();i++){
      //DEBUG1(assert(!(*i).first->is_leaf()));
      pair<T,T> prediction=(*i).first->predict(sample);
      predictions[prediction.first]+=1;
      predictions_probabilities[prediction.first]+=prediction.second;
    }
    T prediction=utils::argmax(predictions);
    T probability=predictions_probabilities[prediction]/total_predictions;
    return pair<T,T>(prediction, probability);
  }

  private:
   


};


#endif //ONLINE_RANDOM_FOREST_CLASSIFIER_H



