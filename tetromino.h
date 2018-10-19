#pragma once
#include "avr.h"
#include "position.h"

namespace tetromino {
enum SHAPE {
	I,J,L,O,S,T,Z
};
enum DIRECTION : byte {
	TOP =  0,
	RIGHT = 1,
	BOTTOM = 2,
	LEFT = 3,
};

enum VALIDATION_ERROR : byte{
	OVER_BELOW = (1 << 0),
	OVER_ABOVE = (1 << 1),
	OVER_RIGHT = (1 << 2),
	OVER_LEFT = (1 << 3),
	COLLIDE = (1 << 4),
};

inline byte getX(byte xy){
	return (xy & 0xF0) >> 4;
}

inline byte getY(byte xy){
	return xy & 0x0F;
}

inline byte setXy(byte x, byte y){
	return (y & 0x0F) | ((x & 0x0F) << 4);
}

inline Pos getPos(byte xy){
	Pos pos;
	pos.pos_x = byte((xy & 0xF0) >> 4);
	pos.pos_y = byte(xy & 0x0F);
	return pos;
}

struct SPECIFICATIONS{
	byte directions;
	byte shape[4];

};
#define BV_(bit) 1 << bit

//const PROGMEM SPECIFICATIONS tetrominos[7] = {
//   {TOP | RIGHT                ,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(0,3)}},
//   {TOP | RIGHT | BOTTOM | LEFT,{setXy(1,1),setXy(1,0),setXy(1,2),setXy(0,2)}},
//   {TOP | RIGHT | BOTTOM | LEFT,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(1,2)}},
//   {TOP                        ,{setXy(0,0),setXy(0,1),setXy(1,0),setXy(1,1)}},
//   {TOP | RIGHT                ,{setXy(1,0),setXy(1,1),setXy(0,1),setXy(2,0)}},
//   {TOP | RIGHT| BOTTOM | LEFT ,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(1,1)}},
//   {TOP | RIGHT                ,{setXy(1,0),setXy(0,0),setXy(1,1),setXy(2,1)}},
//};
const SPECIFICATIONS tetrominos[7] = {
   {BV_(TOP) | BV_(RIGHT)										,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(0,3)}},
   {BV_(TOP) | BV_(RIGHT) | BV_(BOTTOM) | BV_(LEFT)	,{setXy(1,1),setXy(1,0),setXy(1,2),setXy(0,2)}},
   {BV_(TOP) | BV_(RIGHT) | BV_(BOTTOM) | BV_(LEFT)	,{setXy(0,1),setXy(0,0),setXy(0,2),setXy(1,2)}},
   {BV_(TOP)														,{setXy(0,0),setXy(0,1),setXy(1,0),setXy(1,1)}},
   {BV_(TOP) | BV_(RIGHT)										,{setXy(1,0),setXy(1,1),setXy(0,1),setXy(2,0)}},
   {BV_(TOP) | BV_(RIGHT) | BV_(BOTTOM) | BV_(LEFT)	,{setXy(1,0),setXy(0,0),setXy(1,1),setXy(2,0)}},
   {BV_(TOP) | BV_(RIGHT)										,{setXy(1,0),setXy(0,0),setXy(1,1),setXy(2,1)}},
};
}



using namespace tetromino;
class Tetromino
{
public:

	Tetromino(tetromino::SHAPE shape, byte heigth, byte *field, tetromino::DIRECTION direction, Pos pos);

	void setPos(Pos pos) {pos_ = pos;}
	Pos getPos() {return pos_;}

	void setDirection(tetromino::DIRECTION direction){direction_ = direction;}
	tetromino::DIRECTION getDirection() {return direction_;}

	tetromino::SHAPE getShape() {return shape_;}

	bool getPositions(Pos(&positions)[4]);
	static bool getPositions(Pos(&positions)[4],tetromino::SHAPE shape, tetromino::DIRECTION direction, Pos pos);

	static DIRECTION rotate(DIRECTION direction, SHAPE shape);
	void rotate();

	byte isValid();
	byte isValid(tetromino::SHAPE shape, tetromino::DIRECTION direction, Pos pos);


	static byte getPossibleDirections(tetromino::SHAPE shape);
	byte getPossibleDirections();
	static byte possibleDirections(tetromino::SHAPE shape);
	byte possibleDirections();
private:
	const tetromino::SHAPE shape_;
	tetromino::DIRECTION direction_;
	Pos pos_;
	const byte heigth_;
	byte * field_;
};

