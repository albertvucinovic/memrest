#ifndef OOB_ERROR_H
#define OOB_ERROR_H

template<class T>
struct oob_error{
  T oob_absolute_error;
  T oob_samples_count;
  oob_error(){}
  oob_error(T a, T c):oob_absolute_error(a), oob_samples_count(c){}
  T error(){
    return oob_absolute_error/oob_samples_count;
  }
};

#endif //OOB_ERROR_H
