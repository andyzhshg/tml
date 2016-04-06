// Name: stopwatch.cpp
// Description: a stopwatch class for time measure
// Author: zhangshiguang
// Email: andyzhshg@gmail.com
// Date: 2016-03

#include "stopwatch.h"
#include <stdlib.h>


namespace tml {

stopwatch::stopwatch() {
    gettimeofday(&ts_start, NULL);
}

void stopwatch::start() {
    gettimeofday(&ts_start, NULL);
}

void stopwatch::click(int label) {
    timeval tv;
    gettimeofday(&tv, NULL);
    std::map<int, timeval>::iterator it = time_map.find(label);
    if (it == time_map.end()) {
        time_map.insert(std::pair<long, timeval>(label, tv));        
    } else {
        it->second = tv;
    }
}

long stopwatch::seconds(int label) {
    std::map<int, timeval>::iterator it = time_map.find(label);
    if (it == time_map.end()) {
        click(label);
        return seconds(label);
    }
    long time_cost = (it->second.tv_sec - ts_start.tv_sec) * 1000000 + (it->second.tv_usec - ts_start.tv_usec);
    return time_cost / 1000000;
}

long stopwatch::miliseconds(int label) {
    std::map<int, timeval>::iterator it = time_map.find(label);
    if (it == time_map.end()) {
        click(label);
        return seconds(label);
    }
    long time_cost = (it->second.tv_sec - ts_start.tv_sec) * 1000000 + (it->second.tv_usec - ts_start.tv_usec);
    return time_cost / 1000;    
}
}

