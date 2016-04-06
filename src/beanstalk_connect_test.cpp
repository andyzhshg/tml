// Name:            beanstalk_connect_test.h
// Description:     beanstalk 客户端测试程序
// Author:          zhangshiguang
// Email:           zhangshiguang@gmail.com
// Date:            2016-03

#include <stdio.h>
#include "beanstalk_connect.h"
#include <boost/thread/thread.hpp>

using namespace std;

//任务生成线程
void task_gen_func() {
    tml::beanstalk_connect conn("127.0.0.1", 7888);
    conn.connect();
    conn.use("foo");
    
    int count = 1;
    while (true) {
        char buf[128];
        sprintf(buf, "job %d", count++);
        int64_t job_id;
        conn.put(buf, job_id);
        printf("[GENERATE] %ld - %s\n", job_id, buf);
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
}

//任务消费线程
void task_consume_func() {
    tml::beanstalk_connect conn("127.0.0.1", 7888);
    conn.connect();
    conn.watch("foo");
    
    while (true) {
        Beanstalkpp::Job job;
        conn.reserve(job);
        printf("[CONSUME] %d - %s\n", job.getJobId(), job.asString().c_str());
        conn.del(job);
    }
}

int main(int argc, char *argv[]) {
    boost::thread t1(task_gen_func);
    boost::thread t2(task_consume_func);    
    t1.join();
    t2.join();
    return 0;
}
