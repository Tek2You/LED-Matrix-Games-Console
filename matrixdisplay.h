#pragma once
#include "avr.h"

class MatrixDisplay
{
public:
	 MatrixDisplay(byte height, byte width);
	 void show();
	 void clear();
	 void setPixel(byte col, byte row, bool value);
	 void setRow(byte row, byte value);
	 void setArray(byte * array);
	 const byte rows() {return height_;}
	 const byte cols() {return width_;}
private:
	 byte * rows_;
    const byte height_;
	 const byte width_;
    byte latch_pin_;
	 byte clock_pin_;
};
