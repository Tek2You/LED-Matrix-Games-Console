﻿#include "snake.h"
#include "operators.h"
#include "avr/eeprom.h"

#define ON_ENTRY bit(7)
#define CHANGE   bit(6)
// these are the actual input pins (of PINC)
#define BTN_LEFT bit(0)
#define BTN_DOWN bit(1)
#define BTN_UP  bit(2)
#define BTN_RIGHT bit(3)

#define INPUT_MASK BTN_DOWN | BTN_LEFT | BTN_RIGHT | BTN_UP

#define TIMEOUT1 bit(4)
#define TIMEOUT2 bit(5)

static unsigned int EE_highscore EEMEM = 0;
unsigned int Snake::highscore_ = eeprom_read_word(&EE_highscore);

Snake::Snake(Display *display, unsigned long *t)
   :Game(display), timer_(t),
     body_end_(0), body_start_(0)
{
	body_len_ = 0;
	body_buffer_len_;
	direction_ = START;
	setSpeed(2);
}

Snake::~Snake()
{
	free(body_buffer_);
}

void Snake::reset()
{
	head_pos_ = Pos(3,7);
	display_->clear();
	body_len_ = 2;
	body_start_ = 1;
	body_buffer_[0] = Pos(1,7);
	body_buffer_[1] = Pos(2,7);
	eat_pos_ = Pos(5,7);
	direction_ = START;
	render();
}

void Snake::clear()
{
	display_->clear();
}

bool Snake::move()
{
	Pos vect;
	switch (direction_) {
	case UP:
		vect = Pos(0,1);
		break;
	case DOWN:
		vect = Pos(0,-1);
		break;
	case RIGHT :
		vect = Pos(1,0);
		break;
	case LEFT:
		vect = Pos(-1,0);
		break;
	default:
		return false;
		break;
	}

	vect += head_pos_;
	// check if highscore is broken. Directly save to avoid a not save in case of reset or poweroff.
	if(body_len_ - 2 > highscore_){
		highscore_ = body_len_-2;
		eeprom_write_word(&EE_highscore,highscore_);
		is_new_highscore_	= true;
	}
	if(!validate(vect)){ // game end
		return true;
	}
	if(++body_start_ >= body_buffer_len_){
		body_start_ = 0;
	}
	body_buffer_[body_start_] = head_pos_;
	head_pos_ = vect;

	eat();
	render();
	return false;
}

void Snake::setSpeed(byte v)
{
	switch (v) {
	case 0:
		period_ = 600;
		break;
	case 1:
		period_ = 500;
		return;
	case 3:
		period_ = 300;
		break;
	case 4:
		period_ = 200;
		break;
	case 2:
	default:
		period_ = 400;
		break;
	}
}

bool Snake::process(byte &event)
{
	bool button_set = true;
	if(event & CHANGE){
		if(event & INPUT_MASK){
			if(event & BTN_UP){
				// is not 180° rotation or no rotation(in this case the snake will make
				// a additinal ste,  what we avoid with dont allow this)
				if(direction_ != Snake::UP && direction_ != Snake::DOWN){
					direction_ = Snake::UP;
				}
			}
			else if(event & BTN_LEFT){
				if(direction_ != Snake::LEFT && direction_ != Snake::RIGHT){
					direction_ = Snake::LEFT;
				}
			}

			else if(event & BTN_RIGHT){
				if(direction_ != Snake::LEFT && direction_ != Snake::RIGHT){
					direction_ = Snake::RIGHT;
				}
			}

			else if(event & BTN_DOWN){
				if(direction_ != Snake::DOWN && direction_ != Snake::UP){
					direction_= Snake::DOWN;
				}
			}
			else{
				button_set = false;
			}
		}
	}
	if(event & TIMEOUT1 || button_set){
		*timer_ = millis() + period_;
		if(move()){ // game ends
			return true;
		}
	}
	return false;
}

void Snake::start()
{
	*timer_ = millis() + period_;
	direction_ = Snake::START;
}

unsigned int Snake::highscore()
{
	return highscore_ = eeprom_read_word(&EE_highscore);
}

void Snake::render()
{
	display_->clear();
	display_->setPixel(head_pos_.pos_x,head_pos_.pos_y,true);
	display_->setPixel(eat_pos_.pos_x,eat_pos_.pos_y,true);
	for(int i = 0; i < body_len_; i++){
		Pos * p = getBodyPos(i);
		display_->setPixel(p->pos_x,p->pos_y,true);
	}
}

bool Snake::eat()
{
	if(head_pos_ == eat_pos_){
		Pos p;
		do{
			p = Pos(random() % 8,random() % 16);
		}
		while(!isValid(p));
		eat_pos_= p;
		body_len_++;
		return true;

	}
	return false;
}


bool Snake::validate(Pos &pos)
{
	// out of range
	if(pos.pos_x < 0)
		pos.pos_x = 7;
	else if(pos.pos_x > 7)
		pos.pos_x = 0;
	if(pos.pos_y < 0)
		pos.pos_y = 15;
	else if(pos.pos_y > 15)
		pos.pos_y = 0;
	return isValid(pos);
}

bool Snake::isValid(Pos &pos)
{
	//  if colides, return true
	for(int i = 0; i < body_len_; i++){
		Pos * p = getBodyPos(i);
		if(pos == *p){
			return false;
		}
	}
	return true;
}


Pos *Snake::getBodyPos(int& pos)
{
	if(pos >= body_len_ || pos < 0){
		return nullptr;
	}
	if(body_start_ - pos < 0){
		return body_buffer_ + body_buffer_len_ - (pos - body_start_);
	}
	else{
		return body_buffer_ + body_start_ - pos;
	}
	//	return body_buffer_ +(pos % body_buffer_len_);
}

void Snake::resetHighscore()
{
	eeprom_write_word(&EE_highscore,highscore_ = 0);
}

