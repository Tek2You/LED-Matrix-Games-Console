#include "matrixdisplay.h"
#include "ATmega-master/include/spi.h"

const byte row_order[] = {4,2,1,3,5,0,6,7};
const byte col_order[] = {1,3,6,2,4,7,0,5};

MatrixDisplay::MatrixDisplay(byte height)
    :height_(height)
{
	 // allocate memory for (columnwise) display content
	 rows_ = static_cast<byte*>(malloc(height_));

    //	// declare selected pins of Port C as OUTPUT
    DDRC |= _BV(latch_pin_);
    //	// set all eight pins of PortD as output
    DDRD = 0xFF;

    SPI_Init();
    clear();
}

void MatrixDisplay::show(){
	 for(int col = 0; col < 8; col++) {
		  bitClear(PORTC, latch_pin_); // clear latc
		  for(byte *matrix_row = rows_ + col, *end = rows_ + (height_) + col; matrix_row != end; matrix_row+=8){
			   SPI_SendByte(~*matrix_row);
        }
		  bitSet(PORTC, clock_pin_);
        PORTD = 0x00; // avoid glowing of prev/next row
		  bitSet(PORTC, latch_pin_); // set latch
        // select row to be displayed
		  PORTD = _BV(col_order[col]);
    }
    for(byte i = 0; i < 4; i++){
        SPI_SendByte(0);
    }
    PORTD = 0;
}

void MatrixDisplay::clear(){

}

void MatrixDisplay::setPixel(byte x, byte y, bool value)
{
	bitWrite(rows_[y],x,value);
}
