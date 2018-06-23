#pragma once
#include "avr.h"
#include "matrixdisplay.h"
#include "tetromino.h"

class Game : MatrixDisplay
{
public:
	 Game(byte height, byte width);
	 void render();

private:
	 const byte height_;
	 const byte width_;
	 byte * field_; // field without current tetromino
	 Tetromino * current_tetromino_;
};
