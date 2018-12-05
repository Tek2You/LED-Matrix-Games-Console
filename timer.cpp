#include "timer.h"

Timer::Timer(unsigned int interval) : interval_(interval)
{
	next_time_ = millis() + interval_;
}

bool Timer::process(const unsigned long &t)
{
	// overgive current time, if use more timers(effiziently)
	if (next_time_ <= t)
	{
		next_time_ = t + interval_;
		return true;
	}
	return false;
}

bool Timer::process()
{
	unsigned long t = millis();
	return process(t);
}

void Timer::start()
{
	next_time_ = millis() + interval_;
}

void Timer::stop()
{
	next_time_ = 0xFFFFFFFF;
}

unsigned int Timer::interval() const
{
	return interval_;
}

void Timer::setInterval(unsigned int interval)
{
	interval_ = interval;
}
