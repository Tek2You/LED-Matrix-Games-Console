/* buttonautotrigger.cpp : for automove with two buttons
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

#include "buttonautotrigger.h"

ButtonAutoTrigger::ButtonAutoTrigger(const Button *button1, const Button *button2, const unsigned int first_interval,
												 const unsigned int interval)
	 : dir_(NO_BTN), button1_(button1), button2_(button2), first_interval_(first_interval), interval_(interval)
{
}

bool ButtonAutoTrigger::process(const unsigned long &t)
{
	if (activated_ = false)
	{
		timer_.clear();
		return false;
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
	// timer	// timer
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
