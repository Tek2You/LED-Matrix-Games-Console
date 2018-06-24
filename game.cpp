#include "game.h"

Game::Game(Display *display):
   display_(display)
{
	// allocate memory to the section for gamestate without tetromino
	field_ = static_cast<byte*>(malloc(display_->rows()));
	current_tetromino_ = new Tetromino(tetromino::I, display_->rows(), field_, tetromino::LEFT,tetromino::POS{2,5});
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

void Game::rotate()
{
	current_tetromino_->rotate();
}

bool Game::step()
{

}

bool Game::newTetromino(tetromino::TETROMINO shape, tetromino::DIRECTION direction)
{
	if(current_tetromino_ != nullptr){
		delete current_tetromino_;
		current_tetromino_ = nullptr;
	}
	current_tetromino_ = new Tetromino(shape, display_->rows(), field_, direction,tetromino::POS{4,char(display_->rows())});
	tetromino::POS points[4];
	current_tetromino_->getPositions(points);

	for(int i = 0; i < 4; i++){
		if(points[i].pos_y > display_->rows()){
			tetromino::POS pos = current_tetromino_->getPos();
			pos.pos_y -= (points[i].pos_y - display_->rows());
			current_tetromino_->setPos(pos);
		}
	}
	render();
	if(current_tetromino_->isValid() != 0){ // not valid
		return true;
	}
	return false;
}

