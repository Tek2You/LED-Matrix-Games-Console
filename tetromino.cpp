#include "tetromino.h"

Tetromino::Tetromino(tetromino::TETROMINO shape, tetromino::DIRECTION direction,byte pos_x, byte pos_y)
    :shape_(shape), direction_(direction),pos_x_(pos_x),pos_y_(pos_y)
{

}

byte Tetromino::isValid()
{
	 return isValid(shape_,direction_,pos_x_,pos_y_);
}

byte Tetromino::isValid(tetromino::TETROMINO shape, tetromino::DIRECTION, byte pos_x, byte pos_y)
{
	tetromino::SPECIFICATIONS foo;
	memcpy_P(&foo, &tetromino::tetrominos + shape, sizeof(tetromino::SPECIFICATIONS));


}
