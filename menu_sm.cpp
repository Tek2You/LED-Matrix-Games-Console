/* game_sm.cpp : game statemaschine class containing functions for every state
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see http://www.gnu.org/licenses/.
 */

#include "menu_sm.h"
#include "avr/eeprom.h"
#include "avr/wdt.h"
#include "defaults.h"
#include "operators.h"

#include "dodge.h"
#include "snake.h"
#include "spaceinvaders.h"
#include "tetris.h"

#undef TRANSITION
#define TRANSITION(s, event) transition(&MenuSM::s, event)

#define LOAD_EFFECT_STANDART(s, event)                                                                                 \
	{                                                                                                                   \
		load_following_state_ = STATE_CAST(&MenuSM::s);                                                                  \
		TRANSITION(stateLoadEffect, event);                                                                              \
		return;                                                                                                          \
	}

#define LOAD_EFFECT_BEGIN(s, event)                                                                                    \
	{                                                                                                                   \
		static bool load_passed = false;                                                                                 \
		if (load_passed)                                                                                                 \
		{                                                                                                                \
			load_passed = false;                                                                                          \
		}                                                                                                                \
		else                                                                                                             \
		{                                                                                                                \
			load_passed = true;                                                                                           \
			LOAD_EFFECT_STANDART(s, event);                                                                               \
		}                                                                                                                \
	}

byte EE_speed EEMEM = DEFAULT_SPEED;
byte EE_language EEMEM = DEFAULT_LANGUAGE;
byte EE_brightness EEMEM = DEFAULT_BRIGHTNESS;

MenuSM::MenuSM(Display *display, Event *event)
	 : StateMachine(STATE_CAST(&MenuSM::stateDefault)), display_(display), language_(EN)
{
	display_->text1_.setShiftSpeed(5);
	display_->text2_.setShiftSpeed(5);
	display_->text1_.setAlignment(Text::MIDDLE);
	display_->text2_.setAlignment(Text::MIDDLE);
	speed_ = eeprom_read_byte(&EE_speed);
	language_ = (eeprom_read_byte(&EE_language) ? DE : EN);
	brightness_ = (eeprom_read_byte(&EE_brightness));
	if (brightness_ > 3)
	{
		brightness_ = 3;
	}
	display->setBrightness(brightness_);
	event->setOnEntry();
	process(event);
	event->clear();
}

void MenuSM::transition(Function function, Event *event)
{
	setState(static_cast<StateMachine<Event *>::State>(function));
	event->clearFlags();
	event->setOnEntry();
	this->StateMachine::process(event);
}

bool MenuSM::processMenuStop(Event *event)
{
	if (event->buttonStop().pressed())
	{
		TRANSITION(stateDefault, event);
		return true;
	}
	return false;
}

MenuSM::MenuItem::Button MenuSM::MenuItem::advance(Event *event, char &item, const char num, const char min)
{
	if (event->buttonLeft().pressed())
	{
		if (--item < min) item = num - 1;
	}
	else if (event->buttonRight().pressed())
	{
		if (++item >= num) item = min;
	}
	else if (event->buttonDown().pressed())
	{
		return DOWN_BTN;
	}
	else if (event->buttonUp().pressed())
	{
		return UP_BTN;
	}
	return NO_BTN;
}

