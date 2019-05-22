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
	byte directions = tetromino::tetrominos[shape_].directions;
	if (directions == bit(direction_))
	{
		return; // not rotateable
	}

	Direction tmp = direction_;
	do
	{
		tmp = Direction((byte(tmp) + 1) % 4);
	} while (directions & bit(tmp));
	direction_ = tmp;
}

byte Tetromino::validationErrors(const StaticList<byte> *field) const
{
	Pos positions[4];
	getPoints(positions);
	byte valid_errors = 0;
	for (const SmartPos *pos = tetromino::tetrominos[shape_].pos; pos < &tetromino::tetrominos[shape_].pos[4]; pos++)
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
	byte directions = tetromino::tetrominos[shape_].directions;
	byte num = Tetromino::possibleDirectionNum();

	if (num <= 1)
	{
		direction_ = TOP;
		return;
	}
	byte rand = ((millis()) % (num));
	for (int i = 0, j = 0; i < 4; i++)
	{
		if (bitRead(directions, i))
		{
			if (j == rand)
			{
				direction_ = Direction(j);
				return;
			}
			j++;
		}
	}
}

byte Tetromino::possibleDirectionNum() const
{
	byte num = 0;
	byte directions = tetromino::tetrominos[shape_].directions;
	for (byte i = 0x01; i; i = i << 1)
		if (i & directions) num++;
	return num;
}

Specifications Tetromino::specifications() const {}

bool Tetromino::getPoints(Pos (&positions)[4]) const
{
	Specifications brick = tetrominos[shape_];

	if (!brick.directions & BV_(direction_)) // direction not available
		return false;

	const Pos pos_rotate = brick.pos[0];
	for (int i = 0; i < 4; i++)
	{
		const Pos dot_pos = brick.pos[i].toPos() - pos_rotate;

		Pos tmp_pos;
		switch (direction_)
		{
		case BOTTOM:
			tmp_pos = dot_pos;
			break;
		case LEFT:
			tmp_pos.pos_x = dot_pos.y();
			tmp_pos.pos_y = -dot_pos.pos_x;
			break;
		case TOP:
			tmp_pos.pos_x = -dot_pos.pos_x;
			tmp_pos.pos_y = -dot_pos.y();
			break;
		case RIGHT:
			tmp_pos.pos_x = -dot_pos.y();
			tmp_pos.pos_y = dot_pos.pos_x;
			break;
		default:
			break;
		}

		positions[i] = pos_ + tmp_pos;
	}
	return true;
}
