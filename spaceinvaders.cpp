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

const unsigned int speeds[] PROGMEM = {
	 2000, 65, 400, 200, // very slow
	 1600, 60, 400, 160, // slow
	 1200, 45, 300, 140, // medium fast
	 1000, 55, 350, 120, // fast
	 800,  50, 300, 100, // very fast
};

SpaceInvaders::SpaceInvaders(Display *display)
	 : Game(display, SPACE_INVADERS), invaders_(StaticList<byte>(display->rows())), speed_(2)
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
	step_timer_ = new Timer(readSpeed(StepInterval));
	event->addTrigger(step_timer_);
	shot_timer_ = new Timer(readSpeed(ShotInterval));
	event->addTrigger(shot_timer_);
	auto_move_ = new ButtonAutoTrigger(&event->buttonLeft(), &event->buttonRight(), readSpeed(FirstMoveInterval),
												  readSpeed(MoveInterval));
	event->addTrigger(auto_move_);

	render();
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

unsigned int SpaceInvaders::readSpeed(const SpaceInvaders::SpeedFlag flag) const
{
	return pgm_read_word(&speeds[flag + 4 * speed_]);
}

bool SpaceInvaders::onButtonChange(Event *event)
{

	// when button was pressed, start a shot
	if (event->buttonDown().pressed())
	{
		shots_ << Shot(pos_);
	}

	render();
	return false;
}

bool SpaceInvaders::onTriggered(Event *event)
{
	// step counter (increases invaders)
	if (step_timer_->triggered())
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
	if (shot_timer_->triggered())
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

	if (auto_move_->triggered())
	{
		ButtonAutoTrigger::Direction dir = auto_move_->direction();
		if (dir == ButtonAutoTrigger::BTN_1)
			left();
		else if (dir == ButtonAutoTrigger::BTN_2)
			right();

		// validation
		if (bitRead(invaders_[1], pos_))
		{
			updateHighscore();
			return true;
		}
	}

	render();
	return false;
}

void SpaceInvaders::onStop(Event *event)
{
	step_timer_->stop();
	shot_timer_->stop();
	auto_move_->stop();
	Game::onStop(event);
}

void SpaceInvaders::onContinue(Event *event)
{
	step_timer_->restart();
	shot_timer_->restart();
	auto_move_->restart();
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
