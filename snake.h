#pragma once
#include "game.h"
#include "position.h"

class Snake : public Game
{
public:
	Snake(Display * display);
	~Snake();
	bool up();
	bool right();
	bool left();
	bool down();
	void reset();
	void clear();
	bool process();
	void start();
	int getPoints(){return body_len_;}
private:
	enum Direction { UP, RIGHT, DOWN, LEFT};

	Direction direction_;
	Pos head_pos_;
	Pos eat_pos_;

	const int body_buffer_len_ = 80;
	Pos body_buffer_[80];
	int body_start_;
	int body_end_;

	int body_len_;
//	Pos * body_end_;

	void render();
	bool eat();
	bool validate(Pos &pos);
	bool isValid(Pos &pos);
	Pos * getBodyPos(int pos);
};