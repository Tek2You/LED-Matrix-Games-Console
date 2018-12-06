#include "position.h"

Pos::Pos(char x, char y) : pos_x(x), pos_y(y) {}

Pos::Pos(const SmartPos &pos)
{
	pos_x = pos.x();
	pos_y = pos.y();
}

Pos Pos::operator+(const Pos &pos)
{

	return Pos(pos.pos_x + pos_x, pos.pos_y + pos_y);
}

Pos &Pos::operator+=(const Pos &pos)
{
	pos_x += pos.pos_x;
	pos_y += pos.pos_y;
}

Pos Pos::operator-(const Pos &pos)
{
	return Pos(pos_x - pos.pos_x, pos_y - pos.pos_y);
}

Pos &Pos::operator-=(const Pos &pos)
{
	pos_x -= pos.pos_x;
	pos_y -= pos.pos_y;
}

bool Pos::operator==(const Pos &pos)
{
	return (pos_x == pos.pos_x && pos_y == pos.pos_y);
}

Pos &Pos::operator=(const SmartPos &pos)
{
	pos_x = pos.x();
	pos_y = pos.y();
	return *this;
}

bool Pos::operator!=(const Pos &pos)
{
	return !operator==(pos);
}

Pos Pos::operator~()
{
	return Pos(pos_y, pos_x);
}

SmartPos::SmartPos(unsigned char x, unsigned char y)
{
	setXy(x, y);
}

Pos SmartPos::toPos()
{
	return Pos(x(), y());
}

unsigned char SmartPos::x() const
{
	return (compact_pos_ & 0xF0) >> 4;
}

unsigned char SmartPos::y() const
{
	return compact_pos_ & 0x0F;
}

unsigned char SmartPos::setXy(unsigned char x, unsigned char y)
{
	compact_pos_ = (y & 0x0F) | ((x & 0x0F) << 4);
}
