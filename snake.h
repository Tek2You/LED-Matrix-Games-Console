/* snake.cpp : Snake game library
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

class Snake : public Game
{
public:
	Snake(Display *display);
	~Snake();
	void start(Event *event);
	void clear();

	unsigned int points() const
	{
		return body_.size() - 3;
	}
	void setSpeed(byte v) override;

	static unsigned int highscore();
	static void resetHighscore();

protected:
	bool onButtonChange(Event *event) override;
	bool onTimerOverflow(Event *event) override;
	void onStop(Event *event) override;
	void onContinue(Event *event) override;

private:
	void render();
	bool eat();
	bool isValid(const Pos &pos);
	bool validate(Pos &pos);
	bool move();

	enum Direction
	{
		UP,
		RIGHT,
		DOWN,
		LEFT,
		START
	};

	int period_;

	// snake values
	Direction direction_;
	Direction new_direction_;
	Pos move_vect_;
	List<Pos> body_;
	Pos eat_pos_;
	static unsigned int highscore_;
};
