/* spaceinvaders.h : Space Invaders game library
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once
#include "game.h"

#include "list.h"
#include "staticlist.h"
#include "timer.h"
#include "buttonautotrigger.h"

class Shot
{
public:
	Shot();
	Shot(byte col);
	byte row_;
	byte col_;
};

class SpaceInvaders : public Game
{
public:
	SpaceInvaders(Display *display);

	static void resetHighscore();
	static unsigned int highscore() { return highscore_; }
	// Game interface
public:
	void start(Event *event) override;
	void setSpeed(const byte v) override { speed_ = v; }
	unsigned int score() const override { return score_; }

protected:
	bool onButtonChange(Event *event) override;
	bool onTriggered(Event *event) override;
	void onStop(Event *event) override;
	void onContinue(Event *event) override;
	void render() override;

private:
	StaticList<byte> invaders_;
	List<Shot> shots_;
	byte pos_;

	// score
	unsigned int score_ = 0;
	static unsigned int highscore_;

	Timer * step_timer_;
	Timer * shot_timer_;
	ButtonAutoTrigger * auto_move_;

	enum SpeedFlag
	{
		StepInterval = 0,
		ShotInterval = 1,
		FirstMoveInterval = 2,
		MoveInterval = 3,
	};

	void left();
	void right();
	bool processShot(Shot &s);
	void insertRow();
	void updateHighscore(const byte offset = 0);

	byte speed_;
	unsigned int readSpeed(const SpeedFlag flag) const;

};
