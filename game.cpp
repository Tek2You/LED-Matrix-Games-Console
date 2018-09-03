#include "game.h"

Game::Game(Display *display):
   display_(display)
{
	// allocate memory to the section for gamestate without tetromino
	field_ = static_cast<byte*>(malloc(display_->rows()));
	current_tetromino_ = new Tetromino(tetromino::I, display_->rows(), field_, tetromino::LEFT,tetromino::POS{2,5});
}

Game::~Game()
{
	if(current_tetromino_ != nullptr){
		delete current_tetromino_;
	}
	free(field_);
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

bool Game::rotate()
{
	/*
	 * function scheme:
	 * 1. get current values
	 * 2. calculate next(possible) direction. Possible means not that the brick dosnt collides
	 * 3. Check if the new direction is valid
	 *    4. if not: try to change to a valid position
	 *
	*/

	tetromino::DIRECTION direction = current_tetromino_->getDirection();
	tetromino::DIRECTION new_direction;
	tetromino::POS position = current_tetromino_->getPos();
	tetromino::POS new_position = position;
	tetromino::SHAPE shape = current_tetromino_->getShape();

	byte possible_directions = current_tetromino_->possibleDirections();
	new_direction = Tetromino::rotate(direction,shape);

	// position is not valid
	byte valid_output = current_tetromino_->isValid(shape,new_direction,position);
	if(valid_output != 0)
	{
		if(valid_output & tetromino::OVER_LEFT) // left over
		{
			++new_position.pos_x;
			while(current_tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_LEFT){
				new_position.pos_x++;
			}
		}
		else if(valid_output & tetromino::OVER_RIGHT) // right over
		{
			new_position.pos_x --;
			while(current_tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_RIGHT){
				new_position.pos_x--;
			}
		}
		if(valid_output & tetromino::OVER_BELOW) // over below
		{
			new_position.pos_y ++;
			while(current_tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_BELOW){
				new_position.pos_y++;
			}
		}
		else if(valid_output & tetromino::OVER_ABOVE) // above over
		{
			new_position.pos_y --;
			while(current_tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_ABOVE){
				new_position.pos_y--;
			}
		}
		if(valid_output & tetromino::COLLIDE) // collides with exiting tetromino
		{

		}
	}

}

bool Game::step()
{

}

bool Game::newTetromino()
{
	if(current_tetromino_ != nullptr){
		delete current_tetromino_;
		current_tetromino_ = nullptr;
	}
	tetromino::SHAPE shape = randomTetrominoShape();
	current_tetromino_ = new Tetromino(shape, display_->rows(), field_, randomTetrominoDirection(shape),tetromino::POS{4,char(display_->rows())});
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

tetromino::SHAPE Game::randomTetrominoShape()
{
	return tetromino::SHAPE(rand());
}

tetromino::DIRECTION Game::randomTetrominoDirection(tetromino::SHAPE shape){
	byte directions = Tetromino::possibleDirections(shape);
	int num;
	for(int i = 0; i < 8; i++){
		if(bitRead(directions, i))
			num++;
	}
	int i_direction = rand() % num;
	for(int i = 0, j; i < 4; i++){
		if(bitRead(directions, i)){
			if(j == num){
				byte value; bitSet(value, i);
				return tetromino::DIRECTION(value);
			}
			++j;
		}
	}
}

