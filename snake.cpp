#include "snake.h"
#include "avr/eeprom.h"
#include "operators.h"

static unsigned int EE_highscore EEMEM = 0;
unsigned int Snake::highscore_ = eeprom_read_word(&EE_highscore);

Snake::Snake(Display *display)
    : Game(display)
{
	direction_ = START;
	new_direction_ = START;
	setSpeed(2);
}

Snake::~Snake()
{
}

void Snake::clear()
{
	display_->clear();
}

void Snake::start(Event *event)
{
	clear();
	body_.append(Pos(1, 7));
	body_.append(Pos(2, 7));
	body_.append(Pos(3, 7)); // head
	eat_pos_ = Pos(5, 7);
	direction_ = START;
	new_direction_ = START;
	render();

	event->setFlag(Event::ProcessPinChanges);
	event->setFlag(Event::ProcessTimerOverflows);
	event->removeAllTimers();
	event->addTimer(0, period_);
	event->timer(0).start();
}

bool Snake::move()
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

	vect += body_.last();
	// check if highscore is broken. Directly save to avoid a not save in case of
	// reset or poweroff.
	if (body_.size() - 3 > highscore_)
	{
		highscore_ = body_.size() - 3;
		eeprom_write_word(&EE_highscore, highscore_);
		is_new_highscore_ = true;
	}
	if (!validate(vect))
	{ // game end
		return true;
	}

	body_.append(vect);
	if (!eat())
	{
		body_.removeFirst();
	}

	render();
	return false;
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
		return;
	case 3:
		period_ = 300;
		break;
	case 4:
		period_ = 200;
		break;
	case 2:
	default:
		period_ = 400;
		break;
	}
}

unsigned int Snake::highscore()
{
	return highscore_ = eeprom_read_word(&EE_highscore);
}

void Snake::render()
{
	display_->clear();
	display_->setPixel(eat_pos_.pos_x, eat_pos_.pos_y, true);
	for (int i = 0; i < body_.size(); i++)
	{
		Pos p = body_.itemAt(i);
		display_->setPixel(p.pos_x, p.pos_y, true);
	}
}

bool Snake::eat()
{
	if (body_.last() == eat_pos_)
	{
		Pos p = Pos(char(millis() % 8), char(millis() % 16));
		while (!isValid(p))
		{
			p = Pos(char(millis() % 8), char(millis() % 16));
		}

		eat_pos_ = p;
		return true;
	}
	return false;
}

bool Snake::validate(Pos &pos)
{
	// out of range
	if (pos.pos_x < 0)
		pos.pos_x = 7;
	else if (pos.pos_x > 7)
		pos.pos_x = 0;
	if (pos.pos_y < 0)
		pos.pos_y = 15;
	else if (pos.pos_y > 15)
		pos.pos_y = 0;
	return isValid(pos);
}

bool Snake::isValid(Pos &pos)
{
	//  if colides, return true
	for (int i = 0; i < body_.size(); i++)
	{
		if (body_.itemAt(i) == pos)
		{
			return false;
		}
	}
	return true;
}

void Snake::resetHighscore()
{
	eeprom_write_word(&EE_highscore, highscore_ = 0);
}

bool Snake::onButtonChange(Event *event)
{
	if (event->changed() && event->isPressed())
	{
		// is not 180° rotation or no rotation(in this case the snake will make
		// a additinal ste,  what we avoid with dont allow this)
		if (event->buttonUpChanged() && event->buttonUpState())
		{
			if (direction_ != Snake::DOWN)
			{
				new_direction_ = Snake::UP;
			}
		}
		else if (event->buttonRightChanged() && event->buttonRightState())
		{
			if (direction_ != Snake::LEFT)
			{
				new_direction_ = Snake::RIGHT;
			}
		}

		else if (event->buttonLeftChanged() && event->buttonLeftState())
		{
			if (direction_ != Snake::RIGHT && direction_ != Snake::START)
			{
				new_direction_ = Snake::LEFT;
			}
		}

		else if (event->buttonDownChanged() && event->buttonDownState())
		{
			if (direction_ != Snake::UP)
			{
				new_direction_ = Snake::DOWN;
			}
		}
	}
}

bool Snake::onTimerOverflow(Event *event)
{
	if (event->timer(0).overflow())
	{
		return move();
	}
	return false;
}
