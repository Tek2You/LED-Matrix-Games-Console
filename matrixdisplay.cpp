#include "matrixdisplay.h"
#include "ATmega-master/include/spi.h"


const byte row_order[] = {4,2,1,3,5,0,6,7};
const byte col_order[] = {1,3,6,2,4,7,0,5};

MatrixDisplay::MatrixDisplay(byte height, byte width)
   :height_(height), width_(width)
{
	// allocate memory for (columnwise) display content
	rows_ = static_cast<byte*>(malloc(height_));
	latch_.setPins(PortPin::B,2);
	latch_.output();

	//	// declare selected pins of Port C as OUTPUT
	//	// set all eight pins of PortD as output
	DDRD = 0xFF;

	SPI_Init();
	clear();
}

MatrixDisplay::~MatrixDisplay(){
	free(rows_);
}

void MatrixDisplay::show(){
	for(int col = 0; col < 8; col++) {
		latch_.clear(); // clear latc
		for(byte *matrix_row = rows_ + col, *end = rows_ + (height_) + col; matrix_row != end; matrix_row+=8){
			SPI_SendByte(~*matrix_row);
		}
		PORTD = 0x00; // avoid glowing of prev/next row
		latch_.set(); // set latch
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
	if(x > width_ || y > height_)
		return;
	bitWrite(rows_[y],x,value);
}

void MatrixDisplay::setRow(byte row, int value, int column, byte column_height)
{
	for(int i = column; i < column + column_height; i++){
		bitWrite(rows_[row],i,bitRead(value,i - column));
1	}
}

void MatrixDisplay::setArray(byte *array)
{
	for(int i = 0; i < 16; i++)
	{
		rows_[i] = array[i];
	}
}
