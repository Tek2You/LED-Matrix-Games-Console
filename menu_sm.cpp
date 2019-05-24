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

bool MenuSM::MenuItem::advance(Event *event, bool overflow)
{
	if (event->buttonRight().pressed())
	{
		if (++value_ >= num_) value_ = (overflow ? 0 : num_ - 1);
		return true;
	}
	if (event->buttonLeft().pressed())
	{
		if (--value_ < 0) value_ = (overflow ? num_ - 1 : 0);
		return true;
	}
	return false;
}

bool MenuSM::MenuItem::advance(bool diretion, bool overflow)
{
	if (diretion)
	{
		if (++value_ >= num_) value_ = (overflow ? 0 : num_ - 1);
		return true;
	}
	else
	{
		if (--value_ < 0) value_ = (overflow ? num_ - 1 : 0);
		return true;
	}
	return false;
}

void MenuSM::stateDefault(Event *event)
{
	static const Item items[6] = {Item{"highscore", "Highscore", 0x00081c2018043810},
	                              Item{"settings", "Einstellungen", 0x000003c3c3c3c0000},
	                              Item{"Tetris", "Tetris", 0xfffff7e300081c00},
	                              Item{"Snake", "Snake", 0x3c20203c04045c00},
	                              Item{"Space Invaders", "Space Invaders", 0x1c08000841d46b91},
	                              Item{"Dodge", "Dodge", 0x381003c00e30310}};

	if (event->onEntry())
	{
		item_.init(6, last_played_game_ + 2);
		display_->loadMenuConfig();
		event->setupMenu();
	}
	else
	{
		if (event->buttonDown().pressed())
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
		if (event->buttonStop().pressed())
		{
			last_played_game_ = Game::TETRIS;
			item_.value_ = 2;
		}
		if (!item_.advance(event)) return;
	}

	showItem(items[item_.value_]);
}

void MenuSM::stateGame(Event *event)
{
	if (event->onEntry())
	{
		display_->loadsGameCofig();
		event->setupGame();

		switch (last_played_game_)
		{
		case Game::TETRIS:
			game_ = new Tetris(display_);
			break;
		case Game::SNAKE:
			game_ = new Snake(display_);
			break;
		case Game::DODGE:
			game_ = new SpaceInvaders(display_);
			break;
		case Game::SPACE_INVADERS:
			game_ = new Dodge(display_);
			break;
		default:
			return;
		}
		game_->setSpeed(speed_);
		game_->start(event);
		return;
	}
	else if (game_->process(event))
	{
		TRANSITION(stateGameOver, event);
	}
}

