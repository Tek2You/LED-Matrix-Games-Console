#pragma once
#include "game.h"
#include "position.h"

class Snake : public Game
{
public:
	Snake(Display * display):
	   Game(display),
	   body_end_(0), body_start_(0)
	{
		body_len_ = 0;
		body_buffer_len_;
		direction_ = START;
	}
	~Snake();
	void start();
	bool process(byte &event) override;
	bool up();
	bool right();
	bool left();
	bool down();
	void reset();
	void clear();
	unsigned int points(){return body_len_-2;}
	void setSpeed(byte v) override;
	static unsigned int highscore();

	enum Direction { UP, RIGHT, DOWN, LEFT, START};
	static void resetHighscore();

private:

	Direction direction_;
	Pos head_pos_;
	Pos eat_pos_;

	const int body_buffer_len_ = 80;
	Pos body_buffer_[80];
	int body_start_;
	int body_end_;

	unsigned int body_len_;
	void render();
	bool eat();
	bool validate(Pos &pos);
	bool isValid(Pos &pos);
	Pos * getBodyPos(int pos);

	static unsigned int highscore_;

	// Game interface
public:
};
