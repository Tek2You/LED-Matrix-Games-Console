#include "runningman.h"
#include "position.h"
#include "operators.h"
#include "avr/eeprom.h"

static unsigned int EE_highscore EEMEM = 0;
unsigned int RunningMan::highscore_ = eeprom_read_word(&EE_highscore);

RunningMan::RunningMan(Display * display) : Game(display)
{
	field_ = static_cast<byte*>(malloc(display_->rows()+3));
	current_field_start_ = 0;
	score_ = 0;
}

RunningMan::~RunningMan()
{
	free(field_);
}

void RunningMan::start()
{
	for(int i = 0; i < display_->rows() + 3; i++){
		field_[i] = 0;
	}
	current_field_start_ = 0;
	man_pos_ = Pos(0,0);
	next_hind_ = 8;
	man_state_ = 1;
	is_jumping_ = false;
	score_ = 0;
	jump_height_ = 4;
	jump_period_ = 5;
}

bool RunningMan::process()
{
	man_state_ = ! man_state_;
	if(man_pos_.pos_x < 3){
		man_pos_.pos_x++;
	}

	if(next_hind_-- == 0){
		next_hind_ = 12;
		newHind();
	}
	*row(0) = 0;
	if(++current_field_start_ ==  display_->rows()+3)
		current_field_start_ = 0;
	render();
	score_++;
	if(!isValid(man_pos_)){
		if(score_ > highscore_){
			highscore_ = score_;
			is_new_highscore_ = true;
			eeprom_write_word(&EE_highscore,highscore_);
		}
		return true;
	}
	return false;
}

void RunningMan::clear()
{

}

void RunningMan::reset()
{

}

bool RunningMan::right() // processes jump
{
	if(is_jumping_){
		if(jump_count_ < jump_height_){
			man_pos_.pos_y--;
		}
		else if(jump_count_ > jump_height_ + jump_period_){
			man_pos_.pos_y++;
			if(jump_count_ == jump_height_ * 2 + jump_period_){
				jump_count_ = 0;
				is_jumping_ = false;
				jump_height_ = 4;
				jump_period_ = 5;
			}
		}
		jump_count_++;
	}
	render();
	if(!isValid(man_pos_)){
		if(score_ > highscore_){
			highscore_ = score_;
			is_new_highscore_ = true;
			eeprom_write_word(&EE_highscore,highscore_);
		}
		return true;
	}
	return false;
}

bool RunningMan::left()
{

}

bool RunningMan::up() // starts jump
{
	if(!is_jumping_){
		is_jumping_ = true;
		jump_count_ = 0;
	}
}

bool RunningMan::down()
{
	if(jump_count_ < 3){
		jump_height_ = 3;
		jump_period_ = 3;
	}
}

unsigned int RunningMan::points()
{
	return score_;
}

byte *RunningMan::row(byte n)
{
//	if(current_field_start_ + n > display_->rows()+2){
//		return field_ + n - (display_->rows()+2-current_field_start_);
//	}
//	else{
//		return field_ + current_field_start_ + n;
//	}
	return field_ + ((current_field_start_ + n) % 19);
}

unsigned int RunningMan::highscore()
{
	return highscore_;
}

void RunningMan::resetHighscore()
{
	eeprom_write_word(&EE_highscore,highscore_ = 0);
}

void RunningMan::newHind()
{
	int hind_shape = micros() % 7;
	for(Pos p : hinds_[hind_shape].positions){
		p.pos_x += display_->rows(); // go outside range
		bitSet(*row(p.pos_x),p.pos_y);
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
	display_->clear();

	for(int i = 0; i < 16; i++){
		display_->setRow(i,*row(i));
	}

	for(Pos p : man_points){
		p += man_pos_;
		display_->setPixel(p.pos_y, p.pos_x, true);
	}
	Pos p	= man_moving_points[(man_state_ == 1	? 1 : 0)];
	p += man_pos_;
	display_->setPixel(p.pos_y,p.pos_x,true);
}
