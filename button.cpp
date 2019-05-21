/* button.cpp : Library for query and debounce of a button
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
#include "button.h"

Button::Button(PortPin pin, const unsigned int debounce_time) : pin_(pin), debounce_time_(debounce_time)
{
	pin_.input();
	pin_.set();
}

void Button::processDebounce()
{
	checkChange();
	if (state_ & (1 << InDebounce))
	{
		if (++count_ >= debounce_time_)
		{
			count_ = 0;
			bitToggle(state_, State); // only toggle bit because it changed surely(else we need to read)
			bitSet(state_, Changed);
		}
	}
}

void Button::checkChange()
{
	if (pin_.read() == state()) // state is already inverted, so if state changed it will pass
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
