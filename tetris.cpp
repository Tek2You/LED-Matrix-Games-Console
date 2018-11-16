﻿#include "tetris.h"
#include "string.h"
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
unsigned int Tetris::highscore_ = eeprom_read_word(&EE_highscore);

Tetris::Tetris(Display *display, unsigned long *t1, unsigned long *t2)
   : Game(display), down_timer_(t1), move_timer_(t2)
{
	// allocate memory to the section for gamestate without tetromino
	field_ = static_cast<byte*>(malloc(display_->rows()));
	tetromino_ = nullptr;
	//	tetromino_ = new Tetromino(tetromino::I, display_->rows(), field_, tetromino::LEFT,tetromino::Pos{2,5});
	general_step_interval = 1800;
	general_down_interval = 180;
	general_first_move_interval = 550;
	general_move_interval = 200;
	reset();
}

Tetris::~Tetris()
{
	if(tetromino_ != nullptr){
		delete tetromino_;
	}
	free(field_);

	points_ = 0;
}

void Tetris::start()
{
	newTetromino();
}

bool Tetris::process(byte &event)
{
	static bool btn_down_state	= false;
	static bool btn_left_state = false;
	static bool btn_right_state = false;
	static unsigned long step_interval;

	static int general_step_interval;
	static int general_down_interval;
	static int general_first_move_interval;
	static int general_move_interval;

	unsigned long now = millis();
	if(event & ON_ENTRY){
		step_interval = general_step_interval;
		*down_timer_ = now + step_interval;
		return false;
	}
	if(event & CHANGE){
		if(event & INPUT_MASK){
			if(event & BTN_UP){
				 up();
			}

			// btn down
			if(event & BTN_DOWN){
				if(btn_down_state == false){
					step_interval = general_down_interval;
					*down_timer_ = step_interval + now;
					btn_down_state = true;
					goto step;
				}
			}
			else if(btn_down_state){
				step_interval = general_step_interval;
				*down_timer_ = step_interval + now;
				btn_down_state = false;
			}
		}

		// btn left
		if(event & BTN_LEFT) {
			if(!btn_left_state) {
				if(!btn_right_state) {
					 left();
					btn_left_state =	true;
					 *move_timer_ = now + general_first_move_interval;
				}
			}
		}
		else{
			if(btn_left_state)
				 *move_timer_ = 0;
			btn_left_state = false;
		}

		// btn right
		if(event & BTN_RIGHT){
			if(! btn_right_state){
				if(!btn_left_state){
					 right();
					btn_right_state =	true;
					 *move_timer_ = now + general_first_move_interval;
				}
			}
		}
		else{
			if(btn_right_state)
				 *move_timer_ = 0;
			btn_right_state = false;
		}

	}
	if(event & TIMEOUT2){
		if(btn_left_state)
			if(event & BTN_LEFT){
				 left();
				 *move_timer_ = now + general_move_interval;
			}
			else{
				btn_left_state = false;
			}
		else if(btn_right_state){
			if(event & BTN_RIGHT){
				 right();
				 *move_timer_ = now + general_move_interval;
			}
			else{
				btn_right_state = false;
			}
		}
	}
	if(event & TIMEOUT1){
step:
		if( down()){ // game ends
//			return true;
		}
		if(!(event & BTN_DOWN))
			step_interval = general_step_interval;
		else if(event & BTN_DOWN)
			step_interval = general_down_interval;
		*down_timer_ =	now + step_interval;
	}
	return false;
}

void Tetris::render()
{
	display_->clear();
	display_->setArray(field_);
	if(tetromino_ != nullptr){
		Pos positions[4];
		tetromino_->getPositions(positions);
		for(Pos p : positions){
			display_->setPixel(p.pos_x,p.pos_y, true);
		}
	}
}

bool Tetris::up()
{
	/*
	 * function scheme:
	 * 1. get current values
	 * 2. calculate next(possible) direction. Possible means not that the brick dosnt collides
	 * 3. Check if the new direction is valid
	 *    4. if not: try to change to a valid position
	 *
	*/
	if(tetromino_ == nullptr)
		return false;
	tetromino::DIRECTION direction = tetromino_->direction();
	tetromino::DIRECTION new_direction;
	Pos position = tetromino_->pos();
	Pos new_position = position;
	tetromino::SHAPE shape = tetromino_->shape();

	byte possible_directions = tetromino_->getPossibleDirections();
	new_direction = Tetromino::rotate(direction,shape);

	// position is not valid
	byte valid_output = tetromino_->isValid(shape,new_direction,position);
	if(valid_output)
	{
		if(valid_output & tetromino::OVER_LEFT) // left over
		{
			++new_position.pos_x;
			while(valid_output = tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_LEFT){
				new_position.pos_x++;
			}
		}
		else if(valid_output & tetromino::OVER_RIGHT) // right over
		{
			new_position.pos_x --;
			while(valid_output = tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_RIGHT){
				new_position.pos_x--;
			}
		}
		if(valid_output & tetromino::OVER_BELOW) // over below
		{
			new_position.pos_y ++;
			while(valid_output = tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_BELOW){
				new_position.pos_y++;
			}
		}
		else if(valid_output & tetromino::OVER_ABOVE) // above over
		{
			new_position.pos_y --;
			while(valid_output = tetromino_->isValid(shape,new_direction,new_position) & tetromino::OVER_ABOVE){
				new_position.pos_y--;
			}
		}
		if(valid_output & tetromino::COLLIDE) // collides with exiting tetromino
		{
			return false;
		}
	}
	tetromino_->setPos(new_position);
	tetromino_->setDirection(new_direction);
	render();
	return true;
}

