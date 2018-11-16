#pragma once
#include "game.h"
#include "position.h"
#include "tetromino.h" // only for combined position

class RunningMan : public Game
{
public:
	RunningMan(Display *display);
	~RunningMan();

	// Game interface
public:
	void start() override;
	bool process() override;
	void clear() override;
	void reset() override;
	bool right() override;
	bool left() override;
	bool up() override;
	bool down() override;
	unsigned int points() override;
	byte *row(byte n);
	static unsigned int highscore();
	static void resetHighscore();

private:
	void newHind();
	bool isValid(Pos pos);
protected:
	void render() override;
	bool man_state_;
	Pos man_pos_;
	byte next_hind_;
	byte * field_;
	char current_field_start_;
	bool is_jumping_ = false;
	byte jump_count_;
	unsigned int score_ = 0;
	static unsigned int highscore_;
	byte jump_height_;
	byte jump_period_;

	const SmartPos man_points[6] = {SmartPos(0,7),SmartPos(0,5),SmartPos(1,6),SmartPos(1,5),SmartPos(1,4),SmartPos(2,5)};
	const SmartPos man_moving_points[2] = {SmartPos(1,7),SmartPos(2,7)};

	struct Hind
	{
		SmartPos positions[4];
	};

	const Hind hinds_[7] = {{{SmartPos(0,6), SmartPos(1,7),SmartPos(1,6), SmartPos(2,6)}},
	                        {{SmartPos(0,7), SmartPos(0,6),SmartPos(0,5), SmartPos(0,7)}},
	                        {{SmartPos(0,7), SmartPos(0,6),SmartPos(1,7), SmartPos(1,6)}},
	                        {{SmartPos(0,7), SmartPos(4,6),SmartPos(0,7), SmartPos(0,7)}},
	                        {{SmartPos(0,7), SmartPos(1,7),SmartPos(1,6), SmartPos(2,7)}},
	                        {{SmartPos(0,7), SmartPos(0,6),SmartPos(0,5), SmartPos(0,7)}},
	                        {{SmartPos(0,7), SmartPos(0,7),SmartPos(0,7), SmartPos(0,7)}}
	                       };
};
