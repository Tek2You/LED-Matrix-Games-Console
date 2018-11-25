#pragma once
#include "statemachine.h"
#include "display.h"
#include "tetris.h"
#include "snake.h"
#include "jump.h"


#define ON_ENTRY bit(7)
#define CHANGE   bit(6)
// these are the actual input pins (of PINC)
#define BTN_LEFT bit(0)
#define BTN_DOWN bit(1)
#define BTN_UP  bit(2)
#define BTN_RIGHT bit(3)

#define TIMEOUT1 bit(4)
#define TIMEOUT2 bit(5)
// mask of all input pins
#define INPUT_MASK (BTN_DOWN | BTN_LEFT | BTN_RIGHT | BTN_UP)

class Display;
class GameSM : public StateMachine
{
public:
	GameSM(Display * display);
	void processStateMaschine(byte event);
private:
	// states
	void stateDefault(byte event);
	void stateTetris(byte event);
	void stateSnake(byte event);
	void stateJump(byte event);
	void stateSettingsMenu(byte event);
	void stateGameOver(byte event);
	void stateSpeedMenu(byte event);
	void stateLanguageMenu(byte event);
	void stateLoadEffect(byte event);
	void stateHighscoreMenu(byte event);
	void stateResetMenu(byte event);

	class MenuItem{
	public:
		void init(char num, char initial){
			num_ = num;
			value_ = initial;
		}
		void init(char num){
			num_ = num;
		}

		enum Button{
			NO_BTN = 0,
			UP_BTN = 1,
			DOWN_BTN = 2,
		};

		Button advance(byte event);
		static Button advance(byte event, char &item, const char num, const char min = 0);
		char value_ = 0;

	private:
		char num_ = 0;
	};

	enum ProcessCriterum{
		EVER = 1 << 0,
		TIMER1 = 1 << 1,
		TIMER2 = 1 << 2,
		PCINT = 1 << 3
	};

	enum Language{
		EN = 0,
		DE = 1,
	};

	byte process_criterium_;
	unsigned long process_timer1_;
	unsigned long process_timer2_;

	Display * display_;
	Game * game_; // pointer to the current game

	Language language_ = EN;
	byte speed_ = 0;
	State load_following_state_; // state load effect need a pointer to the state after its
};
