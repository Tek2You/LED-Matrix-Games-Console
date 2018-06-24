#include "avr.h"
#include "tetris_sm.h"

TetrisSM sm;

int main(void)
{
	// init pin change interrup for buttons
	PCMSK1 |= INPUT_MASK; // select mask
	PCICR |= PCIE1; // enable mask1
	sei(); // enable global interrupts
	while(1){
		byte event;
		sm.process(event);
	}
	return 0;
}
