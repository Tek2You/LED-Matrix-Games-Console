#pragma once
#include "game.h"
#include "position.h"

class Snake : public Game
{
public:
	Snake(Display * display, unsigned long * t);
	~Snake();
	void start();
	bool process(byte &event) override;
	void reset();
	void clear();

	unsigned int points(){return body_len_-2;}
	void setSpeed(byte v) override;

	static unsigned int highscore();
	static void resetHighscore();

private:
	void render();
	bool eat();
	bool isValid(Pos &pos);
	bool validate(Pos &pos);
	Pos * getBodyPos(int &pos);
	bool move();

	enum Direction { UP, RIGHT, DOWN, LEFT, START};

	// processing timer variables
	unsigned long * timer_;
	int period_;

	// snake values
	Direction direction_;
	Pos head_pos_;
	Pos eat_pos_;
	unsigned int body_len_;
	const int body_buffer_len_ = 80;
	Pos body_buffer_[80];
	int body_start_;
	int body_end_;
	static unsigned int highscore_;
};


