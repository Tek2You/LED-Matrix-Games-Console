#include "tetris_sm.h"


#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&NameTagSM::s)); \
	process(ON_ENTRY); \
	}

TetrisSM::TetrisSM()
   :StateMachine(STATE_CAST(&TetrisSM::stateDefault))
{

}

void TetrisSM::stateDefault(byte event)
{

}
