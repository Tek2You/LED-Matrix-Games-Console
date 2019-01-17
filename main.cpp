#include "avr.h"
#include "avr/wdt.h"
#include "display.h"
#include "event.h"
#include "game_sm.h"

FUSES = {
    LFUSE_DEFAULT | (byte)~FUSE_CKDIV8, // run at 8MHz
    HFUSE_DEFAULT &FUSE_EESAVE,         // protect EEPROM from erase
    EFUSE_DEFAULT,
};

Event event;
Display dp(16, 8);
GameSM sm(&dp, &event);

byte button_event;
int counter = 0;

bool have_input = false;
byte debounce_count[4] = {0, 0, 0, 0};
byte button_transitional_states;

void initHardware();

int main(void)
{
	initHardware();
	//	wdt_enable(WDTO_120MS);

	// containing loop for the main programm
	while (1)
	{
		wdt_reset();
		dp.show();
		if (event.controlButtonPressed() || counter++ >= 0xFF) // pre-devider for proccing function
		{
			counter = 0;
			dp.update();
			dp.disable();
			if (event.process())
			{
				sm.process(&event);
				event.clear();
			}
			have_input = false;
		}
	}
	return 0;
}
void check_buttons();

// pin change interrupt routine
ISR(PCINT1_vect)
{
	event.checkButtons();
}

ISR(PCINT0_vect)
{
	event.checkButtons();
}

// debounce check counter.
// checks button change and takes over the button states if doubounce count completed
ISR(TIMER2_COMPA_vect)
{
	TCNT2 = 200;
	event.process();
	event.processDebounce();
	//	check_buttons();
	//	for (int i = 0; i < 4; i++)
	//	{
	//		if (debounce_count[i] && debounce_count[i]++ == 20)
	//		{
	//			debounce_count[i] = 0;
	//			event.setButtonLeftState(bitRead(button_transitional_states, 0));
	//			event.setButtonDownState(bitRead(button_transitional_states, 1));
	//			event.setButtonUpState(bitRead(button_transitional_states, 2));
	//			event.setButtonRightState(bitRead(button_transitional_states, 3));
	//			have_input = true;
	//		}
	//	}
}
// checks any button change and enables if changed an debounce count for the considering button
// void check_buttons()
//{
//	byte tmp = ~PINC & INPUT_MASK;
//	byte change = button_transitional_states ^ tmp;
//	button_transitional_states = tmp;
//	for (int i = 0; i < 4; i++)
//	{
//		if (bitRead(change, i))
//		{
//			debounce_count[i] = 1;
//		}
//	}
//}

void initHardware()
{
	// init pin change interrup for buttons
	PCMSK1 |= 0x0F; // select mask
	PCMSK0 |= (1 << PCINT0);
	PCICR |= (1 << PCIE1) | (1 << PCIE0); // enable mask1
	init();

	TCNT2 = 200;
	TCCR2A |= (1 << WGM21);
	TIMSK2 |= (1 << OCIE2A);
	TCCR2B |= (1 << CS20);
	sei(); // enable global interrupts
}
