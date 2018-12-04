#include "game_sm.h"
#include "avr/eeprom.h"
#include "defaults.h"
#include "operators.h"

#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&GameSM::s)); \
	process_criterium_ = 0;\
	Event e;\
	e.setOnEntry();\
	process(&e); \
	}

#define LOAD_EFFECT_STANDART(s) {\
	load_following_state_ = STATE_CAST(&GameSM::s);\
	TRANSITION(stateLoadEffect);\
	return;\
	}

#define LOAD_EFFECT_BEGIN(s) {\
	static bool load_passed = false;\
	if(load_passed){\
	load_passed = false;\
	}\
	else{\
	load_passed = true;\
	LOAD_EFFECT_STANDART(s);\
	}\
	}


byte EE_speed EEMEM = DEFAULT_SPEED;
byte EE_language EEMEM = DEFAULT_LANGUAGE;

GameSM::GameSM(Display *display)
   :StateMachine(STATE_CAST(&GameSM::stateDefault)), display_(display), process_timer1_ (0), process_timer2_(0), language_(EN)
{
	display_->text1_.setShiftSpeed(5);
	display_->text2_.setShiftSpeed(5);
	speed_ = eeprom_read_byte(&EE_speed);
	language_ = (eeprom_read_byte(&EE_language) ? DE : EN);
	Event e;
	e.setOnEntry();
	process(&e);
}

void GameSM::processStateMaschine(Event *event)
{
	bool process = false;
	if((process_criterium_ & EVER) || (process_criterium_ & ProcessCriterum::PCINT && event->changed())){
		process = true;
	}
	unsigned long now = millis();
	if (process_criterium_ & ProcessCriterum::TIMER1 && process_timer1_ && process_timer1_ <= now){
		event->event_ |= TIMEOUT1;
		process_timer1_ = 0;
		process = true;
	}

	if (process_criterium_ & ProcessCriterum::TIMER2 && process_timer2_ && process_timer2_ <= now){
		event->event_ |= TIMEOUT2;
		process_timer2_ = 0;
		process = true;
	}
	if(process)
		this->process(event);
}

GameSM::MenuItem::Button GameSM::MenuItem::advance(Event* event, char& item, const char num, const char min) {

	switch (event->event_ & INPUT_MASK) {
	case BTN_LEFT:
		if (--item < min)
			item = num-1;
		break;
	case BTN_RIGHT:
		if (++item >= num)
			item = min;
		break;
	case BTN_DOWN:
		return DOWN_BTN;
	case BTN_UP:
		return UP_BTN;
	}
	return NO_BTN;
}

void GameSM::stateDefault(Event* event)
{

	const char * texts[2][5] = {{"Tetris", "Snake", "Running Julian", "highscore", "setting"}, {"Tetris","Snake","Rennender Julian", "Highscore","Einstellungen"}};
	static MenuItem item;
	if(event->onEntry()){
		item.init(5);
		process_criterium_ |= PCINT;
		display_->loadMenuConfig();
	}

	else if(event->changed() && event->isPressed()){
		if(item.advance(event) == MenuItem::DOWN_BTN){
			switch(item.value_){
			case 0:
				TRANSITION(stateTetris);
				break;
			case 1:
				TRANSITION(stateSnake);
				break;
			case 2:
				TRANSITION(stateJump);
				break;
			case 3:
				TRANSITION(stateHighscoreMenu);
				break;
			case 4:
				TRANSITION(stateSettingsMenu);
				break;
			default:
				break;
			}
			return;
		}
	}

	switch(item.value_){
	case 0:
		display_->setIcon(0xfffff7e300081c00);
		break;
	case 1:
		display_->setIcon(0x3c20203c04045c00);
		break;
	case 2:
		display_->setIcon(0x60600a040e040000);
		break;
	case 3:
		display_->setIcon(0x00081c2018043810);
		break;
	case 4:
		display_->setIcon(0x00003c3c3c3c0000);
		break;
	default:
		break;
	}
	display_->text1_.setText(texts[language_][item.value_]);
}


