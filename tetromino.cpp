#include "tetromino.h"

Tetromino::Tetromino(tetromino::SHAPE shape, byte heigth, byte * field, tetromino::DIRECTION direction, Pos pos)
   :shape_(shape), heigth_(heigth), field_(field), direction_(direction),pos_(pos)
{

}

bool Tetromino::getPositions(Pos (&positions)[4])
{
	return getPositions(positions,shape_,direction_, pos_);
}

bool Tetromino::getPositions(Pos (&positions)[4], SHAPE shape, DIRECTION direction, Pos pos)
{
	tetromino::SPECIFICATIONS brick = tetrominos[shape];
	if(!brick.directions & BV_(direction))
		return false;
	Pos pos_rotate = brick.shape[0];
	for(int i = 0; i < 4; i++)
	{
		Pos rotated_brick_part_pos, brick_position;

		brick_position = brick.shape[i].pos() - pos_rotate;

		if(direction == tetromino::BOTTOM){
			rotated_brick_part_pos = brick_position;
		}
		else if(direction == tetromino::LEFT){
			rotated_brick_part_pos.pos_x = brick_position.pos_y;
			rotated_brick_part_pos.pos_y = -brick_position.pos_x;
		}
		else if(direction == tetromino::TOP){
			rotated_brick_part_pos.pos_x = -brick_position.pos_x;
			rotated_brick_part_pos.pos_y = -brick_position.pos_y;
		}
		else if(direction == tetromino::RIGHT){
			rotated_brick_part_pos.pos_x = -brick_position.pos_y;
			rotated_brick_part_pos.pos_y = brick_position.pos_x;
		}
		positions[i].pos_x = pos.pos_x + rotated_brick_part_pos.pos_x;
		positions[i].pos_y = pos.pos_y + rotated_brick_part_pos.pos_y;
	}
	return true;
}

DIRECTION Tetromino::rotate(DIRECTION direction, SHAPE shape)
{
	byte directions = getPossibleDirections(shape);
	if(directions == bit(direction)){
		return direction; // not rotateable
	}

	DIRECTION new_direction = direction;

	for(int i = 0; i < 4; i++){
		new_direction = DIRECTION((byte(new_direction)+1)%4);
		if(directions & bit(new_direction))
			return new_direction;
	}
	return direction;
}

void Tetromino::rotate()
{
	rotate(direction_,shape_);
}



byte Tetromino::getPossibleDirections() const{
	return getPossibleDirections(shape_);
}


byte Tetromino::getPossibleDirections(tetromino::SHAPE shape){
	SPECIFICATIONS brick = tetrominos[shape];
	return brick.directions;
}

byte Tetromino::possibleDirections(tetromino::SHAPE shape){
	byte num = 0;
	byte directions = getPossibleDirections(shape);
	for(int i = 0; i < 4; i++){
		if(bitRead(directions, i))
			num++;
	}
	return num;
}

byte Tetromino::possibleDirections() const
{
	possibleDirections(shape_);
}


byte Tetromino::isValid() const
{
	return isValid(shape_,direction_, pos_);
}

byte Tetromino::isValid(tetromino::SHAPE shape, tetromino::DIRECTION direction, Pos pos) const
{
	Pos positions[4];
	byte valid_errors = 0;
	getPositions(positions,shape,direction,pos);
	for(int i = 0; i < 4; i++){
		if(positions[i].pos_y > (heigth_ - 1))
		{
			valid_errors |= tetromino::OVER_ABOVE;
		}
		else if(positions[i].pos_y < 0)
		{
			valid_errors |= tetromino::OVER_BELOW; // is over below
		}
		if(positions[i].pos_x < 0)
		{
			valid_errors |= tetromino::OVER_LEFT; // is left over
		}

		else if(positions[i].pos_x > 7)
		{
			valid_errors |= tetromino::OVER_RIGHT; // is right over
		}

		if(bitRead(*(field_ + positions[i].pos_y),byte(positions[i].pos_x)))
		{
			valid_errors |= tetromino::COLLIDE; // collides with exiting tetromino part
		}
	}
	return valid_errors; // no collisions - valid
}
