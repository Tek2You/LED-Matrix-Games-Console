#include "event.h"

Event::Event() : event_(0), flags_(0)
{
}

bool Event::hasPressed() const
{
	return (buttonDownChanged() && buttonDownState()) ||
	       (buttonLeftChanged() && buttonLeftState()) ||
	       (buttonRightChanged() && buttonRightState()) || (buttonUpChanged() && buttonUpState());
}

void Event::setButtonUpState(const bool state)
{
	if (state != buttonUpState())
	{
		bitSet(event_, 6);
		bitWrite(event_, 2, state);
	}
}

void Event::setButtonDownState(const bool state)
{
	if (state != buttonDownState())
	{
		bitSet(event_, 5);
		bitWrite(event_, 1, state);
	}
}

void Event::setButtonRightState(const bool state)
{
	if (state != buttonRightState())
	{
		bitSet(event_, 7);
		bitWrite(event_, 3, state);
	}
}

void Event::setButtonLeftState(const bool state)
{
	if (state != buttonLeftState())
	{
		bitSet(event_, 4);
		bitWrite(event_, 0, state);
	}
}

void Event::clear()
{
	// call after processed all
	event_ &= ~(event_ & CHANGE);
	event_ &= ~ON_ENTRY;
	unsigned int t = millis();
	for (int i = 0; i < timers_.size(); i++)
	{
		timers_.itemAt(i).clearOverflow();
	}
	overflow_ = false;
}

bool Event::process()
{
	processTimers();
	return (flag(Event::ProcessEveryCycle) || (flag(Event::ProcessPinChanges) && changed()) ||
	        (flag(Event::ProcessTimerOverflows) && overflow_));
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
