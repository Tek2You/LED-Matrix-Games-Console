#pragma once
#include "statemachine.h"
#include "display.h"
#include "game.h"

#define ON_ENTRY bit(7)
#define CHANGE   bit(6)
// these are the actual input pins (of PINC)
#define BTN_LEFT bit(0)
#define BTN_DOWN bit(1)
#define BTN_FLIP  bit(2)
#define BTN_RIGHT bit(3)
// mask of all input pins
#define INPUT_MASK (BTN_DOWN | BTN_LEFT | BTN_RIGHT | BTN_FLIP)

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

	GameSM(Display * display);

private: // states
	void stateDefault(byte event);
	void stateGame(byte event);
	void stateSettingsMenu(byte event);

private:

	Display * display_;
	Game * game_;
	bool advance(byte event, char &item, const char num, const char min = 0);

	byte language_ = 0;
};
