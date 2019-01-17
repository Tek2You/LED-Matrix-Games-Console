#include "game.h"
#include "operators.h"

Game::Game(Display *display) : display_(display), is_new_highscore_(false)
{
}

Game::~Game()
{
}

bool Game::process(Event *event)
{
	bool output = false;
	if (event->controlButtonPressed())
	{
		if (onButtonChange(event))
			output = true;
	}
	if (event->generalOverflow())
	{
		if (onTimerOverflow(event))
			output = true;
	}
	return output;
}

bool Game::onButtonChange(Event *event)
{
}

bool Game::onTimerOverflow(Event *event)
{
}

extern "C" void __cxa_pure_virtual()
{
	while (1)
		;
}
