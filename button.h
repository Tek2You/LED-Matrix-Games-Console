/* button.h : Library for query and debounce of a button
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

#pragma once
#include "Vportpin.h"
#include "avr.h"

class Button
{
public:
	Button(PortPin pin, const unsigned int debounce_time);
	void processDebounce();
	void checkChange();
	inline bool changed() const { return bitRead(state_, Changed); }
	inline bool state() const { return bitRead(state_, State); }
	inline bool pressed() const { return state() && changed(); }
	inline bool released() const { return (!state()) && changed(); }
	inline bool clear() { bitClear(state_, Changed); }

private:
	enum ButtonSpecifications
	{
		State = 0,
		Changed = 1,
		InDebounce = 2,
	};

	byte state_;
	unsigned int count_;
	PortPin pin_;
	const unsigned int debounce_time_;
};
