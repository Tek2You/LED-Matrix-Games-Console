#pragma once
#include "trigger.h"
#include "timer.h"
#include "button.h"

class ButtonAutoTrigger : public Trigger
{
public:
	ButtonAutoTrigger();

	enum Direction{
		BTN_1,
		BTN_2,
		NO_BTN,
	} dir_;

	// Trigger interface
protected:

	bool process(const unsigned long &timer_);
	void stop() override;
	void start() override;

private:
	Timer timer_;

	int first_interval_;
	int interval_;

	bool activated_ = true;

	Button * button1_;
	Button * button2_;
};
