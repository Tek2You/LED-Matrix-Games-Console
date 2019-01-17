#include "button.h"

Button::Button(PortPin pin, byte debounce_time) : pin_(pin), debounce_time_(debounce_time)
{
	pin_.input();
	pin_.set();
}

void Button::processDebounce()
{
	checkChange();
	if (state_ & (1 << InDebounce))
	{
		if (++count_ == debounce_time_)
		{
			count_ = 0;
			bitToggle(state_, State); // only toggle bit because it changed surely(else we need to read)
			bitSet(state_, Changed);
		}
	}
}

void Button::checkChange()
{
	if (pin_.read() != state())
	{
		if (!(state_ & (1 << InDebounce)))
		{
			bitSet(state_, InDebounce);
			count_ = 0;
		}
	}

	else if (state_ & (1 << InDebounce))
	{
		bitClear(state_, InDebounce);
	}
	return;
}
