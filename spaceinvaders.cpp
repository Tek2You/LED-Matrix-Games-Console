#include "spaceinvaders.h"

SpaceInvaders::SpaceInvaders(Display *display) : Game(display, SPACE_INVADERS) {
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

	render();
}

void SpaceInvaders::setSpeed(byte v)
{

}

unsigned int SpaceInvaders::score() const
{

}

bool SpaceInvaders::onButtonChange(Event *event)
{
	if (event->buttonRight().pressed())
	{
		if(pos_ < 7)
		pos_++;
		render();
	}
	else if (event->buttonLeft().pressed())
	{
		if(pos_ != 0)
			pos_--;
		render();
	}
}

bool SpaceInvaders::onTimerOverflow(Event *event)
{

}

void SpaceInvaders::onStop(Event *event)
{

}

void SpaceInvaders::onContinue(Event *event)
{

}

void SpaceInvaders::render()
{
	display_->clear();
	display_->setPixel(pos_-1,0);
	display_->setPixel(pos_,0);
	display_->setPixel(pos_,1);
	display_->setPixel(pos_+1,0);
	display_->show();
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
