#pragma once
#include "avr.h"

namespace tetromino {
enum SHAPE : byte {
	I,J,L,O,S,T,Z
};
enum DIRECTION : byte {
	TOP =  1 << 0,
	RIGHT = 1 << 1,
	BOTTOM = 1 << 2,
	LEFT = 1 << 3,
};

enum VALIDATION_ERROR : byte{
	OVER_BELOW = 1 << 0,
	OVER_ABOVE = 1 << 1,
	OVER_RIGHT = 1 << 2,
	OVER_LEFT = 1 << 3,
	COLLIDE = 1 << 4,
};

struct POS {
	POS(){}
	POS(char x, char y) : pos_x(x), pos_y(y){}
	char pos_x;
	char pos_y;
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

inline POS getPos(byte xy){
	POS pos;
	pos.pos_x = (xy & 0xF0) >> 4;
	pos.pos_y = xy & 0x0F;
	return pos;
}

struct SPECIFICATIONS{
	byte directions;
	byte shape[4];

};
const PROGMEM SPECIFICATIONS tetrominos[7] = {
   {TOP | RIGHT                ,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(0,3)}},
   {TOP | RIGHT | BOTTOM | LEFT,{setXy(1,1),setXy(1,0),setXy(1,2),setXy(0,2)}},
   {TOP | RIGHT | BOTTOM | LEFT,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(1,2)}},
   {TOP                        ,{setXy(0,0),setXy(0,1),setXy(1,0),setXy(1,1)}},
   {TOP | RIGHT                ,{setXy(1,0),setXy(1,1),setXy(0,1),setXy(2,0)}},
   {TOP | RIGHT| BOTTOM | LEFT ,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(1,1)}},
   {TOP | RIGHT                ,{setXy(1,0),setXy(0,0),setXy(1,1),setXy(2,1)}},
};
const SPECIFICATIONS tetrominos1[7] = {
   {TOP | RIGHT                ,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(0,3)}},
   {TOP | RIGHT | BOTTOM | LEFT,{setXy(1,1),setXy(1,0),setXy(1,2),setXy(0,2)}},
   {TOP | RIGHT | BOTTOM | LEFT,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(1,2)}},
   {TOP                        ,{setXy(0,0),setXy(0,1),setXy(1,0),setXy(1,1)}},
   {TOP | RIGHT                ,{setXy(1,0),setXy(1,1),setXy(0,1),setXy(2,0)}},
   {TOP | RIGHT| BOTTOM | LEFT ,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(1,1)}},
   {TOP | RIGHT                ,{setXy(1,0),setXy(0,0),setXy(1,1),setXy(2,1)}},
};
}


using namespace tetromino;
class Tetromino
{
public:

	Tetromino(tetromino::SHAPE shape, byte heigth, byte *field, tetromino::DIRECTION direction, tetromino::POS pos);

	void setPos(tetromino::POS pos) {pos_ = pos;}
	tetromino::POS getPos() {return pos_;}

	void setDirection(tetromino::DIRECTION direction);
	tetromino::DIRECTION getDirection() {return direction_;}

	tetromino::SHAPE getShape() {return shape_;}

	bool getPositions(tetromino::POS(&positions)[4]);
	bool getPositions(tetromino::POS(&positions)[4],tetromino::SHAPE shape, tetromino::DIRECTION direction, tetromino::POS pos);

	static DIRECTION rotate(DIRECTION direction, SHAPE shape);
	void rotate();

	byte isValid();
	byte isValid(tetromino::SHAPE shape, tetromino::DIRECTION direction, tetromino::POS pos);


	static byte possibleDirections(tetromino::SHAPE shape);
	byte possibleDirections();
private:
	const tetromino::SHAPE shape_;
	tetromino::DIRECTION direction_;
	tetromino::POS pos_;
	const byte heigth_;
	byte * field_;
};

