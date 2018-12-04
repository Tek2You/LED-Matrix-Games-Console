#pragma once
#include "avr.h"

template <class t>
class StateMachine
{
public:
	typedef void (StateMachine::*State)(t);
	StateMachine(State initial) : current_state(initial) {}

	inline void process(t event) {
		(this->*current_state)(event);
	}
	inline void setState(State state) {
		current_state = state;
	}

private:
	State current_state;
};

#define STATE_CAST(s) static_cast<StateMachine::State>(s)
#define TRANSITION(s) setState(STATE_CAST(s))
