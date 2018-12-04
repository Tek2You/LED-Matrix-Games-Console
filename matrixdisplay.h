#pragma once
#include "avr.h"

class MatrixDisplay
{
public:
	MatrixDisplay(byte width, byte height);
	~MatrixDisplay();
	void show();
	void clear();
	void setPixel(byte col, byte row, bool value);
	void setArray(byte *array);
	const byte rows() { return 16; }
	const byte cols() { return width_; }
	int setString(const char *s, int column, char cursor_pos, char spacing = 1,
	              byte offset = 0);
	byte setChar(char ch, int column, byte offset);
	void clearRows(byte start, byte end);
	void setColumn(byte column, byte value, byte offset = 0);
	void clearColumns(byte start, byte end, byte offset);
	void setRow(byte row, int value);
	void disable();

	// compute width (in columns) of a single char
	static int width(char ch);
	// compute width (in columns) of text
	static int width(const char *s, char spacing = 1);
	// write integer value into string digits of max size
	char *formatInt(char *digits, byte size, int value);

protected:
	static const byte *letterStart(char ch);
	static const byte letterWidth(char ch);
	byte *columnPtr(byte column) const;
	byte orderCols(byte value);
	byte mapCol(byte row);

private:
	byte *rows_;
	const byte height_;
	const byte width_;
};
