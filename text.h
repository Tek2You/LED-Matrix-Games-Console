/* text.h : Support text options for the led matrix (also shifeted)
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
#include "matrixdisplay.h"
#include "timer.h"

class Text : public Timer
{
public:
	Text(MatrixDisplay *display);

	enum ShiftMode
	{
		NO_SHIFT = 0,
		SHIFT = 1,
		OFF = 2
	};

	enum Align
	{
		LEFT,
		MIDDLE,
		RIGHT,
	};

	void setText(const char *text, bool show = true);
	void setNumber(const int &number, bool show = true);
	void setAlignment(const Align align) { alignment_ = align; }

	void clear();

	void setShiftSpeed(const int speed);
	int shiftSpeed() const { return speed_; }

	void setOffset(const byte offset) { offset_ = offset; }
	byte offset() const { return offset_; }

	void setCursor(const char pos);
	char getCursor() const { return curser_pos_; }

	void setOperationRows(const byte start, const byte end);
	void setOperationCols(const byte start, const byte end);

	void setShiftStartCol(byte col)
	{
		shift_start_col_ = col;
		setText(text_);
	}

private:
	void shift();
	void onOverflow() override;
	void computeShiftMode();

	MatrixDisplay *display_;

	const char *text_;  // displayed text
	const char *first_; // first visible character
	int current_shift_start_col_;
	byte shift_start_col_;
	ShiftMode shift_mode_;
	int speed_; // columns per second
	byte offset_;
	char curser_pos_;
	byte start_col_, end_col_, start_row_, end_row_;
	char number_buffer_[10];
	Align alignment_;
};
