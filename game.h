#pragma once
#include "avr.h"
#include "display.h"
#include "tetromino.h"

inline void * operator new(size_t size)
{
  return malloc(size);
}

inline void operator delete(void * ptr)
{
  free(ptr);
}

class Game
{
public:
	 Game(Display * display);
	 void process();
	 void render();
	 void rotate();
	 bool step();
	 bool newTetromino(tetromino::TETROMINO shape, tetromino::DIRECTION direction);


private:
	 Display * display_;
	 byte * field_; // field without current tetromino
	 Tetromino * current_tetromino_;
};
