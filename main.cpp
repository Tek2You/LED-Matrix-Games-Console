#include "avr.h"
#include "game_sm.h"
#include "display.h"

FUSES = {
   LFUSE_DEFAULT | (byte)~FUSE_CKDIV8, // run at 8MHz
   HFUSE_DEFAULT & FUSE_EESAVE, // protect EEPROM from erase
   EFUSE_DEFAULT,
};

Display dp(16,8);
GameSM sm(&dp,255);

byte button_event;
int counter = 0;

byte have_input = 0;
byte input_count = 0;

void initGame(){
	// init pin change interrup for buttons
	DDRC &= ~INPUT_MASK;
	PORTC |= INPUT_MASK;
	PCMSK1 |= INPUT_MASK; // select mask
	PCICR |= (1 << PCIE1); // enable mask1
	init();

	// init timer
	TIMSK1 |= (1 << TOIE1);
	TCCR1B |= /*(1 << ICNC1) |*/ (1 << CS10);


	sei(); // enable global interrupts
}

int main(void)
{
	bitSet(DDRB,1);
//	bitSet(PORTB,1);
	initGame();
	while(1){
		dp.show();
		if(have_input || counter++ >= 0x2FF){
			dp.disable();
			counter = 0;
			sm.processStateMaschine((have_input | (~PINC & INPUT_MASK)));
		}
		have_input = 0;
	}
	return 0;
}

ISR(PCINT1_vect){
	if(input_count)
		return;
	input_count = 1;
}

ISR(TIMER1_OVF_vect){
	if(input_count && input_count++ == 5){
		input_count = 0;
	}
	if(input_count == 2)
		have_input = CHANGE;
}


