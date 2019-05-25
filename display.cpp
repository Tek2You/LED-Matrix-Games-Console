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

void Display::setIcon(const Icon icon, const byte offset, const bool show)
{
	for (int r = offset; r < 8 + offset; r++)
	{
		setRow(r, icon.data_.i8[7 + offset - r]);
	}
	if(show)
		Display::show();
}

void Display::update()
{
	unsigned long t = millis();
	text1_.process(t);
	text2_.process(t);
}

void Display::loadMenuConfig()
{
	text1_.clear();
	text2_.clear();
	text1_.setOffset(8);
	text1_.setShiftStartCol(3);

	text2_.setOffset(0);
	text2_.setShiftStartCol(3);
	clear();
	show();
}

void Display::loadsGameCofig()
{
	text1_.clear();
	text2_.clear();
	clear();
	show();
}

void Display::setBar(byte n, const byte max)
{
	byte cols = this->cols() / float(max) * float(n);
	clear();
	for (n = 0; n < cols; n++)
	{
		setColumn(n, 0xFF);
	}
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


