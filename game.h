#pragma once
#include "avr.h"
#include "display.h"
#include "tetromino.h"

class Game
{
public:
	 Game(Display * display,byte height, byte width);
	 void process();
	 void render();
private:
	 Display * display_;
	 const byte height_;
	 const byte width_;
	 byte * field_; // field without current tetromino
	 Tetromino * current_tetromino_;
};