void GameSM::stateTetris(Event *event)
{
	if(event->onEntry()){
		display_->loadsGameCofig();
		if(game_){
			delete game_ ;
			game_ = nullptr;
		}
		game_ = new Tetris(display_,&process_timer1_,&process_timer2_);
		game_->setSpeed(speed_);
		game_->reset();
		game_->start();
		process_criterium_ |= PCINT | TIMER1 | TIMER2;
		return;
	}

	if(game_->process(event)){
		TRANSITION(stateGameOver);
		return;
	}
}

void GameSM::stateSnake(Event *event)
{
	if(event ->onEntry()){
		display_->loadsGameCofig();
		if(game_ != nullptr){
			delete game_ ;
			game_ = nullptr;
		}
		game_ = new Snake(display_,&process_timer1_);
		game_->reset();
		game_->start();
		process_criterium_ |= PCINT | TIMER1;
		return;
	}

	if(game_->process(event)){
		TRANSITION(stateGameOver);
		return;
	}
}

void GameSM::stateJump(Event *event)
{
	if(event->onEntry()){
		display_->loadsGameCofig();
		if(game_ != nullptr){
			delete game_ ;
			game_ = nullptr;
		}
		game_ = new Jump(display_,&process_timer1_, &process_timer2_);
		game_->setSpeed(speed_);
		game_->reset();
		game_->start();
		process_criterium_ |= PCINT | TIMER1 | TIMER2;
		return;
	}

	if(game_->process(event)){
		TRANSITION(stateGameOver);
	}
}

void GameSM::stateGameOver(Event *event){
	if(event->onEntry()){
		LOAD_EFFECT_BEGIN(stateGameOver);
		display_->text1_.clear();
		display_->text2_.clear();
		process_criterium_ = PCINT;
		if(game_ != nullptr){
			process_criterium_ = PCINT;
			display_->loadMenuConfig();
			if(game_->isNewHighscore()){
				display_->text1_.setText((language_ == EN ? "new highscore!" : "neuer Highscore!"));
			}
			else {
				display_->text1_.setText("Game Over");
			}
			display_->text2_.setNumber(game_->points());
			// delete game
			delete game_;
			game_ = nullptr;
		}
		return;
	}

	if(event->changed() && event->isPressed()){
		TRANSITION(stateDefault);
		return;
	}
}

void GameSM::stateSettingsMenu(Event *event)
{
	static MenuItem item;
	const char *menu_text[2][2] = {{"speed", "language"},{"Geschwindigkeit", "Sprache"}};
	if(event->onEntry()){
		display_->loadMenuConfig();
		item.init(2);
		process_criterium_ = PCINT;
	}
	else if(event->changed() && event->isPressed()){
		switch(item.advance(event)){
		case MenuItem::DOWN_BTN:
			switch (item.value_) {
			case 0:
				TRANSITION(stateSpeedMenu);
				break;
			case 1:
				TRANSITION(stateLanguageMenu);
				break;
			default:
				break;
			}
			return;
		case MenuItem::UP_BTN:
			TRANSITION(stateDefault);
			return;
		default:
			break;
		}
	}

	switch (item.value_) {
	case 0:
		display_->setIcon(0x0000122448241200);
		break;
	case 1:
		display_->setIcon(0x2060ff818181ff00);
		break;
	default:
		break;
	}
	display_->text1_.setText(menu_text[language_][item.value_]);
}

void GameSM::stateSpeedMenu(Event *event)
{
	static MenuItem item;
	if(event->onEntry()){
		display_->loadMenuConfig();
		item.init(5,speed_);
		process_criterium_ = PCINT;
	}

	else if(event->changed() && event->isPressed()){
		switch(item.advance(event)){
		case MenuItem::DOWN_BTN:
			speed_ = item.value_;
			eeprom_write_byte(&EE_speed,speed_);
			LOAD_EFFECT_STANDART(stateSettingsMenu);
		case MenuItem::UP_BTN:
			TRANSITION(stateSettingsMenu);
			return;
		default:
			break;
		}
	}
	display_->clear();
	display_->text1_.setNumber(item.value_+1);
	byte cols = display_->cols() / 5.0 * (item.value_+1);
	for(int col = 0; col < cols; col++){
		display_->setColumn(col,0xFF);
	}
}

