#include "snake.h"
#include "operators.h"

Snake::Snake(Display *display):
   Game(display)
{
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

void Snake::process()
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
	head_pos_.pos_x += move_x;
	head_pos_.pos_y += move_y;
	validate(new_pos);
}

void Snake::start()
{
	reset();
}

void Snake::render()
{

}

void Snake::eat()
{
	if(head_pos_.pos_x == eat_pos_.pos_x && head_pos_.pos_y == eat_pos_.pos_y){
		eat_pos_.pos_x = random() % 8;
		eat_pos_.pos_y = random() % 16;
	}
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
		Pos * p = getBodyPos(pos);
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