void MenuSM::stateDefault(Event *event)
{
	static const Item items[6] = {Item{"highscore", "Highscore", 0x00081c2018043810},
											Item{"settings", "Einstellungen", 0x000003c3c3c3c0000},
											Item{"Tetris", "Tetris", 0xfffff7e300081c00},
											Item{"Snake", "Snake", 0x3c20203c04045c00},
											Item{"Dodge", "Dodge", 0x381003c00e30310},
											Item{"Space Invaders", "Space Invaders", 0x1c08000841d46b91}};
	;

	if (event->onEntry())
	{
		item_.init(6,last_played_game_ + 2);
		display_->loadMenuConfig();
		event->setupMenu();
	}
	else if (event->buttonStop().pressed())
	{
		last_played_game_ = Game::TETRIS;
		item_.value_ = 2;
//		event->clear();
//		stateDefault(event);
	}
	else if (event->controlButtonPressed())
	{
		if (item_.advance(event) == MenuItem::DOWN_BTN)
		{
			switch (item_.value_)
			{
			// last because we begin counting with 2
			case 0:
				TRANSITION(stateHighscoreMenu, event);
				break;
			case 1:
				TRANSITION(stateSettingsMenu, event);
				break;
			default:
				last_played_game_ = Game::GameType(item_.value_ - 2);
				TRANSITION(stateGame, event);
				break;
			}
			return;
		}
	}
	else
	{
		return;
	}

	showItem(items[item_.value_]);
}

void MenuSM::stateGame(Event *event)
{
	if (event->onEntry())
	{
		if (game_)
		{
			delete game_;
			game_ = nullptr;
		}

		switch (last_played_game_)
		{
		case Game::TETRIS:
			game_ = new Tetris(display_);
			break;
		case Game::SNAKE:
			game_ = new Snake(display_);
			break;
		case Game::DODGE:
			game_ = new Dodge(display_);
			break;
		case Game::SPACE_INVADERS:
			game_ = new SpaceInvaders(display_);
			break;
		default:
			return;
		}
		game_->setSpeed(speed_);
		game_->start(event);
		return;
	}
	if (game_->process(event))
	{
		TRANSITION(stateGameOver, event);
	}
}

void MenuSM::stateGameOver(Event *event)
{
	if (event->onEntry())
	{
		LOAD_EFFECT_BEGIN(stateGameOver, event);
		display_->text1_.clear();
		display_->text2_.clear();
		if (game_ != nullptr)
		{
			event->setupMenu();
			display_->loadMenuConfig();
			if (game_->type() == Game::SNAKE && game_->score() >= 124)
			{
				display_->text1_.setText((language_ == EN ? "you got it!" : "Geschafft!"), false);
			}
			else
			{
				if (game_->isNewHighscore())
				{
					display_->text1_.setText((language_ == EN ? "new highscore!" : "Neuer Highscore!"), false);
				}
				else
				{
					display_->text1_.setText("Game Over", false);
				}
			}
			display_->text2_.setNumber(game_->score(), false);
			display_->show();
			// delete game
			delete game_;
			game_ = nullptr;
		}
		return;
	}
	else if (processMenuStop(event))
	{
		return;
	}
	if (event->controlButtonPressed())
	{
		TRANSITION(stateDefault, event);
		return;
	}
}

void MenuSM::stateSettingsMenu(Event *event)
{
	static const Item items[3] = {Item{"speed", "Geschwindigkeit", 0x0000122448241200},
											Item{"language", "Sprache", 0x2060ff818181ff00},
											Item{"brightness", "Helligkeit", 0x0007133558900000}};
	if (event->onEntry())
	{
		display_->loadMenuConfig();
		item_.init(3, 0);
		event->setupMenu();
	}
	else if (processMenuStop(event))
	{
		return;
	}
	else if (event->controlButtonPressed())
	{
		switch (item_.advance(event))
		{
		case MenuItem::DOWN_BTN:
			switch (item_.value_)
			{
			case 0:
				TRANSITION(stateSpeedMenu, event);
				break;
			case 1:
				TRANSITION(stateLanguageMenu, event);
				break;
			case 2:
				TRANSITION(stateBrightnessMenu, event);
				break;
			default:
				break;
			}
			return;
		case MenuItem::UP_BTN:
			TRANSITION(stateDefault, event);
			return;
		default:
			break;
		}
	}
	else
	{
		return;
	}
	showItem(items[item_.value_]);
}

