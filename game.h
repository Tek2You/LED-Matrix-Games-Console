#pragma once
#include "avr.h"
#include "avr/eeprom.h"
#include "display.h"
#include "event.h"

class Game
{
public:
	Game(Display *display);
	virtual ~Game();
	virtual void start(Event *event) = 0;
	virtual bool process(Event *event);

	virtual void setSpeed(byte v) = 0;

	virtual unsigned int points() const = 0;
	virtual bool isNewHighscore() const
	{
		return is_new_highscore_;
	}

protected:
	virtual bool onButtonChange(Event *event);
	virtual bool onTimerOverflow(Event *event);

protected:
	virtual void render() = 0;
	Display *display_;
	bool is_new_highscore_ = false;
};
