#pragma once
#include "avr.h"
#include "display.h"
#include "game.h"
#include "tetromino.h"

class Tetris : public Game
{
public:
	Tetris(Display *display);
	~Tetris();

	void start(Event *event);

	unsigned int points() const { return points_; }
	void setSpeed(const byte v) override;

	// highscore functions
	static unsigned int highscore();
	static void resetHighscore();

protected:
	// game interface
	bool onButtonChange(Event *event) override;
	bool onTimerOverflow(Event *event) override;
	void render();

private:
	// private functions only for tetris
	void clear();
	bool rotate();
	bool right();
	bool left();
	bool down();

	void checkRowsFinished();
	bool newTetromino();
	void takeOverTetromino();
	// random tetromino data functions
	tetromino::SHAPE randomTetrominoShape();
	tetromino::DIRECTION randomTetrominoDirection(const tetromino::SHAPE shape);

	// from speed calculated delay intervals
	int general_step_interval_;
	int general_down_interval_;
	int general_first_move_interval_;
	int general_move_interval_;

	byte *field_; // field without current tetromino
	Tetromino *tetromino_;
	unsigned int points_ = 0;
	static unsigned int highscore_;
};
