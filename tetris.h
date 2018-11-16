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
	void start();
	bool process(byte& event) override;
	bool up();
	bool right();
	bool left();
	bool down();
	void reset();
	void clear();
	unsigned int points(){return points_;}
	void setSpeed(byte v) override;

	static unsigned int highscore();

	static void resetHighscore();

private:
	void render();

	void checkRowsFinished();
	bool newTetromino();
	void takeOverTetromino();

	byte * field_; // field without current tetromino
	Tetromino * tetromino_;
	unsigned int points_ = 0;
	static unsigned int highscore_;

	tetromino::SHAPE randomTetrominoShape();
	tetromino::DIRECTION randomTetrominoDirection(tetromino::SHAPE shape);


};
