﻿#include "tetris.h"
#include "string.h"

Tetris::Tetris(Display *display)
   : Game(display)
{
	// allocate memory to the section for gamestate without tetromino
	field_ = static_cast<byte*>(malloc(display_->rows()));
	tetromino_ = nullptr;
	//	tetromino_ = new Tetromino(tetromino::I, display_->rows(), field_, tetromino::LEFT,tetromino::Pos{2,5});
}

Tetris::~Tetris()
{
	if(tetromino_ != nullptr){
		delete tetromino_;
	}
	free(field_);
}

void Tetris::render()
{
	display_->clear();
	display_->setArray(field_);
	if(tetromino_ != nullptr){
		Pos positions[4];
		tetromino_->getPositions(positions);
		for(Pos p : positions){
			display_->setPixel(p.pos_x,p.pos_y, true);
		}
	}
}

bool Tetris::up()
{
	/*
	 * function scheme:
	 * 1. get current values
	 * 2. calculate next(possible) direction. Possible means not that the brick dosnt collides
	 * 3. Check if the new direction is valid
	 *    4. if not: try to change to a valid position
	 *
	*/
	if(tetromino_ == nullptr)
		return false;
	tetromino::DIRECTION direction = tetromino_->getDirection();
	tetromino::DIRECTION new_direction;
	Pos position = tetromino_->getPos();
	Pos new_position = position;
	tetromino::SHAPE shape = tetromino_->getShape();

	byte possible_directions = tetromino_->getPossibleDirections();
	new_direction = Tetromino::rotate(direction,shape);

	// position is not valid
	byte valid_output = tetromino_->isValid(shape,new_direction,position);
	if(valid_output)
	{
		if(valid_output & tetromino::OVER_LEFT) // left over
		{
			++new_position.pos_x;
			while(valid_output = tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_LEFT){
				new_position.pos_x++;
			}
		}
		else if(valid_output & tetromino::OVER_RIGHT) // right over
		{
			new_position.pos_x --;
			while(valid_output = tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_RIGHT){
				new_position.pos_x--;
			}
		}
		if(valid_output & tetromino::OVER_BELOW) // over below
		{
			new_position.pos_y ++;
			while(valid_output = tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_BELOW){
				new_position.pos_y++;
			}
		}
		else if(valid_output & tetromino::OVER_ABOVE) // above over
		{
			new_position.pos_y --;
			while(valid_output = tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_ABOVE){
				new_position.pos_y--;
			}
		}
		if(valid_output & tetromino::COLLIDE) // collides with exiting tetromino
		{
			return false;
		}
	}
	tetromino_->setPos(new_position);
	tetromino_->setDirection(new_direction);
	render();
	return true;
}

bool Tetris::right()

{
	if(tetromino_ == nullptr)
		return false;
	Pos pos = tetromino_->getPos();
	pos.pos_x++;
	if(tetromino_->isValid(tetromino_->getShape(),tetromino_->getDirection(),pos)){  // not valid
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return true;
}

bool Tetris::left()
{
	if(tetromino_ == nullptr)
		return false;
	Pos pos = tetromino_->getPos();
	pos.pos_x--;
	if(tetromino_->isValid(tetromino_->getShape(),tetromino_->getDirection(),pos)){  // not valid
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return true;
}

bool Tetris::down()
{
	if(tetromino_ == nullptr)
		return false;
	Pos pos = tetromino_->getPos();
	pos.pos_y--;
	byte valid_output = tetromino_->isValid(tetromino_->getShape(),tetromino_->getDirection(),pos);
	if(valid_output){  // not valid
		takeOverTetromino();
		checkRowsFinished();
		if(newTetromino())
			return true;
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return false;
}

void Tetris::reset()
{
	if(tetromino_ != nullptr){
		delete(tetromino_);
		tetromino_ = nullptr;
	}
	this->clear();
	display_->clear();
}

void Tetris::clear()
{
	for(byte * ptr = field_; ptr < &field_[display_->rows()];ptr++){
		*ptr = 0;
	}
}

bool Tetris::process()
{

}

void Tetris::start()
{
	newTetromino();
}

bool Tetris::newTetromino()
{
	if(tetromino_ != nullptr){
		delete tetromino_;
		tetromino_ = nullptr;
	}

	tetromino::SHAPE shape = randomTetrominoShape();
	tetromino_ = new Tetromino(shape, display_->rows(), field_,	randomTetrominoDirection(shape),Pos(4,display_->rows()-1));

	Pos points[4];
	tetromino_->getPositions(points);
	for(Pos p : points){
		if(p.pos_y > display_->rows() - 1){
			Pos pos = tetromino_->getPos();
			pos.pos_y -= (p.pos_y - (display_->rows()-1));
			tetromino_->setPos(pos);
			tetromino_->getPositions(points);
		}
	}
	render();
	if(tetromino_->isValid() & COLLIDE){ // not valid
		return true;

	}
	return false;
}

void Tetris::takeOverTetromino()
{
	Pos positions[4];
	tetromino_->getPositions(positions);
	for(Pos p : positions){
		bitWrite(field_[p.pos_y],p.pos_x,true);
	}
	render();
}

void Tetris::checkRowsFinished()
{
	for(int i = 0; i < display_->rows(); i++){
		while(field_[i] == 0xFF){ // row is full
			field_[i] = 0;
			points_++;
			for(int j = i; j < display_->rows() - 1;j++){
				field_[j] = field_[j+1];
			}
		}
	}
	render();
}

tetromino::SHAPE Tetris::randomTetrominoShape()
{
	return tetromino::SHAPE(micros() % 7);
}

tetromino::DIRECTION Tetris::randomTetrominoDirection(tetromino::SHAPE shape){
	byte directions = Tetromino::getPossibleDirections(shape);
	byte num = Tetromino::possibleDirections(shape);

	if(num <= 1){
		return TOP;
	}

	byte i_direction = ((millis()) % (num));
	for(int i = 0, j = 0; i < 4; i++){
		if(bitRead(directions,i)){
			if(j == i_direction){
				tetromino::DIRECTION dir = tetromino::DIRECTION(j);
				return dir;
			}
			j++;
		}
	}
}
