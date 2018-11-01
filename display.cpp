#include "display.h"
#include "operators.h"

Display::Display (byte height, byte width)
   : MatrixDisplay(height,width)
{
	text1_ = new Text(this);
	text2_ = new Text(this);
	loadMenuConfiguration();
}

Display::~Display()
{
	free(text1_);
	free(text2_);
}

void Display::setIcon(uint64_t icon, byte offset)
{
	Icon i;
	i.i64 = icon;
	for(int r = offset; r < 8 + offset; r++){
		setRow(r, i.array[7-r]);
	}
}


void Display::update()
{
	text1_->update();
	text2_->update();
}

void Display::loadMenuConfiguration()
{
	text2_->clear();
	text1_->setOffset(3);
	text1_->setOperationCols(0,7);
	text1_->setOperationRows(0,15);
	text1_->setShiftStartCol(3);
	text1_->clear();
	clear();
}

// shift text, depending on shift_mode_ and shift_speed_


