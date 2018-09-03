#include "avr.h"
#include "game_sm.h"
#include "display.h"



Display dp(16,8);
GameSM sm(&dp);


int main(void)
{
	// init pin change interrup for buttons
	PCMSK1 |= INPUT_MASK; // select mask
	PCICR |= PCIE1; // enable mask1
	sei(); // enable global interrupts
	while(1){
		byte event;
		sm.process(event);
		dp.show();
	}
	return 0;
}

ISR(PCINT1_vect){

}

