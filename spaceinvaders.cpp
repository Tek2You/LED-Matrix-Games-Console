#include "spaceinvaders.h"

SpaceInvaders::SpaceInvaders(Display *display)
	 : Game(display, SPACE_INVADERS), invaders_(StaticList<byte>(display->rows()))
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
	event->addTimer();
	event->timer(2).setInterval(50);
	event->timer(2).start();
	render();
}

void SpaceInvaders::setSpeed(byte v)
{
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
	if (event->buttonDown().pressed())
	{
		shots_ << Shot(pos_, event);
		render();
	}

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

	if (event->timer(0).overflow())
	{
	}

	if (event->timer(2).overflow())
	{

		for (Shot &s : shots_)
		{
			if (processShot(s))
			{
				shots_.remove(s);
			}
			render();
		}
	}

	//	render();

	return false;
}

void SpaceInvaders::onStop(Event *event)
{
	event->timer(0).stop();
	event->timer(1).stop();
	Game::onStop(event);
}

void SpaceInvaders::onContinue(Event *event)
{
	event->timer(0).restart();
	Game::onContinue(event);
}

void SpaceInvaders::render()
{
	display_->clear();

	for (Shot s : shots_)
	{
		for (int i = s.start_row_; i < s.end_row_; i++)
		{
			display_->setPixel(s.col_, i);
		}
	}

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

Shot::Shot() : t_(nullptr), col_(0) {}

Shot::Shot(byte col, Event *event) : col_(col)
{
	end_row_ = 2;
	start_row_ = 2;
	//	event->addTimer();
	//	t_ = &event->timers_.last();
	//	t_->setInterval(100);
	//	t_->start();
}
// Shot::Shot() { t_ = nullptr; }

// Shot::Shot(byte col, Event *event) : col_(col)
//{
//	end_row = 4;
//}

bool SpaceInvaders::processShot(Shot &s)
{
	//	if (s.t_ == nullptr || !s.t_->overflow()) return false;
	if (s.end_row_ < display_->rows()) s.end_row_++;
	if (s.end_row_ >= 4 && s.start_row_ < display_->rows()) s.start_row_++;
	if (s.start_row_ == display_->rows())
	{

		// shot complete
		// need to remove
		//		s.t_->stop();
		return true;
	}
	//	s.t_->restart();

	//		if (bitRead(end_row_, end_row_) || start_row_ > list->maxSize())
	//		{
	//			return true;
	//		}
	//		return false;
	//	}
	return false;
}
