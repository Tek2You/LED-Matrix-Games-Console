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
	// set speed for case that it wouldnt be done
	setSpeed(2);
}

Snake::~Snake() {}

void Snake::start(Event *event)
{
	// load standard flags
	event->setupGame();

	// Create initial Snake
	body_ << SmartPos(1, 7);
	body_ << SmartPos(2, 7);
	body_ << SmartPos(3, 7); // head

	// Initial eat
	eat_pos_ = SmartPos(5, 7);

	// setup directions
	direction_ = START;
	new_direction_ = START;

	// Setup timer for moving
	event->addTrigger(new Timer(interval_));
	event->trigger(0)->start();

	// print display first time
	render();
}

// stores intervals for diferent speeds in progmem
const unsigned int periods[] PROGMEM = {600, 500, 400, 275, 180};

void Snake::setSpeed(const byte v)
{
	// read speed out of progmem
	interval_ = pgm_read_word(&periods[v]);
}

void Snake::resetHighscore()
{
	// reset highscore
	highscore_ = 0;
	// take over to eeprom
	eeprom_write_word(&EE_highscore, highscore_);
}

bool Snake::onButtonChange(Event *event)
{
	if (event->controlButtonPressed())
	{
		// ensure that there is not ability to make a rotation by 180°
		if (event->buttonUp().pressed())
		{
			if (direction_ != Snake::DOWN && direction_ != Snake::UP)
			{
				new_direction_ = Snake::UP;
				event->trigger(0)->restart();
				return tick();
			}
		}
		else if (event->buttonRight().pressed())
		{
			if (direction_ != Snake::LEFT && direction_ != Snake::RIGHT)
			{
				new_direction_ = Snake::RIGHT;
				event->trigger(0)->restart();
				return tick();
			}
		}

		else if (event->buttonLeft().pressed())
		{
			if (direction_ != Snake::RIGHT && direction_ != Snake::LEFT && direction_ != Snake::START)
			{
				new_direction_ = Snake::LEFT;
				event->trigger(0)->restart();
				return tick();
			}
		}

		else if (event->buttonDown().pressed())
		{
			if (direction_ != Snake::UP && direction_ != Snake::DOWN)
			{
				new_direction_ = Snake::DOWN;
				event->trigger(0)->restart();
				return tick();
			}
		}
	}
	return false;
}

bool Snake::onTimerOverflow(Event *event)
{
	if (event->trigger(0)->triggered())
	{
		return tick();
	}
	return false;
}

void Snake::onStop(Event *event)
{
	// stop timer during break
	event->trigger(0)->stop();
	// let the stop symbol and score print
	Game::onStop(event);
}

void Snake::onContinue(Event *event)
{
	// restart the stopped timer
	event->trigger(0)->start();
	// executes render and clears the score text
	Game::onContinue(event);
}

void Snake::render()
{
	// first of all, clear the display
	display_->clear();
	// set the eat pixel
	display_->setPixel(eat_pos_);
	// interate through all body pixels
	for (SmartPos tmp : body_)
	{
		// now set those pixels
		display_->setPixel(tmp.toPos());
	}
	// finaly, show on display
	display_->show(false);
}

bool Snake::eat(const Pos pos)
{
	// check if eat was received
	if (pos != eat_pos_) return false;

	// prevent entering a remaining loop when the game is won
	if (body_.size() >= display_->rows() * display_->cols()) return true;
	unsigned long time = millis();

	// temporary new pos
	Pos tmp;
	do
	{
		wdt_reset();
		// generate a random pos
		tmp = Pos((time + char(rand())) % 8, (time + (char(rand()))) % 16);
		// try as long as the position doesnt match
	} while (!isValid(tmp) || tmp == pos);

	// when free position is found, set it as the new eat
	eat_pos_ = tmp;
	return true;
}

bool Snake::tick()
{
	// equalize directions
	direction_ = new_direction_;

	// figure out move vector
	Pos vect;
	switch (direction_)
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

	// new head
	// already remove it, to ensure a true validation
	vect += body_.last();

	// move head inside the game field
	validate(vect);

	// process eat
	if (!eat(vect))
	{
		// only remove the Pixel, when we didnt eat, ohterwise the snake will grow
		body_.removeFirst();
	}
	// check for new highscore. Directly save to avoid a not save in case of
	// reset or poweroff.
	if (score() + 1 > highscore_) // + 1 because we didnt add the next vect because of the isValid() function
	{
		// write new highscore
		highscore_ = score() + 1;
		// save new highscore in eeprom
		eeprom_write_word(&EE_highscore, highscore_);
		is_new_highscore_ = true;
	}

	// check for colidation
	if (!isValid(vect))
	{
		// append the buffer to ensure correct score(computed out of the size of the list)
		body_ << vect.toSmartPos();
		return true;
	}
	body_ << vect.toSmartPos();

	render();
	return false;
}

// check if the given position is valid,
// so isnt touching any point of the snake
bool Snake::isValid(const Pos &pos) const
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
