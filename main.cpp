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

int counter = 0;

bool process_debounce = false;
bool check_buttons = false;

void initHardware();

int main(void)
{
	initHardware();
	wdt_enable(WDTO_120MS);

	// containing loop for the main programm
	while (1)
	{
		wdt_reset();
		dp.show();
		if (process_debounce)
		{
			process_debounce = false;
			dp.disable();
			event.processDebounce();
		}

		if (check_buttons)
		{
			check_buttons = false;
			event.checkButtons();
		}

		if (event.controlButtonPressed() || event.buttonStop().pressed() ||
		    counter++ >= 0xFF) // pre-devider for proccing function
		{
			counter = 0;
			dp.update();
			dp.disable();
			if (event.process())
			{
				sm.process(&event);
				event.clear();
			}
		}
	}
	return 0;
}

// pin change interrupt routine
ISR(PCINT1_vect)
{
	check_buttons = true;
}

ISR(PCINT0_vect)
{
	check_buttons = true;
}

// debounce check counter.
// checks button change and takes over the button states if doubounce count completed
ISR(TIMER2_COMPA_vect)
{
	TCNT2 = 200;
	process_debounce = true;
}

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
