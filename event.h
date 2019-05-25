/* event.h : Library provides timers and buttons for statemaschine
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
#include "button.h"
#include "list.h"
#include "timer.h"

class Event
{
public:
	Event();

	inline Button &buttonUp() { return button_up_; }
	inline Button &buttonDown() { return button_down_; }
	inline Button &buttonRight() { return button_right_; }
	inline Button &buttonLeft() { return button_left_; }
	inline Button &buttonStop() { return button_stop_; }

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

	enum EntryMode{
		NotOnEntry = 0,
		ForwardEntry = 1,
		BackwardEntry = 2,
	};

	inline const EntryMode onEntry() const { return entry_;}
	inline void setOnEntry(const EntryMode mode = ForwardEntry)
	{
		removeAllTriggers();
		entry_ = mode;
	}

	bool process();
	void addTrigger(Trigger *trigger);
	Trigger *trigger(const byte index);
	bool overflow(const byte index);
	void removeTrigger(const byte index);
	void removeAllTriggers();

	enum Flags
	{
		ProcessEveryCycle = (1 << 0),
		ProcessPinChanges = (1 << 1),
		ProcessTriggers = (1 << 2),
		ProcessStop = (1 << 3),
	};

	void setupGame() { flags_ = (Event::ProcessPinChanges | Event::ProcessTriggers | Event::ProcessStop); }

	void setupMenu() { flags_ = (Event::ProcessPinChanges | Event::ProcessStop); }

	inline void clearFlags() { flags_ = 0; }
	inline void setFlag(Flags flag, bool set = true)
	{
		if (set)
			flags_ |= flag;
		else
			flags_ &= ~flag;
	}
	inline bool flag(Flags flag) { return flags_ & flag; }
	bool generalOverflow() const { return triggered_; }

	List<Trigger *> triggers_;

private:
	bool processTriggers();

	EntryMode entry_;
	Button button_up_;
	Button button_down_;
	Button button_left_;
	Button button_right_;
	Button button_stop_;
	byte flags_;
	bool triggered_ = false;
};
