#pragma once
#include "matrixdisplay.h"
#include "text.h"



class Display : public MatrixDisplay
{
public:

	Display(byte height, byte width);
	~Display();

	void update();
	void loadMenuConfiguration();

	Text *text1_, *text2_;
private:

};
