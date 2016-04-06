// Name:            connection pool
// Description:     a simple connection pool
// Author:          zhangshiguang
// Email:           andyzhshg@gmail.com
// Date:            2016-03

#ifndef __TML_CONNECTION_POOL_INC__
#define __TML_CONNECTION_POOL_INC__

#include <queue>
#include <set>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

namespace tml {
template<typename T>
class connection_pool {
public:
    //auto_reconnect default true if you don't want auto connect, pass false
    connection_pool(bool auto_reconnect = true) {
        if (auto_reconnect) {
            boost::thread(boost::bind(reconnect, this));
        }
    }
    
    //add a connection to pool
    bool add(T *conn) {
        boost::mutex::scoped_lock lock(mutex);
        idle.push(conn);
        cond.notify_one();
        return true;
    }
    
    //get a connection from pool
    T* get(int timeout = 0) {
        boost::mutex::scoped_lock lock(mutex);
        if (!idle.empty()) {
            T* conn = idle.front();
            idle.pop();
            in_use.insert(conn);
            return conn;
        }
        while (idle.empty()) {
            if (timeout > 0) {
                if (boost::cv_status::timeout == cond.wait_for(lock, boost::chrono::milliseconds(timeout))) {
                    return NULL;
                }
            } else {
                cond.wait(lock);
            }
        }
        T* conn = idle.front();
        idle.pop();
        in_use.insert(conn);
        return conn;
    }
    
    //put a connection back to pool
    bool put_back(T *conn) {
        boost::mutex::scoped_lock lock(mutex);
        idle.push(conn);
        in_use.erase(conn);
        cond.notify_one();
        return true;
    }
    
    //mark a connection as invalid
    bool mark_invalid(T* conn) {
        boost::mutex::scoped_lock lock(mutex);
        in_use.erase(conn);
        bad.push(conn);
        return true;
    }
    
private:
    //auto reconnect in a seperate thread
    static void reconnect(connection_pool<T> *pool) {
        while (true) {
            if (pool->bad.empty()) {
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                continue;
            }
            T* conn = NULL;
            {
                boost::mutex::scoped_lock lock(pool->mutex);
                if (!pool->bad.empty()) {
                    conn = pool->bad.front();
                    pool->bad.pop();
                }
            }
            if (conn) {
                if (conn->reconnect()) {
                    boost::mutex::scoped_lock lock(pool->mutex);
                    pool->idle.push(conn);
                    pool->cond.notify_one();
                } else {
                    boost::mutex::scoped_lock lock(pool->mutex);
                    pool->bad.push(conn);
                }
            }
            boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }
    }
    
    std::queue<T *> idle;
    std::queue<T *> bad;
    std::set<T *> in_use;
    boost::mutex mutex;
    boost::condition cond;
};
}

#endif //__TML_CONNECTION_POOL_INC__
