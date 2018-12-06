#pragma once
#include "game.h"
#include "position.h"
#include "tetromino.h" // only for combined position

class Jump : public Game
{
public:
	Jump(Display *display);
	~Jump();

	// Game interface
public:
	void start(Event *event);
	bool process(Event *event);
	void clear();
	void reset();

	unsigned int points() const;
	void setSpeed(byte v);
	static unsigned int highscore();
	static void resetHighscore();

private:
	int forward_period_, jump_period_;
	void newHind();
	bool isValid(Pos pos);
	void forward();
	void jump(Event *event);
	byte *row(byte n);

protected:
	void render();
	bool man_state_;
	Pos man_pos_;
	byte next_hind_;
	byte *field_;
	char current_field_start_;
	bool is_jumping_ = false;
	byte jump_count_;
	unsigned int score_ = 0;
	static unsigned int highscore_;
	int jump_height_;
	int jump_lenght_;

	const SmartPos man_points[6] = {SmartPos(0, 7), SmartPos(0, 5),
	                                SmartPos(1, 6), SmartPos(1, 5),
	                                SmartPos(1, 4), SmartPos(2, 5)};
	const SmartPos man_moving_points[2] = {SmartPos(1, 7), SmartPos(2, 7)};

	struct Hind
	{
		SmartPos positions[4];
	};

	const Hind hinds_[7] = {
	    {{SmartPos(0, 6), SmartPos(1, 7), SmartPos(1, 6), SmartPos(2, 6)}},
	    {{SmartPos(0, 7), SmartPos(0, 6), SmartPos(0, 5), SmartPos(0, 7)}},
	    {{SmartPos(0, 7), SmartPos(0, 6), SmartPos(1, 7), SmartPos(1, 6)}},
	    {{SmartPos(0, 7), SmartPos(4, 6), SmartPos(0, 7), SmartPos(0, 7)}},
	    {{SmartPos(0, 7), SmartPos(1, 7), SmartPos(1, 6), SmartPos(2, 7)}},
	    {{SmartPos(0, 7), SmartPos(0, 6), SmartPos(0, 5), SmartPos(0, 7)}},
	    {{SmartPos(0, 7), SmartPos(0, 7), SmartPos(0, 7), SmartPos(0, 7)}}};
};