bool Tetris::right()

{
	if(tetromino_ == nullptr)
		return false;
	Pos pos = tetromino_->pos();
	pos.pos_x++;
	if(tetromino_->isValid(tetromino_->shape(),tetromino_->direction(),pos)){  // not valid
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return true;
}

bool Tetris::left()
{
	if(tetromino_ == nullptr)
		return false;
	Pos pos = tetromino_->pos();
	pos.pos_x--;
	if(tetromino_->isValid(tetromino_->shape(),tetromino_->direction(),pos)){  // not valid
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return true;
}

bool Tetris::down()
{
	if(tetromino_ == nullptr)
		return false;
	Pos pos = tetromino_->pos();
	pos.pos_y--;
	byte valid_output = tetromino_->isValid(tetromino_->shape(),tetromino_->direction(),pos);
	if(valid_output){  // not valid
		takeOverTetromino();
		checkRowsFinished();
		if(newTetromino())
			return true;
		return false;
	}
	tetromino_->setPos(pos);
	render();
	return false;
}

void Tetris::reset()
{
	if(tetromino_){
		delete(tetromino_);
		tetromino_ = nullptr;
	}
	clear();
	display_->clear();
	points_ = 0;

}

void Tetris::clear()
{
	for(byte * ptr = field_; ptr < &field_[display_->rows()];ptr++){
		*ptr = 0;
	}
}

void Tetris::setSpeed(byte v)
{
	switch (v) {
	case 0:
		general_step_interval = 1800;
		general_down_interval = 180;
		general_first_move_interval = 550;
		general_move_interval = 200;
		break;
	case 1:
		general_step_interval = 1400;
		general_down_interval = 140;
		general_first_move_interval = 380;
		general_move_interval = 160;
		break;
	case 3:
		general_step_interval = 800;
		general_down_interval = 80;
		general_first_move_interval = 240;
		general_move_interval = 100;
		break;
	case 4:
		general_step_interval = 500;
		general_down_interval = 50;
		general_first_move_interval = 150;
		general_move_interval = 60;
		break;
	case 2:
		general_step_interval = 1000;
		general_down_interval = 100;
		general_first_move_interval = 300;
		general_move_interval = 120;
		break;
	default:
		break;
	}
}

unsigned int Tetris::highscore()
{
	return highscore_;
}

bool Tetris::newTetromino()
{
	if(tetromino_ != nullptr){
		delete tetromino_;
		tetromino_ = nullptr;
	}

	tetromino::SHAPE shape = randomTetrominoShape();
	tetromino_ = new Tetromino(shape, display_->rows(), field_,	randomTetrominoDirection(shape),Pos(4,display_->rows()-1));

	Pos points[4];
	tetromino_->getPositions(points);
	for(Pos p : points){
		if(p.pos_y > display_->rows() - 1){
			Pos pos = tetromino_->pos();
			pos.pos_y -= (p.pos_y - (display_->rows()-1));
			tetromino_->setPos(pos);
			tetromino_->getPositions(points);
		}
	}
	render();
	if(tetromino_->isValid() & COLLIDE){ // not valid
		return true;
	}
	return false;
}

void Tetris::takeOverTetromino()
{
	Pos positions[4];
	tetromino_->getPositions(positions);
	for(Pos p : positions){
		bitWrite(field_[p.pos_y],p.pos_x,true);
	}
	render();
}

void Tetris::resetHighscore()
{
	eeprom_write_word(&EE_highscore,highscore_ = 0);
}

void Tetris::checkRowsFinished()
{
	for(int i = 0; i < display_->rows(); i++){
		while(field_[i] == 0xFF){ // row is full
			field_[i] = 0;
			points_++;
			for(int j = i; j < display_->rows() - 1;j++){
				field_[j] = field_[j+1];
			}
		}
	}
	render();
	if(points_ > highscore_){
		highscore_ = points_;
		eeprom_write_word(&EE_highscore,highscore_);
		is_new_highscore_ = true;
	}
}

tetromino::SHAPE Tetris::randomTetrominoShape()
{
	return tetromino::SHAPE(micros() % 7);
}

tetromino::DIRECTION Tetris::randomTetrominoDirection(tetromino::SHAPE shape){
	byte directions = Tetromino::getPossibleDirections(shape);
	byte num = Tetromino::possibleDirections(shape);

	if(num <= 1){
		return TOP;
	}

	byte i_direction = ((millis()) % (num));
	for(int i = 0, j = 0; i < 4; i++){
		if(bitRead(directions,i)){
			if(j == i_direction){
				tetromino::DIRECTION dir = tetromino::DIRECTION(j);
				return dir;
			}
			j++;
		}
	}
}

