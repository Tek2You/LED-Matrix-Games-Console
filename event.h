#include "avr.h"
#include "button.h"
#include "list.h"
#include "timer.h"
#pragma once

#define ON_ENTRY bit(8)
// these are the actual input pins (of PINC)
//#define BTN_LEFT bit(0)
//#define BTN_DOWN bit(1)
//#define BTN_UP bit(2)
//#define BTN_RIGHT bit(3)
//#define BTN_LEFT_CHANGE bit(4)
//#define BTN_DOWN_CHANGE bit(5)
//#define BTN_UP_CHANGE bit(6)
//#define BTN_RIGHT_CHANGE bit(7)

//#define TIMEOUT1 bit(9)
//#define TIMEOUT2 bit(10)
// mask of all input pins
//#define CHANGE (BTN_LEFT_CHANGE | BTN_DOWN_CHANGE | BTN_UP_CHANGE | BTN_RIGHT_CHANGE)
//#define INPUT_MASK (BTN_DOWN | BTN_LEFT | BTN_RIGHT | BTN_UP)

class Event
{
public:
	Event();

	inline Button &buttonUp()
	{
		return button_up_;
	}
	inline Button &buttonDown()
	{
		return button_down_;
	}
	inline Button &buttonRight()
	{
		return button_right_;
	}
	inline Button &buttonLeft()
	{
		return button_left_;
	}
	inline Button &buttonStop()
	{
		return button_stop_;
	}

	void processDebounce();
	void checkButtons();

	inline bool controlButtonChanged()
	{
		return buttonRight().changed() || buttonLeft().changed() || buttonRight().changed() || buttonLeft().changed();
	}
	inline bool controlButtonPressed()
	{
		return buttonUp().pressed() || buttonDown().pressed() || buttonRight().pressed() || buttonLeft().pressed();
	}

	void clear();

	const inline bool onEntry() const
	{
		return on_entry_;
	}
	inline void setOnEntry()
	{
		on_entry_ = true;
	}

	bool process();
	bool processTimers();
	void addTimer(unsigned long interval = 0);
	Timer &timer(byte index);
	bool overflow(byte &index);
	void removeTimer(byte &index);
	void removeAllTimers();

	enum Flags
	{
		ProcessEveryCycle = (1 << 0),
		ProcessPinChanges = (1 << 1),
		ProcessTimerOverflows = (1 << 2),

	};

	inline void clearFlags()
	{
		flags_ = 0;
	}
	inline void setFlag(Flags flag, bool set = true)
	{
		bitWrite(flags_, flag, set);
	}
	inline bool flag(Flags flag)
	{
		return bitRead(flags_, flag);
	}
	bool generalOverflow() const
	{
		return overflow_;
	}

private:
	bool on_entry_;
	Button button_up_;
	Button button_down_;
	Button button_left_;
	Button button_right_;
	Button button_stop_;
	List<Timer> timers_;
	byte flags_;
	bool overflow_ = false;
};