void GameSM::stateLanguageMenu(Event *event)
{
	const char * menu_text[2][2] = {{"english","Deutsch"},{"english","Deutsch"}};
	static MenuItem item;
	if(event->onEntry()){
		display_->loadMenuConfig();
		item.init(2,(language_==DE?0:1));
		process_criterium_ = PCINT;
	}
	else if(event->changed() && event->isPressed()){
		switch(item.advance(event)){ // enter pressed
		case MenuItem::DOWN_BTN:
			language_ = (item.value_ == 0?EN:DE);
			eeprom_write_byte(&EE_language,byte(language_));
			LOAD_EFFECT_STANDART(stateSettingsMenu);
			return;
		case MenuItem::UP_BTN:
			TRANSITION(stateSettingsMenu);
			return;
		default:
			break;
		}
	}

	switch (item.value_) {
	case 0:
		display_->text2_.setText("E");
		break;
	case 1:
		display_->text2_.setText("D");
		break;
	default:
		break;
	}
	display_->text1_.setText(menu_text[language_][item.value_]);
}

void GameSM::stateLoadEffect(Event *event)
{
	static byte count = 0;
	if(event->onEntry()){
		count = 0;
		display_->text1_.clear();
		display_->text2_.clear();
		process_criterium_ = TIMER1;
		process_timer1_ = millis() + 50;
	}
	if(event->timeOut1()){
		if(count >= display_->rows()){
			if(load_following_state_){
				setState(load_following_state_);
				load_following_state_ = nullptr;
				process_criterium_ = 0;
				Event e;
				e.setOnEntry();
				process(&e);
			}
			else{
				TRANSITION(stateDefault);
			}
			return;
		}
		display_->setRow(count,0xFF);
		process_timer1_ = millis() + 50;
		count++;
		return;
	}
}

void GameSM::stateHighscoreMenu(Event *event)
{
	static MenuItem item;
	if(event->onEntry()){
		item.init(4,0);
		process_criterium_ |= PCINT;
	}
	else if(event->changed() && event->isPressed()){
		byte advanced = item.advance(event);
		if(advanced){
			if(item.value_ == 3){
				if(advanced == MenuItem::DOWN_BTN){
					TRANSITION(stateResetMenu);
				}
				else{
					TRANSITION(stateDefault);
				}
				return;
			}
			else{
				TRANSITION(stateDefault);
				return;
			}
		}
	}
	switch (item.value_) {
	case 0:
		display_->setIcon(0xfffff7e300081c00);
		display_->text1_.setNumber(Tetris::highscore());
		break;
	case 1:
		display_->setIcon(0x3c20203c04045c00);
		display_->text1_.setNumber(Snake::highscore());
		break;
	case 2:
		display_->setIcon(0x60600a040e040000);
		display_->text1_.setNumber(Jump::highscore());
		break;
	case 3:
		display_->setIcon(0xbd42a59999a542bd);
		display_->text1_.setText(language_ == EN ? "reset" : "Zur""\x1c""cksetzen");
	default:
		break;
	}
}

void GameSM::stateResetMenu(Event *event){
	if(event->onEntry()){
		display_->loadMenuConfig();
		process_criterium_ = PCINT;
	}

	else if(event->changed() && event->isPressed()){
		if(event->isPressed()){
			if(event->buttonDownState()){
				Tetris::resetHighscore();
				Snake::resetHighscore();
				Jump::resetHighscore();
				LOAD_EFFECT_STANDART(stateDefault);
				return;
			}
			if(event->buttonUpState()){
				TRANSITION(stateHighscoreMenu);
			}
		}
	}
	display_->text1_.setText((language_ == EN ? "reset scores" : "Highscores zur""\x1c""cksetzen"));
	display_->setIcon(0x00040a1120408000);
}

GameSM::MenuItem::Button GameSM::MenuItem::advance(Event *event)
{
	advance(event,value_,num_);
}

