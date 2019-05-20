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

#pragma once
#include "avr.h"
#include "trigger.h"

class TimerNew : public Trigger
{
public:
	TimerNew(const unsigned int interval = 0);

	void setInterval(const unsigned int interval) { interval_ = interval; }
	unsigned int interval() const { return interval_; }

	// Trigger interface
	bool process(const unsigned long &t) override;
	void stop() override;
	void start() override;

private:
	unsigned int interval_;
	unsigned long next_time_;
};

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
	void setInterval(const unsigned int interval);

	bool overflow() const;
	void clearOverflow();

protected:
	virtual void onOverflow() {}

private:
	unsigned long next_time_;
	unsigned int interval_;
	bool overflow_ = false;
};
