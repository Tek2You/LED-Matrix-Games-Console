#pragma once
#include "game.h"

#include "list.h"

class SpaceInvaders : public Game
{
public:
	SpaceInvaders(Display *display);

	// Game interface
public:
	void start(Event *event) override;
	void setSpeed(byte v) override;
	unsigned int score() const override;

protected:
	bool onButtonChange(Event *event) override;
	bool onTimerOverflow(Event *event) override;
	void onStop(Event *event) override;
	void onContinue(Event *event) override;
	void render() override;

private:
	struct Shot
	{
        Shot(){
            t_ = nullptr;
        }
        Shot(Event * event){
            event->addTimer();
            t_ = &event->timers_.last();
        }
		bool process(int colum_value)
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
		byte start_row_;
		byte end_row_;
		byte col_;
		Timer *t_;
	};
	List<Shot> shots_;
};
