//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_TIMEUTILS_H
#define CBUNGEE_TIMEUTILS_H

#include <cstdint>
#include <sys/time.h>

class TimeUtils {
public:
    static uint64_t getCurrentTimeMillis(){
        return getCurrentTimeMicros()/1000;
    }
    static uint64_t getCurrentTimeMicros(){
        struct timeval tp;
        gettimeofday(&tp, NULL);
        uint64_t ms = tp.tv_sec * 1000*1000 + tp.tv_usec;
        return ms;
    }
};
#endif //CBUNGEE_TIMEUTILS_H
