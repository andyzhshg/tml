// Name:            redis_connect
// Description:     redis connect client
// Author:          zhangshiguang
// Email:           andyzhshg@gmail.com
// Date:            2016-03

#include "redis_connect.h"
#include <string.h>

namespace tml {
redis_connect::redis_connect(const std::string &ip, int port) 
    : ip(ip), port(port), context(NULL) {
}

redis_connect::~redis_connect() {
    cleanup();
}

bool redis_connect::reconnect() {
    connect();
}

bool redis_connect::set(const std::string &key, const std::string &value) {
    if (!prepare()) {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(context, "SET %b %b", key.data(), key.size(), value.data(), value.size());
    if (reply == NULL) {
        return false;
    } else if (reply->type != REDIS_REPLY_STATUS || strcasecmp(reply->str, "OK") != 0) {
        freeReplyObject(reply);
        return false;
    } else {
        freeReplyObject(reply);
    }
    return true;    
}

bool redis_connect::get(const std::string &key, std::string &value) {
    if (!prepare()) {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(context, "GET %b", key.data(), key.size());
    if (reply == NULL) {
        return false;
    } else if (reply->type != REDIS_REPLY_STRING) {
        freeReplyObject(reply);
        return false;
    }
    value = std::string(reply->str, reply->len);
    freeReplyObject(reply);
    return true;   
}

bool redis_connect::del(const std::string &key) {
    if (!prepare()) {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(context, "DEL %b", key.data(), key.size());
    if (reply == NULL) {
        return false;
    } else if (reply->type != REDIS_REPLY_INTEGER || reply->integer != 1) {
        freeReplyObject(reply);
        return false;
    } else {
        freeReplyObject(reply);
        return true;
    }    
}

bool redis_connect::exists(const std::string &key) {
    if (!prepare()) {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(context, "EXISTS %b", key.data(), key.size());
    if (reply == NULL) {
        return false;
    }
    else if (reply->type != REDIS_REPLY_INTEGER) {
        freeReplyObject(reply);
        return false;
    }
    bool exists = reply->integer == 1;
    freeReplyObject(reply);
    return exists;    
}

bool redis_connect::expire(const std::string &key, int seconds) {
    if (!prepare()) {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(context, "EXPIRE %b %d", key.data(), key.size(), seconds);
    if (reply == NULL) {
        freeReplyObject(reply);
        return false;
    } else if (reply->type != REDIS_REPLY_INTEGER) {
        freeReplyObject(reply);
        return false;
    } else if (reply->integer != 1) {
        freeReplyObject(reply);
        return false;
    } else {
        freeReplyObject(reply);
    }
    return true;    
}

bool redis_connect::rpush(const std::string &key, const std::string &value) {
    if (!prepare()) {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(context, "RPUSH %b %b", key.data(), key.size(), value.data(), value.size());
    if (reply == NULL) {
        freeReplyObject(reply);
        return false;
    } else if (reply->type != REDIS_REPLY_INTEGER) {
        freeReplyObject(reply);
        return false;
    } else if (reply->integer != 1) {
        freeReplyObject(reply);
        return false;
    } else {
        freeReplyObject(reply);
    }
    return true;
}

bool redis_connect::lpop(const std::string &key, std::string &res) {
    if (!prepare()) {
        return false;
    }
    redisReply *reply = (redisReply *)redisCommand(context, "LPOP %b", key.data(), key.size());
    if (reply == NULL) {
        return false;
    } else if (reply->type != REDIS_REPLY_STRING) {
        freeReplyObject(reply);
        return false;
    }
    res = std::string(reply->str, reply->len);
    freeReplyObject(reply);
    return true;
}

bool redis_connect::blpop(const std::vector<std::string> &keys, std::pair<std::string, std::string> &res, int expire) {
    if (!prepare()) {
        return false;
    }
    std::vector<const char *> argv;
    std::vector<size_t> argv_len;
    argv.push_back("BLPOP");
    argv_len.push_back(strlen("BLPOP"));
    for (std::vector<std::string>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
        argv.push_back(it->data());
        argv_len.push_back(it->size());
    }
    char str_exp[64];
    sprintf(str_exp, "%d", expire);
    argv.push_back(str_exp);
    argv_len.push_back(strlen(str_exp));
    
    redisReply *reply = (redisReply *)redisCommandArgv(context, argv.size(), &argv[0], &argv_len[0]);
    bool ok = true;
    if (reply == NULL) {
        ok = false;
    } else if (reply->type != REDIS_REPLY_ARRAY) {
        ok = false;
        freeReplyObject(reply);
    } else if (reply->elements != 2) {
        ok = false;
        freeReplyObject(reply);
    } else {
        redisReply *reply_key = reply->element[0];
        redisReply *reply_val = reply->element[1];
        res.first = std::string(reply_key->str, reply_key->len);
        res.second = std::string(reply_val->str, reply_val->len);
        freeReplyObject(reply);
    }
    return ok;
}

void redis_connect::connect() {
    cleanup();
    context = redisConnect(ip.c_str(), port);
    if (context && context->err) {
        cleanup();
    }
}

void redis_connect::cleanup() {
    if (context) {
        redisFree(context);
        context = NULL;
    }
}

bool redis_connect::prepare() {
    if (context == NULL || (context && context->err != 0)) {
        connect();
    }
    return context != NULL && context->err == 0;
}
}
