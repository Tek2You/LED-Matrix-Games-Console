#include "game.h"

Game::Game(byte height,byte width):
   height_(height), width_(width), MatrixDisplay(height_)
{
	// allocate memory to the section for gamestate without tetromino
	field_ = static_cast<byte*>(malloc(height_));
	current_tetromino_ = new Tetromino(tetromino::I, field_, tetromino::LEFT,tetromino::POS{2,5});

}

void Game::render()
{
	for(byte i = 0; i < height_; i++)
	{
		rows_[i] = field_[i];
	}
	if(current_tetromino_ != nullptr){
		tetromino::POS positions[4];
		current_tetromino_->getPositions(positions);
		for(int i = 0; i < 4; i++){
			setPixel(positions[i].pos_x,positions[i].pos_y, true);
		}
	}
}

