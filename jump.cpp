#include "jump.h"
#include "position.h"
#include "operators.h"
#include "avr/eeprom.h"

static unsigned int EE_highscore EEMEM = 0;
unsigned int Jump::highscore_ = eeprom_read_word(&EE_highscore);

Jump::Jump(Display * display, unsigned long *t1, unsigned long *t2) : Game(display),
   forward_timer_(t1), jump_timer_(t2)
{
	field_ = static_cast<byte*>(malloc(display_->rows()+3));
	current_field_start_ = 0;
	score_ = 0;
	setSpeed(2);
}

Jump::~Jump()
{
	free(field_);
}

void Jump::start()
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
	jump_count_ = 0;
	jump_height_ = 4;
	jump_lenght_ = 5;
	*forward_timer_ = millis() + forward_period_;
}

bool Jump::process(Event *event)
{
	if(event->timeOut1()){
		forward();
		*forward_timer_ = millis() + forward_period_;
	}
	if(event->timeOut2()){
		jump();
	}
	if(event->changed()){
		if(event->buttonRightState()){
			if(!jump_count_){
				*jump_timer_ = millis() + jump_period_;
				jump();
			}
		}
		else if(jump_count_ && jump_count_ < 3){
			jump_height_ = 3;
			jump_lenght_ = 3;
		}
	}

	if(!isValid(man_pos_)){
		if(score_ > highscore_){
			highscore_ = score_;
			is_new_highscore_ = true;
			eeprom_write_word(&EE_highscore,highscore_);
			*forward_timer_ = 0;
			*jump_timer_ = 0;
		}
		return true;
	}
	return false;
}

void Jump::clear()
{

}

void Jump::reset()
{

}

unsigned int Jump::points() const
{
	return score_;
}

void Jump::setSpeed(byte v)
{
	switch (v) {
	case 0:
		forward_period_ = 450;
		jump_period_ = 337;
		break;
	case 1:
		forward_period_ = 375;
		jump_period_ = 280;
		return;
	case 3:
		forward_period_ = 225;
		jump_period_ = 169;
		break;
	case 4:
		forward_period_ = 150;
		jump_period_ = 112;
		break;
	case 2:
	default:
		forward_period_ = 300;
		jump_period_ = 225;
		break;
	}
}

byte *Jump::row(byte n)
{
	//	if(current_field_start_ + n > display_->rows()+2){
	//		return field_ + n - (display_->rows()+2-current_field_start_);
	//	}
	//	else{
	//		return field_ + current_field_start_ + n;
	//	}
	return field_ + ((current_field_start_ + n) % 19);
}

unsigned int Jump::highscore()
{
	return highscore_;
}

void Jump::resetHighscore()
{
	eeprom_write_word(&EE_highscore,highscore_ = 0);
}

void Jump::forward()
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
}

void Jump::jump()
{
	if(jump_count_ < jump_height_){
		man_pos_.pos_y--;
	}
	else if(jump_count_ > jump_height_ + jump_lenght_){
		man_pos_.pos_y++;
		if(jump_count_ == jump_height_ * 2 + jump_lenght_){
			jump_count_ = 0;
			is_jumping_ = false;
			jump_height_ = 4;
			jump_lenght_ = 5;
			*jump_timer_ = 0;
			return;
		}
		else{
		}
	}
	*jump_timer_ = millis() + jump_period_;
	jump_count_++;
	render();
}

void Jump::newHind()
{
	int hind_shape = millis() % 7;
	for(Pos p : hinds_[hind_shape].positions){
		p.pos_x += display_->rows(); // go outside range
		bitSet(*row(p.pos_x),p.pos_y);
	}
}

bool Jump::isValid(Pos pos)
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

void Jump::render()
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
