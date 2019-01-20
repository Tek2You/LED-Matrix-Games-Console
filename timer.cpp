/* timer.cpp : Timer library that puts on a basic timer function
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see http://www.gnu.org/licenses/.
 */

#include "timer.h"

Timer::Timer(unsigned long interval) : interval_(interval), overflow_(false), next_time_(0xFFFFFFFE)
{
	if (interval_ != 0)
	{
		next_time_ = millis() + interval_;
	}
}

bool Timer::process(const unsigned long &t)
{
	// overgive current time, if use more timers(effiziently)
	if (next_time_ <= t)
	{
		next_time_ = t + interval_;
		overflow_ = true;
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

unsigned int Timer::interval() const
{
	return interval_;
}

void Timer::setInterval(unsigned int interval)
{
	interval_ = interval;
}
