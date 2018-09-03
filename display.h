#pragma once
#include "matrixdisplay.h"


class Display : public MatrixDisplay
{
public:
	 Display(byte height, byte width);
	 void setText(const char * text);
	 void update();
};
