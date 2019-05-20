/* snake.h : Snake game library
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
#include "game.h"
#include "list.h"
#include "position.h"
#include "staticlist.h"

class Snake : public Game
{
public:
	Snake(Display *display);
	~Snake();

	void start(Event *event);


	void setSpeed(const byte v) override;

	// score functions
	unsigned int score() const { return body_.size() - 3; }
	static unsigned int highscore() { return highscore_; }
	static void resetHighscore();

protected:
	// game interface
	bool onButtonChange(Event *event) override;
	bool onTimerOverflow(Event *event) override;
	void onStop(Event *event) override;
	void onContinue(Event *event) override;
	void render();

private:
	// local declared methods
	// checks if the snake had touched the eat
	bool eat(const Pos pos);
	// moves the snake by one point
	bool tick();
	// checks if the current position is valid
	bool isValid(const Pos &pos) const;
	// moves the position into the game field and checks valibility11
	bool validate(Pos &pos);

	// datatype abled to store a direction
	enum Direction
	{
		UP,
		RIGHT,
		DOWN,
		LEFT,
		START
	};

	// interval speed
	unsigned int interval_;
	// saves the current direction of the snake
	Direction direction_;
	// saves the wanted direction of the snake
	// Its only to ensure, that the snake is only rotated by 90° and not more
	Direction new_direction_;
	// list storing the hole body data
	StaticList<SmartPos> body_;
	// position of the current point of eat
	Pos eat_pos_;
	// highscore of all runs
	static unsigned int highscore_;
};
