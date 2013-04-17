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
#include <deque>
using std::deque;
#include <algorithm>
using std::sort;

#include <lambda_queue.h>

#define NUMBER_OF_SAMPLES_TO_BE_MATURE 500
#define FRACTION_OF_IMMATURE_TREES .3333


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
  
  typedef deque<TreeNodeOOB> trees_priority_queue;
  shared_ptr<trees_priority_queue> best_trees_sorted_by_oob;
  int number_of_immature_trees;
  int number_of_samples_to_be_mature;

  

  lambda_queue *parallel_queue;

  OnlineRandomForestClassifier(){}

  OnlineRandomForestClassifier(
    int number_of_trees,
    int number_of_features,
    int number_of_decision_functions,
    int min_samples_to_split,
    int max_samples_to_hold,
    int max_tree_depth,
    int number_of_samples_to_be_mature=NUMBER_OF_SAMPLES_TO_BE_MATURE,
    float fraction_of_immature_trees=FRACTION_OF_IMMATURE_TREES
  ):
    number_of_trees(number_of_trees),
    number_of_features(number_of_features),
    number_of_decision_functions(number_of_decision_functions),
    min_samples_to_split(min_samples_to_split),
    max_samples_to_hold(max_samples_to_hold),
    max_tree_depth(max_tree_depth),
    number_of_samples_to_be_mature(number_of_samples_to_be_mature)
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
    number_of_immature_trees=number_of_trees*fraction_of_immature_trees;
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

  bool compare_trees_by_oob(const TreeNodeOOB& lhs, const TreeNodeOOB& rhs) const{
    return lhs.second->error()<rhs.second->error();
  }
  class comparator{
    public:
      comparator(){}
      bool operator()(const TreeNodeOOB& lhs, const TreeNodeOOB& rhs) const{
        return lhs.second->error()<rhs.second->error();
      }
  };


  void prepare_trees_and_drop_the_worst(){
    vector<TreeNodeOOB> immature_trees;
    //sort out the mature and immature trees
    best_trees_sorted_by_oob.reset(new trees_priority_queue());
    while(!trees->empty()){
      TreeNodeOOB tree=trees->back();
      trees->pop_back();
      if(tree.second->oob_samples_count>=number_of_samples_to_be_mature){
        //the tree is mature
        best_trees_sorted_by_oob->push_back(tree);
      }
      else{
        immature_trees.push_back(tree);
      }
    }

    comparator c;
    //sort best trees by oob
    std::sort(best_trees_sorted_by_oob->begin(), best_trees_sorted_by_oob->end(), c);
    //sort the immature trees also
    std::sort(immature_trees.begin(), immature_trees.end(), c);

    //drop the worst mature tree
    if(best_trees_sorted_by_oob->size()>=number_of_trees-number_of_immature_trees){
      //we can drop the worst mature tree
      TreeNodeOOB worst_tree=best_trees_sorted_by_oob->back();
      cout<<"Dropping the tree with oob="<<worst_tree.second->error()<<endl;
      best_trees_sorted_by_oob->pop_back();
      //we need to replace that tree with a newborn tree, which we add to immature trees
      shared_ptr<vector<shared_ptr<Sample<T>>>> initial_samples(new vector<shared_ptr<Sample<T>>>());
      immature_trees.push_back(
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

    //push the best trees back to the trees vector
    while(!best_trees_sorted_by_oob->empty()){
      TreeNodeOOB tree=best_trees_sorted_by_oob->front();
      best_trees_sorted_by_oob->pop_front();
      trees->push_back(tree);
    }

    //push the immature trees to trees
    for(auto i=immature_trees.begin();i!=immature_trees.end();i++){
      trees->push_back(*i);
    }
  }

  void update(shared_ptr<Sample<T>> sample){
      for(auto i=trees->begin();i!=trees->end();i++){
        auto f=[this, i, sample](){ this->update_tree(*i, sample); };
        parallel_queue->push(f);
      }
      parallel_queue->sync();
      prepare_trees_and_drop_the_worst();
  }
  void update(shared_ptr<Sample<T>> sample,int times){
    for(int k=0;k<times;k++){
        update(sample);
    }
  }

  pair<T,T> predict(vector<T>& sample){
    pair<T,T> temp;
    map<T,int> predictions;
    map<T,T> predictions_probabilities;
    //float total_predictions=trees->size();
    //for(auto i=trees->begin();i!=trees->end();i++){
    //  //DEBUG1(assert(!(*i).first->is_leaf()));
    //  pair<T,T> prediction=(*i).first->predict(sample);
    //  predictions[prediction.first]+=1;
    //  predictions_probabilities[prediction.first]+=prediction.second;
    //}
    float total_predictions=number_of_trees-number_of_immature_trees;
    for(int i=0;i<total_predictions;i++){
      pair<T,T> prediction=(*trees)[i].first->predict(sample);
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



