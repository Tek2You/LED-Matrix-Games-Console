#pragma once

class SmartPos;

struct Pos
{
	Pos()
	{
	}
	Pos(char x, char y);
	Pos(const SmartPos &pos);
	Pos operator+(const Pos &pos);
	Pos operator+(const Pos &pos) const;
	Pos &operator+=(const Pos &pos);
	Pos operator-(const Pos &pos);
	Pos &operator-=(const Pos &pos);
	bool operator==(const Pos &pos);
	Pos &operator=(const SmartPos &pos);
	bool operator!=(const Pos &pos);
	Pos operator~();
	char pos_x = 0;
	char pos_y = 0;
};

struct SmartPos
{
	SmartPos()
	{
	}
	SmartPos(unsigned char x, unsigned char y);
	Pos toPos();
	unsigned char x() const;
	unsigned char y() const;
	inline unsigned char setXy(unsigned char x, unsigned char y);
	unsigned char compact_pos_;
};
