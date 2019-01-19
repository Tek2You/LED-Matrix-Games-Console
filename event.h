#pragma once
#include "avr.h"
#include "button.h"
#include "list.h"
#include "timer.h"

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
		return buttonUp().changed() || buttonDown().changed() || buttonRight().changed() || buttonLeft().changed();
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
		ProcessStop = (1 << 3)
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
