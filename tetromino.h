#pragma once
#include "avr.h"
#include "matrixdisplay.h"

namespace tetromino {
enum TETROMINO : byte {
    I,J,L,O,S,T,Z
};
enum DIRECTION : byte {
    TOP =  1 << 0,
    BOTTOM = 1 << 1,
    RIGHT = 1 << 2,
    LEFT = 1 << 3,
};

inline byte getX(byte xy){
    return (xy & 0xF0) >> 4;
}

inline byte getY(byte xy){
    return xy & 0x0F;
}

inline byte setXy(byte x, byte y){
    return (y & 0x0F) | ((x & 0xF0) << 4);
}

struct SPECIFICATIONS{
    byte directions;
    byte shape[4];

};
const PROGMEM SPECIFICATIONS tetrominos[7] = {
   {TOP | RIGHT                ,{setXy(0,0),setXy(0,1),setXy(0,2),setXy(0,3)}},
   {TOP | RIGHT | BOTTOM | LEFT,{setXy(1,0),setXy(1,1),setXy(1,2),setXy(0,2)}},
   {TOP | RIGHT | BOTTOM | LEFT,{setXy(0,0),setXy(0,1),setXy(0,2),setXy(1,2)}},
   {TOP                        ,{setXy(0,0),setXy(0,1),setXy(1,0),setXy(1,1)}},
   {TOP | RIGHT                ,{setXy(0,1),setXy(1,1),setXy(1,0),setXy(2,0)}},
   {TOP | RIGHT| BOTTOM | LEFT ,{setXy(0,0),setXy(0,1),setXy(0,2),setXy(1,1)}},
   {TOP | RIGHT                ,{setXy(0,0),setXy(1,0),setXy(1,1),setXy(2,1)}},
};
}

class Tetromino
{
public:
    Tetromino(tetromino::TETROMINO shape, tetromino::DIRECTION direction, byte pos_x, byte pos_y);
    void setPos(byte x,byte y);
    void getPos(byte &x,	byte &y) {x = pos_x_; y = pos_y_;}

    void setDirection(tetromino::DIRECTION direction);
    tetromino::DIRECTION getDirection() {return direction_;}

    byte isValid();
    byte isValid(tetromino::TETROMINO shape,tetromino::DIRECTION,byte pos_x,byte pos_y);

private:
    const tetromino::TETROMINO shape_;
    tetromino::DIRECTION direction_;
    byte pos_x_;
    byte pos_y_;

};

