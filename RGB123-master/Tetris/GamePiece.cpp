//Copyright (C) 2013 Valentin Ivanov http://www.breakcontinue.com.
//
//This work is licensed under a Creative Commons 
//Attribution-NonCommercial-ShareAlike 3.0 Unported License.
//See http://creativecommons.org/licenses/by-nc-sa/3.0/
//
//All other rights reserved.
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
//EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
//WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

#include "GamePiece.h"

GamePiece::GamePiece(uint8_t rows, uint8_t columns, uint8_t * inputArray)
{
        _freeData = true;
	this->Rows = rows; 
	this->Columns = columns;
	if( !inputArray )
        {
		this->_data = (uint8_t*)malloc(rows * columns);
        }
	else
        {
		this->_data = inputArray;
                _freeData = false;
        }
}

GamePiece::GamePiece(const GamePiece& copyFrom)
{
        _freeData = true;
	this->Rows = copyFrom.Rows;
	this->Columns = copyFrom.Columns;

	this->_data = (uint8_t*)malloc(Rows * Columns);
	memcpy(this->_data, copyFrom._data, Rows*Columns);
}

GamePiece::~GamePiece()
{
  if( _freeData )
    free(this->_data);
}

uint8_t& GamePiece::operator()(const int rowIndex, const int columnIndex)
{
	return _data[rowIndex*Columns+columnIndex];
}

GamePiece * GamePiece::rotateLeft()
{
	GamePiece * pRet = new GamePiece(Columns, Rows);
	for (int row = 0; row < Rows; row++)
		for (int col = 0; col < Columns; col++)
			(*pRet)(pRet->Rows - col - 1, row) = (*this)(row, col);  

	return pRet;
}

GamePiece * GamePiece::rotateRight()
{
	GamePiece * pRet = new GamePiece(Columns, Rows);
	for (int row = 0; row < Rows; row++)
		for (int col = 0; col < Columns; col++)
			(*pRet)(col, pRet->Columns - row - 1) = (*this)(row, col);  

	return pRet;  
}
