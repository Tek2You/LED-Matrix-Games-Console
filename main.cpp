#include "avr.h"
#include "game_sm.h"
#include "display.h"
#include "avr/wdt.h"
FUSES = {
   LFUSE_DEFAULT | (byte)~FUSE_CKDIV8, // run at 8MHz
   HFUSE_DEFAULT & FUSE_EESAVE, // protect EEPROM from erase
   EFUSE_DEFAULT,
};

Display dp(16,8);
GameSM sm(&dp);

byte button_event;
int counter = 0;

byte have_input = 0;
byte debounce_count[4] =  {0,0,0,0};
byte button_states, button_transitional_states, prev_button_transitional_states;

void initGame(){
	// init pin change interrup for buttons
	DDRC &= ~INPUT_MASK;
	PORTC |= INPUT_MASK;
	PCMSK1 |= INPUT_MASK; // select mask
	PCICR |= (1 << PCIE1); // enable mask1
	init();

	TCNT2 = 200;
	TCCR2A |= (1 << WGM21);
	TIMSK2 |= (1 << OCIE2A);
	TCCR2B |= (1 << CS20);
	sei(); // enable global interrupts
}

int main(void)
{
	bitSet(DDRB,1);
	initGame();
	wdt_enable(WDTO_15MS);
	while(1){
		wdt_reset();
		dp.show();
		if(have_input || counter++ >= 0xFF){
			dp.update();
			dp.disable();
			counter = 0;
			sm.processStateMaschine(have_input | button_states);
		}
		have_input = 0;
	}
	return 0;
}
void check_buttons();

ISR(PCINT1_vect){
	check_buttons();
}

ISR(TIMER2_COMPA_vect){
	TCNT2 = 200;
	check_buttons();
	for(int i = 0; i < 4; i++){
		if(debounce_count[i] && debounce_count[i]++ == 20){
			debounce_count[i] = 0;
			bitWrite(button_states,i,bitRead(button_transitional_states,i));
			have_input = CHANGE;
		}
	}
}

void check_buttons(){
	button_transitional_states = ~PINC & INPUT_MASK;
	byte change = button_transitional_states ^ prev_button_transitional_states;
	for(int i = 0; i < 4; i++){
		if(bitRead(change,i)){
			debounce_count[i] = 1;
		}
	}
	prev_button_transitional_states = button_transitional_states;
}


