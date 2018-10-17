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
//	 void setString(char * string, int start_column = 0, byte row = 0);
	 const byte rows() {return 16;}
	 const byte cols() {return width_;}
	 int setString(const char *s, int column, char cursor_pos, char spacing = 1, byte offset = 0);
	 byte setChar(char ch, int column, byte offset);
	 void clearRows(byte start, byte end);
	 void setColumn(byte column, byte value, byte offset = 0);
	 void setRow(byte row, int value);
	 void disable();

	 // compute width (in columns) of a single char
	 static int width(char ch);
	 // compute width (in columns) of text
	 static int width(const char *s, char spacing=1);
	 // write integer value into string digits of max size
	 char *formatInt(char *digits, byte size, int value);

protected:
	 static const byte *letterStart(char ch);
	 const byte letterWidth(char ch);
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
