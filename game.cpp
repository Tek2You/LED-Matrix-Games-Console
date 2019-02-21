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

Game::Game(Display *display, GameType game_type) : display_(display), game_type_(game_type), is_new_highscore_(false), stop_state_(false)
{
}

Game::~Game()
{
}

bool Game::process(Event *event)
{
	bool output = false;
	static bool first_released_ = false;
	static byte reset_count = 0;
	/* Stop Button
	 * Enable Stop directly after push
	 * Disable Stop after released after second
	 */
	if (event->buttonStop().pressed())
	{
		if (!stop_state_) // first time pressed -> stop game
		{
			stop_state_ = true;
			onStop(event);
			event->addTimer(500); // add timer for reset game
		}
		else // second time pressed
		{
			event->timers_.last().restart();
		}
		return false;
	}
	else if (event->buttonStop().released())
	{
		if (reset_count)
		{ // tryed to leave
			reset_count = 0;
			first_released_ = false;
			display_->setRow(15, 0);
			display_->show();
			event->timers_.last().setInterval(500);
		}
		if (stop_state_)
		{
			if (!first_released_) // released first time
			{
				first_released_ = true;
			}
			else // released second time -> continue game
			{
				first_released_ = false;
				stop_state_ = false;
				onContinue(event);
				event->timers_.removeLast();
				return false;
			}
		}
	}
	else if (stop_state_)
	{
		if (event->timers_.last().overflow() && event->buttonStop().state())
		{
			event->timers_.last().setInterval(200);
			event->timers_.last().restart();
			display_->setPixel(reset_count, 15);
			display_->show();
			reset_count++;
			if (reset_count >= 8)
			{
				stop_state_ = false;
				first_released_ = false;
				reset_count = 0;
				event->timers_.removeLast();
				return true;
			}
		}
		return false;
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
	display_->text1_.setOffset(0);
	display_->text1_.setNumber(this->score(),false);
	display_->setIcon(0x0000242424240000, 8,false);
	display_->show();
}

void Game::onContinue(Event *event)
{
	display_->text1_.clear();
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
