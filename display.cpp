#include "display.h"

Display::Display (byte height, byte width)
   : MatrixDisplay(height,width)
{
	text1_ = new Text(this);
	text1_ = new Text(this);
}

void Display::update()
{
	text1_->update();
	text2_->update();
}

// shift text, depending on shift_mode_ and shift_speed_


