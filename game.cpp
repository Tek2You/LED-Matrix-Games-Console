#include "game.h"

Game::Game(byte height,byte width):
   height_(height), width_(width), MatrixDisplay(height_)
{
	// allocate memory to the section for gamestate without tetromino
	field_ = static_cast<byte*>(malloc(height_));
	current_tetromino_ = new Tetromino(tetromino::I,tetromino::LEFT,4,16);

}

