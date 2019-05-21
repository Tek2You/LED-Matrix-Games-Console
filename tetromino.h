/* tetromino.h : Tetromino for Tetris game
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
#include "position.h"

namespace tetromino
{
enum Shape
{
	I,
	J,
	L,
	O,
	S,
	T,
	Z
};
enum Direction : byte
{
	TOP = 0,
	RIGHT = 1,
	BOTTOM = 2,
	LEFT = 3,
};

enum VALIDATION_ERROR : byte
{
	VALID = 0,
	OVER_BELOW = (1 << 0),
	OVER_ABOVE = (1 << 1),
	OVER_RIGHT = (1 << 2),
	OVER_LEFT = (1 << 3),
	COLLIDE = (1 << 4),
};

struct Specifications
{
	byte directions;
	SmartPos shape[4];
};

#define BV_(bit) 1 << bit

const Specifications tetrominos[7] = {
    {BV_(TOP) | BV_(RIGHT), {SmartPos(0, 1), SmartPos(0, 0), SmartPos(0, 2), SmartPos(0, 3)}},
    {BV_(TOP) | BV_(RIGHT) | BV_(BOTTOM) | BV_(LEFT), {SmartPos(1, 1), SmartPos(1, 0), SmartPos(1, 2), SmartPos(0, 2)}},
    {BV_(TOP) | BV_(RIGHT) | BV_(BOTTOM) | BV_(LEFT), {SmartPos(0, 1), SmartPos(0, 0), SmartPos(0, 2), SmartPos(1, 2)}},
    {BV_(TOP), {SmartPos(0, 0), SmartPos(0, 1), SmartPos(1, 0), SmartPos(1, 1)}},
    {BV_(TOP) | BV_(RIGHT), {SmartPos(1, 0), SmartPos(1, 1), SmartPos(0, 1), SmartPos(2, 0)}},
    {BV_(TOP) | BV_(RIGHT) | BV_(BOTTOM) | BV_(LEFT), {SmartPos(1, 0), SmartPos(0, 0), SmartPos(1, 1), SmartPos(2, 0)}},
    {BV_(TOP) | BV_(RIGHT), {SmartPos(1, 0), SmartPos(0, 0), SmartPos(1, 1), SmartPos(2, 1)}},
};
} // namespace tetromino

using namespace tetromino;
class Tetromino
{
public:
	Tetromino(Shape shape, byte heigth, byte *field, Direction direction, Pos pos);

	void setPos(const Pos &pos)
	{
		pos_ = pos;
	}
	const Pos &pos() const
	{
		return pos_;
	}

	void setDirection(const Direction direction)
	{
		direction_ = direction;
	}
	const Direction direction() const
	{
		return direction_;
	}

	const Shape shape() const
	{
		return shape_;
	}

	bool getPositions(Pos (&positions)[4]) const;
	static bool getPositions(Pos (&positions)[4], const Shape &shape, const Direction &direction, const Pos &pos);

	static const Direction rotate(const Direction direction, const Shape shape);
	void rotate();

	byte isValid() const;
	byte isValid(const Shape shape, const Direction direction, const Pos pos) const;

	static byte possibleDirections(const Shape shape);
	byte possibleDirections() const;
	static byte possibleDirectionNum(const Shape shape);
	byte possibleDirectionNum() const;

private:
	const Shape shape_;
	Direction direction_;
	Pos pos_;
	const byte heigth_;
	const byte *field_;
};
