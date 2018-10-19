#pragma once
#include "game.h"

class Snake : public Game
{
public:
	 Snake(Display * display);
	 ~Snake();
	 bool up(); // rotate
	 bool right();
	 bool left();
	 bool down(); // makes a step
	 void reset();
	 void clear();
	 void process();
	 void start();
	 int getPoints(){return points_;}
private:
	 enum Direction { UP, RIGHT, DOWN, LEFT};

	 Direction direction_;

	 void render();
	 int points_;
};
