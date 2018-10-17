#pragma once
#include "matrixdisplay.h"
#include "text.h"



class Display : public MatrixDisplay
{
public:

	Display(byte height, byte width);
	void update();




	Text *text1_, *text2_;
private:
	void computeShiftMode();

};
