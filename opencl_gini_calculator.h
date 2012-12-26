#ifndef GINI_OPENCL
#define GINI_OPENCL

#include <iostream>
#include <cstdio>
#include <string>
#include <ctemplate/template.h>
#include <map>
#include <pair>

using namespace std;

template<typename T>
class OpenCLGiniCalculator{
  public:
    string float_type='float';
    OpenCLGiniCalculator(string float_type='float')
      :float_type(float_type)
    {}

    ~OpenCLGiniCalculator(){
    }

    T* opencl_gini_matrix(int num_features, int num_samples, T *matrix, T *classes){
      ctemplate::TemplateDictionary dict("kernel");
      dict["prime"] = "4294967291";
      dict["float_type"]=this->float_type;
      dict["num_samples"] = to_string(num_samples);
      dict["num_features"]=to_string(num_features);
      std::string kernel_with_args;
      ctemplate::StringToTemplateCache("kernel", kernel, ctemplate::DO_NOT_STRIP);
      ctemplate::ExpandTemplate("kernel", ctemplate::DO_NOT_STRIP, &dict, &kernel_with_args);
      std::cout << kernel_with_args;

      
    }

  private:
const char *kernel=R"KERNEL(
//we are using a hash function to collect frequency counts for classes
//may be inacurate, but the probability for that should be low
#define hash(k) as_int((k)) % {{prime}} % {{num_samples}}

//A is a 2d matrix, stored in rows of num_samples x num_features

//a good value for BLOCK_SIZE is probably num_samples
#define SAMPLES {{num_samples}}
#define FEATURES 1
#define LOCAL_MATRIX_SIZE SAMPLES*FEATURES

//A and gini_res have the same dimensions, gini_res will hold the gini scores of all the possible splits
//After this kernel, another one that finds the maximum of all elements in gini_res should be applied
__kernel 
void gini(
  __global {{float_type}} *A, 
  __constant {{float_type}} *sample_classes, 
  __global {{float_type}} *gini_res)
{
  __local {{float_type}} A_local[LOCAL_MATRIX_SIZE];
  __local {{float_type}} sample_classes_local[{{num_samples}}];

  unsigned int thread_feature = get_global_id(0);
  unsigned int thread_sample = get_global_id(1);

  int main_index=thread_sample*{{num_features}}+thread_feature;

  A_local[thread_sample]=A[main_index];

  sample_classes_local[thread_sample]=sample_classes[thread_sample];

  barrier(CLK_LOCAL_MEM_FENCE);

  int my_classes_counts[{{num_samples}}];
  int classes_counts[{{num_samples}}*2];

  //initializing classes counts
  for(int i=0;i<{{num_samples}};i++){
    //classes counts for left samples
    classes_counts[2*i]=0;
    //classes counts for right samples
    classes_counts[2*i+1]=0;
    my_classes_counts[i]=0;  
  }

  //classes counts before the split
  for(int i=0; i<{{num_samples}}; i++){
    int hashed_index=hash(sample_classes_local[i]);
    my_classes_counts[hashed_index]++;
  }

  //We now classify the samples acording to threshold
  //We get left and right classes counts after split
  //TODO: probably would be more efficient if A_local would be transposed
  {{float_type}} threshold = A_local[thread_sample];
  {{float_type}} left_total=0.;
  {{float_type}} right_total=0.;
  for(int i=0; i<{{num_samples}}; i++){
    //int index=i*{{num_features}}+thread_feature;
    int hashed_index=hash(sample_classes_local[i]);
    //A_local[index]>threshold returns a 0 or 1, so it increments the index for the right class
    int class=(A_local[i]>threshold);
    right_total=right_total+({{float_type}})class;
    left_total=left_total+(1-({{float_type}})class);
    classes_counts[2*hashed_index+class]++;
  }

  //now we have the counts for gini, now we compute it
  {{float_type}} my_gini=1.0;
  {{float_type}} left_gini=1.0;
  {{float_type}} right_gini=1.0;
  {{float_type}} current_class_count;
  {{float_type}} current_class_probability;
  {{float_type}} current_class_probability_squared;
  for(int i=0;i<{{num_samples}}; i++){
    current_class_count=({{float_type}})classes_counts[2*i+0];
    current_class_probability=current_class_count/left_total;
    current_class_probability_squared=current_class_probability*current_class_probability;
    left_gini=left_gini-current_class_probability_squared;

    current_class_count=({{float_type}})classes_counts[2*i+1];
    current_class_probability=current_class_count/right_total;
    current_class_probability_squared=current_class_probability*current_class_probability;
    right_gini=right_gini-current_class_probability_squared;

    current_class_count=({{float_type}})my_classes_counts[i];
    current_class_probability=current_class_count/{{num_samples}};
    current_class_probability_squared=current_class_probability*current_class_probability;
    my_gini=my_gini-current_class_probability_squared;
  }
   
  {{float_type}} split_score=my_gini-1/{{num_samples}}*(left_total*left_gini+right_total*right_gini);

  if(!isnan(split_score))
    gini_res[main_index]=split_score;
  else
    gini_res[main_index]=0.;
}

)KERNEL";

};

#endif //GINI_OPENCL
