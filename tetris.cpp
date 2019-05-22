/* tetris.cpp : Tetris game library
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

#include "tetris.h"
#include "avr/eeprom.h"
#include "operators.h"

static unsigned int EE_highscore EEMEM = 0;
unsigned int Tetris::highscore_ = eeprom_read_word(&EE_highscore);

const int speeds[] PROGMEM = {
	 1800, 180, 400, 200, 80,  // very slow
	 1400, 140, 400, 160, 90,  // slow
	 1000, 100, 300, 140, 100, // medium fast
	 800,  80,  350, 120, 110, // fast
	 500,  50,  300, 100, 120, // very fast
};

Tetris::Tetris(Display *display) : Game(display, TETRIS), blink_cycle_(DEFAULT), field_(display->rows()), points_(0)
{
	display_->clear();
	display->show();
	setSpeed(2);

	for (int i = 0; i < display->rows(); i++)
	{
		field_ << 0;
	}
}

Tetris::~Tetris() { points_ = 0; }

void Tetris::start(Event *event)
{
	event->setupGame();
	step_timer_ = new Timer(readSpeed(StepInterval));
	event->addTrigger(step_timer_);
	blink_timer_ = new Timer(readSpeed(BlinkInterval));
	event->addTrigger(blink_timer_);
	move_trigger_ = new ButtonAutoTrigger(&event->buttonLeft(), &event->buttonRight(), readSpeed(FirstMoveInterval),
													  readSpeed(MoveInterval));
	event->addTrigger(move_trigger_);

	newTetromino();
}

void Tetris::render()
{
	display_->clear();
	display_->setArray(field_.toArray());
	Pos positions[4];
	tetromino_.getPoints(positions);
	for (Pos p : positions)
	{
		display_->setPixel(p);
	}
	display_->show();
}

void Tetris::onStop(Event *event)
{
	step_timer_->stop();
	blink_timer_->stop();
	move_trigger_->stop();
	blink_cycle_ = DEFAULT;
	Game::onStop(event);
}

void Tetris::onContinue(Event *event)
{
	step_timer_->setInterval((event->buttonDown().state() ? readSpeed(DownInterval) : readSpeed(StepInterval)));
	step_timer_->start();
	move_trigger_->start();
	Game::onContinue(event);

	clearFullRows(event); // clears all rows, if interrupted during clear effect
}

bool Tetris::rotate()
{
	/*
	 * sheme of work:
	 * 1. get current values
	 * 2. calculate next(possible) direction. Possible is'nt meaning that the brick
	 * dosnt collides
	 * 3. Check if the new direction is valid
	 * 4. if not: try to change to a valid position
	 */

	// 1.	get current values

	Tetromino tmp = tetromino_;

	tmp.rotate();

	// 2. calculate new (wanted) direction
	// 3. check valid
	byte valid_output = tmp.validationErrors(&field_);

	// if not valid, try to move to make valid
	if (valid_output != VALID)
	{
		if (valid_output & OVER_LEFT)
		{
			while (valid_output = tmp.validationErrors(&field_) & OVER_LEFT)
			{
				tmp.pos_.pos_x++;
			}
		}
		else if (valid_output & OVER_RIGHT)
		{
			while (valid_output = tmp.validationErrors(&field_) & OVER_RIGHT)
			{
				tmp.pos_.pos_x--;
			}
		}
		if (valid_output & OVER_BELOW)
		{
			while (valid_output = tmp.validationErrors(&field_) & OVER_BELOW)
			{
				tmp.pos_.pos_y++;
			}
		}
		else if (valid_output & OVER_ABOVE)
		{
			while (valid_output = tmp.validationErrors(&field_) & OVER_ABOVE)
			{
				tmp.pos_.pos_y--;
			}
		}
		if (valid_output & COLLIDE || tmp.validationErrors(&field_)) // collides with exiting tetromino
		{
			// not even valid
			return false;
		}
	}
	tetromino_ = tmp;
	render();
	return true;
}

void Tetris::move(const ButtonAutoTrigger::Direction dir)

{
	Tetromino tmp = tetromino_;
	tmp.pos_.pos_x += (dir == ButtonAutoTrigger::BTN_1 ? -1 : 1);
	if (tmp.isValid(&field_))
	{
		tetromino_ = tmp;
		render();
	}
}

bool Tetris::tick(Event *event)
{
	Tetromino tmp = tetromino_;
	tmp.pos_.pos_y--;
	if (tmp.isValid(&field_))
	{
		tetromino_ = tmp;
		render();
		return false;
	}
	if (blink_cycle_ == DEFAULT) takeOverTetromino();
	return clearFullRows(event);
}

