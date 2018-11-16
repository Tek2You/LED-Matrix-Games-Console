#include "snake.h"
#include "operators.h"

static unsigned int EE_highscore EEMEM = 0;
unsigned int Snake::highscore_ = eeprom_read_word(&EE_highscore);

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

void Snake::setSpeed(byte v)
{

}

bool Snake::process(byte &event)
{
	char move_x, move_y;
	switch (direction_) {
	case UP:
		move_x = 0;
		move_y = 1;
		break;
	case DOWN:
		move_x = 0;
		move_y = -1;
		break;
	case RIGHT:
		move_x = 1;
		move_y = 0;
		break;
	case LEFT:
		move_x = -1;
		move_y = 0;
		break;
	case START:
		return false;
	default:
		move_x = 0;
		move_y = 0;
		break;
	}
	Pos new_pos = head_pos_;
	new_pos.pos_x += move_x;
	new_pos.pos_y += move_y;
	// check if highscore is broken. Directly save to avoid a not save in case of reset or poweroff.
	if(body_len_ - 2 > highscore_){
		highscore_ = body_len_-2;
		eeprom_write_word(&EE_highscore,highscore_);
		is_new_highscore_	= true;
	}
	if(validate(new_pos)){ // game end
		return true;
	}
	if(++body_start_ >= body_buffer_len_){
		body_start_ = 0;
	}

	body_buffer_[body_start_] = head_pos_;
	head_pos_ = new_pos;

	if(eat()){
		body_len_++;
	}
	render();
	return false;
}

void Snake::start()
{
	reset();
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
	if(head_pos_.pos_x == eat_pos_.pos_x && head_pos_.pos_y == eat_pos_.pos_y){
		Pos p;
		do{
			p.pos_x = random() % 8;
			p.pos_y = random() % 16;
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
