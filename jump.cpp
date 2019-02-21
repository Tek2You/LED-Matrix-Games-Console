/* jump.cpp : Jump game library
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

#include "jump.h"
#include "avr/eeprom.h"
#include "operators.h"
#include "position.h"

static uint16_t EE_highscore EEMEM = 0;
uint16_t Jump::highscore_ = eeprom_read_word(&EE_highscore);

Jump::Jump(Display *display) : Game(display,JUMP)
{
	field_ = static_cast<byte *>(malloc(display_->rows() + 4));
	current_field_start_ = 0;
	score_ = 0;
	setSpeed(2);
}

Jump::~Jump()
{
	free(field_);
}

void Jump::start(Event *event)
{
	for (int i = 0; i < display_->rows() + 4; i++)
	{
		field_[i] = 0;
	}
	current_field_start_ = 0;
	man_pos_ = Pos(0, 0);
	next_hind_ = 8;
	man_state_ = 1;
	is_jumping_ = false;
	score_ = 0;
	jump_count_ = 0;
	jump_height_ = 4;
	jump_lenght_ = 5;
	render();

	event->removeAllTimers();
	event->addTimer(forward_period_); // timer already starts if given a initzializing interval
	event->addTimer();
	event->setFlag(Event::ProcessPinChanges);
	event->setFlag(Event::ProcessTimerOverflows);
	event->setFlag(Event::ProcessStop);
}

unsigned int Jump::score() const
{
	return score_;
}

void Jump::setSpeed(const byte v)
{
	switch (v)
	{
	case 0:
		forward_period_ = 450;
		jump_period_ = 337;
		break;
	case 1:
		forward_period_ = 375;
		jump_period_ = 280;
		return;
	case 3:
		forward_period_ = 225;
		jump_period_ = 169;
		break;
	case 4:
		forward_period_ = 150;
		jump_period_ = 112;
		break;
	case 2:
	default:
		forward_period_ = 300;
		jump_period_ = 225;
		break;
	}
}

byte *Jump::row(const byte n)
{
	return field_ + ((current_field_start_ + n) % 20);
}

unsigned int Jump::highscore()
{
	return highscore_;
}

void Jump::resetHighscore()
{
	eeprom_write_word(&EE_highscore, highscore_ = 0);
}

bool Jump::onButtonChange(Event *event)
{
	if (event->buttonRight().changed())
	{
		if (event->buttonRight().state())
		{
			if (jump_count_ == 0 && !is_jumping_) // begin Jump
			{
				is_jumping_ = true;
				event->timer(1).setInterval(jump_period_);
				event->timer(1).start();
				jump(event);
			}
		}
		else if (jump_count_ && jump_count_ < 3)
		{
			jump_height_ = 3;
			jump_lenght_ = 3;
		}
	}
	return false;
}

bool Jump::onTimerOverflow(Event *event)
{

	if (event->timer(0).overflow())
	{
		tick();
	}
	if (event->timer(1).overflow())
	{
		jump(event);
	}
	if (!isValid(man_pos_))
	{
		if (score_ > highscore_)
		{
			highscore_ = score_;
			is_new_highscore_ = true;
			eeprom_write_word(&EE_highscore, highscore_);
		}
		event->timer(0).stop();
		event->timer(1).stop();
		return true;
	}
	return false;
}

void Jump::onStop(Event *event)
{
	Game::onStop(event);
	event->timer(0).stop();
	event->timer(1).stop();
}

void Jump::onContinue(Event *event)
{
	event->timer(0).start();
	if (is_jumping_)
	{
		event->timer(1).start();
	}
	Game::onContinue(event);
}

void Jump::tick()
{
	man_state_ = !man_state_;
	if (man_pos_.pos_x < 3)
	{
		man_pos_.pos_x++;
	}

	if (next_hind_-- == 0)
	{
		next_hind_ = 12;
		newHind();
	}
	*row(0) = 0;
	if (++current_field_start_ == display_->rows() + 4) current_field_start_ = 0;
	render();
	score_++;
}

void Jump::jump(Event *event)
{
	if (jump_count_ < jump_height_)
	{
		man_pos_.pos_y--;
	}
	else if (jump_count_ > jump_height_ + jump_lenght_)
	{
		man_pos_.pos_y++;
		if (jump_count_ == jump_height_ * 2 + jump_lenght_)
		{
			jump_count_ = 0;
			is_jumping_ = false;
			jump_height_ = 4;
			jump_lenght_ = 5;
			event->timer(1).stop();
			return;
		}
	}
	jump_count_++;
	render();
}

void Jump::newHind()
{
	int hind_shape = millis() % 7;
	for (Pos p : hinds_[hind_shape].positions)
	{
		p.pos_x += display_->rows(); // go outside range
		bitSet(*row(p.pos_x), p.pos_y);
	}
}

bool Jump::isValid(const Pos pos)
{
	for (Pos p : man_moving_points)
	{
		p += pos;
		if (bitRead(*row(p.pos_x), p.pos_y)) return false;
	}
	for (Pos p : man_points)
	{
		p += pos;
		if (bitRead(*row(p.pos_x), p.pos_y)) return false;
	}
	return true;
}

void Jump::render()
{
	display_->clear();
	for (int i = 0; i < 16; i++)
	{
		display_->setRow(i, *row(i));
	}

	for (Pos p : man_points)
	{
		p += man_pos_;
		display_->setPixel(~p, true);
	}
	Pos p = (man_moving_points[(man_state_ == 1 ? 1 : 0)]);
	p += man_pos_;
	display_->setPixel(~p, true);
	display_->show(false);
}
