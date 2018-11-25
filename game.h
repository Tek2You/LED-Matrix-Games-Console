#pragma once
#include "avr.h"
#include "display.h"
#include "avr/eeprom.h"

class Game
{
public:
	Game(Display * display);
	virtual ~Game();
	virtual void start() = 0;
	virtual bool process(byte& event)=0;
	virtual void clear()=0;
	virtual void reset()=0;

	virtual void setSpeed(byte v) = 0;

	virtual unsigned int points() const =0;
	virtual bool isNewHighscore() const{
		return is_new_highscore_;
	}

protected:
	virtual void render()=0;
	Display * display_;
	bool is_new_highscore_ = false;
};
