#pragma once
#include "avr.h"
#include "display.h"
#include "tetromino.h"
#include "operators.h"
#include "game.h"

class Tetris : public Game
{
public:
	Tetris(Display * display);
	~Tetris();
	bool up();
	bool right();
	bool left();
	bool down();
	void reset();
	void clear();
	bool process();
	void start();
	int getPoints(){return points_;}


private:
	void render();

	void checkRowsFinished();
	bool newTetromino();
	void takeOverTetromino();

	byte * field_; // field without current tetromino
	Tetromino * tetromino_;
	int points_;

	tetromino::SHAPE randomTetrominoShape();
	tetromino::DIRECTION randomTetrominoDirection(tetromino::SHAPE shape);
};
