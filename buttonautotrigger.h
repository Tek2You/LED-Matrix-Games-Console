#pragma once
#include "button.h"
#include "timer.h"
#include "trigger.h"

class ButtonAutoTrigger : public Trigger
{
public:
	ButtonAutoTrigger(const Button *button1, const Button *button2, const unsigned int first_interval = 200,
							const unsigned int interval = 150);
	// trigger public interface

	void stop() override;
	void start() override;

	enum Direction
	{
		BTN_1,
		BTN_2,
		NO_BTN,
	} dir_;

	Direction direction() { return dir_; }

	void setInterval(const unsigned int interval) { interval_ = interval; }
	void setFirstInterval(const unsigned int interval) { first_interval_ = interval; }

	// Trigger interface
protected:
	bool process(const unsigned long &timer_);

private:
	Timer timer_;

	unsigned int first_interval_;
	unsigned int interval_;

	bool activated_ = true;

	const Button *button1_;
	const Button *button2_;
};
