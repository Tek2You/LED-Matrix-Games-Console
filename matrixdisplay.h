#pragma once
#include "avr.h"
#include "Vportpin.h"

class MatrixDisplay
{
public:
	 MatrixDisplay(byte height, byte width);
	 ~MatrixDisplay();
	 void show();
	 void clear();
	 void setPixel(byte col, byte row, bool value);
	 void setRow(byte row, int value, int column = 0, byte column_height = 8);
	 void setArray(byte * array);
	 void setString(char * string, int start_column = 0, byte row = 0);
	 const byte rows() {return height_;}
	 const byte cols() {return width_;}
private:
	 byte * rows_;
    const byte height_;
	 const byte width_;
	 PortPin latch_;
	 PortPin clock_;
};
