/* position.h : 2 demensional Vector Library
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

struct SmartPos;

struct Pos
{
	Pos()
	{
	}
	Pos(char x, char y);
	Pos(const SmartPos &pos);
	SmartPos toSmartPos();
	Pos operator+(const Pos &pos);
	Pos operator+(const Pos &pos) const;
	Pos &operator+=(const Pos &pos);
	Pos operator-(const Pos &pos);
	Pos &operator-=(const Pos &pos);
	bool operator==(const Pos &pos);
	Pos &operator=(const SmartPos &pos);
	bool operator!=(const Pos &pos);
	Pos operator~();
	char pos_x = 0;
	char pos_y = 0;
};

struct SmartPos
{
	SmartPos()
	{
	}
	SmartPos(unsigned char x, unsigned char y);
	Pos toPos();
	unsigned char x() const;
	unsigned char y() const;
	inline unsigned char setXy(unsigned char x, unsigned char y);
	unsigned char compact_pos_;
};
