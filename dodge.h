/* dodge.h : Dodge game class
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
#include "avr.h"
#include "game.h"

class Dodge : public Game
{
public:
	Dodge(Display *display);

	// Game interface
public:
	void start(Event *event) override;
	void setSpeed(byte v) override;

	unsigned int score() const
	{
		return score_;
	}
	static unsigned int highscore()
	{
		return highscore_;
	}
	static void resetHighscore();

	struct LineData
	{
		byte settings_;
		byte data_[5];
		static byte set(byte width, byte top_margin, byte bottom_margin)
		{
			if (width > 5)
				width = 5;
			if (top_margin > 3)
				top_margin = 3;
			if (bottom_margin > 3)
				bottom_margin = 3;
			return width | (top_margin << 3) | (bottom_margin << 5);
		}
		byte width()
		{
			return (settings_ & 0b00000111);
		}
		byte topMargin()
		{
			return (settings_ >> 3) & 0b00000011;
		}
		byte bottomMargin()
		{
			return (settings_ >> 5) & 0b00000011;
		}
	};

protected:
	bool onButtonChange(Event *event) override;
	bool onTimerOverflow(Event *event) override;
	void onStop(Event *event) override;
	void onContinue(Event *event) override;
	void render() override;

private:
	byte randomLineId();
	byte appendElement(byte element);
	void appending();
	void right();
	void left();

	unsigned int score_;
	bool dot_state_;
	Pos pos_;
	List<byte> elements_;
	static unsigned int highscore_;
};
