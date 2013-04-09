#ifndef OOB_ERROR_H
#define OOB_ERROR_H

template<class T>
struct oob_error{
  T oob_absolute_error;
  T oob_samples_count;
  oob_error(){}
  oob_error(T a, T c):oob_absolute_error(a), oob_samples_count(c){}
  T error(){
    //the +1 is because of the division by zero
    return (oob_absolute_error+1)/(oob_samples_count+1);
  }
};

#endif //OOB_ERROR_H
