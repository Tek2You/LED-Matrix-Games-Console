#include "text.h"

Text::Text(MatrixDisplay *display) : display_(display)
{

}

void Text::update() {
	if (shift_mode_ == SHIFT) {
		unsigned long now = millis();
		if (shift_next_time_ <= now) {
			shift();
			shift_next_time_ = now + speed_time_;
		}
	}
}

// shift text by one column and start over if nothing is shown anymore
void Text::shift()
{
	--start_col_;
	if (display_->setString(first_, start_col_, curser_pos_ + text_-first_ , 1, offset_) <= 0) {
		first_ = text_;
		start_col_ = 3;
	} else { // should we advance to next start char?
		byte w = display_->width(*first_);
		if (start_col_ + w < 0) {
			++first_;
			start_col_ += w+1; // char width + 1 column spacing
		}
	}
}

// display (and remember) text (for future shifting)
void Text::setText(const char* text)
{
	first_ = text_ = text;
	display_->clear();
	//	clearRows();
	// need to recompute actual shift mode?
	computeShiftMode();
}

void Text::computeShiftMode()
{

	shift_mode_ = (display_->width(text_) > 8 ? SHIFT : NO_SHIFT);

	if (shift_mode_ == SHIFT){ // when shifting, start in column 7
		start_col_ = 3;
		shift_next_time_ = millis() + speed_time_;
	}
	else // otherwise, start in first column + setString once
		display_->setString(text_, start_col_ = 0, curser_pos_,offset_);
}

void Text::setShiftSpeed(int speed) {
	speed_ = speed;
	speed_time_ = 1000/speed_;
}

void Text::setCursor(char pos)
{
	curser_pos_ = pos;
}

