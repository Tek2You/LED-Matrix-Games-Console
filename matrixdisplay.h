#pragma once
#include "avr.h"

class MatrixDisplay
{
public:
	 MatrixDisplay(byte height);
	 void show();
	 void clear();
	 void setPixel(byte x, byte y, bool value);
	 byte * rows_;
private:
    const byte height_;
    byte latch_pin_;
	 byte clock_pin_;
};
