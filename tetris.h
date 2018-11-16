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
	bool up() override;
	bool right() override;
	bool left() override;
	bool down() override;
	void reset() override;
	void clear() override;
	unsigned int points(){return points_;}
	void setSpeed(byte v) override;

	// highscore functions
	static unsigned int highscore();
	static void resetHighscore();

private:
	unsigned long *down_timer_, *move_timer_;
	int down_period_, move_period_;

	// from speed calculated delay intervals
	int general_step_interval_;
	int general_down_interval_;
	int general_first_move_interval_;
	int general_move_interval_;

	// current button states
	bool btn_down_state_;
	bool btn_left_state_;
	bool btn_right_state_;

	byte * field_; // field without current tetromino
	Tetromino * tetromino_;
	unsigned int points_ = 0;
	static unsigned int highscore_;

	// game interface
	void render();

	// private functions only for tetris
	void checkRowsFinished();
	bool newTetromino();
	void takeOverTetromino();
	// random tetromino data functions
	tetromino::SHAPE randomTetrominoShape();
	tetromino::DIRECTION randomTetrominoDirection(tetromino::SHAPE shape);
};
