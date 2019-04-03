#pragma once
#include "game.h"



class SpaceInvaders : public Game
{
public:
    SpaceInvaders();

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
	 struct ShotCol{
		int pixels_;
		byte shot_;
	 };
	 ShotCol * shot_cols_;
};
