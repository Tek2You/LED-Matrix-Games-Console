/* game.h : Abstract class for all games
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

#pragma once
#include "avr.h"
#include "avr/eeprom.h"
#include "display.h"
#include "event.h"

class Game
{
public:
	enum GameType{
		TETRIS,
		SNAKE,
		DODGE,
		SPACE_INVADERS,
	};

	Game(Display *display, GameType game_type);
	virtual ~Game();

	// start the game
	virtual void start(Event *event) = 0;

	// process timer and buttons(no need to override)
	virtual bool process(Event *event);

	// set the speed of the game
	virtual void setSpeed(byte v) = 0;

	// score and highscore methods
	virtual unsigned int score() const = 0;
	virtual bool isNewHighscore() const
	{
		return is_new_highscore_;
	}

	GameType type(){
		return game_type_;
	}

protected:
	virtual bool onButtonChange(Event *event);
	virtual bool onTriggered(Event *event);
	virtual void onStop(Event *event);
	virtual void onContinue(Event *event);
	virtual void render() = 0;

	Display *display_;
	bool is_new_highscore_ = false;
	bool stop_state_;
	const GameType game_type_;
};
