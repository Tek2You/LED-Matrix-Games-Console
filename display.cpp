#include "display.h"
#include "operators.h"

Display::Display (byte height, byte width)
   : MatrixDisplay(height,width), text1_(this), text2_(this)
{
	loadMenuConfiguration();
}

Display::~Display()
{

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
	text1_.update();
	text2_.update();
}

void Display::loadMenuConfiguration()
{
	clear();
	text1_.clear();
	text2_.clear();
	text1_.setOffset(8);
	text1_.setOperationRows(9,16);
	text1_.setOperationCols(0,7);
	text1_.setShiftStartCol(3);

	text2_.setOffset(0);
	text2_.setOperationRows(0,8);
	text2_.setOperationCols(0,7);
	text2_.setShiftStartCol(3);
}

// shift text, depending on shift_mode_ and shift_speed_


