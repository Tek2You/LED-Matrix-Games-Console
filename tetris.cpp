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

Tetris::Tetris(Display *display)
	 : Game(display, TETRIS), move_dir_(NO_MOVE), blink_cycle_(DEFAULT), field_(display->rows())
{
	tetromino_ = nullptr;
	setSpeed(2);

	for (int i = 0; i < display->rows(); i++)
	{
		field_ << 0;
	}

	clear();
	display_->clear();
	points_ = 0;
}

Tetris::~Tetris()
{
	if (tetromino_ != nullptr)
	{
		delete tetromino_;
	}

	points_ = 0;
}

void Tetris::start(Event *event)
{
	event->setupGame();
	event->addTrigger(new Timer(step_interval_));  // tick timer
	event->addTrigger(new Timer);
	event->addTrigger(new Timer(blink_interval_)); // blink timer
	event->addTrigger(new ButtonAutoTrigger(&event->buttonLeft(), &event->buttonRight(), first_move_interval_, move_interval_));

	newTetromino();
}

const int interval[] PROGMEM = {
	 1800, 180, 400, 200, 80,  // very slow
	 1400, 140, 400, 160, 90,  // slow
	 1000, 100, 300, 140, 100, // medium fast
	 800,  80,  350, 120, 110, // fast
	 500,  50,  300, 100, 120, // very fast
};

void Tetris::setSpeed(const byte v)
{
	step_interval_ = pgm_read_word(&interval[v * 5]);
	down_interval_ = pgm_read_word(&interval[v * 5 + 1]);
	first_move_interval_ = pgm_read_word(&interval[v * 5 + 2]);
	move_interval_ = pgm_read_word(&interval[v * 5 + 3]);
	blink_interval_ = pgm_read_word(&interval[v * 5 + 4]);
}

void Tetris::render()
{
	display_->clear();
	display_->setArray(field_.toArray());
	if (tetromino_ != nullptr)
	{
		Pos positions[4];
		tetromino_->getPositions(positions);
		for (Pos p : positions)
		{
			display_->setPixel(p.pos_x, p.pos_y, true);
		}
	}
	display_->show(true);
}

void Tetris::onStop(Event *event)
{
	event->trigger(0)->stop();
	event->trigger(1)->stop();
	event->trigger(2)->stop();
	blink_cycle_ = DEFAULT;
	Game::onStop(event);
}

void Tetris::onContinue(Event *event)
{
	clearFullRowsImmediately(); // clears all rows, if interrupted during clear effect
	(static_cast<Timer *>(event->trigger(0)))
		 ->setInterval((event->buttonDown().state() ? down_interval_ : step_interval_));
	event->trigger(0)->start();

	Game::onContinue(event);
	// for the second timer, the restart is not required, because its used for button long pressed
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

	if (tetromino_ == nullptr) return false;

	// 1.	get current values
	Pos tmp_pos = tetromino_->pos();
	const Shape shape = tetromino_->shape();

	// 2. calculate new (wanted) direction
	const Direction new_dir = Tetromino::rotate(tetromino_->direction(), shape);

	// 3. check valid
	byte valid_output = tetromino_->isValid(shape, new_dir, tmp_pos);

	// if not valid, try to move to make valid
	if (valid_output)
	{
		if (valid_output & OVER_LEFT)
		{
			while (valid_output = tetromino_->isValid(shape, new_dir, tmp_pos) & OVER_LEFT)
			{
				tmp_pos.pos_x++;
			}
		}
		else if (valid_output & OVER_RIGHT)
		{
			while (valid_output = tetromino_->isValid(shape, new_dir, tmp_pos) & OVER_RIGHT)
			{
				tmp_pos.pos_x--;
			}
		}
		if (valid_output & OVER_BELOW)
		{
			while (valid_output = tetromino_->isValid(shape, new_dir, tmp_pos) & OVER_BELOW)
			{
				tmp_pos.pos_y++;
			}
		}
		else if (valid_output & OVER_ABOVE)
		{
			while (valid_output = tetromino_->isValid(shape, new_dir, tmp_pos) & OVER_ABOVE)
			{
				tmp_pos.pos_y--;
			}
		}
		if (valid_output & COLLIDE || tetromino_->isValid(shape, new_dir, tmp_pos)) // collides with exiting tetromino
		{
			return false;
		}
	}
	tetromino_->setPos(tmp_pos);
	tetromino_->setDirection(new_dir);
	render();
	return true;
}

bool Tetris::right()

