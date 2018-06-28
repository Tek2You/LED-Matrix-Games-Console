#include "game_sm.h"


#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&NameTagSM::s)); \
	process(ON_ENTRY); \
	}

TetrisSM::TetrisSM(Display *display)
   :StateMachine(STATE_CAST(&TetrisSM::stateDefault)), display_(display)
{
	Game game(display);
}

void TetrisSM::stateDefault(byte event)
{

}
