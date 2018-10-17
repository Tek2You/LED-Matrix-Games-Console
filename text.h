#pragma once
#include "avr.h"
#include "matrixdisplay.h"

class Text {
public:
    Text(MatrixDisplay *display);


    enum ShiftMode { NO_SHIFT = 0,
                      SHIFT = 1
                   };

    enum TexMode { TOP,
                    BOTTOM,
                    MIDDLE,
                    DISABLED,
                 };

    enum NumberSize { P5,
                       P7
                    };

    void setText(const char * text);
    void setNumber(int number);

    void shift();

    void update();

    void setShiftSpeed(int speed);
    int shiftSpeed() const { return speed_; }

    void setOffset(byte offset){offset_ = offset;}
    byte offset(){return offset_;}

    void setCursor(char pos);
    char getCursor() const { return curser_pos_; }

    void setMode();
    byte mode(){return mode_;}

private:
    void computeShiftMode();
    MatrixDisplay * display_;

    const char* text_;  // displayed text
    const char* first_; // first visible character
    int  start_col_;
    ShiftMode shift_mode_;
    unsigned long shift_next_time_;
    int speed_; // columns per second
    int speed_time_;
    byte offset_;
    char curser_pos_;
    byte mode_;
};
