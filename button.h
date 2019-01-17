#pragma once
#include "Vportpin.h"
#include "avr.h"

class Button
{
public:
	Button(PortPin pin, byte debounce_time);
	void processDebounce();
	void checkChange();
	inline bool changed() const
	{
		return bitRead(state_, Changed);
	}
	inline bool state() const
	{
		return bitRead(state_, State);
	}
	inline bool pressed() const
	{
		return state() && changed();
	}
	inline bool clear()
	{
		bitClear(state_, Changed);
	}

private:
	enum ButtonSpecifications
	{
		State = 0,
		Changed = 1,
		InDebounce = 2,
	};

	byte state_;
	byte count_;
	PortPin pin_;
	const byte debounce_time_;
};
