#pragma once
#include "statemachine.h"
#include "display.h"
#include "game.h"

#define ON_ENTRY bit(7)
#define CHANGE   bit(6)
// these are the actual input pins (of PINC)
#define BTN_LEFT bit(0)
#define BTN_DOWN bit(1)
#define BTN_ROTATE  bit(2)
#define BTN_RIGHT bit(3)

#define TIMEOUT bit(4)
// mask of all input pins
#define INPUT_MASK (BTN_DOWN | BTN_LEFT | BTN_RIGHT | BTN_ROTATE)

class Display;
class GameSM : public StateMachine
{
public:
	inline void * operator new(size_t size)
	{
	  return malloc(size);
	}

	inline void operator delete(void * ptr)
	{
	  free(ptr);
	}

	GameSM(Display * display, byte speed);
	void processStateMaschine(byte event);


private: // states
	class MenuItem{
	public:
		void init(byte num, byte initial){
			num_ = num;
			value_ = initial;
		}
		byte advance(byte event);
		static byte advance(byte event, char &item, const char num, const char min = 0);
		char value_;
	private:
		char num_;
	};


	void stateDefault(byte event);
	void stateGame(byte event);
	void stateSettingsMenu(byte event);
	void stateShowResult(byte event);

	enum ProcessCriterum{
		EVER = 1 << 0,
		TIMER = 1 << 1,
		PCINT = 1 << 2
	};


	byte process_criterium_;
	unsigned long process_timer_process_time_;

	Display * display_;
	Game * game_;

	char language_ = 0;
	char speed_;
	char step_counter_ = 0;
};
