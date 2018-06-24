#include "game.h"

Game::Game(Display *display, byte height, byte width):
   display_(display), height_(height), width_(width)
{
	// allocate memory to the section for gamestate without tetromino
	field_ = static_cast<byte*>(malloc(height_));
	current_tetromino_ = new Tetromino(tetromino::I, height_, field_, tetromino::LEFT,tetromino::POS{2,5});

}

void Game::process(){

}

void Game::render()
{
	display_->setArray(field_);
	if(current_tetromino_ != nullptr){
		tetromino::POS positions[4];
		current_tetromino_->getPositions(positions);
		for(int i = 0; i < 4; i++){
			display_->setPixel(positions[i].pos_x,positions[i].pos_y, true);
		}
	}
}

