/* tetromino.cpp : Tetromino for Tetris game
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

#include "tetromino.h"

Tetromino::Tetromino() {}

void Tetromino::rotate()
{
	// chose next direction
	direction_ = Direction((byte(direction_) + 1) % 4);
}

byte Tetromino::validationErrors(const StaticList<byte> *field) const
{
	Pos positions[4];
	getPoints(positions);
	byte valid_errors = 0;
	for (const Pos *pos = &positions[0]; pos < &positions[4]; pos++)
	{
		if (pos->y() > (field->size() - 1))
		{
			valid_errors |= OVER_ABOVE;
		}
		else if (pos->y() < 0)
		{
			valid_errors |= OVER_BELOW; // is over below
		}
		if (pos->x() < 0)
		{
			valid_errors |= OVER_LEFT; // is left over
		}
		else if (pos->x() > 7)
		{
			valid_errors |= OVER_RIGHT; // is right over
		}
		if (bitRead(field->itemAt(pos->y()), pos->x()))
		{
			valid_errors |= COLLIDE; // collides with exiting tetromino part
		}
	}
	return valid_errors; // no collisions - valid
}

void Tetromino::randomShape()
{
	// use millis as random seed
	shape_ = Shape(millis() % 7);
}

void Tetromino::randomDirection()
{
	// use millis as random seed
	direction_ = Direction(millis() % 4);
}

void Tetromino::getPoints(Pos (&positions)[4]) const
{
	PGM_P p = (PGM_P)pgm_read_word(&(tetrominos_[shape_]));
	int array = pgm_read_word(p + direction_ * 2);

	for (int i = 0, j = 0; i < 16; i++)
	{
		if (array & (1 << i))
		{
			positions[j] = Pos(i / 4 - 1, i % 4 - 2) + pos_;
			if (j++ == 4)
			{
				return;
			}
		}
	}
}
