/* snake.cpp : Snake game library
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

#include "snake.h"
#include "avr/eeprom.h"
#include "avr/wdt.h"
#include "operators.h"

static unsigned int EE_highscore EEMEM = 0;
unsigned int Snake::highscore_ = eeprom_read_word(&EE_highscore);

Snake::Snake(Display *display) : Game(display, SNAKE), body_(display->rows() * display->cols())
{
	direction_ = START;
	new_direction_ = START;
	setSpeed(2);
}

Snake::~Snake() {}

void Snake::start(Event *event)
{
	body_ << SmartPos(1, 7);
	body_ << SmartPos(2, 7);
	body_ << SmartPos(3, 7); // head
	eat_pos_ = SmartPos(5, 7);
	direction_ = START;
	new_direction_ = START;
	render();

	event->setFlag(Event::ProcessPinChanges);
	event->setFlag(Event::ProcessTimerOverflows);
	event->setFlag(Event::ProcessStop);
	event->removeAllTimers();
	event->addTimer(period_);
	event->timer(0).start();
	event->addTimer(20);
	event->timer(1).start();
}

void Snake::setSpeed(byte v)
{
	switch (v)
	{
	case 0:
		period_ = 600;
		break;
	case 1:
		period_ = 500;
		break;
	case 3:
		period_ = 275;
		break;
	case 4:
		period_ = 180;
		break;
	case 2:
	default:
		period_ = 400;
		break;
	}
}

unsigned int Snake::highscore() { return highscore_; }
void Snake::resetHighscore() { eeprom_write_word(&EE_highscore, highscore_ = 0); }

bool Snake::onButtonChange(Event *event)
{
	if (event->controlButtonPressed())
	{
		// is not 180° rotation or no rotation(in this case the snake will make
		// a additinal ste,  what we avoid with dont allow this)
		if (event->buttonUp().pressed())
		{
			if (direction_ != Snake::DOWN)
			{
				new_direction_ = Snake::UP;
			}
		}
		else if (event->buttonRight().pressed())
		{
			if (direction_ != Snake::LEFT)
			{
				new_direction_ = Snake::RIGHT;
			}
		}

		else if (event->buttonLeft().pressed())
		{
			if (direction_ != Snake::RIGHT && direction_ != Snake::START)
			{
				new_direction_ = Snake::LEFT;
			}
		}

		else if (event->buttonDown().pressed())
		{
			if (direction_ != Snake::UP)
			{
				new_direction_ = Snake::DOWN;
			}
		}
	}
	return false;
}

bool Snake::onTimerOverflow(Event *event)
{
	if(event->timer(1).overflow()){
		eat_state_ = !eat_state_;
		render();
	}
	if (event->timer(0).overflow())
	{
		return tick();
	}
	return false;
}

void Snake::onStop(Event *event)
{
	event->timer(0).stop();
	Game::onStop(event);
}

void Snake::onContinue(Event *event)
{
	event->timer(0).start();
	Game::onContinue(event);
}

void Snake::render()
{
	display_->clear();
	if(eat_state_) {
		display_->setPixel(eat_pos_.pos_x, eat_pos_.pos_y, true);
	}
	for (SmartPos tmp : body_)
	{
		display_->setPixel(tmp.x(), tmp.y(), true);
	}
	display_->show(false);
}

bool Snake::eat(Pos pos)
{
	if (pos == eat_pos_)
	{
		// ensure that we dont enter a remaining loop if the game is won
		if (body_.size() >= display_->rows() * display_->cols()) return true;
		unsigned long time = millis();
		Pos tmp;
		do
		{
			wdt_reset();
			tmp = Pos((time + char(rand())) % 8, (time + (char(rand()))) % 16);
		} while (!isValid(tmp) || tmp == pos);
		eat_pos_ = tmp;
		return true;
	}
	return false;
}

bool Snake::tick()
{
	Pos vect;
	switch (direction_ = new_direction_)
	{
	case UP:
		vect = Pos(0, 1);
		break;
	case DOWN:
		vect = Pos(0, -1);
		break;
	case RIGHT:
		vect = Pos(1, 0);
		break;
	case LEFT:
		vect = Pos(-1, 0);
		break;
	default:
		return false;
		break;
	}

	// make the new head position outof the vector and last body pos

	vect += body_.last();
	validate(vect);
	if (!eat(vect))
	{
		body_.removeFirst(); // first remove last item to ensure that there is a correct validation
	}
	// check if highscore is broken. Directly save to avoid a not save in case of
	// reset or poweroff.
	if (score() + 1 > highscore_) // + 1 because we didnt add the next vect because of the isValid() function
	{
		highscore_ = score() + 1;
		eeprom_write_word(&EE_highscore, highscore_);
		is_new_highscore_ = true;
	}
	if (!isValid(vect))
	{                              // game end
		body_ << vect.toSmartPos(); // append the the buffer to ensure correct score(computed out of the size of the list)
		return true;
	}
	body_ << vect.toSmartPos();

	render();
	return false;
}

// check if the given position is valid,
// so isnt touching any point of the snake
bool Snake::isValid(const Pos &pos)
{
	//  if colides, return true
	for (SmartPos tmp : body_)
	{
		if (pos == tmp.toPos())
		{
			return false;
		}
	}
	return true;
}

// moves the given direction into the specified game field
// and check if the position is valid afterwards
bool Snake::validate(Pos &pos)
{
	// out of range
	if (pos.pos_x < 0)
		pos.pos_x = display_->cols() - 1;
	else if (pos.pos_x > display_->cols() - 1)
		pos.pos_x = 0;
	if (pos.pos_y < 0)
		pos.pos_y = display_->rows() - 1;
	else if (pos.pos_y > display_->rows() - 1)
		pos.pos_y = 0;
	return isValid(pos);
}
