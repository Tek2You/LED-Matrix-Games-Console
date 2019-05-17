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

#include "game_sm.h"
#include "avr/eeprom.h"
#include "avr/wdt.h"
#include "defaults.h"
#include "operators.h"

#include "dodge.h"
#include "snake.h"
#include "spaceinvaders.h"
#include "tetris.h"

#undef TRANSITION
#define TRANSITION(s, event)                                                                                           \
	{                                                                                                                   \
		setState(STATE_CAST(&GameSM::s));                                                                                \
		event->clearFlags();                                                                                             \
		event->setOnEntry();                                                                                             \
		process(event);                                                                                                  \
	}

#define LOAD_EFFECT_STANDART(s, event)                                                                                 \
	{                                                                                                                   \
		load_following_state_ = STATE_CAST(&GameSM::s);                                                                  \
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

GameSM::GameSM(Display *display, Event *event)
	 : StateMachine(STATE_CAST(&GameSM::stateDefault)), display_(display), language_(EN)
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

bool GameSM::processMenuStop(Event *event)
{
	if (event->buttonStop().pressed())
	{
		TRANSITION(stateDefault, event);
		return true;
	}
	return false;
}

GameSM::MenuItem::Button GameSM::MenuItem::advance(Event *event, char &item, const char num, const char min)
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

void GameSM::stateDefault(Event *event)
{
	static const Item items [5] PROGMEM  = {
	   Item{"highscore", "Highscore", 0x00081c2018043810},
	   Item{"settings", "Einstellungen", 0x00081c2018043810},
	   Item{"Tetris", "Tetris", 0xfffff7e300081c00},
	   Item{"Snake", "Snake", 0x3c20203c04045c00},
	   Item{"Dodge", "Dodge", 0x381003c00e30310}};

	static MenuItem item;

	if (event->onEntry())
	{
		item.init(6);
		item.value_ = last_played_game_ + 2;
		display_->loadMenuConfig();
		event->setFlag(Event::ProcessPinChanges);
		event->setFlag(Event::ProcessStop);
	}
	else if (event->buttonStop().pressed())
	{
		last_played_game_ = Game::TETRIS;
		item.value_ = 2;
		event->clear();
		stateDefault(event);
	}
	else if (event->controlButtonPressed())
	{
		if (item.advance(event) == MenuItem::DOWN_BTN)
		{
			switch (item.value_)
			{
			// last because we begin counting with 2
			case 0:
				TRANSITION(stateHighscoreMenu, event);
				break;
			case 1:
				TRANSITION(stateSettingsMenu, event);
				break;
			default:
				last_played_game_ = Game::GameType(item.value_ - 2);
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

	display_->setIcon(items[item.value_].icon_,0,false);

	display_->text1_.setText(items[item.value_].text_[language_]);
	display_->show();
}

void GameSM::stateGame(Event *event)
{
	if (event->onEntry())
	{
		display_->loadsGameCofig();
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
			//			game_ = new SpaceInvaders(display_);
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

void GameSM::stateGameOver(Event *event)
{
	if (event->onEntry())
	{
		LOAD_EFFECT_BEGIN(stateGameOver, event);
		display_->text1_.clear();
		display_->text2_.clear();
		if (game_ != nullptr)
		{
			event->setFlag(Event::ProcessPinChanges);
			event->setFlag(Event::ProcessStop);
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

void GameSM::stateSettingsMenu(Event *event)
{
	static MenuItem item;
	const char *menu_text[2][3] = {{"speed", "language", "brightness"}, {"Geschwindigkeit", "Sprache", "Helligkeit"}};
	if (event->onEntry())
	{
		display_->loadMenuConfig();
		item.init(3, 0);
		event->setFlag(Event::ProcessPinChanges);
		event->setFlag(Event::ProcessStop);
	}
	else if (processMenuStop(event))
	{
		return;
	}
	else if (event->controlButtonPressed())
	{
		switch (item.advance(event))
		{
		case MenuItem::DOWN_BTN:
			switch (item.value_)
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

	switch (item.value_)
	{
	case 0:
		display_->setIcon(0x0000122448241200, 0, false);
		break;
	case 1:
		display_->setIcon(0x2060ff818181ff00, 0, false);
		break;
	case 2:
		display_->setIcon(0x0007133558900000, 0, false);
		break;
	default:
		break;
	}
	display_->text1_.setText(menu_text[language_][item.value_]); // contains the showing on display
}

void GameSM::stateSpeedMenu(Event *event)
{
	static MenuItem item;
	if (event->onEntry())
	{
		display_->loadMenuConfig();
		item.init(5, speed_);
		event->setFlag(Event::ProcessPinChanges);
		event->setFlag(Event::ProcessStop);
	}
	else if (processMenuStop(event))
	{
		return;
	}
	else if (event->controlButtonPressed())
	{
		switch (item.advance(event))
		{
		case MenuItem::DOWN_BTN:
			speed_ = item.value_;
			eeprom_write_byte(&EE_speed, speed_);
			LOAD_EFFECT_STANDART(stateSettingsMenu, event);
		case MenuItem::UP_BTN:
			TRANSITION(stateSettingsMenu, event);
			return;
		default:
			break;
		}
	}
	else
	{
		return;
	}
	display_->clear();
	display_->text1_.setNumber(item.value_ + 1, false);
	byte cols = display_->cols() / 5.0 * (item.value_ + 1);
	for (int col = 0; col < cols; col++)
	{
		display_->setColumn(col, 0xFF);
	}
	display_->show();
}

void GameSM::stateBrightnessMenu(Event *event)
{
	static MenuItem item;
	if (event->onEntry())
	{
		display_->loadMenuConfig();
		item.init(4, brightness_);
		event->setFlag(Event::ProcessPinChanges);
		event->setFlag(Event::ProcessStop);
	}
	else if (processMenuStop(event))
	{
		return;
	}
	else if (event->controlButtonPressed())
	{
		switch (item.advance(event))
		{
		case MenuItem::DOWN_BTN:
			brightness_ = item.value_;
			display_->setBrightness(brightness_);
			eeprom_write_byte(&EE_brightness, brightness_);
			LOAD_EFFECT_STANDART(stateSettingsMenu, event);
			return;
		case MenuItem::UP_BTN:
			TRANSITION(stateSettingsMenu, event);
			return;
		default:
			break;
		}
	}
	else
	{
		return;
	}
	display_->clear();
	display_->text1_.setNumber(item.value_ + 1, false);
	byte cols = display_->cols() / 4.0 * (item.value_ + 1);
	for (int col = 0; col < cols; col++)
	{
		display_->setColumn(col, 0xFF);
	}
	display_->show();
	display_->setBrightness(item.value_);
}

void GameSM::stateLanguageMenu(Event *event)
{
	const char *menu_text[2] = {"english", "Deutsch"};
	static MenuItem item;
	if (event->onEntry())
	{
		display_->loadMenuConfig();
		item.init(2, (language_ == DE ? 0 : 1));
		event->setFlag(Event::ProcessPinChanges);
		event->setFlag(Event::ProcessStop);
	}
	else if (processMenuStop(event))
	{
		return;
	}
	else if (event->controlButtonPressed())
	{
		switch (item.advance(event))
		{ // enter pressed
		case MenuItem::DOWN_BTN:
			language_ = (item.value_ == 0 ? EN : DE);
			eeprom_write_byte(&EE_language, byte(language_));
			LOAD_EFFECT_STANDART(stateSettingsMenu, event);
			return;
		case MenuItem::UP_BTN:
			TRANSITION(stateSettingsMenu, event);
			return;
		default:
			break;
		}
	}
	else
	{
		return;
	}

	switch (item.value_)
	{
	case 0:
		display_->text2_.setText("E", false);
		break;
	case 1:
		display_->text2_.setText("D", false);
		break;
	default:
		break;
	}
	display_->text1_.setText(menu_text[item.value_]);
}

void GameSM::stateLoadEffect(Event *event)
{
	static int count = 0;
	if (event->onEntry())
	{
		count = 0;
		display_->text1_.clear();
		display_->text2_.clear();
		event->removeAllTimers();
		event->addTimer(12);
		event->setFlag(Event::ProcessTimerOverflows);
		event->setFlag(Event::ProcessStop);
	}
	else if (processMenuStop(event))
	{
		return;
	}
	if (event->timer(0).overflow())
	{
		if (count >= display_->rows() * display_->cols() / 2)
		{
			if (load_following_state_)
			{
				setState(load_following_state_);
				load_following_state_ = nullptr;
				event->removeAllTimers();
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

void GameSM::stateHighscoreMenu(Event *event)
{
	static MenuItem item;
	if (event->onEntry())
	{
		item.init(4, 0);
		event->setFlag(Event::ProcessPinChanges);
		event->setFlag(Event::ProcessStop);
	}
	else if (processMenuStop(event))
	{
		return;
	}
	else if (event->controlButtonPressed())
	{
		byte advanced = item.advance(event);
		if (advanced)
		{
			if (item.value_ == 3)
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
	switch (item.value_)
	{
	case 0:
		display_->setIcon(0xfffff7e300081c00, 0, false);
		display_->text1_.setNumber(Tetris::highscore());
		break;
	case 1:
		display_->setIcon(0x3c20203c04045c00, 0, false);
		display_->text1_.setNumber(Snake::highscore());
		break;
	case 2:
		display_->setIcon(0x381003c00e30310, 0, false);
		display_->text1_.setNumber(Dodge::highscore());
		break;
	case 3:
		display_->setIcon(0xbd42a59999a542bd, 0, false);
		display_->text1_.setText(language_ == EN ? "reset"
															  : "Zur"
																 "\x1c"
																 "cksetzen");
	default:
		break;
	}
}

void GameSM::stateResetMenu(Event *event)
{
	if (event->onEntry())
	{
		display_->loadMenuConfig();
		event->setFlag(Event::ProcessPinChanges);
		event->setFlag(Event::ProcessStop);
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

void GameSM::processGame(Event *event)
{
	if (game_->process(event))
	{
		TRANSITION(stateGameOver, event);
	}
}

GameSM::MenuItem::Button GameSM::MenuItem::advance(Event *event) { return advance(event, value_, num_); }
