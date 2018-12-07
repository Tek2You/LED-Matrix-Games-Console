#include "text.h"

Text::Text(MatrixDisplay *display)
    : display_(display), curser_pos_(-1), shift_mode_(OFF), offset_(0),
      start_col_(0), end_col_(7), start_row_(0), end_row_(15),
      shift_start_col_(3)
{
	setShiftSpeed(5);
}

void Text::update()
{
	if (shift_mode_ == SHIFT)
	{
		unsigned long now = millis();
		if (shift_next_time_ <= now)
		{
			shift();
			shift_next_time_ = now + speed_time_;
		}
	}
}

void Text::clear()
{
	display_->clearRows(start_row_, end_row_);
	first_ = text_ = nullptr;
	shift_mode_ = OFF;
}

// shift text by one column and start over if nothing is shown anymore
void Text::shift()
{
	//	if(current_shift_start_col_ == 1)
	//		return;
	--current_shift_start_col_;
	if (display_->setString(first_, current_shift_start_col_,
	                        curser_pos_ + text_ - first_, 1, offset_) <= 0)
	{
		first_ = text_;
		current_shift_start_col_ = shift_start_col_;
	}
	else
	{ // should we advance to next start char?
		byte w = display_->width(*first_);
		if (current_shift_start_col_ + w < 0)
		{
			++first_;
			current_shift_start_col_ += w + 1; // char width + 1 column spacing
		}
	}
}

// display (and remember) text (for future shifting)
void Text::setText(const char *text)
{
	first_ = text_ = text;
	display_->clearRows(start_row_, end_row_);
	computeShiftMode();
}

void Text::setNumber(const int &value)
{
	setText(display_->formatInt(number_buffer_, 10, value));
}

void Text::computeShiftMode()
{

	shift_mode_ =
	    (display_->width(text_) > end_col_ - start_col_ ? SHIFT : NO_SHIFT);

	if (shift_mode_ == SHIFT)
	{ // when shifting, start in column 7
		current_shift_start_col_ = shift_start_col_;
		shift_next_time_ = millis() + speed_time_;
	}
	else // otherwise, start in first column + setString once
		display_->setString(text_, current_shift_start_col_ = 0, curser_pos_, 1,
		                    offset_);
}

void Text::setShiftSpeed(const int speed)
{
	speed_ = speed;
	speed_time_ = 1000 / speed_; // from shifts per second to mseconds per shift
}

void Text::setCursor(char pos) { curser_pos_ = pos; }

void Text::setOperationRows(const byte start, const byte end)
{
	start_row_ = start;
	end_row_ = end;
}

void Text::setOperationCols(const byte start, const byte end)
{
	start_col_ = start;
	end_col_ = end;
	setText(text_);
}
