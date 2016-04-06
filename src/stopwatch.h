// Name: stopwatch.h
// Description: a stopwatch class for time measure
// Author: zhangshiguang
// Email: andyzhshg@gmail.com
// Date: 2016-03

#ifndef __TML_STOP_WATCH__
#define __TML_STOP_WATCH__

#include <sys/time.h>
#include <map>
#include <limits.h>

namespace tml {
    class stopwatch {
    public:
        stopwatch();
        void start();
        void click(int label = INT_MIN);
        long seconds(int label = INT_MIN);
        long miliseconds(int label = INT_MIN);
    private:
        std::map<int, timeval> time_map;
        timeval ts_start;
    };
}

#endif //__TML_STOP_WATCH__
