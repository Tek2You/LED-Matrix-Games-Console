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

class Tetris : public Game
{
public:
	Tetris(Display *display);
	~Tetris();

	void start(Event *event);
	void setSpeed(const byte v) override;

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
	bool onTimerOverflow(Event *event) override;
	void onStop(Event *event) override;
	void onContinue(Event *event) override;
	void render();

private:
	// private functions only for tetris
	void clear();
	bool rotate();
	bool right();
	bool left();
	bool tick(Event *event);

	bool clearFullRows(Event *event);
	bool rowsFull() const;

	bool newTetromino();
	void takeOverTetromino();
	// random tetromino data functions
	Shape randomTetrominoShape();
	Direction randomTetrominoDirection(const Shape &shape);

	// from speed calculated delay intervals
	unsigned int step_interval_;
	unsigned int down_interval_;
	unsigned int first_move_interval_;
	unsigned int move_interval_;
	unsigned int blink_interval_;

	enum MoveDirection{
		NO_MOVE = 0,
		RIGHT_MOVE = 1,
		LEFT_MOVE = 2,
	} move_dir_;


	StaticList<byte> field_; // field without current tetromino
	Tetromino *tetromino_;
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
	bool debug  =0;
	byte blink_cycle_;
	byte blink_start_row_;
	byte blink_end_row_;
	void clearFullRowsImmediately();
};
