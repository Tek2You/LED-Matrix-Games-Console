#pragma once
#include "avr.h"
#include "display.h"
#include "tetromino.h"
#include "game.h"

class Tetris : public Game
{
public:
	Tetris(Display * display, unsigned long *t1, unsigned long *t2);
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
	unsigned long *down_timer_, *move_timer_;
	int down_period_, move_period_;

	int general_step_interval = 1800;
	int general_down_interval = 180;
	int general_first_move_interval = 550;
	int general_move_interval = 200;

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
