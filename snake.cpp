#include "snake.h"
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
}

Snake::~Snake()
{
	free(body_buffer_);
}

bool Snake::up()
{
	if(direction_ != DOWN && direction_ != UP){
		direction_ = UP;
		byte dummy;
		return process(dummy);
	}
	return false;
}

bool Snake::right()
{
	if(direction_ != LEFT && direction_ != RIGHT){
		direction_ = RIGHT;
		byte dummy;
		return process(dummy);
	}
	return false;
}

bool Snake::left()
{
	if(direction_ != RIGHT && direction_ != LEFT){
		direction_ = LEFT;
		byte dummy;
		return process(dummy);
	}
	return false;
}

bool Snake::down()
{
	if(direction_ != UP && direction_ != DOWN){
		direction_ = DOWN;
		byte dummy;
		return process(dummy);
	}
	return false;
}

void Snake::reset()
{
	head_pos_.pos_x = 3;
	head_pos_.pos_y = 7;
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

bool Snake::move(Pos vect)
{
	vect += head_pos_;
	// check if highscore is broken. Directly save to avoid a not save in case of reset or poweroff.
	if(body_len_ - 2 > highscore_){
		highscore_ = body_len_-2;
		eeprom_write_word(&EE_highscore,highscore_);
		is_new_highscore_	= true;
	}
	if(validate(vect)){ // game end
		return true;
	}
	if(++body_start_ >= body_buffer_len_){
		body_start_ = 0;
	}

	body_buffer_[body_start_] = head_pos_;
	head_pos_ = vect;

	if(eat()){
		body_len_++;
	}
	render();
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
	Pos move_vect;
	bool button_set = true;
	if(event & CHANGE){
		if(event & INPUT_MASK){
			if(event & BTN_UP){
				// is not 180° rotation or no rotation(in this case the snake will make
				// a additinal ste,  what we avoid with dont allow this)
				if(direction_ != Snake::UP && direction_ != Snake::DOWN){
					direction_ = Snake::UP;
					move_vect = Pos(0,1);
				}
			}
			else if(event & BTN_LEFT){
				if(direction_ != Snake::LEFT && direction_ != Snake::RIGHT){
					direction_ = Snake::LEFT;
					move_vect = Pos(0,-1);
				}
			}

			else if(event & BTN_RIGHT){
				if(direction_ != Snake::LEFT && direction_ != Snake::RIGHT){
					direction_ = Snake::RIGHT;
					move_vect = Pos(1,0);
				}
			}

			else if(event & BTN_DOWN){
				if(direction_ != Snake::DOWN && direction_ != Snake::UP){
					direction_= Snake::DOWN;
					move_vect = Pos(-1,0);
				}
			}
			else{
				button_set = false;
			}
			if(button_set)
				*timer_ = millis() + period_;
		}
	}
	if(event & TIMEOUT1 || button_set){
		*timer_ = millis() + period_;
		if(move(move_vect)){ // game ends
//			return true;
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
		while(isValid(p));
		eat_pos_= p;
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
			return true;
		}
	}
	return false;
}

Pos *Snake::getBodyPos(int pos)
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
}

void Snake::resetHighscore()
{
	eeprom_write_word(&EE_highscore,highscore_ = 0);
}