void MenuSM::stateGameOver(Event *event)
{
	if (event->onEntry())
	{
		LOAD_EFFECT_BEGIN(stateGameOver, event);
		display_->loadsGameCofig();
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
	else // not on entry
	{
		if (event->controlButtonPressed())
		{
			TRANSITION(stateDefault, event);
			return;
		}
		if (processMenuStop(event))
		{
		}
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
	else // not on entry
	{

		if (processMenuStop(event))
		{
			return;
		}

		if (event->buttonDown().pressed())
		{
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
		}

		if (event->buttonUp().pressed())
		{
			TRANSITION(stateDefault, event);
			return;
		}

		if (!item_.advance(event)) return;
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
	else // not on entry
	{
		if (processMenuStop(event))
		{
			return;
		}
		if (event->buttonDown().pressed())
		{
			speed_ = item_.value_;
			eeprom_write_byte(&EE_speed, speed_);
			LOAD_EFFECT_STANDART(stateSettingsMenu, event);
			return;
		}
		if (event->buttonUp().pressed())
		{
			TRANSITION(stateSettingsMenu, event);
			return;
		}
		if ((auto_trigger = static_cast<ButtonAutoTrigger *>(event->trigger(0)))->triggered())
		{
			item_.advance(auto_trigger->direction() == ButtonAutoTrigger::BTN_2, false);
		}
	}

	display_->setBar(item_.value_ + 1, item_.num_);
	display_->text1_.setNumber(item_.value_ + 1);
}

void MenuSM::stateBrightnessMenu(Event *event)
{
	ButtonAutoTrigger *auto_trigger;

	if (event->onEntry())
	{
		display_->loadMenuConfig();
		item_.init(4, brightness_);
		event->setupGame();
		event->addTrigger(new ButtonAutoTrigger(&event->buttonLeft(), &event->buttonRight(), 500, 400));
	}
	else // not on entry
	{
		if (processMenuStop(event))
		{
			return;
		}
		if (event->buttonDown().pressed())
		{
			brightness_ = item_.value_;
			display_->setBrightness(brightness_);
			eeprom_write_byte(&EE_brightness, brightness_);
			LOAD_EFFECT_STANDART(stateSettingsMenu, event);
			return;
		}
		if (event->buttonUp().pressed())
		{
			TRANSITION(stateSettingsMenu, event);
			return;
		}
		if ((auto_trigger = static_cast<ButtonAutoTrigger *>(event->trigger(0)))->triggered())
		{
			item_.advance(auto_trigger->direction() == ButtonAutoTrigger::BTN_2, false);
		}
	}

	display_->setBar(item_.value_ + 1, item_.num_);
	display_->text1_.setNumber(item_.value_ + 1);
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
	else // not on entry
	{
		if (processMenuStop(event))
		{
			return;
		}
		if (event->buttonDown().pressed())
		{
			language_ = Language(item_.value_);
			eeprom_write_byte(&EE_language, byte(language_));
			LOAD_EFFECT_STANDART(stateSettingsMenu, event);
			return;
		}
		if (event->buttonDown().pressed())
		{
			TRANSITION(stateSettingsMenu, event);
			return;
		}
		if (!item_.advance(event)) return;
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
	else
	{
		if (processMenuStop(event))
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
}

void MenuSM::stateHighscoreMenu(Event *event)
{
	if (event->onEntry())
	{
		item_.init(5, 1);
		event->setupMenu();
	}
	else
	{
		if (processMenuStop(event))
		{
			return;
		}
		if (event->buttonDown().pressed())
		{
			if (item_.value_ == 0)
			{
				TRANSITION(stateResetMenu, event);
			}
			else
			{
				TRANSITION(stateDefault, event);
			}
			return;
		}
		if (event->buttonUp().pressed())
		{
			TRANSITION(stateDefault, event);
			return;
		}
		if (!item_.advance(event))
		{
			return;
		}
	}
	Display::Icon icon(0);
	int highscore;

	switch (item_.value_)
	{
	case 0:
		display_->setIcon(0xbd42a59999a542bd, 0, false);
		display_->text1_.setText(language_ == EN ? "reset"
															  : "Zur"
																 "\x1c"
																 "cksetzen");
		return;
	case 1:
		icon = Display::Icon(0xfffff7e300081c00);
		highscore = Tetris::highscore();
		break;
	case 2:
		icon = Display::Icon(0x3c20203c04045c00);
		highscore = Snake::highscore();
		break;
	case 3:
		icon = Display::Icon(0x1c08000841d46b91);
		highscore = SpaceInvaders::highscore();
		break;
	case 4:
		icon = Display::Icon(0x381003c00e30310);
		highscore = Dodge::highscore();
		break;
	default:
		break;
		;
	}
	display_->setIcon(icon, false);
	display_->text1_.setNumber(highscore);
}

void MenuSM::stateResetMenu(Event *event)
{
	if (event->onEntry())
	{
		display_->loadMenuConfig();
		event->setupMenu();
	}
	else
	{
		if (processMenuStop(event))
		{
			return;
		}
		if (event->buttonDown().pressed())
		{
			Tetris::resetHighscore();
			Snake::resetHighscore();
			Dodge::resetHighscore();
			SpaceInvaders::resetHighscore();
			LOAD_EFFECT_STANDART(stateDefault, event);
			return;
		}
		if (event->buttonUp().pressed())
		{
			TRANSITION(stateHighscoreMenu, event);
			return;
		}
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
