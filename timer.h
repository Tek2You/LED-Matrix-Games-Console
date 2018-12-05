#pragma once
#include "avr.h"

class Timer
{
public:
    Timer(unsigned int interval = 0);
    bool process(const unsigned long &t);
    bool process();

    void start();
    inline void restart() { start(); }
    inline void stop();

    unsigned int interval() const;
    void setInterval(unsigned int interval);

private:
    unsigned long next_time_;
    unsigned int interval_;
};
