#pragma once
#include "game.h"
#include "list.h"
#include "position.h"

class Snake : public Game
{
public:
	Snake(Display *display, unsigned long *t);
	~Snake();
	void start();
	bool process(Event *event) override;
	void reset();
	void clear();

	unsigned int points() const { return body_.size() - 1; }
	void setSpeed(byte v) override;

	static unsigned int highscore();
	static void resetHighscore();

private:
	void render();
	bool eat();
	bool isValid(Pos &pos);
	bool validate(Pos &pos);
	bool move();

	enum Direction
	{
		UP,
		RIGHT,
		DOWN,
		LEFT,
		START
	};

	// processing timer variables
	unsigned long *timer_;
	int period_;

	// snake values
	Direction direction_;
	List<Pos> body_;
	Pos eat_pos_;
	static unsigned int highscore_;
};
