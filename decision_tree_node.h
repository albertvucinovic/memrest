#include <memory>
#include <cstdlib>
#include <set>
using namespace std;

//T should be float or double
template <class T>
class DecisionTreeNode{
  public:
    int number_of_features;
    int number_of_decision_functions;
    int min_samples_to_split;
    int max_samples_to_hold;
    //if max_tree_depth>0 then the tree depth can increese
    int max_tree_depth;
    unique_ptr<vector<shared_ptr<Sample<T>>>> samples;

    unique_ptr<DecisionTreeNode<T>> left;
    unique_ptr<DecisionTreeNode<T>> right;

    unique_ptr<vector<int>> randomly_selected_features;

    int criterion_feature;
    T criterion_threshold;

    DecisionTreeNode(
      int number_of_features,
      int number_of_decision_functions,
      int min_samples_to_split,
      int max_samples_to_hold,
      int max_tree_depth
    ):
      number_of_features(number_of_features),
      number_of_decision_functions(number_of_decision_functions),
      min_samples_to_split(min_samples_to_split),
      max_samples_to_hold(max_samples_to_hold),
      max_tree_depth(max_tree_depth)
    {
      samples.reset(new vector<shared_ptr<Sample<T>>>());
      randomly_select_decision_functions();
    }

    ~DecisionTreeNode(){
    }

    bool is_leaf(){
      return !left;
    }
    
    bool criterion(vector<T>& features){
       return features[criterion_feature]>criterion_threshold;
    }

    void update(shared_ptr<Sample<T>> sample){
      N=samples->size();
      if(is_leaf()){
        if(N<max_samples_to_hold){
          samples->push_back(sample);
          if((N+1)%min_samples_to_split==0 && max_tree_depth>0){
            find_and_apply_best_split();
          }
        }
        else{
          freeze();
        }
      }
      else{
        if(criterion(*(sample->features)){
          right->update(sample);
        }
        else{
          left->update(sample);
        }
      }
    }

    virtual void find_and_apply_best_split()=0; 

    virtual T predict(vector<T> sample)=0;

    virtual T update_oob(shared_ptr<Sample<T>> sample)=0;
    
    virtual void freeze_prediction()=0;

    //Releases its shared pointers for samples, and sets up the node to predict the learned value
    void freeze(){
      freeze_prediction();
      samples.reset();
      randomly_selected_features.reset();
    }
  protected:
    void randomly_select_decision_functions(){
      set<int> selected;
      while(selected.size()<number_of_decision_functions){
        selected.insert(rand()%number_of_features);
      }
      randomly_selected_features.reset(new vector<int>());
      for(auto i=selected.begin();i!=selected.end();i++){
        (*randomly_selected_features).push_back(*i);
      }
    }
  private:
    
};
