#include "buttonautotrigger.h"

ButtonAutoTrigger::ButtonAutoTrigger(const Button *button1, const Button *button2, unsigned int first_interval, unsigned int interval)
	 : dir_(NO_BTN), button1_(button1), button2_(button2), first_interval_(first_interval), interval_(interval)
{
}

bool ButtonAutoTrigger::process(const unsigned long &t)
{
	if (activated_ = false){
		timer_.clear();
		return false;
	}
	// timer

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

	else if (button2_->changed())
	{
		if (button2_->state())
		{
			if (!button1_->state())
			{
				triggered_ = true;
				onTriggered();
				timer_.setInterval(first_interval_);
				timer_.start();
				dir_ = BTN_2;
			}
		}
		else if (dir_ == BTN_2)
		{
			timer_.clear();
			if (button1_->state())
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
	else if (timer_.process(t))
	{
		timer_.clear();
		triggered_ = true;
		onTriggered();
		timer_.setInterval(interval_);
		timer_.start();
	}
	return triggered_;
}

void ButtonAutoTrigger::stop()
{
	activated_ = false;
	timer_.stop();
}

void ButtonAutoTrigger::start() { activated_ = true; }
