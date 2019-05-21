/* event.cpp : Library provides timers and buttons for statemaschine
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

#include "event.h"

Event::Event()
	 : on_entry_(false), flags_(0), button_up_(PortPin(PortPin::C, 2), 100), button_down_(PortPin(PortPin::C, 1), 100),
		button_right_(PortPin(PortPin::C, 3), 100), button_left_(PortPin(PortPin::C, 0), 100),
		button_stop_(PortPin(PortPin::B, 1), 100)
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
	for (Trigger *t : triggers_)
	{
		t->clear();
	}
	on_entry_ = false;
	triggered_ = false;
}

bool Event::process()
{
	processTriggers();
	return (flag(Event::ProcessEveryCycle) || (flag(Event::ProcessPinChanges) && controlButtonChanged()) ||
			  (flag(Event::ProcessTriggers) && triggered_) || (flag(Event::ProcessStop) && buttonStop().changed()));
}

bool Event::processTriggers()
{
	unsigned long time = millis();
	for (Trigger *t : triggers_)
	{
		if (t->process(time))
		{
			triggered_ = true;
		}
	}
	return triggered_;
}

void Event::addTrigger(Trigger *trigger) { triggers_.append(trigger); }

Trigger *Event::trigger(const byte index) { return triggers_.itemAt(index); }

bool Event::overflow(const byte index) { triggers_.itemAt(index)->triggered(); }

void Event::removeTimer(const byte index)
{
	delete triggers_.itemAt(index);
	triggers_.remove(index);
}

void Event::removeAllTimers()
{
	for (Trigger *t : triggers_)
	{
		delete t;
	}
	triggers_.removeAll();
}
