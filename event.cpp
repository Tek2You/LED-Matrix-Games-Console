#include "event.h"

Event::Event() : event_(0) {}

void Event::setButtonUpState(bool state)
{
	if (state != buttonUpState())
	{
		bitSet(event_, 6);
		bitWrite(event_, 2, state);
	}
}

void Event::setButtonDownState(bool state)
{
	if (state != buttonDownState())
	{
		bitSet(event_, 5);
		bitWrite(event_, 1, state);
	}
}

void Event::setButtonRightState(bool state)
{
	if (state != buttonRightState())
	{
		bitSet(event_, 7);
		bitWrite(event_, 3, state);
	}
}

void Event::setButtonLeftState(bool state)
{
	if (state != buttonLeftState())
	{
		bitSet(event_, 4);
		bitWrite(event_, 0, state);
	}
}

void Event::clear()
{
	event_ &= ~(event_ & CHANGE);
	event_ &= ~ON_ENTRY;
}

bool Event::processTimers()
{
	unsigned int t = millis();
	for (int i = 0; i < timers_.size(); i++)
	{
		TimerItem timer = timers_.itemAt(i);
		timer.overflow_ = timer.process(t);
	}
}

void Event::addTimer(byte &index, unsigned int interval)
{
	TimerItem t(interval);
	timers_.append(t);
}

Timer Event::getTimer(byte &index)
{
	return timers_.itemAt(index);
}

bool Event::takeOverflow(byte &index)
{
	TimerItem t = timers_.itemAt(index);
	if (t.overflow_)
	{
		t.overflow_ = false;
		return true;
	}
	return false;
}
void Event::removeTimer(byte &index)
{
	timers_.remove(index);
}

void Event::removeAllTimers()
{
	timers_.removeAll();
}
