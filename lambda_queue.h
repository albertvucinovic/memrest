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
  

  lambda_queue(int number_of_threads){
    notified=false;
    for(int i=0;i<number_of_threads;i++){
      threads.push_back(std::thread(&lambda_queue::work, this));
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

  private:

  std::queue<std::function<void()>> queue;
  std::mutex m;
  std::condition_variable cond_var;
  std::vector<std::thread> threads;
  bool notified;

  void work_wrapper(){
    m.lock();
    bool locked=true;
    if(!queue.empty()){
      std::cout<<"Thread "<<std::this_thread::get_id()<<':'<<std::endl;
      std::function<void()> job=queue.front();
      queue.pop();
      m.unlock();
      locked=false;
      job();
    }
    else{
      m.unlock();
      locked=false;
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    if(locked){
      m.unlock();
    }
  }

  void work(){
    while(true){
      work_wrapper();
    }
  }

};

#endif
