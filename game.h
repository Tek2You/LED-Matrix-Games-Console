#pragma once
#include "avr.h"
#include "display.h"
#include "tetromino.h"

class Game
{
public:
	inline void * operator new(size_t size)
	{
		 return malloc(size);
	}

	inline void operator delete(void * ptr)
	{
		 free(ptr);
	}
	Game(Display * display);
	~Game();
	void render();
	bool rotate();
	bool right();
	bool left();
	bool step();
	void reset();
	void clear();
	void begin();
	int getPoints(){return points_;}
	void checkRowsFinished();


private:
	Display * display_;
	byte * field_; // field without current tetromino
	Tetromino * tetromino_;
	int points_;

	bool newTetromino();
	void takeOverTetromino();
	tetromino::SHAPE randomTetrominoShape();
	tetromino::DIRECTION randomTetrominoDirection(tetromino::SHAPE shape);
};
