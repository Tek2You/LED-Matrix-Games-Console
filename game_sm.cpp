#include "game_sm.h"


#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&GameSM::s)); \
	process(ON_ENTRY); \
	process_criterium_ = 0;\
	}

GameSM::GameSM(Display *display, byte speed)
   :StateMachine(STATE_CAST(&GameSM::stateDefault)), display_(display), speed_(speed), process_timer_process_time_ (0)
{
	process(ON_ENTRY);
}

void GameSM::processStateMaschine(byte event)
{
	if((process_criterium_ & EVER) || (process_criterium_ & ProcessCriterum::PCINT && event & CHANGE)){
		this->process(event);
	}

	else if (process_criterium_ & ProcessCriterum::TIMER && process_timer_process_time_ && process_timer_process_time_ > millis()){
		this->process(event);
		process_timer_process_time_ = 0;
	}
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

	const char * texts[2][2] = {{"Start Game","Setting"}, {"Spiel Starten","Einstellungen"}};
	static MenuItem item;
	if(event & ON_ENTRY){
		process_criterium_ |= PCINT;
		item.init(2,0);
//		item = 0;
	}

	else if(event & INPUT_MASK && event & CHANGE){
		bitToggle(PORTB,1);
		byte advance_output = item.advance(event);
//		byte advance_output = MenuItem::advance(event,item,2);

		if(advance_output == 1){
			switch(item.value_){
			case 0:
				TRANSITION(stateGame);
				break;
			case 1:
				TRANSITION(stateSettingsMenu);
				break;
			}
			return;
		}
		else if(advance_output == 0){
			display_->update();
			return;
		}
	}
	display_->setText(texts[language_][item.value_]);
}

void GameSM::stateGame(byte event)
{
	if(event & ON_ENTRY){
		if(game_ != nullptr){
			delete(game_);
		}
		game_ = new Game(display_);
		game_->reset();
		game_->begin();

	}
	if(event & CHANGE && event & INPUT_MASK){
		if(event & BTN_ROTATE){
			//			game_->rotate();
		}

		if(event & BTN_LEFT){
			//			game_->left();
		}

		else if(event & BTN_RIGHT){
			//			game_->right();
		}

		if(event & BTN_DOWN){
			//			if(step_counter_ >= speed_ / 3){
			//				goto label;
			//			}
		}
	}

	//	if(step_counter_++ >= speed_){
	//label:
	//		step_counter_ = 0;
	//		if(game_->step()){ // game ends
	////			TRANSITION(stateShowResult);
	//		}
	//	}
}

void GameSM::stateShowResult(byte event){
	if(event & ON_ENTRY){
		int points = game_->getPoints();
		//	display_->setString();
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
