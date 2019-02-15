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
#include "jump.h"
#include "snake.h"
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

	const char *texts[2][6] = {{"Tetris", "Snake", "Jump", "Dodge", "highscore", "setting"},
										{"Tetris", "Snake", "Jump", "Dodge", "Highscore", "Einstellungen"}};

	static MenuItem item;
	if (event->onEntry())
	{
		item.init(6);
		display_->loadMenuConfig();
		event->setFlag(Event::ProcessPinChanges);
		event->setFlag(Event::ProcessStop);
	}
	else if (event->buttonStop().pressed())
	{
		item.value_ = 0;
		event->clear();
		stateDefault(event);
	}
	else if (event->controlButtonPressed())
	{
		if (item.advance(event) == MenuItem::DOWN_BTN)
		{
			switch (item.value_)
			{
			case 0:
				TRANSITION(stateTetris, event);
				break;
			case 1:
				TRANSITION(stateSnake, event);
				break;
			case 2:
				TRANSITION(stateJump, event);
				break;
			case 3:
				TRANSITION(stateDodge, event);
				break;
			case 4:
				TRANSITION(stateHighscoreMenu, event);
				break;
			case 5:
				TRANSITION(stateSettingsMenu, event);
				break;
			default:
				break;
			}
			item.value_ = 0;
			return;
		}
	}
	else
	{
		return;
	}

	switch (item.value_)
	{
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
		display_->setIcon(0x381003c00e30310);
		break;
	case 4:
		display_->setIcon(0x00081c2018043810);
		break;
	case 5:
		display_->setIcon(0x00003c3c3c3c0000);
		break;
	default:
		break;
	}
	display_->text1_.setText(texts[language_][item.value_]);
}

void GameSM::stateTetris(Event *event)
{
	if (event->onEntry())
	{
		display_->loadsGameCofig();
		if (game_)
		{
			delete game_;
			game_ = nullptr;
		}
		game_ = new Tetris(display_);
		game_->setSpeed(speed_);
		game_->start(event);
		return;
	}
	if (game_->process(event))
	{
		TRANSITION(stateGameOver, event);
		return;
	}
}

void GameSM::stateSnake(Event *event)
{
	if (event->onEntry())
	{
		display_->loadsGameCofig();
		if (game_ != nullptr)
		{
			delete game_;
			game_ = nullptr;
		}
		game_ = new Snake(display_);
		game_->setSpeed(speed_);
		game_->start(event);
		return;
	}

	if (game_->process(event))
	{
		TRANSITION(stateGameOver, event);
		return;
	}
}

void GameSM::stateJump(Event *event)
{
	if (event->onEntry())
	{
		display_->loadsGameCofig();
		if (game_ != nullptr)
		{
			delete game_;
			game_ = nullptr;
		}
		game_ = new Jump(display_);
		game_->setSpeed(speed_);
		game_->start(event);
		return;
	}

	if (game_->process(event))
	{
		TRANSITION(stateGameOver, event);
		return;
	}
}

void GameSM::stateDodge(Event *event)
{
	if (event->onEntry())
	{
		display_->loadsGameCofig();
		if (game_ != nullptr)
		{
			delete game_;
			game_ = nullptr;
		}
		game_ = new Dodge(display_);
		game_->setSpeed(speed_);
		game_->start(event);
		return;
	}

	if (game_->process(event))
	{
		TRANSITION(stateGameOver, event);
		return;
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
			if (game_->isNewHighscore())
			{
				display_->text1_.setText((language_ == EN ? "new highscore!" : "neuer Highscore!"));
			}
			else
			{
				display_->text1_.setText("Game Over");
			}
			display_->text2_.setNumber(game_->score());
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
		item.init(3);
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
		display_->setIcon(0x0000122448241200);
		break;
	case 1:
		display_->setIcon(0x2060ff818181ff00);
		break;
	case 2:
		display_->setIcon(0x0007133558900000);
		break;
	default:
		break;
	}
	display_->text1_.setText(menu_text[language_][item.value_]);
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
	static byte count = 0;
	if (event->onEntry())
	{
		count = 0;
		display_->text1_.clear();
		display_->text2_.clear();
		event->removeAllTimers();
		event->addTimer(50);
		event->setFlag(Event::ProcessTimerOverflows);
		event->setFlag(Event::ProcessStop);
	}
	else if (processMenuStop(event))
	{
		return;
	}
	if (event->timer(0).overflow())
	{
		if (count >= display_->rows())
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
		display_->setRow(count, 0xFF);
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
		item.init(5, 0);
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
			if (item.value_ == 4)
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
		display_->setIcon(0x381003c00e30310);
		display_->text1_.setNumber(Dodge::highscore());
		break;
	case 4:
		display_->setIcon(0xbd42a59999a542bd);
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
			Jump::resetHighscore();
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
	display_->text1_.setText((language_ == EN ? "reset scores"
															: "Highscores zur"
															  "\x1c"
															  "cksetzen"));
	display_->setIcon(0x00040a1120408000);
}

GameSM::MenuItem::Button GameSM::MenuItem::advance(Event *event)
{
	return advance(event, value_, num_);
}
