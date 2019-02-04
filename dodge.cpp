/* dodge.cpp : Dodge game class
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

#include "dodge.h"

static unsigned int EE_highscore EEMEM = 0;
unsigned int Dodge::highscore_ = eeprom_read_word(&EE_highscore);

#define ELEMENTS_NUM 11

const Dodge::LineData lines[11] = {
	 {Dodge::LineData::set(3, 2, 0), {0b11100111, 0b11000011, 0b10000001}},
	 {Dodge::LineData::set(1, 2, 0), {0b10101010}},
	 {Dodge::LineData::set(1, 2, 0), {0b01010101}},
	 {Dodge::LineData::set(4, 2, 0), {0b01110110, 0, 0b10000001, 0}},
	 {Dodge::LineData::set(4, 2, 0), {0b10000101, 0b00100000, 0b00001000, 0b10000010}},
	 {Dodge::LineData::set(2, 2, 0), {0b11100111, 0b10000001}},
	 {Dodge::LineData::set(4, 2, 1), {0b01110111, 0b01010101, 0b01010101, 0b01010101}},
	 {Dodge::LineData::set(4, 2, 1), {0b00011000, 0b10000001, 0b11000011}},
	 {Dodge::LineData::set(5, 2, 1), {0b01100000, 0b01001110, 0b01001000, 0b00011000, 0b00000011}},
	 {Dodge::LineData::set(2, 2, 1), {0b00001111, 0b11000000}},
};

Dodge::Dodge(Display *display) : Game(display), dot_state_(true)
{
}

void Dodge::start(Event *event)
{
	event->removeAllTimers();
	event->addTimer(200);
	event->addTimer(); // not yet start button timer
	event->addTimer(50);
	event->setFlag(Event::ProcessPinChanges);
	event->setFlag(Event::ProcessTimerOverflows);

	score_ = 0;
	pos_ = Pos(3, 15);
	elements_.removeAll();
	for (byte i = 0; i < 12; i++)
	{
		elements_ << 0;
	}
	appending();
	render();
	return;
}

void Dodge::setSpeed(byte v)
{
}

void Dodge::resetHighscore()
{
	eeprom_write_word(&EE_highscore, highscore_ = 0);
}

bool Dodge::onButtonChange(Event *event)
{
	if (event->buttonDown().pressed())
	{
		event->timer(0).setInterval(100);
	}
	else if (event->buttonDown().released())
	{
		event->timer(0).setInterval(200);
	}

	Timer &move_timer = event->timer(1);
	// btn left
	if (event->buttonLeft().changed())
	{
		if (event->buttonLeft().state())
		{
			if (!event->buttonRight().state())
			{
				left();
				move_timer.setInterval(200);
				move_timer.start();
			}
		}
		else
		{
			move_timer.stop();
			move_timer.clearOverflow();
		}
	}

	// btn right
	if (event->buttonRight().changed())
	{
		if (event->buttonRight().state())
		{
			if (!event->buttonLeft().state())
			{
				right();
				move_timer.setInterval(200);
				move_timer.start();
			}
		}
		else
		{
			move_timer.stop();
			move_timer.clearOverflow();
		}
	}
}

bool Dodge::onTimerOverflow(Event *event)
{
	if (event->timer(0).overflow())
	{
		if (pos_.pos_y > 13)
			pos_.pos_y--;
		else
			elements_.removeFirst();
		score_++;
		if (score_ > highscore_)
		{
			is_new_highscore_ = true;
		}
		appending();
		if (bitRead(elements_.itemAt(15 - pos_.pos_y), pos_.pos_x))
		{
			if (score_ > highscore_)
			{
				is_new_highscore_ = true;
				eeprom_write_word(&EE_highscore, highscore_ = score_);
			}
			return true;
		}
		render();
	}
	Timer &move_timer = event->timer(1);
	if (move_timer.overflow())
	{
		if (event->buttonLeft().state())
		{
			left();
			move_timer.setInterval(100);
		}

		else if (event->buttonRight().state())
		{
			right();
			move_timer.setInterval(100);
		}
	}
	if (event->timer(2).overflow())
	{
		dot_state_ = !dot_state_;
		render();
	}
	return false;
}

void Dodge::onStop(Event *event)
{
	Game::onStop(event);
	event->timer(0).stop();
	event->timer(1).stop();
	event->timer(2).stop();
}
void Dodge::onContinue(Event *event)
{
	event->timer(0).restart();
	event->timer(1).restart();
	event->timer(2).restart();
	render();
}

void Dodge::render()
{
	display_->clear();
	for (byte i = 0; i < 16; i++)
	{
		display_->setRow(i, elements_.itemAt(15 - i));
	}
	if (dot_state_)
		display_->setPixel(pos_);
}

byte Dodge::randomLineId()
{
	static byte last;
	byte r = millis() % ELEMENTS_NUM;
	while (last == r)
	{
		r = millis() % ELEMENTS_NUM;
	}
	last = r;
	return r;
}

byte Dodge::appendElement(byte element)
{
	LineData line_data = lines[element];
	for (byte i = 0; i < line_data.topMargin(); i++)
	{
		elements_ << 0;
	}
	for (byte i = 0; i < line_data.width(); i++)
	{
		elements_ << line_data.data_[i];
	}
	for (byte i = 0; i < line_data.bottomMargin(); i++)
	{
		elements_ << 0;
	}
}

void Dodge::appending()
{
	while (elements_.size() <= 16)
	{
		appendElement(randomLineId());
	}
}

void Dodge::right()
{
	Pos tmp = pos_;
	tmp.pos_x += 1;
	if (tmp.pos_x > 7)
		return;
	if (bitRead(elements_.itemAt(15 - tmp.pos_y), tmp.pos_x))
	{
		return;
	}
	pos_ = tmp;
	render();
}

void Dodge::left()
{
	Pos tmp = pos_;
	tmp.pos_x -= 1;
	if (tmp.pos_x < 0)
		return;
	if (bitRead(elements_.itemAt(15 - tmp.pos_y), tmp.pos_x))
	{
		return;
	}
	pos_ = tmp;
	render();
}
