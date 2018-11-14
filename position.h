#pragma once

class SmartPos;

struct Pos {
	Pos(){}
	Pos(char x, char y);
	Pos(const SmartPos& pos);
	Pos operator +(const Pos& pos);
	Pos& operator+=(const Pos& pos);
	Pos operator-(const Pos& pos);
	Pos& operator-=(const Pos& pos);
	bool operator==(const Pos& pos);
	Pos& operator=(const SmartPos& pos);
	bool operator!=(const Pos& pos);
	char pos_x = 0;
	char pos_y = 0;
};



class SmartPos
{
public:
	SmartPos() { }
	SmartPos(unsigned char x, unsigned char y);
	Pos pos();
	unsigned char x() const;
	unsigned char y() const;
private:
	inline unsigned char setXy(unsigned char x, unsigned char y);
	unsigned char compact_pos_;
};
