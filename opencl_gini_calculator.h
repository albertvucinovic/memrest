#ifndef GINI_OPENCL
#define GINI_OPENCL

#include <cstdio>
#include <string>
#include <ctemplate/template.h>
#include <map>
#include <utility>
#include <typeinfo>
#include <utils.h>

#include <CL/cl.h>

using std::string;
using std::map;
using std::pair;
using std::printf;
using std::to_string;

//For getting the typename T into the OpenCL kernel
template<typename T>
struct TypeParseTraits;

#define REGISTER_PARSE_TYPE(X) template <> struct TypeParseTraits<X> \
    { static const char* name; } ; const char* TypeParseTraits<X>::name = #X


REGISTER_PARSE_TYPE(float);
REGISTER_PARSE_TYPE(double);
//END For getting the typename T into the OpenCL kernel



template<typename T>
class OpenCLGiniCalculator{
  public:

    static int created;
    static int turn;

    string float_type=TypeParseTraits<T>::name;//"float" or "double"
    map<pair<int,int>, cl_kernel> kernels;
    
    static OpenCLGiniCalculator* get_instance(){
      if(OpenCLGiniCalculator::created<NUMBER_OF_OPENCL_CONTEXTS){
        OpenCLGiniCalculator::single[OpenCLGiniCalculator::turn]=new OpenCLGiniCalculator(TypeParseTraits<T>::name);
        OpenCLGiniCalculator::created++;
      }
      OpenCLGiniCalculator::turn++;
      int instance_num=(OpenCLGiniCalculator::turn-1)%NUMBER_OF_OPENCL_CONTEXTS;
      //cout<<"Getting instance num "<<instance_num<<endl;
      return OpenCLGiniCalculator::single[instance_num];
    }
    ~OpenCLGiniCalculator(){
      // Clean up
      ret = clFlush(this->command_queue);
      ret = clFinish(this->command_queue);
      //TODO:should map over all kernels
      //ret = clReleaseKernel(kernel);
      //ret = clReleaseProgram(program);
      ret = clReleaseCommandQueue(this->command_queue);
      ret = clReleaseContext(context);
    }

    shared_ptr<vector<T>> opencl_gini_matrix(
      pair<int,int> matrix_dimensions, 
      shared_ptr<vector<T>> matrix, 
      shared_ptr<vector<T>> classes)
    {
      cl_kernel kernel;
      int num_features=matrix_dimensions.first;
      int num_samples=matrix_dimensions.second;
      if(kernels[matrix_dimensions]){
        kernel=kernels[matrix_dimensions];
      }
      else{
        ctemplate::TemplateDictionary dict("kernel");
        dict["prime"] = "4294967291";
        dict["float_type"]=this->float_type;
        dict["num_samples"] = to_string(num_samples);
        dict["num_features"]=to_string(num_features);
        std::string kernel_with_args;
        ctemplate::StringToTemplateCache("kernel", kernel_template, ctemplate::DO_NOT_STRIP);
        ctemplate::ExpandTemplate("kernel", ctemplate::DO_NOT_STRIP, &dict, &kernel_with_args);

        // Create a program from the kernel source
        size_t size_of_program=kernel_with_args.size();
        const char *kernel_as_c_string=kernel_with_args.c_str();
        printf("\"PRINTF%sPRINTF\"",kernel_as_c_string);
        cl_program program = clCreateProgramWithSource(context, 1, 
                (const char **)&kernel_as_c_string, (const size_t *)&size_of_program, &ret);
        // Build the program
        ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
        // Create the OpenCL kernel
        kernel = clCreateKernel(program, "gini", &ret);

        kernels[matrix_dimensions]=kernel;
      }

      //TODO: Make this a two step process
      //cl_mem CT = clCreateImage3D(Context, CL_MEM_READ_ONLY , Volume_format, X, Y, Z, 0, 0, 0, &error);
      //error = clEnqueueWriteImage(CommandQue, CT, CL_TRUE, origin, region, rowPitch, slicePitch, sourceData, 0, 0, 0);
      cl_mem A_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
            num_features*num_samples * sizeof(T), &((*matrix)[0]), &ret);
      cl_mem sample_classes_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
            num_samples * sizeof(T), &((*classes)[0]), &ret);
      cl_mem gini_res_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
            num_features*num_samples * sizeof(T), NULL, &ret);

      // Set the arguments of the kernel
      ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&A_mem);
      ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&sample_classes_mem);
      ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&gini_res_mem);

      const size_t global_work_size[]={num_features, num_samples};
      const size_t local_work_size[]={1, num_samples};
      cl_event kernel_event;
      ret = clEnqueueNDRangeKernel(this->command_queue, kernel, 2, NULL, 
        global_work_size, local_work_size, 0, NULL, &kernel_event);

      shared_ptr<vector<T>> result(new vector<T>(num_features*num_samples));
      //we are assuming that the vector storage is continuous as per the c++11 standard
      //ret=clEnqueueBarrier(this->command_queue);

      
      T *C = (T*)malloc(sizeof(T)*num_samples*num_features);
      ret = clEnqueueReadBuffer(this->command_queue, gini_res_mem, CL_TRUE, 0, 
            num_features*num_samples * sizeof(T), &((*result)[0]), 1, &kernel_event, NULL);

      ret = clReleaseMemObject(A_mem);
      ret = clReleaseMemObject(sample_classes_mem);
      ret = clReleaseMemObject(gini_res_mem);

      return result;
    }

  private:
    static OpenCLGiniCalculator *single[NUMBER_OF_OPENCL_CONTEXTS];

    OpenCLGiniCalculator(string float_type)
      :
      float_type(float_type)
    {
      // Get platform and device information
      platform_id = NULL;
      device_id = NULL;   
      ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
      ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, 
              &device_id, &ret_num_devices);
   
      // Create an OpenCL context
      context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
   
      // Create a command queue
      this->command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &ret);

    }

    
    cl_platform_id platform_id;
    cl_device_id device_id;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;
    cl_context context;
    cl_command_queue command_queue;


const char *kernel_template=R"KERNEL(
//we are using a hash function to collect frequency counts for classes
//may be inacurate, but the probability for that should be low
#define hash(k) (as_int((k)) % {{prime}} % {{num_samples}})

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
    int k=sample_classes_local[i];
    int hashed_index=hash(k);
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
    int k=sample_classes_local[i];
    int hashed_index=hash(k);
    //A_local[index]>threshold returns a 0 or 1, so it increments the index for the right class
    int cl=(A_local[i]>threshold);
    right_total=right_total+({{float_type}})cl;
    left_total=left_total+(1-({{float_type}})cl);
    classes_counts[2*hashed_index+cl]++;
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
   
  {{float_type}} split_score=my_gini-1/(({{float_type}}){{num_samples}})*(left_total*left_gini+right_total*right_gini);

  if(!isnan(split_score))
    gini_res[main_index]=split_score;
  else
    gini_res[main_index]=0.;
}

)KERNEL";

};

template<typename T>
int OpenCLGiniCalculator<T>::created=0;

template<typename T>
int OpenCLGiniCalculator<T>::turn=0;

template<typename T>
OpenCLGiniCalculator<T> *OpenCLGiniCalculator<T>::single[NUMBER_OF_OPENCL_CONTEXTS];



#endif //GINI_OPENCL
