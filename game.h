#pragma once
#include "avr.h"
#include "display.h"
#include "tetromino.h"
#include "operators.h"

class Game
{
public:
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
