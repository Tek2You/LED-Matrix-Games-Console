/* dodge.h : Dodge game class
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
#include "game.h"

class Dodge : public Game
{
public:
    Dodge(Display *display);

    // Game interface
public:
    void start(Event *event) override;
    void setSpeed(byte v) override;
    unsigned int points() const{
        return points_;
    }
    bool isNewHighscore() const;

protected:
    bool onButtonChange(Event *event) override;
    bool onTimerOverflow(Event *event) override;
    void onStop(Event *event) override;
    void onContinue(Event *event) override;
    void render() override;
private:
    unsigned int points_;
};
