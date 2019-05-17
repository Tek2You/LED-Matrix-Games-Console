#include "spaceinvaders.h"

SpaceInvaders::SpaceInvaders(Display *display) : Game(display, SPACE_INVADERS)
{
	display->clear();
	display->show();
	pos_ = 3;
}

void SpaceInvaders::start(Event *event)
{
	event->removeAllTimers();
	event->setFlag(Event::ProcessPinChanges);
	event->setFlag(Event::ProcessTimerOverflows);
	event->setFlag(Event::ProcessStop);
	event->addTimer();
	event->addTimer();
	render();
}

void SpaceInvaders::setSpeed(byte v) {
	switch (v)
	{
	case 0:
		step_interval_ = 1800;
		down_interval_ = 180;
		first_move_interval_ = 400;
		move_interval_ = 200;
		break;
	case 1:
		step_interval_ = 1400;
		down_interval_ = 140;
		first_move_interval_ = 400;
		move_interval_ = 160;
		break;
	case 3:
		step_interval_ = 800;
		down_interval_ = 80;
		first_move_interval_ = 350;
		move_interval_ = 120;
		break;
	case 4:
		step_interval_ = 500;
		down_interval_ = 50;
		first_move_interval_ = 300;
		move_interval_ = 100;
		break;
	case 2:
		step_interval_ = 1000;
		down_interval_ = 100;
		first_move_interval_ = 300;
		move_interval_ = 140;
		break;
	default:
		break;
	}
}

unsigned int SpaceInvaders::score() const {}

bool SpaceInvaders::onButtonChange(Event *event)
{
	Timer &move_timer = event->timer(1);
	// btn left
	if (event->buttonLeft().pressed())
	{
		if (!event->buttonRight().state())
		{
			left();
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
	return false;
}

bool SpaceInvaders::onTimerOverflow(Event *event)
{
	Timer &move_timer = event->timer(1);
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
		move_timer.setInterval(move_interval_);
		move_timer.restart();
	}
	return false;
}

void SpaceInvaders::onStop(Event *event) {}

void SpaceInvaders::onContinue(Event *event) {}

void SpaceInvaders::render()
{
	display_->clear();
	display_->setPixel(pos_ - 1, 0);
	display_->setPixel(pos_, 0);
	display_->setPixel(pos_, 1);
	display_->setPixel(pos_ + 1, 0);
	display_->show();
}

void SpaceInvaders::right()
{

	if (pos_ < 7) pos_++;
	render();
}

void SpaceInvaders::left()
{

	if (pos_ != 0) pos_--;
	render();
}

SpaceInvaders::Shot::Shot() { t_ = nullptr; }

SpaceInvaders::Shot::Shot(Event *event)
{
	event->addTimer();
	t_ = &event->timers_.last();
}

bool SpaceInvaders::Shot::process(int colum_value)
{
	if (t_ == nullptr || !t_->overflow()) return false;
	end_row_++;
	if (end_row_ - start_row_ >= 4)
	{
		start_row_++;
	}
	if (bitRead(end_row_, end_row_))
	{
		return true;
	}
	return false;
}
