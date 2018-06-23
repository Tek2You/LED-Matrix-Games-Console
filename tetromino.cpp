#include "tetromino.h"

Tetromino::Tetromino(tetromino::TETROMINO shape, tetromino::DIRECTION direction,byte pos_x, byte pos_y)
    :shape_(shape), direction_(direction),pos_x_(pos_x),pos_y_(pos_y)
{

}

byte Tetromino::isValid()
{

}