bool Tetris::newTetromino()
{
	Tetromino tmp;
	tmp.pos_ = Pos(4, display_->rows());
	tmp.randomShape();
	tmp.randomDirection();

	Pos points[4];
	tmp.getPoints(points);
	// place tetromino full in the field
	for (Pos p : points)
	{
		if (p.pos_y >= display_->rows())
		{
			tmp.pos_.pos_y -= (p.pos_y - (display_->rows() - 1));
			tmp.getPoints(points);
		}
	}
	tetromino_ = tmp;
	render();
	return tetromino_.validationErrors(&field_) & COLLIDE;
}

void Tetris::takeOverTetromino()
{
	Pos positions[4];
	tetromino_.getPoints(positions);
	for (Pos p : positions)
	{
		bitWrite(field_[p.pos_y], p.pos_x, true);
	}
	render();
}

void Tetris::resetHighscore() { eeprom_write_word(&EE_highscore, highscore_ = 0); }

bool Tetris::onButtonChange(Event *event)
{
	if (blink_cycle_ != DEFAULT)
	{
		return false;
	}

	// Rotation
	if (event->buttonUp().pressed())
	{
		rotate();
	}

	// Down(faster and lower)
	if (event->buttonDown().changed())
	{
		unsigned int interval;
		if (event->buttonDown().state())
		{
			if (tick(event)) return true;
			interval = readSpeed(DownInterval);
		}
		else
			interval = readSpeed(StepInterval);
		step_timer_->setInterval(interval);
		step_timer_->restart();
	}
	return false;
}

bool Tetris::onTriggered(Event *event)
{
	if (move_trigger_->triggered())
	{
		move(move_trigger_->direction());
	}

	if (blink_cycle_ != DEFAULT)
	{
		if (blink_timer_->triggered())
		{
			if (clearFullRows(event)) return true;
		}
		return false;
	}

	if (step_timer_->triggered())
	{
		if (tick(event)) // end of the game
		{
			return true;
		}
	}
	return false;
}

bool Tetris::clearFullRows(Event *event)
{
	if (blink_cycle_ == FINISHED_CURRENT)
	{
		const byte diff = blink_end_row_ - blink_start_row_;
		for (int i = 0; i < diff; i++)
		{
			field_.remove(blink_start_row_);
			field_ << 0;
		}
		display_->setArray(field_.toArray());
		display_->show();
		points_ += diff;
		if (points_ > highscore_)
		{
			highscore_ = points_;
			eeprom_write_word(&EE_highscore, highscore_);
			is_new_highscore_ = true;
		}

		if (rowsFull())
		{
			blink_cycle_ = INIT_BLINK;
		}
		else
		{
			blink_cycle_ = DEFAULT;
			blink_timer_->stop();
			display_->setArray(field_.toArray());
			display_->show();
			step_timer_->start();
			step_timer_->setInterval((event->buttonDown().state() ? readSpeed(DownInterval) : readSpeed(StepInterval)));

			if (newTetromino()) return true;
			return false;
		}
	}
	else if (blink_cycle_ == DEFAULT)
	{
		if (rowsFull())
		{
			step_timer_->stop();
			blink_cycle_ = INIT_BLINK;
		}
		else
		{
			// place a new stone, because all is empty
			return newTetromino();
		}
	}
	if (blink_cycle_ == INIT_BLINK)
	{
		for (byte i = 0; i < display_->rows(); i++)
		{
			if (field_[i] == 0xFF)
			{
				blink_start_row_ = i;
				break;
			}
		}
		for (byte i = blink_start_row_ + 1; i < display_->rows(); i++)
		{
			if (field_[i] != 0xFF)
			{
				blink_end_row_ = i;
				break;
			}
		}
		blink_cycle_ = BLINK_OFF_1;
		blink_timer_->start();
		return false;
	}

	display_->setArray(field_.toArray());

	if (blink_cycle_ == BLINK_OFF_1 || blink_cycle_ == BLINK_OFF_2 || blink_cycle_ == BLINK_OFF_3)
	{
		display_->clearRows(blink_start_row_, blink_end_row_);
	}
	display_->show();
	blink_cycle_++;
	return false;
}

bool Tetris::rowsFull() const
{
	for (byte &i : field_)
	{
		if (i == 0xFF) return true;
	}
	return false;
}

unsigned int Tetris::readSpeed(const Tetris::SpeedFlag flag) const { return pgm_read_word(&speeds[flag + speed_ * 5]); }
