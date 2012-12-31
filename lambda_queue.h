#ifndef LAMBDA_QUEUE_H
#define LAMBDA_QUEUE_H

#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <vector>
#include <chrono>

#include <iostream>


class lambda_queue{
  public:
  int number_of_threads;
  int work_batch_size;
  int jobs_counter;

  lambda_queue(int number_of_threads, int work_batch_size):
    number_of_threads(number_of_threads), 
    work_batch_size(work_batch_size)
  {
    jobs_counter=0;
    for(int i=0;i<number_of_threads;i++){
      threads.push_back(std::thread(&lambda_queue::work, this, i));
    }
  }

  ~lambda_queue(){
    for(auto i=threads.begin();i!=threads.end();i++){
      i->join();
    }
  }

  void push(std::function<void()> f){
    m.lock();
    queue.push(f);
    m.unlock();
  }

  void sync(){
    m.lock();
    while(jobs_counter!=work_batch_size){
      m.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(0));
      m.lock();
    }
    jobs_counter=0;
    m.unlock();
  }

  private:

  std::queue<std::function<void()>> queue;
  std::mutex m;
  std::vector<std::thread> threads;

  void work_wrapper(int i){
    m.lock();
    bool locked=true;
    if(!queue.empty()){
      std::function<void()> job=queue.front();
      queue.pop();
      m.unlock();
      job();
      m.lock();
      jobs_counter+=1;
      m.unlock();
      locked=false;
    }
    else{
      m.unlock();
      locked=false;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if(locked){
      m.unlock();
    }
  }

  void work(int i){
    std::cout<<"I is: "<<i<<std::endl;
    while(true){
      work_wrapper(i);
    }
  }

};

#endif
