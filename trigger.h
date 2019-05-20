#pragma once
#include "avr.h"
#include "operators.h"

class Trigger
{
public:
	Trigger();

	virtual bool triggered() const { return triggered_; }
	virtual bool process(const unsigned long &t) = 0;
	virtual bool process();
	virtual void stop() = 0;
	virtual void start() = 0;
	virtual void clear() { triggered_ = false; }

protected:
	virtual bool onTriggered() {}

	bool triggered_ = false;
};
