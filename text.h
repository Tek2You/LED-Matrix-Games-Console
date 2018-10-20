#pragma once
#include "avr.h"
#include "matrixdisplay.h"

class Text {
public:
    Text(MatrixDisplay *display);


    enum ShiftMode { NO_SHIFT = 0,
                     SHIFT = 1,
                     OFF = 2
                   };

    enum NumberSize { P5,
                      P7
                    };

    void setText(const char * text);
    void setNumber(int number);

    void shift();

    void update();

    void clear();

    void setShiftSpeed(int speed);
    int shiftSpeed() const { return speed_; }

    void setOffset(byte offset){offset_ = offset;}
    byte offset(){return offset_;}

    void setCursor(char pos);
    char getCursor() const { return curser_pos_; }

    void setOperationRows(byte start, byte end);
    void setOperationCols(byte start, byte end);

    void setShiftStartCol(byte col){shift_start_col_ = col; setText(text_);}

private:
    void computeShiftMode();
    MatrixDisplay * display_;

    const char* text_;  // displayed text
    const char* first_; // first visible character
    int current_shift_start_col_;
    byte shift_start_col_;
    ShiftMode shift_mode_;
    unsigned long shift_next_time_;
    int speed_; // columns per second
    int speed_time_;
    byte offset_;
    char curser_pos_;
    byte start_col_, end_col_, start_row_, end_row_;
	 char number_buffer_[10];
};
