// Name:            task_queue
// Description:     thread-safe task queue
// Author:          zhangshiguang
// Email:           andyzhshg@gmail.com
// Date:            2016-03

#ifndef __TML_TASK_QUEUE_INC__
#define __TML_TASK_QUEUE_INC__

#include <queue>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

namespace tml {

template<typename T>
class task_queue {
public:
    task_queue() {
    }
    
    bool push(T *t) {
        boost::mutex::scoped_lock lock(mutex);
        tasks.push(t);
        cond.notify_one();
        return true;
    }
    
    T* pop(int timeout = 0) {
        boost::mutex::scoped_lock lock(mutex);
        if (!tasks.empty()) {
            T *t = tasks.front();
            tasks.pop();
            return t;
        }
        while (tasks.empty()) {
            if (timeout > 0) {
                if (boost::cv_status::timeout == cond.wait_for(lock, boost::chrono::milliseconds(timeout))) {
                    return NULL;
                }
            } else {
                cond.wait(lock);
            }
        }
        T *t = tasks.front();
        tasks.pop();
        return t;
    }
    
private:
    std::queue<T *> tasks;
    boost::mutex mutex;
    boost::condition cond;
};

}


#endif //__TML_TASK_QUEUE_INC__
