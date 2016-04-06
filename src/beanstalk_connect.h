// Name:            beanstalk_connect.h
// Description:     beanstalk client, just a simple wrapper on Beanstalkpp
//                  no exception thrown
// Author:          zhangshiguang
// Email:           zhangshiguang@gmail.com
// Date:            2016-03

#ifndef __TML_BEANSTALK_CONNECT_INC__
#define __TML_BEANSTALK_CONNECT_INC__

#include <string>
#include <beanstalk++/beanstalkpp.h>
#include <boost/shared_ptr.hpp>

namespace tml {
class beanstalk_connect {
public:
    beanstalk_connect(const std::string &server, int port);
    beanstalk_connect(const std::string &server, int port, const std::string &tube);    //可以指定tube的构造函数
    bool connect();     //连接
    bool reconnect();   //重连
    bool use(const std::string &tube);      //指定发送任务的tube
    bool watch(const std::string &tube);    //指定监视接受任务的tube
    bool put(const std::string &data, int64_t &job_id); //放置一个任务到当前tube
    bool reserve(Beanstalkpp::Job &job);            //取得一个任务
    int reserveWithTimeout(boost::shared_ptr<Beanstalkpp::Job> &jobPtr, int timeout); //取得一个任务(带超时) 返回0-成功 1-超时 -1-出错
    bool del(const Beanstalkpp::Job &j);            //删除一个任务
    bool release(const Beanstalkpp::Job &j);        //释放一个任务
private:
    boost::shared_ptr<Beanstalkpp::Client> client;  //实际工作的Beanstalkpp客户端指针
    std::string ip;
    int port;
    std::string tube;
};
}

#endif //__TML_BEANSTALK_CONNECT_INC__
