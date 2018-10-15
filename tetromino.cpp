#include "tetromino.h"

Tetromino::Tetromino(tetromino::SHAPE shape, byte heigth, byte * field, tetromino::DIRECTION direction, tetromino::POS pos)
   :shape_(shape), heigth_(heigth), field_(field), direction_(direction),pos_(pos)
{
	//	tetromino::SPECIFICATIONS brick = tetrominos1[I];
	//	if(brick.shape[0] == setXy(0,1)){
	//		bitSet(PORTB,1);
	//	}
}

bool Tetromino::getPositions(tetromino::POS (&positions)[4])
{
	return getPositions(positions,shape_,direction_, pos_);
}

bool Tetromino::getPositions(POS (&positions)[4], SHAPE shape, DIRECTION direction, POS pos)
{
	pos.pos_x = 4;
	pos.pos_y = 4;
	direction = DIRECTION::TOP;
	tetromino::SPECIFICATIONS brick = tetrominos1[shape];
	//	memcpy_P(&brick, &tetrominos + shape, sizeof(SPECIFICATIONS));
//	if(!(direction & brick.directions)){
//		return false; // failure: direction isnt available
//	}
	tetromino::POS pos_flip = tetromino::getPos(brick.shape[0]);
	for(int i = 0; i < 4; i++)
	{
		tetromino::POS rotated_brick_part_pos, brick_position;

		brick_position.pos_x = tetromino::getPos(brick.shape[i]).pos_x - pos_flip.pos_x;
		brick_position.pos_y = tetromino::getPos(brick.shape[i]).pos_y - pos_flip.pos_y;

		if(direction == tetromino::TOP){
			rotated_brick_part_pos = brick_position;
		}
		else if(direction == tetromino::RIGHT){
			rotated_brick_part_pos.pos_x = brick_position.pos_y;
			rotated_brick_part_pos.pos_y = -brick_position.pos_x;
		}
		else if(direction == tetromino::BOTTOM){
			rotated_brick_part_pos.pos_x = -brick_position.pos_x;
			rotated_brick_part_pos.pos_y = -brick_position.pos_y;
		}
		else if(direction == tetromino::LEFT){
			rotated_brick_part_pos.pos_x = -brick_position.pos_y;
			rotated_brick_part_pos.pos_y = brick_position.pos_x;
		}
//		positions[i].pos_x = pos.pos_x + rotated_brick_part_pos.pos_x;
//		positions[i].pos_y = pos.pos_y + rotated_brick_part_pos.pos_y;
		positions[i].pos_x = pos.pos_x + POS(i,i).pos_x;
		positions[i].pos_y = pos.pos_y + POS(i,i).pos_y;
		//		if(positions[1].pos_x == POS(4,4).pos_x && positions[1].pos_y == POS(4,4).pos_y){
		//			bitSet(PORTB,1);
		//		}
//		positions[i].pos_x = 2 + i;
//		positions[i].pos_y = 2 + i;
	}
//	for(int i = 0; i < 4; i++){
//	}
	return true;
}

DIRECTION Tetromino::rotate(DIRECTION direction, SHAPE shape)
{
	byte directions = possibleDirections(shape);
	if(directions == direction){
		return direction; // not rotateable
	}
	DIRECTION new_direction = direction;
	for(int i = 0; i < 4; i++){
		new_direction = DIRECTION(new_direction << 1);
		if(new_direction > (1 << 3)){
			new_direction = TOP;
		}
		if(new_direction & directions){
			direction = new_direction;
			return direction;
		}
	}
	return direction;
}

void Tetromino::rotate()
{
	rotate(direction_,shape_);
}



byte Tetromino::possibleDirections(){
	return possibleDirections(shape_);
}


byte Tetromino::possibleDirections(tetromino::SHAPE shape){
	SPECIFICATIONS brick;
	memcpy_P(&brick, &tetromino::tetrominos + shape, sizeof(tetromino::SPECIFICATIONS));
	return brick.directions;
}


byte Tetromino::isValid()
{
	return isValid(shape_,direction_, pos_);
}

byte Tetromino::isValid(tetromino::SHAPE shape, tetromino::DIRECTION direction, tetromino::POS pos)
{
	tetromino::POS positions[4];
	byte valid_errors;
	getPositions(positions,shape,direction,pos);
	for(int i = 0; i < 4; i++){
		if(positions[i].pos_y > heigth_)
		{
			valid_errors |= tetromino::OVER_ABOVE;
		}
		else if(positions[i].pos_y < 0)
		{
			valid_errors |= tetromino::OVER_BELOW; // is over below
		}
		else if(positions[i].pos_x < 0)
		{
			valid_errors |= tetromino::OVER_LEFT; // is left over
		}

		else if(positions[i].pos_x > 7)
		{
			valid_errors |= tetromino::OVER_RIGHT; // is right over
		}

		else if(bitRead(*(field_ + positions[i].pos_x),byte(positions[i].pos_y)))
		{
			valid_errors |= tetromino::COLLIDE; // collides with exiting tetromino part
		}
	}
	return valid_errors; // no collisions - valid
}
