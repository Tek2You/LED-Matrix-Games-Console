#include "runningman.h"
#include "position.h"

RunningMan::RunningMan(Display * display) : Game(display)
{
	field_ = static_cast<char*>(malloc(display_->rows()+3));
	current_field_start_ = 0;
}

RunningMan::~RunningMan()
{

}

void RunningMan::start()
{
	man_pos_ = Pos(0,0);
	next_hind_ = 8;
}

bool RunningMan::process()
{
	man_state_ = ! man_state_;
	if(man_pos_.pos_x < 2){
		man_pos_.pos_x++;
	}
	if(!next_hind_--){
		next_hind_ = 8;
		newHind();
	}
	*row(0) = 0;
	if(current_field_start_++ ==  display_->rows()+3)
		current_field_start_ = 0;
	if(!isValid(man_pos_)){
		return true;
	}
	render();
	return false;
}

void RunningMan::clear()
{

}

void RunningMan::reset()
{

}

bool RunningMan::right()
{

}

bool RunningMan::left()
{

}

bool RunningMan::up()
{

}

bool RunningMan::down()
{

}

unsigned int RunningMan::points()
{

}

char *RunningMan::row(byte n)
{
	if(current_field_start_ + n > display_->rows()){
		return field_ + n - (display_->rows()+2-current_field_start_);
	}
	else{
		return field_ + n;
	}
}

void RunningMan::newHind()
{
	int hind_shape = random() % 7;
	for(Pos p : hinds_[hind_shape]){
		p += Pos(0,display_->rows());
//		bitSet(*row(p.pos_x),p.pos_y);
	}
}

bool RunningMan::isValid(Pos pos)
{
	for(Pos p: man_moving_points){
		p += pos;
		if(bitRead(*row(p.pos_x),p.pos_y))
			return false;
	}
	for(Pos p: man_points){
		p += pos;
		if(bitRead(*row(p.pos_x),p.pos_y))
			return false;
	}
	return true;
}

void RunningMan::render()
{
	for(int i = 0; i < display_->rows(); i++){
		display_->setRow(i,*row(i));
	}
	display_->clear();
	for(Pos p : man_points){
		p += man_pos_;
		display_->setPixel(p.pos_y, p.pos_x, true);
	}
	Pos p	= man_moving_points[(man_state_ == 1	? 1 : 0)];
	display_->setPixel(p.pos_y,p.pos_x,true);
}
