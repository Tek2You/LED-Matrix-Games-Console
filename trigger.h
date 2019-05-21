/* trigger.h : abstract class for Timers or other devices
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
	inline void restart() { start(); }
	virtual void clear() { triggered_ = false; }

protected:
	virtual void onTriggered() {}

	bool triggered_ = false;
};
