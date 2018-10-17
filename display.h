#pragma once
#include "matrixdisplay.h"


class Display : public MatrixDisplay
{
public:
	Display(byte height, byte width);
	void setText(const char * text);
	void update();

	enum ShiftMode { NO_SHIFT = 0,
		              SHIFT = 1
	               };

//	enum TexMode { TOP,
//		            BOTTOM,
//		            MIDDLE,
//		            DISABLÊD,
//	             };

//	enum NumberSize { P5,
//		               P7
//	                };

	void shift();

	void setShiftSpeed(byte speed);
	byte shiftSpeed() const { return shift_speed_ >> 3; }

	void setOffset(byte offset){offset_ = offset;}
	byte offset(){return offset_;}

	void setCursor(char pos);
	char getCursor() const { return curser_pos_; }

	void setMode();
	byte mode(){return mode_;}

private:
	void computeShiftMode();

	const char* text_;  // displayed text
	const char* first_; // first visible character
	int  start_col_;
	ShiftMode shift_mode_;
	byte shift_count_;
	byte shift_speed_;
	byte offset_;
	char curser_pos_;
	byte mode_;
};
