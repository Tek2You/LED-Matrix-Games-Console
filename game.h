#pragma once
#include "avr.h"
#include "display.h"

class Game
{
public:
    Game(Display * display);
    virtual ~Game();
    virtual void start() = 0;
	 virtual bool process()=0;
    virtual void clear()=0;
    virtual void reset()=0;

    virtual bool right()=0;
    virtual bool left()=0;
    virtual bool up()=0;
    virtual bool down()=0;

	 virtual unsigned int getPoints()=0;

protected:
    virtual void render()=0;
    Display * display_;
};
