// Name:            redis_connect
// Description:     redis connect client
// Author:          zhangshiguang
// Email:           andyzhshg@gmail.com
// Date:            2016-03

#ifndef __REDIS_CONNECT_INC__
#define __REDIS_CONNECT_INC__

#include <hiredis/hiredis.h>
#include <string>
#include <vector>
#include <map>

namespace tml {
class redis_connect {
public:
    redis_connect(const std::string &ip, int port);
    ~redis_connect();
    bool reconnect();
    bool set(const std::string &key, const std::string &value);
    bool get(const std::string &key, std::string &value);
    bool del(const std::string &key);
    bool exists(const std::string &key);
    bool expire(const std::string &key, int seconds);
    bool rpush(const std::string &key, const std::string &value);
    bool lpop(const std::string &key, std::string &res);
    bool blpop(const std::vector<std::string> &keys, std::pair<std::string, std::string> &res, int expire = 0);
private:
    void connect();
    void cleanup();
    bool prepare();
    redisContext *context;
    std::string ip;
    int port;
};
}

#endif //__REDIS_CONNECT_INC__
