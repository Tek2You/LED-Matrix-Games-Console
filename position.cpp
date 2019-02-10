/* position.cpp : 2 demensional Vector Library
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

#include "position.h"

Pos::Pos(char x, char y) : pos_x(x), pos_y(y)
{
}

Pos::Pos(const SmartPos &pos)
{
	pos_x = pos.x();
	pos_y = pos.y();
}

SmartPos Pos::toSmartPos()
{
	return SmartPos(this->pos_x, this->pos_y);
}

Pos Pos::operator+(const Pos &pos)
{

	return Pos(pos.pos_x + pos_x, pos.pos_y + pos_y);
}

Pos Pos::operator+(const Pos &pos) const
{

	return Pos(pos.pos_x + pos_x, pos.pos_y + pos_y);
}

Pos &Pos::operator+=(const Pos &pos)
{
	pos_x += pos.pos_x;
	pos_y += pos.pos_y;
}

Pos Pos::operator-(const Pos &pos)
{
	return Pos(pos_x - pos.pos_x, pos_y - pos.pos_y);
}

Pos &Pos::operator-=(const Pos &pos)
{
	pos_x -= pos.pos_x;
	pos_y -= pos.pos_y;
}

bool Pos::operator==(const Pos &pos)
{
	return (pos_x == pos.pos_x && pos_y == pos.pos_y);
}

Pos &Pos::operator=(const SmartPos &pos)
{
	pos_x = pos.x();
	pos_y = pos.y();
	return *this;
}

bool Pos::operator!=(const Pos &pos)
{
	return !operator==(pos);
}

Pos Pos::operator~()
{
	return Pos(pos_y, pos_x);
}

SmartPos::SmartPos(unsigned char x, unsigned char y)
{
	setXy(x, y);
}

Pos SmartPos::toPos()
{
	return Pos(x(), y());
}

unsigned char SmartPos::x() const
{
	return (compact_pos_ & 0xF0) >> 4;
}

unsigned char SmartPos::y() const
{
	return compact_pos_ & 0x0F;
}

unsigned char SmartPos::setXy(unsigned char x, unsigned char y)
{
	compact_pos_ = (y & 0x0F) | ((x & 0x0F) << 4);
}
