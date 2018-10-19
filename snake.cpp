#include "snake.h"
#include "operators.h"

Snake::Snake(Display *display):
   Game(display),
   body_end_(0), body_start_(0)
{
	body_len_ = 0;
	body_buffer_len_;
	//	body_buffer_ = static_cast<Pos*>(malloc(body_buffer_len_* sizeof(Pos))); // malloc maximum of possible range for the body
	//	body_buffer_end_ = body_buffer_len_ + body_buffer_;
}

Snake::~Snake()
{
	free(body_buffer_);
}

bool Snake::up()
{
	if(direction_ != DOWN && direction_ != UP){
		direction_ = UP;
		return process();
	}
	return false;
}

bool Snake::right()
{
	if(direction_ != LEFT && direction_ != RIGHT){
		direction_ = RIGHT;
		return process();
	}
	return false;
}

bool Snake::left()
{
	if(direction_ != RIGHT && direction_ != LEFT){
		direction_ = LEFT;
		return process();
	}
	return false;
}

bool Snake::down()
{
	if(direction_ != UP && direction_ != DOWN){
		direction_ = DOWN;
		return process();
	}
	return false;
}

void Snake::reset()
{
	head_pos_.pos_x = 3;
	head_pos_.pos_y = 7;
	display_->clear();
	body_len_ = 1;
	body_start_ = 0;
	body_buffer_[0].pos_x = 2;
	body_buffer_[0].pos_y = 7;
	//	body_buffer_[1] = Pos(1,7);
	eat_pos_ = Pos(5,7);
}

void Snake::clear()
{
	display_->clear();
}

bool Snake::process()
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
	default:
		move_x = 0;
		move_y = 0;
		break;
	}
	Pos new_pos = head_pos_;
	new_pos.pos_x += move_x;
	new_pos.pos_y += move_y;
	if(!validate(new_pos)){
		//		return true;
	}
	if(++body_start_ > body_buffer_len_-1){
		body_start_ = 0;
	}

	body_buffer_[body_start_] = head_pos_;
	head_pos_ = new_pos;

	if(eat()){
		body_len_++;
	}
	else{
		if(++body_end_ > body_buffer_len_-1){
			body_end_ = 0;
		}
	}
	render();
	return false;
}

void Snake::start()
{
	reset();
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
		bitToggle(PORTB,1);

		Pos p(0,0);
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
		if(pos.pos_x == p->pos_x && pos.pos_y == p->pos_y){
			return false;
		}
	}
	return true;
}

Pos *Snake::getBodyPos(int pos)
{
	if(pos > body_len_-1 || pos < 0){
		return nullptr;
	}
	if(body_start_ + pos > body_buffer_len_){
		return (body_buffer_ + (pos + body_start_ + (body_len_ - 1) - ((body_buffer_len_-1) - body_start_)));
	}
	else{
		return body_buffer_ + body_start_ + pos;
	}
}
