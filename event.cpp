/* event.cpp : provides timers and buttons for statemaschine
 * Copyright (C) 2019 Felix Haschke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "event.h"

Event::Event()
    : on_entry_(false), flags_(0), button_up_(PortPin(PortPin::C, 2), 5), button_down_(PortPin(PortPin::C, 1), 5),
      button_right_(PortPin(PortPin::C, 3), 5), button_left_(PortPin(PortPin::C, 0), 5),
      button_stop_(PortPin(PortPin::B, 1), 5)

{
}

void Event::processDebounce()
{
	buttonUp().processDebounce();
	buttonDown().processDebounce();
	buttonRight().processDebounce();
	buttonLeft().processDebounce();
	buttonStop().processDebounce();
}

void Event::checkButtons()
{
	buttonUp().checkChange();
	buttonDown().checkChange();
	buttonRight().checkChange();
	buttonLeft().checkChange();
	buttonStop().checkChange();
}

void Event::clear()
{
	buttonUp().clear();
	buttonDown().clear();
	buttonRight().clear();
	buttonLeft().clear();
	buttonStop().clear();
	unsigned int t = millis();
	for (int i = 0; i < timers_.size(); i++)
	{
		timers_.itemAt(i).clearOverflow();
	}
	on_entry_ = false;
	overflow_ = false;
}

bool Event::process()
{
	processTimers();
	return (flag(Event::ProcessEveryCycle) || (flag(Event::ProcessPinChanges) && controlButtonChanged()) ||
	        (flag(Event::ProcessTimerOverflows) && overflow_) ||
	        (/*flag(Event::ProcessStop) && */ buttonStop().changed()));
}

bool Event::processTimers()
{
	unsigned long t = millis();
	for (int i = 0; i < timers_.size(); i++)
	{
		if (timers_.itemAt(i).process(t))
		{
			overflow_ = true;
		}
	}
	return overflow_;
}

void Event::addTimer(unsigned long interval)
{
	Timer t(interval);
	timers_.append(t);
}

Timer &Event::timer(byte index)
{
	return timers_.itemAt(index);
}

bool Event::overflow(byte &index)
{
	timers_.itemAt(index).overflow();
}

void Event::removeTimer(byte &index)
{
	timers_.remove(index);
}

void Event::removeAllTimers()
{
	timers_.removeAll();
}
