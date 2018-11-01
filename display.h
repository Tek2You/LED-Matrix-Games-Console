#pragma once
#include "matrixdisplay.h"
#include "text.h"



class Display : public MatrixDisplay
{
public:

	Display(byte height, byte width);
	~Display();

	void setIcon(uint64_t icon, byte offset=0);
	void update();
	void loadMenuConfiguration();

	Text *text1_, *text2_;
private:
	union Icon{
		uint64_t i64;
		byte array[8];
	};

};
