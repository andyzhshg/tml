// Name:            redis_connect_test
// Description:     redis connect client test
// Author:          zhangshiguang
// Email:           andyzhshg@gmail.com
// Date:            2016-03

#include "redis_connect.h"
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void push_func(const vector<string> &keys) {
    tml::redis_connect conn("127.0.0.1", 6379);
    int cnt = 1;
    while (true) {
        int idx = rand() % keys.size();
        string val = to_string(cnt++);
        if (conn.rpush(keys[idx], val)) {
            cout << std::this_thread::get_id() << " PUSH: " << keys[idx] << " -> " << val << endl;
        } else {
            cout << std::this_thread::get_id() << " PUSH: fail" << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
    }
}

void pop_func(const vector<string> &keys) {
    tml::redis_connect conn("127.0.0.1", 6379);
    while (true) {
        pair<string, string> res;
        if (conn.blpop(keys, res, 0)) {
            cout << std::this_thread::get_id() << " POP:  " << res.first << " -> " << res.second << endl;
        } else {
            cout << std::this_thread::get_id() << " POP:  fail" << endl;
        }
    }
}

void test_bpop() {
    vector<string> keys;
    int cnt = 10;
    for (int i = 0; i < cnt; i++) {
        keys.push_back("K" + to_string(i));
    }
    std::thread pop_thread[cnt];
    for (int i = 0; i < cnt; i++) {
        pop_thread[i] = thread(pop_func, keys);
    }
    std::thread push_thread(push_func, keys);
    for (int i = 0; i < cnt; i++) {
        pop_thread[i].join();
    }
    push_thread.join();    
}

void test_set_get() {
    tml::redis_connect conn("127.0.0.1", 6379);
    conn.set("foo", "bar");
    string val;
    conn.get("foo", val);
    cout << val << endl; 
    conn.del("foo");
    if (conn.get("foo", val)) {
        cout << val << endl;        
    } else {
        cout << "no foo" << endl;
    }
    conn.set("foo", "bar");
    conn.expire("foo", 10);
}

void test_push_pop() {
    tml::redis_connect conn("127.0.0.1", 6379);
    if (conn.rpush(string("foo"), string("bar"))) {
        cout << "PUSH: foo bar" << endl;
    } else {
        cout << "PUSH: foo bar - fail" << endl;
    }
    string res;
    if (conn.lpop("foo", res)) {
        cout << "POP: " << res << endl;
    } else {
        cout << "POP: fail" << endl;
    }
    if (conn.lpop("foo", res)) {
        cout << "POP: " << res << endl;
    } else {
        cout << "POP: fail" << endl;
    }
}

int main(int argc, char *argv[]) {
    test_set_get();
    test_push_pop();
    test_bpop();    
    return 0;
}
