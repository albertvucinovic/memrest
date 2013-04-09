#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include <memory>

#include <boost/config.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/utility.hpp>//serialization for pair<K,V>
#include <boost/serialization/set.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/base_object.hpp>

#include <sample.h>
#include <classification_tree_node.h>
#include <classification_tree_node_opencl.h>
#include <oob_error.h>
#include <lambda_queue.h>
#include <online_random_forest_classifier.h>

namespace boost { 
namespace serialization {

/////////////////////////////////////////////////////////////
// implement serialization for unique_ptr< T >
// note: this must be added to the boost namespace in order to
// be called by the library
template<class Archive, class T>
inline void save(
    Archive & ar,
    const std::unique_ptr< T > &t,
    const unsigned int file_version
){
    // only the raw pointer has to be saved
    // the ref count is rebuilt automatically on load
    const T * const tx = t.get();
    ar << tx;
}

template<class Archive, class T>
inline void load(
    Archive & ar,
    std::unique_ptr< T > &t,
    const unsigned int file_version
){
    T *pTarget;
    ar >> pTarget;
    t.reset(pTarget);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class T>
inline void serialize(
    Archive & ar,
    std::unique_ptr< T > &t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
//implement serialization for the Sample<T> class
template<typename Archive, class T>
void serialize(Archive& ar, Sample<T>& sample, const unsigned int version) {
  ar & sample.prediction & sample.features;
}
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
//serialization for DecisionTreeNode<T, NodeType>
//template <class T, class NodeType>
//struct is_abstract<DecisionTreeNode<T,NodeType>> : boost::true_type{};
//template<typename Archive, class T, class NodeType>
//void serialize(Archive& ar, DecisionTreeNode<T, NodeType>& dtn, const unsigned int version){
//  ar & dtn.number_of_features;
//  ar & dtn.number_of_decision_functions;
//  ar & dtn.min_samples_to_split;
//  ar & dtn.max_samples_to_hold;
//  ar & dtn.max_tree_depth;
//  ar & dtn.samples;
//  ar & dtn.left;
//  ar & dtn.right;
//  ar & dtn.randomly_selected_features;
//  ar & dtn.criterion_feature;
//  ar & dtn.criterion_threshold;
//  ar & dtn.frozen_prediction;
//  ar & dtn.prediction_frozen;
//}
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
//serialization for ClassificationTreeNode<T>
template<typename Archive, class T>
void serialize(Archive& ar, ClassificationTreeNode<T>& dtn, const unsigned int version){
  boost::serialization::base_object<DecisionTreeNode<T, ClassificationTreeNode<T>>>(dtn);
  ar & dtn.number_of_features;
  ar & dtn.number_of_decision_functions;
  ar & dtn.min_samples_to_split;
  ar & dtn.max_samples_to_hold;
  ar & dtn.max_tree_depth;
  ar & dtn.samples;
  ar & dtn.left;
  ar & dtn.right;
  ar & dtn.randomly_selected_features;
  ar & dtn.criterion_feature;
  ar & dtn.criterion_threshold;
  ar & dtn.frozen_prediction;
  ar & dtn.prediction_frozen;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
//serialization for ClassificationTreeNodeOpenCL<T>
template<typename Archive, class T>
void serialize(Archive& ar, ClassificationTreeNodeOpenCL<T>& dtn, const unsigned int version){
  boost::serialization::base_object<DecisionTreeNode<T, ClassificationTreeNodeOpenCL<T>>>(dtn);
  ar & dtn.number_of_features;
  ar & dtn.number_of_decision_functions;
  ar & dtn.min_samples_to_split;
  ar & dtn.max_samples_to_hold;
  ar & dtn.max_tree_depth;
  ar & dtn.samples;
  ar & dtn.left;
  ar & dtn.right;
  ar & dtn.randomly_selected_features;
  ar & dtn.criterion_feature;
  ar & dtn.criterion_threshold;
  ar & dtn.frozen_prediction;
  ar & dtn.prediction_frozen;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
//serialize oob_error<T>
template<typename  Archive, class T>
void serialize(Archive& ar, oob_error<T>& oob, const unsigned int version){
  ar & oob.oob_absolute_error;
  ar & oob.oob_samples_count;
}
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
//serialize OnlineRandomForestClassifier<T, NodeType>
template<typename  Archive, class T, class NodeType>
void save(Archive& ar, const OnlineRandomForestClassifier<T, NodeType>& forest, const unsigned int version){
  ar << forest.number_of_trees;
  ar << forest.number_of_features;
  ar << forest.number_of_decision_functions;
  ar << forest.min_samples_to_split;
  ar << forest.max_samples_to_hold;
  ar << forest.max_tree_depth;
  
  ar << forest.trees;

  ar << forest.number_of_immature_trees;
  ar << forest.number_of_samples_to_be_mature;
}

template<typename  Archive, class T, class NodeType>
void load(Archive& ar, OnlineRandomForestClassifier<T, NodeType>& forest, const unsigned int version){
  ar >> forest.number_of_trees;
  ar >> forest.number_of_features;
  ar >> forest.number_of_decision_functions;
  ar >> forest.min_samples_to_split;
  ar >> forest.max_samples_to_hold;
  ar >> forest.max_tree_depth;
  
  ar >> forest.trees;

  ar >> forest.number_of_immature_trees;
  ar >> forest.number_of_samples_to_be_mature;

  forest.parallel_queue=new lambda_queue(NUMBER_OF_THREADS, forest.number_of_trees);
}

template<typename  Archive, class T, class NodeType>
void serialize(Archive& ar, OnlineRandomForestClassifier<T, NodeType>& forest, const unsigned int file_version){
  boost::serialization::split_free(ar, forest, file_version);
}

/////////////////////////////////////////////////////////////






} // namespace serialization
} // namespace boost

#endif
