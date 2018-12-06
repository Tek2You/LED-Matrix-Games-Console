#pragma once
#include "avr.h"

class Timer
{
public:
	Timer(unsigned long interval = 0);
	bool process(const unsigned long &t);
	bool process();

	void start();
	inline void restart() { start(); }
	inline void stop() { next_time_ = 0xFFFFFFFE; }

	unsigned int interval() const;
	void setInterval(unsigned int interval);

	bool overflow() { return overflow_; }
	void clearOverflow() { overflow_ = false; }

private:
	unsigned long next_time_;
	unsigned long interval_;
	bool overflow_ = false;
};
