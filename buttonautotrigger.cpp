#include "buttonautotrigger.h"

ButtonAutoTrigger::ButtonAutoTrigger() : dir_(NO_BTN) {}

bool ButtonAutoTrigger::process(const unsigned long &t)
{
	if(activated_ = false) return false;
	// timer
	if (timer_.triggered())
	{
		triggered_ = true;
		onTriggered();
		timer_.setInterval(interval_);
		timer_.start();
	}

	// buttons
	if (button1_->changed())
	{
		if (button1_->state())
		{
			if (!button2_->state())
			{
				triggered_ = true;
				onTriggered();
				timer_.setInterval(first_interval_);
				timer_.start();
				dir_ = BTN_1;
			}
		}
		else if (dir_ == BTN_1)
		{
			timer_.clear();
			if (button2_->state())
			{
				dir_ = BTN_2;
				timer_.setInterval(first_interval_);
				timer_.start();
			}
			else
			{
				dir_ = NO_BTN;
				timer_.stop();
			}
		}
	}
	if (button2_->changed())
	{
		if(button2_->state()){
			if (!button1_->state())
			{
				triggered_ = true;
				onTriggered();
				timer_.setInterval(first_interval_);
				timer_.start();
				dir_ = BTN_2;
			}
		}
		else if(dir_ == BTN_2){
			timer_.clear();
			if( button1_->state())
			{
				dir_ = BTN_1;
				timer_.setInterval(first_interval_);
				timer_.start();
			}
			else
			{
				timer_.stop();
				dir_ = NO_BTN;
			};
		}
	}
	return triggered_;
}

void ButtonAutoTrigger::stop() { activated_ = false; timer_.stop();}

void ButtonAutoTrigger::start() { activated_ = true; }
