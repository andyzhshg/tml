// Name:            task_queue_test
// Description:     thread-safe task queue test
// Author:          zhangshiguang
// Email:           andyzhshg@gmail.com
// Date:            2016-03


#include "task_queue.h"
#include <iostream>
#include <boost/thread.hpp>

struct FackTask {
    uint64_t id;
    int data;    
};

void producer_func(tml::task_queue<FackTask> *tasks) {
    static uint64_t id = 0;
    while (true) {
        FackTask *t = new FackTask();
        t->id = id++;
        t->data = rand();
        tasks->push(t);
        std::cout << "[producer][" << boost::this_thread::get_id() << "][id:" << t->id << ", data:" << t->data << "]" << std::endl;
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
}

void consumer_func(tml::task_queue<FackTask> *tasks) {
    while (true) {
        FackTask *t = tasks->pop(10);
        if (t) {
            std::cout << "[consumer][ok][" << boost::this_thread::get_id() << "][id:" << t->id << ", data:" << t->data << "]" << std::endl;
            delete t;
        } else {
            std::cout << "[consumer][timeout][" << boost::this_thread::get_id() << "]" << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    tml::task_queue<FackTask> task_queue;
    const int PRODUCER_THREAD_COUNT = 10;
    const int CONSUMER_THREAD_COUNT = 4;
    boost::thread p[PRODUCER_THREAD_COUNT];
    for (int i = 0; i < PRODUCER_THREAD_COUNT; i++) {
        p[i] = boost::thread(boost::bind(producer_func, &task_queue));        
    }
    boost::thread c[CONSUMER_THREAD_COUNT];
    for (int i = 0; i < CONSUMER_THREAD_COUNT; i++) {
        c[i] = boost::thread(boost::bind(consumer_func, &task_queue));    
    }
    
    for (int i = 0; i < PRODUCER_THREAD_COUNT; i++) {
        p[i].join();
    }
    for (int i = 0; i < CONSUMER_THREAD_COUNT; i++) {
        c[i].join();
    }
    
    return 0;
}
