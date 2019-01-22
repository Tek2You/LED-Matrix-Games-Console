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

const Dodge::LineData lines[2] = {
   {Dodge::LineData::set(3,2,0),{0b11100111,0b11000011, 0b10000001,0}},

};

Dodge::Dodge(Display * display) : Game(display)
{

}

void Dodge::start(Event *event)
{
	pos_ = Pos(3,15);
	elements_.removeAll();
	for(byte i = 0; i < 12; i++){
		elements_ << 0;
	}
	appending();
}

void Dodge::setSpeed(byte v)
{

}

bool Dodge::isNewHighscore() const
{

}

bool Dodge::onButtonChange(Event *event)
{

}

bool Dodge::onTimerOverflow(Event *event)
{
	if(event->timer(0).overflow()){
		elements_.removeFirst();
		appending();
		render();
	}
}
void Dodge::onContinue(Event *event)
{

}

void Dodge::render()
{
	display_->clear();
	display_->setPixel(pos_);

}

byte Dodge::randomLineId()
{
	return millis() % sizeof(lines);
}

byte Dodge::appendElement(byte element)
{
	LineData line_data = lines[element];
	for(byte i = 0; i < line_data.topMargin(); i++){
		elements_ << 0;
	}
	for(byte i = 0; i < line_data.width(); i++){
		elements_ << line_data.data_[i];
	}
	for(byte i = 0; i < line_data.bottomMargin(); i++){
		elements_ << 0;
	}
}

void Dodge::appending()
{
	while(elements_.size() <= 16){
		appendElement(randomLineId());
	}
}
