#pragma once
#include "game.h"

#include "list.h"
#include "staticlist.h"

class Shot
{
public:
	Shot();
	Shot(byte col);
	byte row_;
	byte col_;
};

class SpaceInvaders : public Game
{
public:
	SpaceInvaders(Display *display);

	static void resetHighscore();
    static unsigned int highscore();
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
	StaticList<byte> invaders_;
	List<Shot> shots_;
	byte pos_;

	// score
	unsigned int score_ = 0;
	static unsigned int highscore_;

	int step_interval_;
	int first_move_interval_;
	int move_interval_;

	enum MoveDirection
	{
		NO_MOVE = 0,
		RIGHT_MOVE = 1,
		LEFT_MOVE = 2,
	} move_dir_;

	void left();
	void right();
	bool processShot(Shot &s);
	void insertRow();
    void updateHighscore();
};
