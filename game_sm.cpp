#include "game_sm.h"


#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&GameSM::s)); \
	process(ON_ENTRY); \
	}

GameSM::GameSM(Display *display)
   :StateMachine(STATE_CAST(&GameSM::stateDefault)), display_(display)
{

}

bool GameSM::advance(byte event, char& item, const char num, const char min) {

	switch (event & INPUT_MASK) {
	case BTN_LEFT:
		if (--item < min)
			item = num-1;
		break;
	case BTN_RIGHT:
		if (++item >= num)
			item = min;
		break;
	case BTN_FLIP:
		return true;
	}
	return false;
}

void GameSM::stateDefault(byte event)
{
	const char * texts[2][2] = {{"Start Game","Setting"}, {"Spiel Starten","Einstellungen"}};
	static char menu_item = 0;
	if(event & ON_ENTRY){
		if(game_ != nullptr){
			delete(game_);
		}
		game_ = new Game(display_);
	}
	else if(event & INPUT_MASK && event & CHANGE){
		if(advance(event,menu_item,2)){
			switch(menu_item){
			case 0:
				TRANSITION(stateGame);
				break;
			case 1:
				TRANSITION(stateSettingsMenu);
				break;
			}
		}

		else{
			display_->update();
			return;
		}
		display_->setText(texts[language_][menu_item]);
	}
}

void GameSM::stateGame(byte event)
{

}

void GameSM::stateSettingsMenu(byte event)
{

}
