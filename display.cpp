#include "display.h"

Display::Display (byte height, byte width)
   : MatrixDisplay(height,width)
{

}

void Display::setText(const char *text)
{
	clear();
	const char * first = text;
	setString(first,0,-1,1,0);
}

void Display::update()
{

}
