// Name:            connection pool test
// Description:     a simple connection pool
// Author:          zhangshiguang
// Email:           andyzhshg@gmail.com
// Date:            2016-03

#include "connection_pool.h"
#include <iostream>
#include <boost/thread/thread.hpp>
#include <string>

using namespace std;

class connection {
public:
    connection(int id) : id(id) {
    }
    
    ~connection() {
    }
    
    bool test() {
        boost::this_thread::sleep(boost::posix_time::milliseconds(rand() % 10));
        if (rand() % 10) {
            cout << boost::this_thread::get_id() << ": " << id << ", ok" << endl;
            return true;           
        } else {
            cout << boost::this_thread::get_id() << ": " << id << ", fail" << endl;
            return false;
        }
    }
    
    bool reconnect() {
        if (rand() % 3) {
            cout << boost::this_thread::get_id() << ": " << id << ", reconnect ok" << endl;
            return true;
        } else {
            cout << boost::this_thread::get_id() << ": " << id << ", reconnect fail" << endl;
            return false;           
        }
    }
private:
    int id;
};

void ThreadFunc(tml::connection_pool<connection> *pool) {
    while (true) {
        connection *conn = pool->get();
        if (conn) {
            if (!conn->test()) {
                pool->mark_invalide(conn);
            } else {
                pool->put_back(conn);
            }
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(rand() % 100));
    }
}

int main() {
    tml::connection_pool<connection> pool;
    for (int i = 0; i < 10; i++) {
        pool.add(new connection(i));
    }
    
    boost::thread td[10];
    for (int i = 0; i < 10; i++) {
        td[i] = boost::thread(boost::bind(ThreadFunc, &pool));
    }
    
    for (int i = 0; i < 10; i++) {
        td[i].join();
    }
    
    return 0;
}
