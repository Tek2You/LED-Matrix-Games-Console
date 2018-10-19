#include "snake.h"
#include "operators.h"

Snake::Snake(Display *display):
   Game(display)
{
	body_len_ = 0;
	body_buffer_len_;
	body_buffer_ = static_cast<Pos*>(malloc(body_buffer_len_* sizeof(Pos))); // malloc maximum of possible range for the body
	body_buffer_end_ = body_buffer_len_ + body_buffer_;
}

Snake::~Snake()
{
	free(body_buffer_);
}

bool Snake::up()
{
	direction_ = UP;
	process();
}

bool Snake::right()
{
	direction_ = RIGHT;
	process();
}

bool Snake::left()
{
	direction_ = LEFT;
	process();
}

bool Snake::down()
{
	direction_ = DOWN;
	process();
}

void Snake::reset()
{
	head_pos_ = Pos(7,3);
	body_len_ = 2;
	body_start_ = body_buffer_;
	body_buffer_[0] = Pos(7,2);
	body_buffer_[1] = Pos(7,1);
	eat_pos_ = Pos(7,5);
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
	body_start_;
	Pos new_pos = head_pos_;
	new_pos.pos_x += move_x;
	new_pos.pos_y += move_y;
	if(validate(new_pos)){
		return true;
	}
	if(++body_start_ > body_buffer_end_){
		body_start_ = body_buffer_;
	}
	if(eat()){
		body_len_++;
	}
	render();
}

void Snake::start()
{
	reset();
}

void Snake::render()
{
	display_->clear();
	display_->setPixel(head_pos_.pos_x,head_pos_.pos_y,true);
	for(int i = 0; i < body_len_; i++){
		Pos * p = getBodyPos(i);
		display_->setPixel(p->pos_x,p->pos_y,true);
	}
}

bool Snake::eat()
{
	if(head_pos_.pos_x == eat_pos_.pos_x && head_pos_.pos_y == eat_pos_.pos_y){
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
	if(pos > body_len_)
		return nullptr;
	if(body_start_ + pos > body_buffer_end_){
		//		return (body_buffer_ + (body_start_ + body_len_ - (body_buffer_end_ - body_start_)));
	}
	else{
		return body_start_ + pos;
	}
}
