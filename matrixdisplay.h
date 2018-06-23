#pragma once
#include "avr.h"

class MatrixDisplay
{
public:
    MatrixDisplay(byte height);
    void show();
    void clear();
private:
    const byte height_;
    byte * rows_;
    byte latch_pin_;
    byte clock_pin_;
};
