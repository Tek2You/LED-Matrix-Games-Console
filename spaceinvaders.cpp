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
	// setup position
	pos_ = 3;

	// fill buffer
	for (int i = 0; i < display->rows(); i++)
	{
		invaders_ << 0;
	}

	// initial invaders
	for (int i = 0; i < 3; i++)
	{
		insertRow();
	}

	// set speed for case that it wouldnt be done later
	setSpeed(2);
}

void SpaceInvaders::resetHighscore()
{
	highscore_ = 0;
	eeprom_write_word(&EE_highscore, 0);
}

void SpaceInvaders::start(Event *event)
{
	// sets the event flags
	event->setupGame();
	// setup timers
	event->addTrigger(new Timer(step_interval_));
	event->addTrigger(new Timer(shot_interval_));
	event->addTrigger(new Timer);
	// start shot and step timer
	event->trigger(0)->start();
	event->trigger(1)->start();
	render();
}

const unsigned int intervals[] PROGMEM = {
	 2000, 65, 400, 200, // very slow
	 1600, 60, 400, 160, // slow
	 1200, 45, 300, 140, // medium fast
	 1000, 55, 350, 120, // fast
	 800,  50, 300, 100, // very fast
};

void SpaceInvaders::setSpeed(const byte v)
{
	step_interval_ = pgm_read_word(&intervals[4 * v]);
	shot_interval_ = pgm_read_word(&intervals[4 * v + 1]);
	first_move_interval_ = pgm_read_word(&intervals[4 * v + 2]);
	move_interval_ = pgm_read_word(&intervals[4 * v + 3]);
}

void SpaceInvaders::updateHighscore(const byte offset)
{
	if (score_ > highscore_ + offset)
	{
		highscore_ = score_;
		eeprom_write_word(&EE_highscore, highscore_);
		is_new_highscore_ = true;
	}
}

unsigned int SpaceInvaders::score() const { return score_; }

bool SpaceInvaders::onButtonChange(Event *event)
{

	// when button was pressed, start a shot
	if (event->buttonDown().pressed())
	{
		shots_ << Shot(pos_);
	}

	// store move timer
	Timer *move_timer = static_cast<Timer *>(event->trigger(2));

	// left button
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
			move_timer->setInterval(first_move_interval_);
			move_timer->restart();
			move_dir_ = LEFT_MOVE;
		}
	}
	else if (event->buttonLeft().released() && move_dir_ == LEFT_MOVE)
	{
		move_timer->clear();
		if (event->buttonRight().state())
		{
			move_dir_ = RIGHT_MOVE;
			move_timer->setInterval(first_move_interval_);
			move_timer->restart();
		}
		else
		{
			move_timer->stop();
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
			move_timer->setInterval(first_move_interval_);
			move_timer->restart();
			move_dir_ = RIGHT_MOVE;
		}
	}
	else if (event->buttonRight().released() && move_dir_ == RIGHT_MOVE)
	{
		move_timer->clear();
		if (event->buttonLeft().state())
		{
			move_dir_ = LEFT_MOVE;
			move_timer->setInterval(first_move_interval_);
			move_timer->restart();
		}
		else
		{
			move_timer->stop();
			move_dir_ = NO_MOVE;
		};
	}
	render();
	return false;
}

bool SpaceInvaders::onTimerOverflow(Event *event)
{
	// step counter (increases invaders)
	if (event->trigger(0)->triggered())
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
				updateHighscore(100);
			}
		}
		// validation
		if (invaders_[0] || bitRead(invaders_[1], pos_))
		{
			updateHighscore();
			return true;
		}
	}

	// timer for shots
	if (event->trigger(1)->triggered())
	{
		for (int i = 0; i < shots_.size(); i++)
		{
			shots_[i].row_++;
			if (processShot(shots_[i]))
			{
				shots_.remove(i);
				i--;
			}
		}
	}

	Timer *move_timer = static_cast<Timer*>(event->trigger(2));
	if (move_timer->triggered())
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
		move_timer->setInterval(move_interval_);
		move_timer->restart();
	}

	render();
	return false;
}

void SpaceInvaders::onStop(Event *event)
{
	event->trigger(0)->stop();
	event->trigger(1)->stop();
	event->trigger(2)->stop();
	Game::onStop(event);
}

void SpaceInvaders::onContinue(Event *event)
{
	event->trigger(0)->restart();
	event->trigger(1)->restart();
	Game::onContinue(event);
}

void SpaceInvaders::render()
{
	// clear display
	display_->clear();
	// print invaders to field
	display_->setArray(invaders_.toArray());

	// print shots
	for (Shot s : shots_)
	{
		display_->setPixel(s.col_, s.row_);
	}

	// print shoter
	display_->setPixel(pos_ - 1, 0);
	display_->setPixel(pos_, 0);
	display_->setPixel(pos_, 1);
	display_->setPixel(pos_ + 1, 0);

	// finaly show all on display
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
	// check for hit
	if (bitRead(invaders_[s.row_], s.col_))
	{
		// delete bit as long as it forms a row
		while (bitRead(invaders_[s.row_], s.col_) && s.row_ < display_->rows())
		{
			bitClear(invaders_[s.row_], s.col_);
			score_++;
			s.row_++;
		}
		return true;
	}

	// update highscore only when it has a difference of 100
	updateHighscore(100);
	return s.row_ >= display_->rows();
}

void SpaceInvaders::insertRow()
{
	// remove first
	invaders_.removeFirst();
	// create random value
	byte val = rand() % 256;
	// intert the random seed
	invaders_.append(val);
}
