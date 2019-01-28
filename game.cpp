/* game.cpp : Abstract class for all games
 *            for managing the game from the statemaschine
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see http://www.gnu.org/licenses/.
 */

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
	static byte reset_count = 0;
	if (event->buttonStop().pressed())
	{
		stop_state_ = !stop_state_;
		if (stop_state_)
		{
			onStop(event);
			event->addTimer(400);
		}
		else
		{
			onContinue(event);
		}
	}
	else if (event->buttonStop().released())
	{
		event->timers_.removeLast();
	}
	else if (stop_state_)
	{
		if (event->timers_.last().overflow() && event->buttonStop().state())
		{
			event->timers_.last().setInterval(200);
			display_->setPixel(reset_count,0);
			reset_count++;
			if (reset_count >= 8)
			{
				reset_count = 0;
				event->timers_.removeLast();
				return true;
			}
			return false;
		}
	}
	// check if processing is required
	if (event->controlButtonChanged())
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

void Game::onStop(Event *event)
{
	display_->clear();
	// set stop icon
	display_->setIcon(0x0000242424240000, 4);
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
