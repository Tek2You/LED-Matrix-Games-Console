/* tetris.h : Tetris game library
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
#include "tetromino.h"
#include "staticlist.h"
#include "buttonautotrigger.h"

class Tetris : public Game
{
public:
	Tetris(Display *display);
	~Tetris();

	void start(Event *event);
	void setSpeed(const byte v) override
	{
		speed_ = v;
	}

	// score functions
	unsigned int score() const
	{
		return points_;
	}
	static unsigned int highscore()
	{
		return highscore_;
	}
	static void resetHighscore();

protected:
	// game interface
	bool onButtonChange(Event *event) override;
	bool onTriggered(Event *event) override;
	void onStop(Event *event) override;
	void onContinue(Event *event) override;
	void render();

private:
	// private functions only for tetris
	void clear();
	bool rotate();
	void move(const ButtonAutoTrigger::Direction dir);
	bool tick(Event *event);

	bool clearFullRows(Event *event);
	bool rowsFull() const;

	bool newTetromino();
	void takeOverTetromino();

	Timer * step_timer_;
	Timer * blink_timer_;
	ButtonAutoTrigger * move_trigger_;

	byte speed_;
	enum SpeedFlag{
		StepInterval = 0,
		DownInterval = 1,
		FirstMoveInterval = 2,
		MoveInterval = 3,
		BlinkInterval = 4
	};

	unsigned int readSpeed(const SpeedFlag flag) const;

	StaticList<byte> field_; // field without current tetromino
	Tetromino tetromino_;
	unsigned int points_ = 0;
	static unsigned int highscore_;

	enum BlinkCycle {
		DEFAULT = 0,
		INIT_BLINK = 1,
		BLINK_OFF_1 = 2,
		BLINK_ON_1 = 3,
		BLINK_OFF_2 = 4,
		BLINK_ON_2 = 5,
		BLINK_OFF_3 = 6,
		FINISHED_CURRENT = 7
	};
	byte blink_cycle_;
	byte blink_start_row_;
	byte blink_end_row_;
	void clearFullRowsImmediately();
};
