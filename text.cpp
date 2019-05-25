/* text.cpp : Support text options for the led matrix (also shifeted)
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

#include "text.h"

Text::Text(MatrixDisplay *display)
	 : display_(display), shift_mode_(OFF), offset_(0), shift_start_col_(3), alignment_(MIDDLE)
{
	setShiftSpeed(5);
}

void Text::clear()
{
	display_->clearRows(offset_, offset_ + 8);
	first_ = text_ = nullptr;
	shift_mode_ = OFF;
	Timer::stop();
}

// shift text by one column and start over if nothing is shown anymore

// display (and remember) text (for future shifting)
void Text::setText(const char *text, const bool show)
{
	first_ = text_ = text;
	display_->clearRows(offset_, offset_ + 8);

	computeShiftMode();
	if (show)
	{
		display_->show();
	}
}

void Text::setNumber(const int value, const bool show)
{
	setText(display_->formatInt(number_buffer_, 10, value), show);
}

void Text::setShiftSpeed(const int speed)
{
	speed_ = speed;
	setInterval(1000 / speed_); // from shifts per second to mseconds per shift
}

void Text::setShiftStartCol(const byte col)
{
	shift_start_col_ = col;
	setText(text_);
}

void Text::onTriggered()
{
	shift();
	Timer::clearTriggered();
	display_->show();
}

void Text::shift()
{
	--current_start_col_;
	if (display_->setString(first_, current_start_col_, 1, offset_) <= 0)
	{
		first_ = text_;
		current_start_col_ = shift_start_col_;
	}
	else
	{ // should we advance to next start char?
		byte w = display_->width(*first_);
		if (current_start_col_ + w < 0)
		{
			++first_;
			current_start_col_ += w + 1; // char width + 1 column spacing
		}
	}
}

void Text::computeShiftMode()
{
	int width = display_->width(text_);
	shift_mode_ = (width > display_->cols() - 1 ? SHIFT : NO_SHIFT);

	if (shift_mode_ == SHIFT)
	{ // when shifting, start in column 7
		current_start_col_ = shift_start_col_ + 1;
		shift();
		Timer::start();
	}
	else
	{ // otherwise, start start like the alignment pretends
		if (alignment_ == RIGHT)
		{
			current_start_col_ = display_->cols() - width;
		}
		else if (alignment_ == MIDDLE)
		{
			current_start_col_ = (display_->cols() - width + 1) / 2;
		}
		else
		{ // its left
			current_start_col_ = 0;
		}
		display_->setString(text_, current_start_col_, 1, offset_);
		Timer::stop();
	}
}
