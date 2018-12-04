#pragma once
#include "avr.h"
#include "position.h"

namespace tetromino
{
enum SHAPE
{
	I,
	J,
	L,
	O,
	S,
	T,
	Z
};
enum DIRECTION : byte
{
	TOP = 0,
	RIGHT = 1,
	BOTTOM = 2,
	LEFT = 3,
};

enum VALIDATION_ERROR : byte
{
	OVER_BELOW = (1 << 0),
	OVER_ABOVE = (1 << 1),
	OVER_RIGHT = (1 << 2),
	OVER_LEFT = (1 << 3),
	COLLIDE = (1 << 4),
};

struct SPECIFICATIONS
{
	byte directions;
	SmartPos shape[4];
};

#define BV_(bit) 1 << bit

const SPECIFICATIONS tetrominos[7] = {
    {BV_(TOP) | BV_(RIGHT), {SmartPos(0, 1), SmartPos(0, 0), SmartPos(0, 2), SmartPos(0, 3)}},
    {BV_(TOP) | BV_(RIGHT) | BV_(BOTTOM) | BV_(LEFT), {SmartPos(1, 1), SmartPos(1, 0), SmartPos(1, 2), SmartPos(0, 2)}},
    {BV_(TOP) | BV_(RIGHT) | BV_(BOTTOM) | BV_(LEFT), {SmartPos(0, 1), SmartPos(0, 0), SmartPos(0, 2), SmartPos(1, 2)}},
    {BV_(TOP), {SmartPos(0, 0), SmartPos(0, 1), SmartPos(1, 0), SmartPos(1, 1)}},
    {BV_(TOP) | BV_(RIGHT), {SmartPos(1, 0), SmartPos(1, 1), SmartPos(0, 1), SmartPos(2, 0)}},
    {BV_(TOP) | BV_(RIGHT) | BV_(BOTTOM) | BV_(LEFT), {SmartPos(1, 0), SmartPos(0, 0), SmartPos(1, 1), SmartPos(2, 0)}},
    {BV_(TOP) | BV_(RIGHT), {SmartPos(1, 0), SmartPos(0, 0), SmartPos(1, 1), SmartPos(2, 1)}},
};
} // namespace tetromino

using namespace tetromino;
class Tetromino
{
public:
	Tetromino(tetromino::SHAPE shape, byte heigth, byte *field, tetromino::DIRECTION direction, Pos pos);

	void setPos(const Pos &pos) { pos_ = pos; }
	Pos &pos() { return pos_; }

	void setDirection(tetromino::DIRECTION direction) { direction_ = direction; }
	tetromino::DIRECTION direction() { return direction_; }

	tetromino::SHAPE shape() const { return shape_; }

	bool getPositions(Pos (&positions)[4]);
	static bool getPositions(Pos (&positions)[4], tetromino::SHAPE shape, tetromino::DIRECTION direction, Pos pos);

	static DIRECTION rotate(DIRECTION direction, SHAPE shape);
	void rotate();

	byte isValid() const;
	byte isValid(tetromino::SHAPE shape, tetromino::DIRECTION direction, Pos pos) const;

	static byte getPossibleDirections(tetromino::SHAPE shape);
	byte getPossibleDirections() const;
	static byte possibleDirections(tetromino::SHAPE shape);
	byte possibleDirections() const;

private:
	const tetromino::SHAPE shape_;
	tetromino::DIRECTION direction_;
	Pos pos_;
	const byte heigth_;
	const byte *field_;
};
