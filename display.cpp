#include "display.h"

Display::Display (byte height, byte width)
   : MatrixDisplay(height,width)
   , start_col_(0), shift_mode_(NO_SHIFT), shift_count_(0), curser_pos_(-1)
{
	setShiftSpeed(5);
}

// shift text, depending on shift_mode_ and shift_speed_
void Display::update() {
	if (shift_mode_ & SHIFT) {

		if (++shift_count_ == shift_speed_) {
			shift_count_ = 0;
			shift();
		}
	}
}

// shift text by one column and start over if nothing is shown anymore
void Display::shift()
{
	--start_col_;
	if (setString(first_, start_col_, curser_pos_ + text_-first_ <= 0 , 1, offset_)) {
		first_ = text_;
		start_col_ = 4;
	} else { // should we advance to next start char?
		byte w = width(*first_);
		if (start_col_ + w < 0) {
			++first_;
			start_col_ += w+1; // char width + 1 column spacing
		}
	}
}

// display (and remember) text (for future shifting)
void Display::setText(const char* text)
{
	first_ = text_ = text;
	clear();
	setString(text_, start_col_ = 0, -1,4);

	//	clearRows();
	// need to recompute actual shift mode?
	computeShiftMode();
}

void Display::computeShiftMode()
{
	shift_mode_ = (width(text_) > cols() ? SHIFT : NO_SHIFT);

	if (shift_mode_ & SHIFT){ // when shifting, start in column 7
		bitSet(PORTB,1);
		start_col_ = 3;
	}
	else // otherwise, start in first column + setString once
		setString(text_, start_col_ = 0, curser_pos_);
}

void Display::setShiftSpeed(byte speed) {
	shift_speed_ = speed << 3;
}

void Display::setCursor(char pos)
{
	curser_pos_ = pos;
}

