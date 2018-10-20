#include "game_sm.h"


#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&GameSM::s)); \
	process_criterium_ = 0;\
	process(ON_ENTRY); \
	}

GameSM::GameSM(Display *display, byte speed)
   :StateMachine(STATE_CAST(&GameSM::stateDefault)), display_(display), speed_(speed), process_timer1_ (0), process_timer2_(0)
{
	process(ON_ENTRY);
	display_->text1_->setShiftSpeed(5);
	display_->text2_->setShiftSpeed(5);
}

void GameSM::processStateMaschine(byte event)
{
	bool process = false;
	if((process_criterium_ & EVER) || (process_criterium_ & ProcessCriterum::PCINT && event & CHANGE)){
		process = true;
	}
	unsigned long now = millis();
	if (process_criterium_ & ProcessCriterum::TIMER1 && process_timer1_ && process_timer1_ <= now){
		event |= TIMEOUT1;
		process_timer1_ = 0;
		process = true;
	}

	if (process_criterium_ & ProcessCriterum::TIMER2 && process_timer2_ && process_timer2_ <= now){
		event |= TIMEOUT2;
		process_timer2_ = 0;
		process = true;
	}
	if(process)
		this->process(event);
}

byte GameSM::MenuItem::advance(byte event, char& item, const char num, const char min) {

	switch (event & INPUT_MASK) {
	case BTN_LEFT:
		if (--item < min)
			item = num-1;
		break;
	case BTN_RIGHT:
		if (++item >= num)
			item = min;
		break;
	case BTN_DOWN:
		return 1;
	case BTN_ROTATE:
		return 2;
	}
	return 0;
}


void GameSM::stateDefault(byte event)
{

	const char * texts[2][3] = {{"Tetris", "Snake", "Setting"}, {"Tetris","Snake","Einstellungen"}};
	static MenuItem item;
	if(event & ON_ENTRY){
		process_criterium_ |= PCINT;
		item.init(3,0);
		display_->loadMenuConfiguration();
	}

	else if(event & INPUT_MASK && event & CHANGE){
		byte advance_output = item.advance(event);

		if(advance_output == 1){
			switch(item.value_){
			case 0:
				TRANSITION(stateTetris);
				break;
			case 1:
				TRANSITION(stateSnake);
				break;
			case 2:
				TRANSITION(stateSettingsMenu);
			}
			return;
		}
		else if(advance_output == 0){
			display_->update();
			return;
		}
	}

	display_->text1_->setText(texts[language_][item.value_]);
}

void GameSM::stateTetris(byte event)
{
	static bool btn_down_state	= false;
	static bool btn_left_state = false;
	static bool btn_right_state = false;
	static unsigned long step_interval;
	unsigned long now = millis();
	if(event & ON_ENTRY){
		display_->text1_->clear();
		display_->text2_->clear();
		if(game_ != nullptr){
			delete game_ ;
			game_ = nullptr;
		}
		game_ = new Tetris(display_);
		game_->reset();
		game_->start();
		btn_down_state = false;
		process_criterium_ |= PCINT | TIMER1 | TIMER2;
		step_interval = 1000;
		process_timer1_ = now + step_interval;
		return;
	}

	if(event & CHANGE){
		if(event & INPUT_MASK){
			if(event & BTN_ROTATE){
				game_->up();
			}

			// btn down
			if(event & BTN_DOWN){
				if(btn_down_state == false){
					step_interval = 100;
					process_timer1_ = step_interval + now;
					btn_down_state = true;
					goto step;
				}
			}
			else if(btn_down_state){
				step_interval = 1000;
				process_timer1_ = step_interval + now;
				btn_down_state = false;
			}
		}

		// btn left
		if(event & BTN_LEFT) {
			if(!btn_left_state) {
				if(!btn_right_state) {
					game_->left();
					btn_left_state =	true;
					process_timer2_ = now + 300;
				}
			}
		}
		else{
			if(btn_left_state)
				process_timer2_ = 0;
			btn_left_state = false;
		}

		// btn right
		if(event & BTN_RIGHT){
			if(! btn_right_state){
				if(!btn_left_state){
					game_->right();
					btn_right_state =	true;
					process_timer2_ = now + 300;
				}
			}
		}
		else{
			if(btn_right_state)
				process_timer2_ = 0;
			btn_right_state = false;
		}

	}
	if(event & TIMEOUT2){
		if(btn_left_state)
			if(event & BTN_LEFT){
				game_->left();
				process_timer2_ = now + 120;
			}
			else{
				btn_left_state = false;
			}
		else if(btn_right_state){
			if(event & BTN_RIGHT){
				game_->right();
				process_timer2_ = now + 120;
			}
			else{
				btn_right_state = false;
			}
		}
	}
	if(event & TIMEOUT1){
step:
		if(game_->down()){ // game ends
			TRANSITION(stateGameOver);
			return;
		}
		if(!(event & BTN_DOWN))
			step_interval = 1000;
		else if(event & BTN_DOWN)
			step_interval = 100;
		process_timer1_ =	now + step_interval;
	}
}

