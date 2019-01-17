#include "game.h"
#include "operators.h"

Game::Game(Display *display) : display_(display), is_new_highscore_(false), stop_state_(false)
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
	if (event->buttonStop().pressed())
	{
		stop_state_ = !stop_state_;
		if (stop_state_)
		{
			onStop(event);
		}
		else
		{
			onContinue(event);
		}
	}
	return output;
}

void Game::onStop(Event *event)
{
	display_->clear();
	display_->setIcon(0x0000000024242424, 2); // update
}

void Game::onContinue(Event *event)
{
	render();
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