void MenuSM::stateSpeedMenu(Event *event)
{
	ButtonAutoTrigger *auto_trigger;
	if (event->onEntry())
	{
		item_.init(5, speed_);
		display_->loadMenuConfig();
		event->setupGame();
		event->addTrigger(new ButtonAutoTrigger(&event->buttonLeft(), &event->buttonRight(), 500, 300));
	}
	else if (processMenuStop(event))
	{
		return;
	}
	auto_trigger = static_cast<ButtonAutoTrigger *>(event->trigger(0));
	if (auto_trigger->triggered())
	{
		item_.value_ += (auto_trigger->direction() == ButtonAutoTrigger::BTN_1 ? -1 : 1);
		if (item_.value_ < 0)
			item_.value_ = 0;
		else if (item_.value_ > 4)
			item_.value_ = 4;
	}
	else
	{
		switch (item_.advance(event))
		{
		case MenuItem::DOWN_BTN:
			speed_ = item_.value_;
			eeprom_write_byte(&EE_speed, speed_);
			LOAD_EFFECT_STANDART(stateSettingsMenu, event);
		case MenuItem::UP_BTN:
			TRANSITION(stateSettingsMenu, event);
			return;
		case MenuItem::NO_BTN:
			return;
		default:
			break;
		}
	}

	display_->clear();
	display_->text1_.setNumber(item_.value_ + 1, false);
	byte cols = display_->cols() / 5.0 * (item_.value_ + 1);
	for (int col = 0; col < cols; col++)
	{
		display_->setColumn(col, 0xFF);
	}
	display_->show();
}

void MenuSM::stateBrightnessMenu(Event *event)
{
	ButtonAutoTrigger *auto_trigger;

	if (event->onEntry())
	{
		display_->loadMenuConfig();
		item_.init(4, brightness_);
		event->setupMenu();
		event->setFlag(Event::ProcessTriggers);
		event->addTrigger(new ButtonAutoTrigger(&event->buttonLeft(), &event->buttonRight(), 500, 400));
	}
	else if (processMenuStop(event))
	{
		return;
	}
	auto_trigger = static_cast<ButtonAutoTrigger *>(event->trigger(0));
	if (auto_trigger->triggered())
	{
		item_.value_ += (auto_trigger->direction() == ButtonAutoTrigger::BTN_1 ? -1 : 1);
		if (item_.value_ < 0)
			item_.value_ = 0;
		else if (item_.value_ > 3)
			item_.value_ = 3;
	}
	else
	{
		switch (item_.advance(event))
		{
		case MenuItem::DOWN_BTN:
			brightness_ = item_.value_;
			display_->setBrightness(brightness_);
			eeprom_write_byte(&EE_brightness, brightness_);
			LOAD_EFFECT_STANDART(stateSettingsMenu, event);
			return;
		case MenuItem::UP_BTN:
			TRANSITION(stateSettingsMenu, event);
			return;
		case MenuItem::NO_BTN:
			break;
		default:
			return;
		}
	}

	display_->clear();
	display_->text1_.setNumber(item_.value_ + 1, false);
	byte cols = display_->cols() / 4.0 * (item_.value_ + 1);
	for (int col = 0; col < cols; col++)
	{
		display_->setColumn(col, 0xFF);
	}
	display_->show();
	display_->setBrightness(item_.value_);
}

void MenuSM::stateLanguageMenu(Event *event)
{
	if (event->onEntry())
	{
		item_.init(2, (language_ == DE ? 0 : 1));
		display_->loadMenuConfig();
		event->setupMenu();
	}
	else if (processMenuStop(event))
	{
		return;
	}
	else if (event->controlButtonChanged())
	{
		switch (item_.advance(event))
		{ // enter pressed
		case MenuItem::DOWN_BTN:
			language_ = Language(item_.value_);
			eeprom_write_byte(&EE_language, byte(language_));
			LOAD_EFFECT_STANDART(stateSettingsMenu, event);
			return;
		case MenuItem::UP_BTN:
			TRANSITION(stateSettingsMenu, event);
			return;
		default:
			return;
		}
	}

	display_->text1_.setText(item_.value_ == 0 ? "english" : "Deutsch", false);
	display_->text2_.setText(item_.value_ == 0 ? "E" : "D");
}

