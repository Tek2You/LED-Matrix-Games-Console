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
#include "avr/pgmspace.h"
#include "position.h"
#include "staticlist.h"

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

#define BV_(bit) 1 << bit

const int points_i[4] PROGMEM = {0b0100010001000100, 0b0000000011110000, 0b0100010001000100, 0b0000000011110000};
const int points_j[4] PROGMEM = {0b0000010001001100, 0b0000100011100000, 0b0000011001000100, 0b0000000011100010};
const int points_l[4] PROGMEM = {0b0000010001000110, 0b0000000011101000, 0b0000110001000100, 0b0000001011100000};
const int points_o[4] PROGMEM = {0b0000011001100000, 0b0000011001100000, 0b0000011001100000, 0b0000011001100000};
const int points_s[4] PROGMEM = {0b0000000001101100, 0b0000010001100010, 0b0000000001101100, 0b0000010001100010};
const int points_t[4] PROGMEM = {0b0000000011100100, 0b0000010011000100, 0b0000010011100000, 0b0000010001100100};
const int points_z[4] PROGMEM = {0b0000000011000110, 0b0000010011001000, 0b0000000011000110, 0b0000010011001000};

const int *const tetrominos_[7] PROGMEM = {points_i, points_j, points_l, points_o, points_s, points_t, points_z};
}

using namespace tetromino;

class Tetromino
{
public:
	Tetromino();

	void getPoints(Pos (&positions)[4]) const;
	void rotate();
	byte validationErrors(const StaticList<byte> *field) const;
	inline bool isValid(const StaticList<byte> *field) const { return validationErrors(field) == VALID; }
	void randomShape();
	void randomDirection();

	Shape shape_;
	Direction direction_;
	Pos pos_;
};
