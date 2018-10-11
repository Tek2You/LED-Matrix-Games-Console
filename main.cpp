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
byte counter = 0;

void initGame(){
	// init pin change interrup for buttons
	PCMSK1 |= INPUT_MASK; // select mask
	PCICR |= PCIE1; // enable mask1
	init();

	// init timer 1
	sei(); // enable global interrupts
}

int main(void)
{
	bitSet(DDRB,1);
	bitSet(PORTB,1);
	bitClear(PORTB,1);
	initGame();
	while(1){
		dp.show();
		if(counter++ >= 0xFF){
			counter = 0;
			sm.process(button_event);
		}
	}
	return 0;
}

ISR(PCINT1_vect){
	unsigned long now = millis();
	static unsigned long prev;
	if(now - prev < 100)
		return;
	prev = now;
	button_event = PINC & INPUT_MASK;
}