void GameSM::stateSnake(byte event)
{
	static Snake::Direction dir;
	if(event & ON_ENTRY){
		display_->text1_->clear();
		display_->text2_->clear();
		if(game_ != nullptr){
			delete game_ ;
			game_ = nullptr;
		}
		game_ = new Snake(display_);
		game_->start();
		process_criterium_ |= PCINT | TIMER1;
		process_timer1_ = millis() + 400;
		dir = Snake::START;
		return;
	}


	bool finished = false;
	if(event & CHANGE){
		if(event & INPUT_MASK){
			bool button_set = false;
			if(event & BTN_ROTATE){
				if(dir != Snake::UP && dir != Snake::DOWN){
					dir = Snake::UP;
					button_set = true;
					finished = game_->up();
				}
			}
			else if(event & BTN_LEFT){
				if(dir != Snake::LEFT && dir != Snake::RIGHT){
					dir = Snake::LEFT;
					button_set = true;
					finished = game_->left();
				}
			}

			else if(event & BTN_RIGHT){
				if(dir != Snake::LEFT && dir != Snake::RIGHT){
					dir = Snake::RIGHT;
					button_set = true;
					finished = game_->right();
				}
			}

			else if(event & BTN_DOWN){
				if(dir != Snake::DOWN && dir != Snake::UP){
					dir = Snake::DOWN;
					button_set = true;
					finished = game_->down();
				}
			}
			if(button_set)
				process_timer1_ = millis() + 400;
		}
	}
	if(event & TIMEOUT1){
		process_timer1_ = millis() + 400;
		if(game_->process()){ // game ends
			finished = true;
		}
	}
	if(finished){
		TRANSITION(stateGameOver);
		return;
	}
}


void GameSM::stateGameOver(byte event){
	if(event & ON_ENTRY){
		display_->clear();
		process_criterium_ |= PCINT;
		if(game_ != nullptr){
			int points;
			points = game_->getPoints();
			delete game_;
			game_ = nullptr;

			display_->text2_->setOffset(0);
			display_->text2_->setOperationRows(0,7);
			display_->text1_->setOffset(8);
			display_->text1_->setOperationRows(8,15);
			display_->text1_->setText("Game Over");
			display_->text2_->setNumber(points);
		}
	}
	if(event & CHANGE && event & INPUT_MASK){
		TRANSITION(stateDefault);
		return;
	}
}

void GameSM::stateSettingsMenu(byte event)
{
	static MenuItem item;
	const byte menu_text[2][2] = {

	};
	if(event & ON_ENTRY){
		item.init(2,0);
	}
	else if(event & ON_ENTRY && event & INPUT_MASK){
		if(item.advance(event)){

		}
	}
}

byte GameSM::MenuItem::advance(byte event)
{
	advance(event,value_,num_);
}

