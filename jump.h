/* jump.h : Jump game library
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
#include "display.h"
#include "game.h"
#include "position.h"

class Jump : public Game
{
public:
	Jump(Display *display);
	~Jump();

	void start(Event *event);

	unsigned int points() const;
	void setSpeed(const byte v);

	// highscore functions
	static unsigned int highscore();
	static void resetHighscore();

protected:
	// Game interface
	bool onButtonChange(Event *event) override;
	bool onTimerOverflow(Event *event) override;
	void onStop(Event *event) override;
	void onContinue(Event *event) override;
	void render();

private:
	int forward_period_, jump_period_;
	void newHind();
	bool isValid(const Pos pos);
	void forward();
	void jump(Event *event);
	byte *row(const byte n);

	bool man_state_ = false;
	Pos man_pos_;
	byte next_hind_;
	byte *field_;
	char current_field_start_;
	bool is_jumping_ = false;
	byte jump_count_ = 0;
	unsigned int score_ = 0;
	static unsigned int highscore_;
	byte jump_height_;
	byte jump_lenght_;

	const SmartPos man_points[6] = {SmartPos(0, 7), SmartPos(0, 5), SmartPos(1, 6),
											  SmartPos(1, 5), SmartPos(1, 4), SmartPos(2, 5)};
	const SmartPos man_moving_points[2] = {SmartPos(1, 7), SmartPos(2, 7)};

	struct Hind
	{
		SmartPos positions[4];
	};

	const Hind hinds_[7] = {{{SmartPos(0, 6), SmartPos(1, 7), SmartPos(1, 6), SmartPos(2, 6)}},
									{{SmartPos(0, 7), SmartPos(0, 6), SmartPos(0, 5), SmartPos(0, 7)}},
									{{SmartPos(0, 7), SmartPos(0, 6), SmartPos(1, 7), SmartPos(1, 6)}},
									{{SmartPos(0, 7), SmartPos(4, 6), SmartPos(0, 7), SmartPos(0, 7)}},
									{{SmartPos(0, 7), SmartPos(1, 7), SmartPos(1, 6), SmartPos(2, 7)}},
									{{SmartPos(0, 7), SmartPos(0, 6), SmartPos(0, 5), SmartPos(0, 7)}},
									{{SmartPos(0, 7), SmartPos(0, 7), SmartPos(0, 7), SmartPos(0, 7)}}};
};
