/* spaceinvaders.cpp : Space Invaders game library
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

#include "spaceinvaders.h"

static unsigned int EE_highscore EEMEM = 0;
unsigned int SpaceInvaders::highscore_ = eeprom_read_word(&EE_highscore);

SpaceInvaders::SpaceInvaders(Display *display)
	 : Game(display, SPACE_INVADERS), invaders_(StaticList<byte>(display->rows()))
{
	display->clear();
	display->show();
	pos_ = 3;
	for (int i = 0; i < display->rows(); i++)
	{
		invaders_ << 0;
	}
	for (int i = 0; i < 3; i++)
	{
		insertRow();
	}
	setSpeed(2);
}

void SpaceInvaders::resetHighscore()
{
	highscore_ = 0;
	eeprom_write_word(&EE_highscore, 0);
}

unsigned int SpaceInvaders::highscore() { return highscore_; }

void SpaceInvaders::start(Event *event)
{
	event->setupGame();
	event->addTimer(step_interval_);
	event->addTimer();
	event->addTimer(shot_interval_);
	event->timer(0).start();
	event->timer(1).start();
	render();
}

const unsigned int intervals[] PROGMEM = {
	 2000, 65, 400, 200, // very slow
	 1600, 60, 400, 160, // slow
	 1400, 55, 350, 120, // medium fast
	 1000, 50, 300, 100, // fast
	 800,  45, 300, 140, // very fast
};

void SpaceInvaders::setSpeed(byte v)
{
	step_interval_ = pgm_read_word(&intervals[4 * v]);
	shot_interval_ = pgm_read_word(&intervals[4 * v + 1]);
	first_move_interval_ = pgm_read_word(&intervals[4 * v + 2]);
	move_interval_ = pgm_read_word(&intervals[4 * v + 3]);
}

void SpaceInvaders::updateHighscore()
{
	if (score_ > highscore_)
	{
		highscore_ = score_;
		eeprom_write_word(&EE_highscore, highscore_);
	}
}

unsigned int SpaceInvaders::score() const { return score_; }

bool SpaceInvaders::onButtonChange(Event *event)
{
	if (event->buttonDown().pressed())
	{
		shots_ << Shot(pos_);
	}

	Timer &move_timer = event->timer(2);
	// btn left
	if (event->buttonLeft().pressed())
	{
		if (!event->buttonRight().state())
		{
			left();
			// validation
			if (bitRead(invaders_[1], pos_))
			{
				updateHighscore();
				return true;
			}
			move_timer.setInterval(first_move_interval_);
			move_timer.restart();
			move_dir_ = LEFT_MOVE;
		}
	}
	else if (event->buttonLeft().released() && move_dir_ == LEFT_MOVE)
	{
		move_timer.clearOverflow();
		if (event->buttonRight().state())
		{
			move_dir_ = RIGHT_MOVE;
			move_timer.setInterval(first_move_interval_);
			move_timer.restart();
		}
		else
		{
			move_timer.stop();
			move_dir_ = NO_MOVE;
		}
	}

	// btn right
	if (event->buttonRight().pressed())
	{
		if (!event->buttonLeft().state())
		{
			right();
			// validation
			if (bitRead(invaders_[1], pos_))
			{
				updateHighscore();
				return true;
			}
			move_timer.setInterval(first_move_interval_);
			move_timer.restart();
			move_dir_ = RIGHT_MOVE;
		}
	}
	else if (event->buttonRight().released() && move_dir_ == RIGHT_MOVE)
	{
		move_timer.clearOverflow();
		if (event->buttonLeft().state())
		{
			move_dir_ = LEFT_MOVE;
			move_timer.setInterval(first_move_interval_);
			move_timer.restart();
		}
		else
		{
			move_timer.stop();
			move_dir_ = NO_MOVE;
		};
	}
	render();
	return false;
}

bool SpaceInvaders::onTimerOverflow(Event *event)
{
	Timer &move_timer = event->timer(2);
	if (move_timer.overflow())
	{
		if (move_dir_ == LEFT_MOVE)
		{
			left();
		}

		else if (move_dir_ == RIGHT_MOVE)
		{
			right();
		}
		// validation
		if (bitRead(invaders_[1], pos_))
		{
			updateHighscore();
			return true;
		}
		move_timer.setInterval(move_interval_);
		move_timer.restart();
	}

	if (event->timer(0).overflow())
	{
		// new random row
		insertRow();
		// check for shots
		for (int i = 0; i < shots_.size(); i++)
		{
			if (processShot(shots_[i]))
			{
				shots_.remove(i);
				i--;
				score_++;
				if (score_ > highscore_ + 100)
				{
					highscore_ = score_;
					eeprom_write_word(&EE_highscore, highscore_);
				}
			}
		}
		// validation
		if (invaders_[0] || bitRead(invaders_[1], pos_))
		{
			updateHighscore();
			return true;
		}
	}

	if (event->timer(1).overflow())
	{
		for (int i = 0; i < shots_.size(); i++)
		{
			shots_[i].row_++;
			if (processShot(shots_[i]))
			{
				score_++;
				shots_.remove(i);
				i--;
			}
		}
	}
	render();
	return false;
}

void SpaceInvaders::onStop(Event *event)
{
	event->timer(0).stop();
	event->timer(1).stop();
	event->timer(2).stop();
	Game::onStop(event);
}

void SpaceInvaders::onContinue(Event *event)
{
	event->timer(0).restart();
	event->timer(1).restart();
	Game::onContinue(event);
}

void SpaceInvaders::render()
{
	display_->clear();
	display_->setArray(invaders_.toArray());

	for (byte row = 0; row < invaders_.size(); row++)
	{
		display_->setRow(row, invaders_[row]);
	}

	for (Shot s : shots_)
	{
		display_->setPixel(s.col_, s.row_);
	}

	display_->setPixel(pos_ - 1, 0);
	display_->setPixel(pos_, 0);
	display_->setPixel(pos_, 1);
	display_->setPixel(pos_ + 1, 0);

	display_->show();
}

void SpaceInvaders::right()
{

	if (pos_ < 7) pos_++;
}

void SpaceInvaders::left()
{

	if (pos_ != 0) pos_--;
}

Shot::Shot() : col_(0) {}

Shot::Shot(byte col) : col_(col) { row_ = 2; }

bool SpaceInvaders::processShot(Shot &s)
{
	if (bitRead(invaders_[s.row_], s.col_))
	{
		while (bitRead(invaders_[s.row_], s.col_) && s.row_ < display_->rows())
		{
			bitClear(invaders_[s.row_], s.col_);
			s.row_++;
		}
		return true;
	}
	if (score_ > highscore_ + 100)
	{
		highscore_ = score_;
		eeprom_write_word(&EE_highscore, highscore_);
	}
	return s.row_ >= display_->rows();
}

void SpaceInvaders::insertRow()
{
	if (invaders_.size() >= display_->rows()) invaders_.removeFirst();
	byte val = rand() % 256;
	invaders_.append(val);
}
