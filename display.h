/* display.h : Library provides text and advanced display functions
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once
#include "matrixdisplay.h"
#include "text.h"

class Display : public MatrixDisplay
{
public:
	Display(byte height, byte width);

	void setIcon(uint64_t icon, byte offset = 0, bool show=true);
	void update();
	void loadMenuConfig();
	void loadsGameCofig();
	void setBrightness(byte brigthness) override;

	Text text1_, text2_;

private:
	union Icon {
		uint64_t i64;
		byte array[8];
	};
};
