// Name:            beanstalk_connect.cpp
// Description:     beanstalk client
// Author:          zhangshiguang
// Email:           zhangshiguang@gmail.com
// Date:            2016-03

#include "beanstalk_connect.h"

namespace tml {

beanstalk_connect::beanstalk_connect(const std::string &ip, int port) 
    : ip(ip), port(port), tube("") {
}

beanstalk_connect::beanstalk_connect(const std::string &ip, int port, const std::string &tube) 
    : ip(ip), port(port), tube(tube) {
}

bool beanstalk_connect::connect() {
    try {
        if (!client) {
            client = boost::shared_ptr<Beanstalkpp::Client>(new Beanstalkpp::Client(ip, port));            
        }
        client->connect();        
        if (tube != "") {
            client->watch(tube);
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool beanstalk_connect::reconnect() {
    try {
        client = boost::shared_ptr<Beanstalkpp::Client>(new Beanstalkpp::Client(ip, port));
        return connect();
    } catch (...) {
    }
    return false;
}

bool beanstalk_connect::use(const std::string &tube) {
    try {
        if (client) {
            client->use(tube);
            return true;
        }        
    } catch (...) {
    }
    return false;
}

bool beanstalk_connect::watch(const std::string &tube) {
    try {
        if (client) {
            client->watch(tube);
            return true;
        }        
    } catch (...) {
    }
    return false;
}

bool beanstalk_connect::put(const std::string &data, int64_t &job_id) {
    try {
        if (client) {
            job_id = client->put(data);
            return true;
        }
    } catch (...) {
    }
    return false;
}

bool beanstalk_connect::reserve(Beanstalkpp::Job &job) {
    try {
        if (client) {
            job = client->reserve();
            return true;
        }
    } catch (...) {
    }
    return false;
}

int beanstalk_connect::reserveWithTimeout(boost::shared_ptr<Beanstalkpp::Job> &jobPtr, int timeout) {
    //0 - 成功， 1 - 超时， -1 - 出错
    try {
        if (client) {
            bool ok = client->reserveWithTimeout<Beanstalkpp::Job>(jobPtr, timeout);
            return ok ? 0 : 1;
        }
    } catch (...) {
        return -1;
    }
}

bool beanstalk_connect::release(const Beanstalkpp::Job &j) {
    try {
        if (client) {
            client->release(j);
            return true;
        }
    } catch (...) {
    }
    return false;    
}

bool beanstalk_connect::del(const Beanstalkpp::Job &j) {
    try {
        if (client) {
            client->del(j);
            return true;
        }        
    } catch (...) {
    }
    return false;
}

}
