#pragma once
#include "avr.h"
#include "display.h"
#include "avr/eeprom.h"
#include "event.h"

class Game
{
public:
	Game(Display * display);
	virtual ~Game();
	virtual void start() = 0;
	virtual bool process(Event * event)=0;
	virtual void clear()=0;
	virtual void reset()=0;

	virtual void setSpeed(byte v) = 0;

	virtual unsigned int points() const =0;
	virtual bool isNewHighscore() const{
		return is_new_highscore_;
	}

	virtual void onButtonUpChange(bool state);
	virtual void onButtonDownChange(bool state);
	virtual void onButtonRightChange(bool state);
	virtual void onButtonLeftChange(bool state);

protected:
	virtual void render()=0;
	Display * display_;
	bool is_new_highscore_ = false;
};
