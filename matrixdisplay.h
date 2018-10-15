#pragma once
#include "avr.h"
#include "Vportpin.h"

class MatrixDisplay
{
public:
	 MatrixDisplay(byte width, byte height);
	 ~MatrixDisplay();
	 void show();
	 void clear();
	 void setPixel(byte col, byte row, bool value);
//	 void setRow(byte row, int value, int column = 0, byte column_height = 8);
	 void setArray(byte * array);
	 void setString(char * string, int start_column = 0, byte row = 0);
	 const byte rows() {return 16;}
	 const byte cols() {return width_;}
	 int setString(const char *s, int column, char cursor_pos, char spacing, byte offset);
	 byte setChar(char ch, int column, byte offset);
	 void clearColumns(int start, int end);
	 void setColumn(byte column, byte value, byte offset = 0, bool flipped = 0);

	 void setRow(byte row, int value);

	 void disable();

protected:
	 const byte *letterStart(char ch);
	 byte letterWidth(char ch);
	 byte *columnPtr(byte column) const;
	 byte orderCols(byte value);
	 byte mapCol(byte row);

private:
	 inline void * operator new(size_t size)
	 {
		return malloc(size);
	 }

	 inline void operator delete(void * ptr)
	 {
		free(ptr);
	 }

	 byte * rows_;
	 const byte height_;
	 const byte width_;

	 PortPin latch_;
	 PortPin clock_;
};