void MenuSM::stateLoadEffect(Event *event)
{
	static int count = 0;
	if (event->onEntry())
	{
		count = 0;
		display_->text1_.clear();
		display_->text2_.clear();
		event->addTrigger(new Timer(12));
		event->setFlag(Event::ProcessTriggers);
		event->setFlag(Event::ProcessStop);
	}
	else if (processMenuStop(event))
	{
		return;
	}
	if (event->trigger(0)->triggered())
	{
		if (count >= display_->rows() * display_->cols() / 2)
		{
			if (load_following_state_)
			{
				setState(load_following_state_);
				load_following_state_ = nullptr;
				event->clearFlags();
				event->setOnEntry();
				process(event);
			}
			else
			{
				TRANSITION(stateDefault, event);
			}
			return;
		}
		display_->setPixel(display_->cols() - 1 - (count % display_->cols()), count / display_->cols(), true);
		display_->setPixel(count % display_->cols(), display_->rows() - 1 - (count / display_->cols()), true);
		display_->show();
		count++;
		return;
	}
}

void MenuSM::stateHighscoreMenu(Event *event)
{
	if (event->onEntry())
	{
		item_.init(5, 1);
		event->setupMenu();
	}
	else if (processMenuStop(event))
	{
		return;
	}
	else if (event->controlButtonPressed())
	{
		byte advanced = item_.advance(event);
		if (advanced)
		{
			if (item_.value_ == 0)
			{
				if (advanced == MenuItem::DOWN_BTN)
				{
					TRANSITION(stateResetMenu, event);
				}
				else
				{
					TRANSITION(stateDefault, event);
				}
				return;
			}
			else
			{
				TRANSITION(stateDefault, event);
				return;
			}
		}
	}
	else
	{
		return;
	}
	switch (item_.value_)
	{
	case 0:
		display_->setIcon(0xbd42a59999a542bd, 0, false);
		display_->text1_.setText(language_ == EN ? "reset"
															  : "Zur"
																 "\x1c"
																 "cksetzen");
		break;
	case 1:
		display_->setIcon(0xfffff7e300081c00, 0, false);
		display_->text1_.setNumber(Tetris::highscore());
		break;
	case 2:
		display_->setIcon(0x3c20203c04045c00, 0, false);
		display_->text1_.setNumber(Snake::highscore());
		break;
	case 3:
		display_->setIcon(0x381003c00e30310, 0, false);
		display_->text1_.setNumber(Dodge::highscore());
		break;
	case 4:
		display_->setIcon(0x1c08000841d46b91, 0, false);
		display_->text1_.setNumber(SpaceInvaders::highscore());
	default:
		break;
	}
	display_->show();
}

void MenuSM::stateResetMenu(Event *event)
{
	if (event->onEntry())
	{
		display_->loadMenuConfig();
		event->setupMenu();
	}
	else if (processMenuStop(event))
	{
		return;
	}

	else if (event->controlButtonPressed())
	{
		if (event->buttonDown().state())
		{
			Tetris::resetHighscore();
			Snake::resetHighscore();
			Dodge::resetHighscore();
			LOAD_EFFECT_STANDART(stateDefault, event);
			return;
		}
		if (event->buttonUp().state())
		{
			TRANSITION(stateHighscoreMenu, event);
			return;
		}
	}
	else
	{
		return;
	}
	display_->clear();
	display_->text1_.setText((language_ == EN ? "reset scores"
															: "Highscores zur"
															  "\x1c"
															  "cksetzen"),
									 false);
	display_->setIcon(0x00040a1120408000, false);
	display_->show();
}

void MenuSM::showItem(const MenuSM::Item &item)
{
	display_->setIcon(item.icon_, 0, false);

	display_->text1_.setText(item.text_[language_]);
	display_->show();
}

MenuSM::MenuItem::Button MenuSM::MenuItem::advance(Event *event) { return advance(event, value_, num_); }
