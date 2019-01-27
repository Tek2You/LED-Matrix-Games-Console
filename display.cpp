/* display.cpp : Library provides text and advanced display functions
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

#include "display.h"
#include "operators.h"

Display::Display(byte height, byte width) : MatrixDisplay(height, width), text1_(this), text2_(this)
{
	loadMenuConfig();
}

void Display::setIcon(uint64_t icon, byte offset)
{
	Icon i;
	i.i64 = icon;
	for (int r = offset; r < 8 + offset; r++)
	{
		setRow(r, i.array[7 + offset - r]);
	}
}

void Display::update()
{
	text1_.update();
	text2_.update();
}

void Display::loadMenuConfig()
{
	clear();
	text1_.clear();
	text2_.clear();
	text1_.setOffset(8);
	text1_.setOperationRows(8, 16);
	text1_.setOperationCols(0, 7);
	text1_.setShiftStartCol(3);

	text2_.setOffset(0);
	text2_.setOperationRows(0, 8);
	text2_.setOperationCols(0, 7);
	text2_.setShiftStartCol(3);
}

void Display::loadsGameCofig()
{
	text1_.clear();
	text2_.clear();
	clear();
}

void Display::setBrightness(byte brigthness)
{
	if (brigthness > 3)
	{
		brigthness = 3;
	}
	const byte values[4] = {100, 220, 245, 255};
	MatrixDisplay::setBrightness(values[brigthness]);
}
