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

Dodge::Dodge(Display *display) : Game(display, DODGE), dot_state_(true), score_(0) {}

void Dodge::start(Event *event)
{
	step_timer_ = new Timer(readSpeed(StepInterval));
	event->addTrigger(step_timer_);
	blink_timer_ = new Timer(20);
	event->addTrigger(blink_timer_);
	auto_move_ = new ButtonAutoTrigger(&event->buttonLeft(), &event->buttonRight(), readSpeed(FirstMoveInterval),
												  readSpeed(MoveInterval));
	event->addTrigger(auto_move_);

	pos_ = Pos(3, 15);
	elements_.removeAll();
	for (byte i = 0; i < 12; i++)
	{
		elements_ << 0;
	}
	appendElements();
	render();
	return;
}

const int speeds_[] PROGMEM = {
	 400, 200, 300, 200, // very slow
	 300, 150, 320, 150, // shlow
	 250, 125, 300, 125, // medium fast
	 200, 100, 250, 100, // fast
	 150, 75,  200, 75,  // very fast
};

void Dodge::setSpeed(byte v) { speed_ = v; }

void Dodge::resetHighscore() { eeprom_write_word(&EE_highscore, highscore_ = 0); }

bool Dodge::onButtonChange(Event *event)
{
	if (event->buttonDown().changed())
	{
		unsigned int interval;
		if (event->buttonDown().state())
		{
			if (tick()) return true;
			interval = readSpeed(FastStepInterval);
		}
		else
			interval = readSpeed(StepInterval);
		step_timer_->setInterval(interval);
		step_timer_->restart();
	}
	return false;
}

bool Dodge::onTriggered(Event *event)
{
	if (step_timer_->triggered())
	{
		if (tick()) return true;
	}

	if (auto_move_->triggered())
	{
		ButtonAutoTrigger::Direction dir = auto_move_->direction();
		move(dir);
	}

	if (blink_timer_->triggered())
	{
		dot_state_ = !dot_state_;
	}
	render();
	return false;
}

void Dodge::onStop(Event *event)
{
	Game::onStop(event);
	step_timer_->stop();
	blink_timer_->stop();
	auto_move_->stop();
}
void Dodge::onContinue(Event *event)
{
	Game::onContinue(event);
	step_timer_->restart();
	blink_timer_->restart();
	auto_move_->restart();
}

void Dodge::render()
{
	display_->clear();
	for (byte i = 0; i < 16; i++)
	{
		display_->setRow(i, elements_.itemAt(15 - i));
	}
	if (dot_state_) display_->setPixel(pos_);
	display_->show(false);
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

void Dodge::appendElements()
{
	while (elements_.size() <= 16)
	{
		appendElement(randomLineId());
	}
}

bool Dodge::tick()
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
	appendElements();
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
	return false;
}

void Dodge::move(ButtonAutoTrigger::Direction dir)
{
	Pos tmp = pos_;
	tmp.pos_x += (dir == ButtonAutoTrigger::BTN_1 ? -1 : 1);
	if (tmp.pos_x > 7) return;
	if (tmp.pos_x < 0) return;
	if (bitRead(elements_.itemAt(15 - tmp.pos_y), tmp.pos_x))
	{
		return;
	}
	pos_ = tmp;
}

unsigned int Dodge::readSpeed(const SpeedFlag flag) { return pgm_read_word(&speeds_[flag + speed_ * 4]); }
