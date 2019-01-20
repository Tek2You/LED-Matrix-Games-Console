/* statemaschine.h : Template statemaschine library
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

template <class t>
class StateMachine
{
public:
	typedef void (StateMachine::*State)(t);
	StateMachine(State initial) : current_state(initial)
	{
	}

	inline void process(t event)
	{
		(this->*current_state)(event);
	}
	inline void setState(State state)
	{
		current_state = state;
	}

private:
	State current_state;
};

#define STATE_CAST(s) static_cast<StateMachine::State>(s)
#define TRANSITION(s) setState(STATE_CAST(s))
