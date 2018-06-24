#pragma once
#include "avr.h"

class MatrixDisplay
{
public:
	 MatrixDisplay(byte height);
	 void show();
	 void clear();
	 void setPixel(byte col, byte row, bool value);
	 void setRow(byte row, byte value);
	 void setArray(byte * array);
private:
	 byte * rows_;
    const byte height_;
    byte latch_pin_;
	 byte clock_pin_;
};
