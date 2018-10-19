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
	void process();
	void start();
	int getPoints(){return points_;}
private:
	enum Direction { UP, RIGHT, DOWN, LEFT};

	Direction direction_;
	Pos head_pos_;
	Pos * body_buffer_;
	Pos * body_buffer_end_;

	Pos * body_start_;
//	Pos * body_end_;
	Pos eat_pos_;
	const int body_buffer_len_ = 80;
	int body_len_;

	void render();
	void eat();
	bool validate(Pos &pos);
	bool isValid(Pos &pos);
	Pos * getBodyPos(int pos);

	int points_;
};
