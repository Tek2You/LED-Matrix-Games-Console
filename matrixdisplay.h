/* button.cpp : Display driving library
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

class MatrixDisplay
{
public:
	MatrixDisplay(byte width, byte height);
	~MatrixDisplay();
	void processShow();
	void clear();
	void setPixel(byte col, const byte row, const bool value = true);
	inline void setPixel(const Pos p, const bool value = true) { setPixel(p.pos_x, p.pos_y); }
	void setArray(byte *array);
	const byte rows()
	{
		return height_;
	}
	const byte cols()
	{
		return width_;
	}
	int setString(const char *s, int column, char cursor_pos, char spacing = 1, byte offset = 0);
	byte setChar(char ch, int column, byte offset);
	void clearRows(byte start, byte end);
	void setColumn(byte column, byte value, byte offset = 0);
	void clearColumns(byte start, byte end, byte offset);
	void setRow(byte row, int value);
	void show(bool take_over = true);

	virtual void setBrightness(const byte brightness);

	// compute width (in columns) of a single char
	static int width(char ch);
	// compute width (in columns) of text
	static int width(const char *s, char spacing = 1);
	// write integer value into string digits of max size
	char *formatInt(char *digits, byte size, int value);

protected:
	static const byte *letterStart(char ch);
	static const byte letterWidth(char ch);
	byte *columnPtr(byte column) const;
	byte orderCols(const byte value);
	byte mapCol(byte row);

private:
	inline byte* getBuffer();
	inline byte* getPendingBuffer();
	bool pending_;
	byte *rows1_;
	byte *rows2_;
	const byte height_;
	const byte width_;
	int brigthness_;
};
