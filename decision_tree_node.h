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
    unique_ptr<vector<shared_ptr<Sample<T>>>> samples;

    unique_ptr<DecisionTreeNode<T>> left;
    unique_ptr<DecisionTreeNode<T>> right;

    unique_ptr<vector<int>> randomly_selected_features;
    DecisionTreeNode(
      int number_of_features,
      int number_of_decision_functions,
      int min_samples_to_split,
      int max_samples_to_hold
    ):
      number_of_features(number_of_features),
      number_of_decision_functions(number_of_decision_functions),
      min_samples_to_split(min_samples_to_split),
      max_samples_to_hold(max_samples_to_hold)
    {
      samples.reset(new vector<shared_ptr<Sample<T>>>());
      randomly_select_decision_functions();
    }

    ~DecisionTreeNode(){
    }

  
    virtual T predict(vector<T> sample)=0;
    
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