{
	if (!tetromino_) return false;
	Pos pos = tetromino_->pos();
	pos.pos_x++;
	if (tetromino_->isValid(tetromino_->shape(), tetromino_->direction(), pos))
	{ // not valid
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return true;
}

bool Tetris::left()
{
	if (tetromino_ == nullptr) return false;
	Pos pos = tetromino_->pos();
	pos.pos_x--;
	if (tetromino_->isValid(tetromino_->shape(), tetromino_->direction(), pos))
	{
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return true;
}
bool Tetris::tick(Event *event)
{
	if (tetromino_ == nullptr) return false;
	Pos pos = tetromino_->pos();
	pos.pos_y--;

	if (tetromino_->isValid(tetromino_->shape(), tetromino_->direction(), pos))
	{
		if (blink_cycle_ == DEFAULT) takeOverTetromino();
		return clearFullRows(event);
	}
	tetromino_->setPos(pos);
	render();
	return false;
}

void Tetris::clear()
{
	for (byte &row : field_)
	{
		row = 0;
	}
}

bool Tetris::newTetromino()
{
	if (tetromino_)
	{ // make sure, a tetromino exists before delete
		delete tetromino_;
		tetromino_ = nullptr;
	}
	const Shape shape = randomTetrominoShape();
	tetromino_ = new Tetromino(shape, display_->rows(), field_.toArray(), randomTetrominoDirection(shape),
										Pos(4, display_->rows() - 1));
	Pos points[4];
	tetromino_->getPositions(points);
	// place tetromino full in the field
	for (Pos p : points)
	{
		if (p.pos_y > display_->rows() - 1)
		{
			Pos pos = tetromino_->pos();
			pos.pos_y -= (p.pos_y - (display_->rows() - 1));
			tetromino_->setPos(pos);
			tetromino_->getPositions(points);
		}
	}
	render();
	if (tetromino_->isValid() & COLLIDE)
	{ // not valid
		return true;
	}
	return false;
}

void Tetris::takeOverTetromino()
{
	Pos positions[4];
	tetromino_->getPositions(positions);
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
	Timer *down_timer = static_cast<Timer *>(event->trigger(0));
	// Rotation
	if (event->buttonUp().pressed())
	{
		rotate();
	}

	// Down(faster)
	if (event->buttonDown().pressed())
	{
		if (tick(event)) // check if
		{
			return true;
		}
		down_timer->setInterval(down_interval_);
		down_timer->restart();
	}
	else if (event->buttonDown().released())
	{
		// unset fast down
		down_timer->setInterval(step_interval_);
		down_timer->restart();
	}
	return false;
}



bool Tetris::onTriggered(Event *event)
{
	if (event->trigger(3)->triggered())
	{
		ButtonAutoTrigger::Direction dir = static_cast<ButtonAutoTrigger *>(event->trigger(3))->getDirection();
		if (dir == ButtonAutoTrigger::BTN_1)
			left();
		else if (dir == ButtonAutoTrigger::BTN_2)
			right();
	}

	if (blink_cycle_ != DEFAULT)
	{
		if (event->trigger(2)->triggered())
		{
			if (clearFullRows(event)) return true;
		}
		return false;
	}

	if (event->trigger(0)->triggered())
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
			event->trigger(2)->stop();
			//			clearFullRowsImmediately();
			display_->setArray(field_.toArray());
			event->trigger(0)->start();
			static_cast<Timer *>(event->trigger(0))
				 ->setInterval((event->buttonDown().state() ? down_interval_ : step_interval_));

			if (newTetromino()) return true;
			return false;
		}
	}
	else if (blink_cycle_ == DEFAULT)
	{
		if (rowsFull())
		{
			event->trigger(0)->stop();
			event->trigger(1)->stop();
			blink_cycle_ = INIT_BLINK;
		}
		else
		{
			// place a new stone, because all is empty
			if (newTetromino()) return true;
			return false;
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
		event->trigger(2)->start();
	}
	else if (blink_cycle_ == BLINK_OFF_1 || blink_cycle_ == BLINK_OFF_2 || blink_cycle_ == BLINK_OFF_3)
	{
		display_->setArray(field_.toArray());
		display_->clearRows(blink_start_row_, blink_end_row_);
		display_->show();
		blink_cycle_++;
	}
	else if (blink_cycle_ == BLINK_ON_1 || blink_cycle_ == BLINK_ON_2)
	{
		display_->setArray(field_.toArray());
		display_->show();
		blink_cycle_++;
	}
	return false;
}

void Tetris::clearFullRowsImmediately()
{
	for (byte i = 0; i < field_.size(); i++)
	{
		while (field_[i] == 0xFF)
		{
			field_.remove(i);
			field_ << 0;
			points_++;
		}
	}
	render();
	display_->show();
	blink_cycle_ = DEFAULT;
	takeOverTetromino();
	newTetromino();

	if (points_ > highscore_)
	{
		highscore_ = points_;
		eeprom_write_word(&EE_highscore, highscore_);
		is_new_highscore_ = true;
	}
}

bool Tetris::rowsFull() const
{
	for (byte &i : field_)
	{
		if (i == 0xFF) return true;
	}
	return false;
}

Shape Tetris::randomTetrominoShape() { return Shape(millis() % 7); }

Direction Tetris::randomTetrominoDirection(const Shape &shape)
{
	byte directions = Tetromino::possibleDirections(shape);
	byte num = Tetromino::possibleDirectionNum(shape);

	if (num <= 1)
	{
		return TOP;
	}

	byte rand = ((millis()) % (num));
	for (int i = 0, j = 0; i < 4; i++)
	{
		if (bitRead(directions, i))
		{
			if (j == rand)
			{
				return Direction(j);
			}
			j++;
		}
	}
}
