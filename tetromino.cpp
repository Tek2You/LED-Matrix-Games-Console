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

Tetromino::Tetromino(Shape shape, byte heigth, byte *field, Direction direction, Pos pos)
    : shape_(shape), heigth_(heigth), field_(field), direction_(direction), pos_(pos)
{
}

bool Tetromino::getPositions(Pos (&positions)[4]) const
{
	return getPositions(positions, shape_, direction_, pos_);
}

bool Tetromino::getPositions(Pos (&positions)[4], const Shape &shape, const Direction &direction, const Pos &pos)
{
	Specifications brick = tetrominos[shape];

	if (!brick.directions & BV_(direction)) // direction not available
		return false;

	const Pos pos_rotate = brick.shape[0];
	for (int i = 0; i < 4; i++)
	{
		const Pos dot_pos = brick.shape[i].toPos() - pos_rotate;
		Pos rotated_dot_pos;
		switch (direction)
		{
		case BOTTOM:
			rotated_dot_pos = dot_pos;
			break;
		case LEFT:
			rotated_dot_pos.pos_x = dot_pos.pos_y;
			rotated_dot_pos.pos_y = -dot_pos.pos_x;
			break;
		case TOP:
			rotated_dot_pos.pos_x = -dot_pos.pos_x;
			rotated_dot_pos.pos_y = -dot_pos.pos_y;
			break;
		case RIGHT:
			rotated_dot_pos.pos_x = -dot_pos.pos_y;
			rotated_dot_pos.pos_y = dot_pos.pos_x;
			break;
		default:
			break;
		}

		positions[i] = pos + rotated_dot_pos;
	}
	return true;
}

const Direction Tetromino::rotate(const Direction direction, const Shape shape)
{
	byte directions = possibleDirections(shape);
	if (directions == bit(direction))
	{
		return direction; // not rotateable
	}

	Direction new_direction = direction;

	for (int i = 0; i < 4; i++)
	{
		new_direction = Direction((byte(new_direction) + 1) % 4);
		if (directions & bit(new_direction))
			return new_direction;
	}
	return direction;
}

void Tetromino::rotate()
{
	rotate(direction_, shape_);
}

byte Tetromino::possibleDirections() const
{
	return possibleDirections(shape_);
}

byte Tetromino::possibleDirections(const Shape shape)
{
	Specifications brick = tetrominos[shape];
	return brick.directions;
}

byte Tetromino::possibleDirectionNum(const Shape shape)
{
	byte num = 0;
	byte directions = possibleDirections(shape);
	for (int i = 0; i < 4; i++)
	{
		if (bitRead(directions, i))
			num++;
	}
	return num;
}

byte Tetromino::possibleDirectionNum() const
{
	possibleDirectionNum(shape_);
}

byte Tetromino::isValid() const
{
	return isValid(shape_, direction_, pos_);
}

byte Tetromino::isValid(const Shape shape, const Direction direction, const Pos pos) const
{
	Pos positions[4];
	byte valid_errors = 0;
	getPositions(positions, shape, direction, pos);
	for (int i = 0; i < 4; i++)
	{
		if (positions[i].pos_y > (heigth_ - 1))
		{
			valid_errors |= OVER_ABOVE;
		}
		else if (positions[i].pos_y < 0)
		{
			valid_errors |= OVER_BELOW; // is over below
		}
		if (positions[i].pos_x < 0)
		{
			valid_errors |= OVER_LEFT; // is left over
		}

		else if (positions[i].pos_x > 7)
		{
			valid_errors |= OVER_RIGHT; // is right over
		}

		if (bitRead(*(field_ + positions[i].pos_y), byte(positions[i].pos_x)))
		{
			valid_errors |= COLLIDE; // collides with exiting tetromino part
		}
	}
	return valid_errors; // no collisions - valid
}
