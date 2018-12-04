#include "avr.h"
#include "list.h"
#pragma once

#define ON_ENTRY bit(8)
// these are the actual input pins (of PINC)
#define BTN_LEFT bit(0)
#define BTN_DOWN bit(1)
#define BTN_UP bit(2)
#define BTN_RIGHT bit(3)
#define BTN_LEFT_CHANGE bit(4)
#define BTN_DOWN_CHANGE bit(5)
#define BTN_UP_CHANGE bit(6)
#define BTN_RIGHT_CHANGE bit(7)

#define TIMEOUT1 bit(9)
#define TIMEOUT2 bit(10)
// mask of all input pins
#define CHANGE \
	(BTN_LEFT_CHANGE | BTN_DOWN_CHANGE | BTN_UP_CHANGE | BTN_RIGHT_CHANGE)
#define INPUT_MASK (BTN_DOWN | BTN_LEFT | BTN_RIGHT | BTN_UP)

class Event
{
public:
	Event();
	inline bool buttonUpChanged() { return event_ & BTN_UP_CHANGE; }
	const inline bool buttonDownChanged() { return event_ & BTN_DOWN_CHANGE; }
	const inline bool buttonRightChanged() { return event_ & BTN_RIGHT_CHANGE; }
	const inline bool buttonLeftChanged() { return event_ & BTN_LEFT_CHANGE; }
	const

	    inline bool
	    buttonUpState()
	{
		return event_ & BTN_UP;
	}
	const inline bool buttonDownState() { return event_ & BTN_DOWN; }
	const inline bool buttonRightState() { return event_ & BTN_RIGHT; }
	const inline bool buttonLeftState() { return event_ & BTN_LEFT; }
	const

	    inline bool
	    changed()
	{
		return event_ & CHANGE;
	}
	inline bool isPressed() { return event_ & INPUT_MASK; }

	void setButtonUpState(bool state);
	void setButtonDownState(bool state);
	void setButtonRightState(bool state);
	void setButtonLeftState(bool state);

	void clear();

	inline bool onEntry() { return event_ & ON_ENTRY; }
	inline void setOnEntry() { event_ |= ON_ENTRY; }
	inline bool timeOut1() { return event_ & TIMEOUT1; }
	inline bool timeOut2() { return event_ & TIMEOUT2; }
	int event_;
};
